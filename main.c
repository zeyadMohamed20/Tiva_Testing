#include "tivaware/driverlib/sysctl.h"
#include "Inc/stressTestAllGPIO.h"
#include "Inc/simultaneousReadWriteGPIO.h"
#include "Inc/interruptStressTestGPIO.h"

int main(void)
{
    // Set system clock to 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    stressTestAllGPIO();

    while(1)
    {
    }
}
