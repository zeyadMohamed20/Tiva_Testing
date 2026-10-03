/*
 * TM4C123GH6PM continuous self-test / stress test. No external hardware needed.
 *
 * Startup sequence (on-board LEDs):
 *   1. Lamp test: red, green, blue, each 1 s
 *   2. Button check: blue blinks until SW1 is pressed (blue follows the button),
 *      then violet blinks until SW2 is pressed (violet follows the button).
 *      No press within 60 s = button fault (3 red flashes, final result red)
 *   3. Test round: for each of the 6 tests, 1 s after it finishes the LED lights green (pass)
 *      or red (fail) for 1 s, then goes off. Each test repeats TEST_PASSES times internally
 *   4. Idle: solid green = all passed, solid red = a fault. SW1 runs the round again
 *
 * Results are also in g_stats[] / g_iterations / g_failed_mask (watch in CCS).
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/watchdog.h"
#include "driverlib/systick.h"

#define LED_RED     GPIO_PIN_1
#define LED_BLUE    GPIO_PIN_2
#define LED_GREEN   GPIO_PIN_3
#define SW1_PIN     GPIO_PIN_4
#define SW2_PIN     GPIO_PIN_0

#define RAM_TEST_WORDS      5120u           /* 20 KB of the 32 KB SRAM */
#define FLASH_CRC_BYTES     0x4000u         /* first 16 KB of flash */
#define SYSCLK_HZ           80000000u
#define CLOCK_CHECK_MS      100u
#define WDT_TIMEOUT_S       5u
#define TEST_PASSES         20u             /* each test repeats this many times before it reports */
#define BUTTON_TIMEOUT_MS   60000u          /* a button not pressed within this time counts as a fault */

enum {
    TEST_CPU_INT,
    TEST_CPU_FPU,
    TEST_RAM,
    TEST_FLASH_CRC,
    TEST_CLOCK,
    TEST_GPIO,
    TEST_COUNT
};

typedef struct {
    uint32_t runs;
    uint32_t fails;
} test_stat_t;

volatile test_stat_t g_stats[TEST_COUNT];
volatile uint32_t g_iterations = 0;
volatile uint32_t g_failed_mask = 0;
volatile uint32_t g_clock_ticks_measured = 0;
volatile uint32_t g_ms = 0;

static volatile uint32_t ram_block[RAM_TEST_WORDS];
static uint32_t flash_crc_ref;
static bool flash_crc_ref_valid = false;
static bool fault_latched = false;
static uint32_t rcc_ref, rcc2_ref;

void SysTick_Handler(void)
{
    g_ms++;
}

static void feed_watchdog(void)
{
    WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet() * WDT_TIMEOUT_S);
}

static void delay_ms(uint32_t ms)
{
    uint32_t start = g_ms;
    feed_watchdog();
    while ((g_ms - start) < ms);
}

static void record(int test, bool ok)
{
    g_stats[test].runs++;
    if (!ok) {
        g_stats[test].fails++;
        g_failed_mask |= (1u << test);
        fault_latched = true;
    }
    feed_watchdog();
}

/* Integer ALU, multiply, divide, modulo and 64-bit arithmetic against precomputed results. */
static bool test_cpu_int(void)
{
    uint32_t x = 2463534242u;
    uint32_t i;
    uint64_t a = 0, b = 1, t, sum;
    uint32_t msum = 0;
    bool ok = true;

    for (i = 0; i < 100000u; i++) {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
    }
    ok &= (x == 0x0BB69297u);

    for (i = 0; i < 90u; i++) {
        t = a + b;
        a = b;
        b = t;
    }
    ok &= (a == 2880067194370816120ull);

    for (i = 1; i <= 10000u; i++) {
        msum += (i * 7919u) % 10007u;
    }
    ok &= (msum == 50041187u);

    sum = 0;
    for (i = 1; i <= 5000u; i++) {
        sum += (i * i) / 7u;
    }
    ok &= (sum == 5954165357ull);

    return ok;
}

/* FPv4 single precision. Every operation here is exactly representable, so results must be exact. */
static bool test_cpu_fpu(void)
{
    volatile float acc = 0.0f;
    volatile float v;
    uint32_t i;
    bool ok = true;

    for (i = 1; i <= 4095u; i++) {
        acc += (float)i;
    }
    ok &= (acc == 8386560.0f);

    for (i = 1; i <= 5000000u; i += 49999u) {
        v = (float)i;
        v = v * 3.0f;
        v = v / 3.0f;
        ok &= (v == (float)i);
    }

    for (i = 1; i <= 4096u; i++) {
        v = (float)(i * i);
        ok &= (__builtin_sqrtf(v) == (float)i);
    }
    return ok;
}

/* Fill the block with a pattern and verify, for several patterns. */
static bool ram_pattern(uint32_t pattern)
{
    uint32_t i;
    for (i = 0; i < RAM_TEST_WORDS; i++) ram_block[i] = pattern;
    for (i = 0; i < RAM_TEST_WORDS; i++) if (ram_block[i] != pattern) return false;
    return true;
}

/* March C- over 32-bit words (up/down address orders) plus data and address-line patterns. */
static bool test_ram(void)
{
    int32_t i;
    uint32_t p;
    static const uint32_t patterns[] = {
        0x00000000u, 0xFFFFFFFFu, 0xAAAAAAAAu, 0x55555555u,
        0xCCCCCCCCu, 0x33333333u, 0xF0F0F0F0u, 0x0F0F0F0Fu
    };

    for (p = 0; p < sizeof(patterns) / sizeof(patterns[0]); p++) {
        if (!ram_pattern(patterns[p])) return false;
    }

    /* walking 1 / walking 0 across every data line */
    for (p = 0; p < 32u; p++) {
        if (!ram_pattern(1u << p)) return false;
        if (!ram_pattern(~(1u << p))) return false;
    }

    for (i = 0; i < (int32_t)RAM_TEST_WORDS; i++) ram_block[i] = 0;
    for (i = 0; i < (int32_t)RAM_TEST_WORDS; i++) {
        if (ram_block[i] != 0) return false;
        ram_block[i] = 0xFFFFFFFFu;
    }
    for (i = 0; i < (int32_t)RAM_TEST_WORDS; i++) {
        if (ram_block[i] != 0xFFFFFFFFu) return false;
        ram_block[i] = 0;
    }
    for (i = (int32_t)RAM_TEST_WORDS - 1; i >= 0; i--) {
        if (ram_block[i] != 0) return false;
        ram_block[i] = 0xFFFFFFFFu;
    }
    for (i = (int32_t)RAM_TEST_WORDS - 1; i >= 0; i--) {
        if (ram_block[i] != 0xFFFFFFFFu) return false;
        ram_block[i] = 0;
    }
    for (i = (int32_t)RAM_TEST_WORDS - 1; i >= 0; i--) {
        if (ram_block[i] != 0) return false;
    }

    /* each word stores its own address: catches address-line aliasing */
    for (i = 0; i < (int32_t)RAM_TEST_WORDS; i++) ram_block[i] = (uint32_t)&ram_block[i];
    for (i = 0; i < (int32_t)RAM_TEST_WORDS; i++) {
        if (ram_block[i] != (uint32_t)&ram_block[i]) return false;
    }
    return true;
}

static uint32_t crc32_flash(void)
{
    const uint8_t *p = (const uint8_t *)0x00000000u;
    uint32_t crc = 0xFFFFFFFFu;
    uint32_t n, bit;

    for (n = 0; n < FLASH_CRC_BYTES; n++) {
        crc ^= p[n];
        for (bit = 0; bit < 8u; bit++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
        }
    }
    return ~crc;
}

/* The first pass sets the reference; every later pass must read back the same CRC. */
static bool test_flash_crc(void)
{
    uint32_t crc = crc32_flash();
    if (!flash_crc_ref_valid) {
        flash_crc_ref = crc;
        flash_crc_ref_valid = true;
        return true;
    }
    return crc == flash_crc_ref;
}

/*
 * Clock checks. The board has no second oscillator available to software, so this verifies:
 *  - PLL still locked, no main-oscillator failure, RCC/RCC2 unchanged since boot
 *  - 100 SysTick ms equal 100 ms of Timer0 counts to within 0.1% (catches lost SysTick interrupts)
 */
static bool test_clock(void)
{
    uint32_t t0, t1, elapsed, ms0, expected;
    bool ok;

    ok  = (HWREG(SYSCTL_RIS) & SYSCTL_RIS_PLLLRIS) != 0;
    ok &= (HWREG(SYSCTL_RIS) & SYSCTL_RIS_MOFRIS) == 0;
    ok &= (HWREG(SYSCTL_RCC)  == rcc_ref) && (HWREG(SYSCTL_RCC2) == rcc2_ref);

    ms0 = g_ms;
    while (g_ms == ms0);
    t0 = TimerValueGet(TIMER0_BASE, TIMER_A);
    ms0 = g_ms;
    while ((g_ms - ms0) < CLOCK_CHECK_MS);
    t1 = TimerValueGet(TIMER0_BASE, TIMER_A);

    elapsed = t0 - t1;                      /* timer counts down */
    g_clock_ticks_measured = elapsed;
    expected = (SYSCLK_HZ / 1000u) * CLOCK_CHECK_MS;
    ok &= (elapsed > expected - expected / 1000u) && (elapsed < expected + expected / 1000u);
    return ok;
}

/*
 * GPIO block check without driving any pin: write patterns to the interrupt-mask register of
 * ports E and F (NVIC GPIO interrupts stay disabled) and read them back.
 */
static bool test_gpio(void)
{
    static const uint32_t bases[] = { GPIO_PORTE_BASE, GPIO_PORTF_BASE };
    static const uint8_t patterns[] = {
        0x00, 0xFF, 0xAA, 0x55, 0x0F, 0xF0, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00
    };
    uint32_t b, i;
    bool ok = true;

    for (b = 0; b < sizeof(bases) / sizeof(bases[0]); b++) {
        uint32_t saved = HWREG(bases[b] + GPIO_O_IM);
        for (i = 0; i < sizeof(patterns); i++) {
            HWREG(bases[b] + GPIO_O_IM) = patterns[i];
            ok &= ((HWREG(bases[b] + GPIO_O_IM) & 0xFFu) == patterns[i]);
        }
        HWREG(bases[b] + GPIO_O_IM) = saved;
    }
    return ok;
}

static bool (* const tests[TEST_COUNT])(void) = {
    test_cpu_int, test_cpu_fpu, test_ram, test_flash_crc, test_clock, test_gpio
};

static void leds(uint8_t mask)
{
    GPIOPinWrite(GPIO_PORTF_BASE, LED_RED | LED_BLUE | LED_GREEN, mask);
}

static void blink(uint8_t led, uint32_t count, uint32_t on_ms, uint32_t off_ms)
{
    while (count--) {
        leds(led);  delay_ms(on_ms);
        leds(0);    delay_ms(off_ms);
    }
}

static bool switch_pressed(uint8_t pin)
{
    return GPIOPinRead(GPIO_PORTF_BASE, pin) == 0;
}

/* Blink the LED until the switch is pressed, light it while held, return after release. */
static bool wait_for_button(uint8_t pin, uint8_t led)
{
    uint32_t start = g_ms;
    for (;;) {
        leds(((g_ms / 500u) & 1u) ? led : 0);
        if (switch_pressed(pin)) {
            delay_ms(20);
            if (switch_pressed(pin)) break;
        }
        if ((g_ms - start) > BUTTON_TIMEOUT_MS) {
            leds(0);
            return false;
        }
    }
    leds(led);
    while (switch_pressed(pin));
    delay_ms(50);
    leds(0);
    return true;
}

/* Any unexpected CPU exception: show red; the watchdog then resets the board to the red fault state. */
void app_fault_handler(void)
{
    HWREG(SYSCTL_RCGCGPIO) |= 0x20;
    __asm(" nop");
    __asm(" nop");
    HWREG(GPIO_PORTF_BASE + GPIO_O_DIR) |= LED_RED;
    HWREG(GPIO_PORTF_BASE + GPIO_O_DEN) |= LED_RED;
    HWREG(GPIO_PORTF_BASE + (LED_RED << 2)) = LED_RED;
    for (;;);
}

/* Run one test TEST_PASSES times back to back; it only passes if every pass passes. */
static bool run_test(int t)
{
    uint32_t p;
    for (p = 0; p < TEST_PASSES; p++) {
        bool ok = tests[t]();
        record(t, ok);
        if (!ok) return false;
    }
    return true;
}

/* One full round: for each test, wait 1 s after it finishes, then light green (pass) or red (fail) for 1 s. */
static void run_round(void)
{
    int t;
    delay_ms(500);
    for (t = 0; t < TEST_COUNT; t++) {
        bool ok = run_test(t);
        delay_ms(1000);
        leds(ok ? LED_GREEN : LED_RED);
        delay_ms(1000);
        leds(0);
    }
    g_iterations++;
    delay_ms(500);
}

static void clear_results(void)
{
    uint32_t i;
    for (i = 0; i < TEST_COUNT; i++) {
        g_stats[i].runs = 0;
        g_stats[i].fails = 0;
    }
    g_iterations = 0;
    g_failed_mask = 0;
    fault_latched = false;
    leds(0);
}

int main(void)
{
    bool wdt_reset;

    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    rcc_ref  = HWREG(SYSCTL_RCC);
    rcc2_ref = HWREG(SYSCTL_RCC2);

    SysTickPeriodSet(SysCtlClockGet() / 1000);
    SysTickIntEnable();
    SysTickEnable();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG0));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED | LED_BLUE | LED_GREEN);
    /* PF0 (SW2) is a locked NMI pin and must be unlocked before use as GPIO */
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= SW2_PIN;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, SW1_PIN | SW2_PIN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, SW1_PIN | SW2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFFu);
    TimerEnable(TIMER0_BASE, TIMER_A);

    wdt_reset = (SysCtlResetCauseGet() & SYSCTL_CAUSE_WDOG0) != 0;
    SysCtlResetCauseClear(SysCtlResetCauseGet());

    /* lamp test so the LEDs themselves can be checked */
    blink(LED_RED,   1, 1000, 500);
    blink(LED_GREEN, 1, 1000, 500);
    blink(LED_BLUE,  1, 1000, 500);

    /* skip the interactive part after a watchdog reset so an unattended board recovers */
    if (!wdt_reset) {
        bool sw1_ok = wait_for_button(SW1_PIN, LED_BLUE);
        bool sw2_ok = wait_for_button(SW2_PIN, LED_RED | LED_BLUE);   /* violet */
        if (!sw1_ok || !sw2_ok) {
            g_failed_mask |= (1u << TEST_COUNT);        /* bit 6: a button did not respond */
            fault_latched = true;
            blink(LED_RED, 3, 200, 200);
        }
    }

    WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet() * WDT_TIMEOUT_S);
    WatchdogStallEnable(WATCHDOG0_BASE);
    WatchdogResetEnable(WATCHDOG0_BASE);
    WatchdogEnable(WATCHDOG0_BASE);

    if (wdt_reset) {
        fault_latched = true;       /* previous run hung and the watchdog reset the chip */
    } else {
        run_round();
    }

    /* Idle: solid green = all passed, solid red = fault. SW1 runs the whole round again. */
    while (1) {
        leds(fault_latched ? LED_RED : LED_GREEN);
        feed_watchdog();
        if (switch_pressed(SW1_PIN)) {
            delay_ms(20);
            if (switch_pressed(SW1_PIN)) {
                while (switch_pressed(SW1_PIN));
                delay_ms(50);
                clear_results();
                run_round();
            }
        }
    }
}
