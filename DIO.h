
#ifndef DIO_H
#define DIO_H

#include "Std_Types.h"
#include "DIO_Mcu_HW.h"

typedef struct{
 
	 uint32 Port_Value; // which port to choose
	 uint8  Port_PinModeType;// GPIO or not
	 uint8  Port_PinDirectionType; //
	  
	 uint8  Port_PinInternalAttachType;//
	 uint8  Port_PinOutputCurrentType;//
	 uint8  Port_PinType;//Open drain
	 uint8  Port_PinInitialValue;//
	
	 // uint8 Port_ConfigType; what is this in the video
	
} Port_Config;


typedef struct{
					
				uint8 Port			;
				uint8 Pin       ;
				uint8 Sense			;     /*Level or Edge Triggered*/
				uint8 Both_edges; 		/*If Both edges triger or only choosen one*/
				uint8 Event     ; 		/* Rising(High level) ot Falling(Low level) */

}Pin_INT_Config;
//Port_Config Pins[8] ;// Port_Config struct for every Pin in the port
//Port_Config *PinsPtr = Pins;

/*user interactions MACROS*/

#define PIN_MODE_GPIO          1
#define PIN_MODE_OTHER				 0

#define PIN_OUTPUT						 1
#define PIN_INPUT							 0

#define PULL_UP_RES            1
#define PULL_DOWN_RES          0

#define DRIVE_CURRENT_2M 			0
#define DRIVE_CURRENT_4M			1
#define DRIVE_CURRENT_8M			2

#define OPEN_DRAIN_ACTIVE     1
#define OPEN_DRAIN_INACTIVE   0

enum PINS { PIN0 = 0 ,PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7};

enum PORTS {  PORT_A = 0, PORT_B, PORT_C, PORT_D, PORT_E, PORT_F };

enum INT_Sense { EDGE = 0, LEVEL = 1};

enum INT_Both_Edges { ONLY_ONE = 0 , BOTH_EDGES_TRIGER = 1};

enum INT_Event {Falling_LOW = 0 , Rising_HIGH = 1 };

void Pin_Init(uint8 Pin,  Port_Config *configPtr);
void Port_Init(  Port_Config *configPtr);
void Port_Write(uint32 Port_Number, uint8 Value );
uint8 Port_Read(uint32 Port_Number);
void Write_Pin(uint32 Port_ID, uint8 Pin_ID,uint8 uint8_Value);
uint8 Pin_Read(uint32 Port_ID, uint8 Pin_ID);
void Pin_Flip(uint32 Port_ID, uint8 Pin_ID);

/**** interrupt Functions **/
void GPIO_INTF_Init(const Pin_INT_Config* ConfigPtr);
					
/*callback Part*/
//void GPIOF_Handler(void);
void Inerrupt_CallBack( void (* Ptr_Function)(void) );
					
//void Port_Init( Port_Config *configPtr);
#endif