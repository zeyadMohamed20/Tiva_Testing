#include <stdint.h>
#include "tivaware/inc/tm4c123gh6pm.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/gpio.h"


int main(void)
{
    // Set system clock to 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    stressTestAllGPIO();

    while(1)
    {
    }
}

void stressTestAllGPIO()
{
    // Set system clock to 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    
    // Enable GPIO Ports A to F
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

    while (1)
    {
        // Toggle all pins across all ports
        GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0xFF);  // Set high
        GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0xFF);  
        GPIOPinWrite(GPIO_PORTC_BASE, 0xFF, 0xFF);  
        GPIOPinWrite(GPIO_PORTD_BASE, 0xFF, 0xFF);  
        GPIOPinWrite(GPIO_PORTE_BASE, 0x3F, 0x3F);  
        GPIOPinWrite(GPIO_PORTF_BASE, 0x1F, 0x1F);  

        GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);  // Set low
        GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
        GPIOPinWrite(GPIO_PORTC_BASE, 0xFF, 0x00);
        GPIOPinWrite(GPIO_PORTD_BASE, 0xFF, 0x00);
        GPIOPinWrite(GPIO_PORTE_BASE, 0x3F, 0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x1F, 0x00);
    }
}
