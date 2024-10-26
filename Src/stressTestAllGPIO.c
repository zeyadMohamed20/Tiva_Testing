#include <stdint.h>
#include <stdbool.h>
#include "tivaware/inc/tm4c123gh6pm.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/gpio.h"

// Define the test result indicator pin (e.g., PF2)
#define TEST_RESULT_PASS_PIN GPIO_PIN_3

// Define the test result indicator pin (e.g., PF1)
#define TEST_RESULT_FAIL_PIN GPIO_PIN_1

void stressTestAllGPIO(void)
{
    // Enable GPIO Ports A to F and Port F for the test result indicator
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure GPIO pins in all ports as output
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0xFF);  // All 8 pins of Port A
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);  // All 8 pins of Port B
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, 0xFF);  // All 8 pins of Port C
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, 0xFF);  // All 8 pins of Port D
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, 0x3F);  // 6 pins of Port E
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x1F);  // 5 pins of Port F

    // Configure the TEST_RESULT_PIN on Port F as an output
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, TEST_RESULT_PASS_PIN);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, TEST_RESULT_FAIL_PIN);

    // Flag to track test result
    bool testPassed = true;

    // Toggle all pins across all ports to high
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0xFF);
    GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0xFF);
    GPIOPinWrite(GPIO_PORTC_BASE, 0xFF, 0xFF);
    GPIOPinWrite(GPIO_PORTD_BASE, 0xFF, 0xFF);
    GPIOPinWrite(GPIO_PORTE_BASE, 0x3F, 0x3F);
    GPIOPinWrite(GPIO_PORTF_BASE, 0x1F, 0x1F);

    // Small delay
    SysCtlDelay((SysCtlClockGet() / 3000) * 500);

    // Check if all pins are set to high
    if ((GPIOPinRead(GPIO_PORTA_BASE, 0xFF) != 0xFF) ||
        (GPIOPinRead(GPIO_PORTB_BASE, 0xFF) != 0xFF) ||
        (GPIOPinRead(GPIO_PORTC_BASE, 0xFF) != 0xFF) ||
        (GPIOPinRead(GPIO_PORTD_BASE, 0xFF) != 0xFF) ||
        (GPIOPinRead(GPIO_PORTE_BASE, 0x3F) != 0x3F) ||
        (GPIOPinRead(GPIO_PORTF_BASE, 0x1F) != 0x1F))
    {
        testPassed = false;
    }

    // Toggle all pins across all ports to low
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_PORTC_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, 0x3F, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, 0x1F, 0x00);

    // Small delay
    SysCtlDelay((SysCtlClockGet() / 3000) * 500);

    // Check if all pins are set to low
    if ((GPIOPinRead(GPIO_PORTA_BASE, 0xFF) != 0x00) ||
        (GPIOPinRead(GPIO_PORTB_BASE, 0xFF) != 0x00) ||
        (GPIOPinRead(GPIO_PORTC_BASE, 0xFF) != 0x00) ||
        (GPIOPinRead(GPIO_PORTD_BASE, 0xFF) != 0x00) ||
        (GPIOPinRead(GPIO_PORTE_BASE, 0x3F) != 0x00) ||
        (GPIOPinRead(GPIO_PORTF_BASE, 0x1F) != 0x00))
    {
        testPassed = false;
    }

    // Set TEST_RESULT_PIN based on test result
    if (testPassed)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, TEST_RESULT_PASS_PIN, 0xFF);  // Pass: Set pin high
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, TEST_RESULT_FAIL_PIN, 0xFF);  // Fail: Set pin low
    }
}
