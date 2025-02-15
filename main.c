#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/watchdog.h"
#include "driverlib/systick.h"
#include "driverlib/flash.h"

/* Base addresses for GPIO and system control registers */
#define SYSCTL_RCGCGPIO_R      (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_PRGPIO_R        (*((volatile uint32_t *)0x400FEA08))
#define GPIO_PORTF_DATA_R      (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R       (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R       (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_PUR_R       (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_LOCK_R      (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R        (*((volatile uint32_t *)0x40025524))
#define GPIO_PORTF_AMSEL_R     (*((volatile uint32_t *)0x40025528))
#define GPIO_PORTF_PCTL_R      (*((volatile uint32_t *)0x4002552C))

/* Unlock value for GPIO_CR */
#define GPIO_LOCK_KEY          0x4C4F434B

/* Debounce delay in milliseconds */
#define DEBOUNCE_DELAY_MS 5

// Function prototypes
void cpu_stress_test();
void ram_stress_test();
void gpio_test();
void systick_test();
void led_test();
void delay_ms(uint32_t milliseconds);
void indicate_result(bool success);
void GPIO_BuiltinButtonsLedsInit(void);
void GPIO_RedLedToggle(void);
void GPIO_BlueLedToggle(void);
uint8_t GPIO_SW1GetState(void);
uint8_t GPIO_SW2GetState(void);
uint8_t DebounceButton(uint8_t (*GetStateFunc)(void));

// Global variables
volatile uint32_t tick_count = 0;

// SysTick ISR
void SysTick_Handler(void) {
    tick_count++;
}

int main(void) {
    // Set clock to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initialize SysTick for 1ms interrupts
    SysTickPeriodSet(SysCtlClockGet() / 1000);
    SysTickIntEnable();
    SysTickEnable();

    // Enable GPIO for LED and Switches
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Run all tests
    cpu_stress_test();
    ram_stress_test();
    gpio_test();
    systick_test();
    led_test();

    // Initialize buttons and LEDs
    GPIO_BuiltinButtonsLedsInit();

    // Declare counters before the loop
    uint8_t sw1_prev_state = 1; // Initial state of SW1 (not pressed)
    uint8_t sw2_prev_state = 1; // Initial state of SW2 (not pressed)

    while (1) {
        // Debounced states of the switches
        uint8_t sw1_state = DebounceButton(GPIO_SW1GetState);
        uint8_t sw2_state = DebounceButton(GPIO_SW2GetState);

        // Check SW1 (Red LED toggle)
        if ((sw1_prev_state == 1) && (sw1_state == 0)) {  // Detect falling edge
            GPIO_RedLedToggle();
        }
        sw1_prev_state = sw1_state;  // Update previous state

        // Check SW2 (Blue LED toggle)
        if ((sw2_prev_state == 1) && (sw2_state == 0)) {  // Detect falling edge
            GPIO_BlueLedToggle();
        }
        sw2_prev_state = sw2_state;  // Update previous state
    }
}

// CPU STRESS TEST - Fibonacci calculation
void cpu_stress_test() {
    volatile uint64_t a = 0, b = 1, temp;
    volatile uint64_t expected_fib_40 = 102334155;
    uint64_t i;

    for (i = 2; i < 100; i++) {
        temp = a + b;
        a = b;
        b = temp;
        if (i == 40 && b != expected_fib_40) {
            indicate_result(false);
            return;
        }
    }
    indicate_result(true);
}

// RAM TEST - Write/Read Check
volatile uint32_t test_array[1024];
void ram_stress_test() {
    uint32_t i;
    for (i = 0; i < 1024; i++) {
        test_array[i] = i;
    }

    for (i = 0; i < 1024; i++) {
        if (test_array[i] != i) {
            indicate_result(false);
            return;
        }
    }
    indicate_result(true);
}

// GPIO LOOPBACK TEST
void gpio_test() {
    bool all_passed = true;
    uint8_t pins[2] = {GPIO_PIN_1, GPIO_PIN_3};
    int i;

    for (i = 0; i < 2; i++) {
        GPIOPinWrite(GPIO_PORTF_BASE, pins[i], pins[i]);
        delay_ms(10);
        if (!(GPIOPinRead(GPIO_PORTF_BASE, pins[i]) & pins[i])) {
            all_passed = false;
        }
        GPIOPinWrite(GPIO_PORTF_BASE, pins[i], 0);
        delay_ms(10);
        if (GPIOPinRead(GPIO_PORTF_BASE, pins[i]) & pins[i]) {
            all_passed = false;
        }
    }
    indicate_result(all_passed);
}

// SYSTICK TIMER TEST
void systick_test() {
    tick_count = 0;
    delay_ms(100);
    if (tick_count < 90 || tick_count > 110) {
        indicate_result(false);
        return;
    }
    indicate_result(true);
}

// LED Test
void led_test(void) {
    uint8_t leds[] = {GPIO_PIN_1, GPIO_PIN_1 | GPIO_PIN_3, GPIO_PIN_3, GPIO_PIN_2};
    int i;

    for (i = 0; i < 4; i++) {
        GPIOPinWrite(GPIO_PORTF_BASE, leds[i], leds[i]); // Turn LED ON
        delay_ms(1000);
        GPIOPinWrite(GPIO_PORTF_BASE, leds[i], 0);       // Turn LED OFF
        delay_ms(500);
    }
}

// Delay function using SysTick
void delay_ms(uint32_t milliseconds) {
    tick_count = 0;
    while (tick_count < milliseconds);
}

// LED Indication function
void indicate_result(bool success) {
    uint8_t led_pin = success ? GPIO_PIN_3 : GPIO_PIN_1;
    GPIOPinWrite(GPIO_PORTF_BASE, led_pin, led_pin);
    delay_ms(2000);
    GPIOPinWrite(GPIO_PORTF_BASE, led_pin, 0);
    delay_ms(2000);
}

// Debounce function
uint8_t DebounceButton(uint8_t (*GetStateFunc)(void)) {
    uint8_t state = GetStateFunc();
    delay_ms(5);
    if (state == GetStateFunc()) {
        return state;
    }
    return 1;
}

// GPIO Initialization
void GPIO_BuiltinButtonsLedsInit(void) {
    SYSCTL_RCGCGPIO_R |= (1 << 5);
    while (!(SYSCTL_PRGPIO_R & (1 << 5)));

    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= (1 << 0);

    GPIO_PORTF_AMSEL_R &= ~(0x1F);
    GPIO_PORTF_PCTL_R &= ~(0x000FFFFF);
    GPIO_PORTF_DIR_R &= ~((1 << 0) | (1 << 4));
    GPIO_PORTF_DIR_R |= (1 << 1) | (1 << 2) | (1 << 3);
    GPIO_PORTF_PUR_R |= (1 << 0) | (1 << 4);
    GPIO_PORTF_DEN_R |= 0x1F;

    GPIO_PORTF_DATA_R &= ~((1 << 1) | (1 << 2) | (1 << 3));
}

// LED Toggle Functions
void GPIO_RedLedToggle(void) {
    GPIO_PORTF_DATA_R ^= (1 << 1);
}

void GPIO_BlueLedToggle(void) {
    GPIO_PORTF_DATA_R ^= (1 << 2);
}

// Button State Functions
uint8_t GPIO_SW1GetState(void) {
    return ((GPIO_PORTF_DATA_R & (1 << 4)) >> 4);
}

uint8_t GPIO_SW2GetState(void) {
    return ((GPIO_PORTF_DATA_R & (1 << 0)) >> 0);
}
