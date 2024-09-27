#include <stdint.h>
#include "tivaware/inc/tm4c123gh6pm.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/gpio.h"

int main(void)
{
    // Set system clock to 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    simultaneousReadWriteGPIO();

    while(1)
    {
    }
}

void simultaneousReadWriteGPIO()
{
    // Enable necessary GPIO ports and configure for input/output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0xFF);  // Port A for output
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, 0xFF);   // Port B for input
    GPIOPadConfigSet(GPIO_PORTB_BASE, 0xFF, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // Enable pull-up
    
    while (1)
    {
        // Write to output pins
        GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0xFF);  // Set high
        SysCtlDelay(50000);  // Small delay
        GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);  // Set low
        
        // Read from input pins
        uint8_t inputVal = GPIOPinRead(GPIO_PORTB_BASE, 0xFF);
        // Log or check the read value
    }
}
