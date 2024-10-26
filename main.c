#include <stdint.h>
#include <stdbool.h>
#include <stressTestGPIOInterrupts.h>
#include "tivaware/inc/tm4c123gh6pm.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/driverlib/uart.h"
#include "tivaware/driverlib/pin_map.h"
#include "Inc/stressTestAllGPIO.h"
#include "Inc/stressTestGPIOInterrupts.h"

int main(void)
{
    // Set up system clock to 40 MHz using PLL
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Run your GPIO test
    stressTestAllGPIO();

    // Infinite loop
    while(1);
}


