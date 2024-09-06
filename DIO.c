/************************************************* 
*  FILE  DESCRIPTION
*------------------------------------------------
*     File: DIO.c
*     Description: Contains functions of GPIO 
*
****************************************************/

/*----------------INCLUDES----------------------*/
#include "DIO.h"
#include "DIO_Mcu_HW.h"
#include "NVIC.h"
/*----------------------------------------------*/


void Pin_Init(uint8 Pin,  Port_Config *configPtr){
//	uint8 x = 0;//counter
	uint8 port;
	
	/*Bit banding for all used registers*/
	
	  uint32 PORT_BASE = GPIO_BASE_APB(configPtr->Port_Value);
 /*
	switch(configPtr->Port_Value){
		case GPIO_PORT_A_BASE: port = 0;
													 break;
	  case GPIO_PORT_B_BASE: port = 1;
													 break;
		case GPIO_PORT_C_BASE: port = 2;
													 break;
		case GPIO_PORT_D_BASE: port = 3;
													 break;
		case GPIO_PORT_E_BASE: port = 4;
													 break;
		case GPIO_PORT_F_BASE: port = 5;
													 break;
	}*/
	uint32 *GPIO_RCGCGPIO_PIN = BITBAND_PERI(SYSTEM_CTL_REG_BASE + RCGCGPIO_OFFESET   , configPtr->Port_Value );
	uint32 *GPIO_DATA_PIN		  = BITBAND_PERI(   PORT_BASE + GPIO_DATA_OFFSET , Pin);
	uint32 *GPIO_DIR_PIN 		  = BITBAND_PERI(   PORT_BASE + GPIO_DIR_OFFSET  , Pin);
	uint32 *GPIO_FSEL_PIN 	  = BITBAND_PERI(   PORT_BASE + GPIO_FSEL_OFFSET , Pin);
	uint32 *GPIO_DEN_PIN      = BITBAND_PERI(   PORT_BASE + GPIO_DEN_OFFSET  , Pin);
	uint32 *GPIO_PUR_PIN      = BITBAND_PERI(   PORT_BASE + GPIO_PUR_OFFEST  , Pin);
	uint32 *GPIO_PDR_PIN      = BITBAND_PERI(   PORT_BASE + GPIO_PDR_OFFSET  , Pin);
	uint32 *GPIO_R2R_PIN 		  =	BITBAND_PERI(   PORT_BASE + GPIO_DR2R_OFFEST , Pin);
	uint32 *GPIO_R4R_PIN  		= BITBAND_PERI(   PORT_BASE + GPIO_DR4R_OFFEST , Pin);
	uint32 *GPIO_R8R_PIN  		= BITBAND_PERI(   PORT_BASE + GPIO_DR8R_OFFEST , Pin);
	uint32 *GPIO_ODR_PIN  		= BITBAND_PERI(   PORT_BASE + GPIO_ODR_OFFEST  , Pin);

 /*************************************/	
	
	/*activate clock for this Pin*/
	*GPIO_RCGCGPIO_PIN = STD_ACTIVE ;
	//for( x=0 ;x<1;x++) // i don't need this loop any more it's working fine 
	//{ // waith for 4 clock cycles  ||ask about this ??||
	//}
/*-----------------------------------------------------------------*/		
	/*configure the Pin as GPIO
	* This statment is converted i.e. 
	* activation of GPIO with 0 put to get into the if use 1
	*/
	if( configPtr->Port_PinModeType == STD_HIGH) 
			*GPIO_FSEL_PIN = STD_LOW;  
/*-----------------------------------------------------------------*/	
	/*configure i/p --> 0 or o/p --> 1*/
	*GPIO_DIR_PIN = configPtr->Port_PinDirectionType; 
/*-----------------------------------------------------------------*/		
	/* 1 --> pull up      0 --> pull down*/
	if     ( configPtr->Port_PinInternalAttachType == PULL_UP_RES  )
		*GPIO_PUR_PIN = ENABLE;	
  else if( configPtr->Port_PinInternalAttachType == PULL_DOWN_RES)
	  *GPIO_PDR_PIN = ENABLE;	
/*-----------------------------------------------------------------*/		
	/*drive current choices*/
	switch (configPtr->Port_PinOutputCurrentType){
			
		case DRIVE_CURRENT_2M : *GPIO_R2R_PIN = STD_HIGH ; break ;
	  case DRIVE_CURRENT_4M : *GPIO_R4R_PIN = STD_HIGH ; break ;
		case DRIVE_CURRENT_8M : *GPIO_R8R_PIN = STD_HIGH ; break ; 
	}
/*-----------------------------------------------------------------*/		
	/*Open Drain configure 1 --> Open Drain  */
	
	*GPIO_ODR_PIN = configPtr->Port_PinType ;
/*-----------------------------------------------------------------*/		
	/*enable Pin*/
	
	*GPIO_DEN_PIN = ENABLE ;
/*-----------------------------------------------------------------*/		
	/*initial Value */
	
	*GPIO_DATA_PIN = configPtr->Port_PinInitialValue ;
	
}


void Write_Pin(uint32 Port_ID, uint8 Pin_ID,uint8 uint8_Value){
	
	uint32 *GPIO_DATA_PIN		  = BITBAND_PERI(Port_ID + GPIO_DATA_OFFSET , Pin_ID);
	
	*GPIO_DATA_PIN = uint8_Value;
}

uint8 Pin_Read(uint32 Port_ID, uint8 Pin_ID){

	uint32 *GPIO_DATA_PIN		  = BITBAND_PERI(Port_ID + GPIO_DATA_OFFSET , Pin_ID);

	return (*GPIO_DATA_PIN);

}

void Pin_Flip(uint32 Port_ID, uint8 Pin_ID){
	/*****This function in non-reentrant !!!  *****/
	
	uint32 *GPIO_DATA_PIN		  = BITBAND_PERI(GPIO_BASE_APB(Port_ID) + GPIO_DATA_OFFSET , Pin_ID);
	*GPIO_DATA_PIN	= ~(*GPIO_DATA_PIN);
}

void Port_Init(Port_Config *configPtr){
	
		RCGCGPIO = 0xFF;
		//*again Function select is active love to GPIO function but here i did it respond to high*/
	 if( (configPtr->Port_PinModeType) == STD_HIGH) 	GPIOAFSEL(configPtr->Port_Value) = 0x00 ;  
	 else 																						GPIOAFSEL(configPtr->Port_Value) = 0xFF ;
	
	 if( (configPtr->Port_PinDirectionType) == STD_HIGH)  GPIODIR(configPtr->Port_Value) = 0xFF ;
	 else 																								GPIODIR(configPtr->Port_Value) = 0x00;
	  
	 if( (configPtr->Port_PinInternalAttachType) == STD_HIGH) GPIOPUR(configPtr->Port_Value) = 0xFF ;
	 else 																										GPIOPDR(configPtr->Port_Value) = 0xFF ;
	
	 switch(configPtr->Port_PinOutputCurrentType){
			
	  	case DRIVE_CURRENT_2M : GPIODR2R(configPtr->Port_Value) = 0xFF ; break ;
	    case DRIVE_CURRENT_4M : GPIODR4R(configPtr->Port_Value) = 0xFF ; break ;
		  case DRIVE_CURRENT_8M : GPIODR8R(configPtr->Port_Value) = 0xFF ; break ; 
		 
	 }
	
	 if( (configPtr->Port_PinType) == STD_HIGH)  GPIOODR(configPtr->Port_Value) = 0xFF ;
	 else  																			 GPIOODR(configPtr->Port_Value) = 0x00 ;
	 
		GPIODEN(configPtr->Port_Value)	= 0xFF;
	 
	  GPIODATA(configPtr->Port_Value) = configPtr->Port_PinInitialValue ;
	
}

void Port_Write(uint32 Port_Number, uint8 Value ){
	
		GPIODATA(Port_Number)  = Value ;

}

uint8 Port_Read(uint32 Port_Number){
	
		return GPIODATA(Port_Number);

}

/*---------------------------------------------------------------------------------* 
*												           	CALL BACK PART
*----------------------------------------------------------------------------------*/
 
 /*ponter to function callback for Port F*/
void (*INT_F_FunctionPtr)(void) = NULL;
/**********************************************************************************************
***********************************************************************************************/
void GPIO_INTF_Init(const Pin_INT_Config* ConfigPtr){
	
	
/*--------Choosing Sense edge --> 0 , Level --> 1---------*/
		if(ConfigPtr->Sense == EDGE ){
			CLR_BIT(GPIOIS(ConfigPtr->Port), ConfigPtr->Pin);	
		}
		else{
			SET_BIT(GPIOIS(ConfigPtr->Port), ConfigPtr->Pin);
		}	
/*------------------END Sense--------------------------------*/		
			
/*--------------Choose if Both edges Triger interrupt--------*/
		if(ConfigPtr->Both_edges == ONLY_ONE){
			CLR_BIT(GPIOIBE(ConfigPtr->Port), ConfigPtr->Pin);
		}
		else{
			SET_BIT(GPIOIBE(ConfigPtr->Port), ConfigPtr->Pin);
		}
/*-----------------------END Both Edges----------------------*/		
	
/*----Choosing rising (HIGH level) or Falling(LOW level)-----*/
	
		if(ConfigPtr->Event == Rising_HIGH){
			SET_BIT(GPIOIEV(ConfigPtr->Port), ConfigPtr->Pin);
		}
		else{
			CLR_BIT(GPIOIEV(ConfigPtr->Port), ConfigPtr->Pin);
		}
/*----------------------END Choose Event---------------------*/		
		
		
/*-------------------Enable IRQ from NVIX--------------------*/		
	 
		if(ConfigPtr->Port < 5){
			/* we used the port number for ports (A _E) because their number in NVIC is 
			 * the same as the numbers we gave them in the enum definition in header file */
			NVIC_EnableIRQ(ConfigPtr->Port); 
		}
		else{
			/* Here w only have Port F left with NVIC number "30" */
			NVIC_EnableIRQ(30);
		}
/*----------------------END IRQ Enable-----------------------*/		
		
		
/*---------------------Enable Interrupt----------------------*/		
		
		SET_BIT( GPIOIM(ConfigPtr->Port), ConfigPtr->Pin );
}
/**********************************************************************************************
***********************************************************************************************/
void Inerrupt_CallBack( void (* Ptr_Function)(void) ){
	
	if(Ptr_Function != NULL ){
	
			INT_F_FunctionPtr = Ptr_Function;
	}
}
/**********************************************************************************************
***********************************************************************************************/
void GPIOF_Handler(void){
	/*This definiton just till i make th headr file containing all rgisters of GPIO*/
	#define GPIOICR_F   *( (volatile uint32 *)(0x40025000+0x41C))

	if( INT_F_FunctionPtr != NULL ){
		
		INT_F_FunctionPtr();
		GPIOICR_F |= ( 1 << 0 ) ;
	}

}