#include <stdint.h>
#include <stdbool.h>
#include "tivaware/inc/tm4c123gh6pm.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/interrupt.h"

// Define LED pins for test results
#define TEST_RESULT_PASS_PIN GPIO_PIN_3   // Green LED on PF3
#define TEST_RESULT_FAIL_PIN GPIO_PIN_1   // Red LED on PF1

// Global flags to track interrupts on pins 0 and 4 for Port F
volatile bool interruptOccurredF0 = false;
volatile bool interruptOccurredF4 = false;

// Interrupt handler for GPIO Port F
void GPIOF_Handler(void)
{
    uint32_t status = GPIOIntStatus(GPIO_PORTF_BASE, false);  // Get raw interrupt status

    // Short debounce delay
    SysCtlDelay(SysCtlClockGet() / (3 * 1000));  // ~1 ms delay assuming a 3-cycle SysCtlDelay

    if (status & GPIO_PIN_0) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);  // Clear interrupt flag for PF0
        interruptOccurredF0 = true;                 // Update flag for PF0
    }

    if (status & GPIO_PIN_4) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);  // Clear interrupt flag for PF4
        interruptOccurredF4 = true;                 // Update flag for PF4
    }
}

void stressTestGPIOInterrupts(void)
{
    // Enable GPIO Port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Wait for Port F to be ready
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}

    // Configure test result LEDs as output on PF1 and PF3
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, TEST_RESULT_PASS_PIN | TEST_RESULT_FAIL_PIN);
    GPIOPinWrite(GPIO_PORTF_BASE, TEST_RESULT_PASS_PIN | TEST_RESULT_FAIL_PIN, 0);  // Clear LEDs

    // Configure pins 0 and 4 as input on Port F
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // Enable pull-up resistors on PF0 and PF4 to avoid floating pins
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Register interrupt handler for Port F
    GPIOIntRegister(GPIO_PORTF_BASE, GPIOF_Handler);

    // Set interrupt type to both edges for pins 0 and 4 on Port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_BOTH_EDGES);

    // Enable interrupts for pins 0 and 4 on Port F
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);

    // Enable global interrupts
    IntMasterEnable();

    // Simulate toggling Port F pins 0 and 4 to trigger interrupts
    int i;
    for (i = 0; i < 10; i++) {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_0 | GPIO_PIN_4);
        SysCtlDelay(SysCtlClockGet() / 1500);  // Small delay
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        SysCtlDelay(SysCtlClockGet() / 1500);  // Small delay
    }

    // Check if interrupts occurred on pins 0 and 4 of Port F
    if (interruptOccurredF0 && interruptOccurredF4) {
        GPIOPinWrite(GPIO_PORTF_BASE, TEST_RESULT_PASS_PIN, TEST_RESULT_PASS_PIN);  // Pass LED on
    } else {
        GPIOPinWrite(GPIO_PORTF_BASE, TEST_RESULT_FAIL_PIN, TEST_RESULT_FAIL_PIN);  // Fail LED on
    }

    // Cleanup
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
    GPIOIntUnregister(GPIO_PORTF_BASE);
    IntMasterDisable();
}
