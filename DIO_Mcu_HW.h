/**************************************************************************
*   FILE DESCRIPTION
*--------------------------------------------------------------------------
*		    File: DIO_Mcu_HW.h	
*				
*
*				Description:  Contais Adresses of MCU registers and pointers to it 
*
*
****************************************************************************/

#ifndef DIO_MCU_HW_H
#define DIO_MCU_HW_H

/****************************************************************************
* INCLUDES
******************************************************************************/
#include "Std_types.h"

/*****************************************************************************
* GLOBAL CONSTANT MACROS
*******************************************************************************/
/*BASE Addrsses for APB */
#define SYSTEM_CTL_REG_BASE					0x400FE000
/*this is useless now (Macro function replaced it)*/
/*
#define GPIO_PORT_A_BASE				    0x40004000
#define GPIO_PORT_B_BASE						0x40005000
#define GPIO_PORT_C_BASE				    0x40006000
#define GPIO_PORT_D_BASE						0x40007000
#define GPIO_PORT_E_BASE				    0x40024000
#define GPIO_PORT_F_BASE						0x40025000
*/
/*registers offests*/
#define RCGCGPIO_OFFESET    0x608
#define GPIO_DIR_OFFSET   	0x400
#define GPIO_FSEL_OFFSET 		0x420
#define GPIO_DATA_OFFSET		0x3FC
#define GPIO_DEN_OFFSET			0x51C
#define GPIO_PUR_OFFEST     0x510
#define GPIO_PDR_OFFSET			0x514
#define GPIO_DR2R_OFFEST		0x500
#define GPIO_DR4R_OFFEST		0x504
#define GPIO_DR8R_OFFEST		0x508
#define GPIO_ODR_OFFEST		  0x50C

#define GPIO_IS_OFFSET				0x404
#define GPIO_IBE_OFFSET				0x408
#define GPIO_IEV_OFFSET				0x40C
#define GPIO_IM_OFFSET				0x410
#define GPIO_RIS_OFFSET				0x414
#define GPIO_MIS_OFFSET				0x418
#define GPIO_ICR_OFFSET				0x41C

#define GPIO_BASE_APB(port)        ( port < 4)? ( 0x40004000+ port*0x1000 ): (0x40020000+ port*0x1000)

#define RCGCGPIO       				 *((volatile unsigned long *)(SYSTEM_CTL_REG_BASE + RCGCGPIO_OFFESET)) // Clock Eable

#define GPIODIR(port) 				*((volatile unsigned long *)(GPIO_BASE_APB(port) 	   + GPIO_DIR_OFFSET ))
#define GPIOAFSEL(port)				*((volatile unsigned long *)(GPIO_BASE_APB(port) 	   + GPIO_FSEL_OFFSET))
#define GPIODEN(port)         *((volatile unsigned long *)(GPIO_BASE_APB(port) 	   + GPIO_DEN_OFFSET ))
#define GPIODATA(port)        *((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_DATA_OFFSET))//don't forget to find solution to port masking by address i have activated all bits 
#define GPIOPUR(port)         *((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_PUR_OFFEST ))
#define GPIOPDR(port)         *((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_PDR_OFFSET ))
#define GPIODR2R(port)        *((volatile unsigned long *)(GPIO_BASE_APB(port)	 	 + GPIO_DR2R_OFFEST))
#define GPIODR4R(port)        *((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_DR4R_OFFEST))
#define GPIODR8R(port)        *((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_DR8R_OFFEST))
#define GPIOODR(port)         *((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_ODR_OFFEST ))

#define GPIOIS(port)					*((volatile unsigned long *)(GPIO_BASE_APB(port) 	   + GPIO_IS_OFFSET	 ))
#define GPIOIBE(port)					*((volatile unsigned long *)(GPIO_BASE_APB(port) 	   + GPIO_IBE_OFFSET ))
#define GPIOIEV(port)					*((volatile unsigned long *)(GPIO_BASE_APB(port) 	   + GPIO_IEV_OFFSET ))
#define GPIOIM(port)					*((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_IM_OFFSET	 ))
#define GPIORIS(port)					*((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_RIS_OFFSET ))
#define GPIOMIS(port)					*((volatile unsigned long *)(GPIO_BASE_APB(port)		 + GPIO_MIS_OFFSET ))
#define GPIOICR(port)					*((volatile unsigned long *)(GPIO_BASE_APB(port)	 	 + GPIO_ICR_OFFSET ))
                              





/*Bit Banding
*  To use it define a Pointer and assign the macro function to it
*  ex : uint32 *GPIOA_DATA_PIN1 = BITBAND_PERI(GPIOA_DATA_ADDR,0);
*  A pointer to the PIN 0 of DATA_PORTA
*/
/**Peripheral Bit Banding**/
#define BITBAND_PERI_BASE 0x40000000 // Base Address of priphral bit-band
#define ALIAS_PERI_BASE   0x42000000 // base address of pripheral alias band
/***this formula to use bit banding on priphrals***/
#define BITBAND_PERI(PERI_ADDRESS,BIT)  ((uint32 *)( ALIAS_PERI_BASE + (((uint32)PERI_ADDRESS - BITBAND_PERI_BASE)*32) +(BIT*4)) )
/**SRAM Bit Banding**/
#define BITBAND_SRAM_BASE  0x20000000	//Base Address of SRAM bit_band
#define ALIAS_SRAM_BASE 	 0x22000000 //Base Address of SRAM alias band
/***this formula to use bit banding on SRAM***/
#define BITBAND_SRAM(SRAM_ADDRESS,BIT)	((uint32 *)( ALIAS_SRAM_BASE + (((uint32)SRAM_ADDRESS - BITBAND_SRAM_BASE)*32) + (BIT*4)) ) 



#endif /*DIO_MCU_HW_H*/