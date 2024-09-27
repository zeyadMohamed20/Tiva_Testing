#include <stdint.h>
#include "tivaware/inc/tm4c123gh6pm.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/gpio.h"

int main(void)
{
    // Set system clock to 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    interruptStressTestGPIO();

    while(1)
    {
    }
}

void GPIOInterruptHandler()
{
    // Clear interrupt flag
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    
    // Handle interrupt event (e.g., increment counter)
}

void interruptStressTestGPIO()
{
    // Enable and configure interrupts for GPIOF pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    
    IntEnable(INT_GPIOF);  // Enable processor interrupts
    IntMasterEnable();     // Enable global interrupts
    
    while (1)
    {
        // Toggle pins to trigger interrupts
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
        SysCtlDelay(50000);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
    }
}
