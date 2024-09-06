/*-------------------------------------------------------
*		FILE DESCRIPTION
*--------------------------------------------------------
*				File: NVIC.h
*				Description: header file for NVIC register 
*
*-------------------------------------------------------*/

#include "Std_Types.h"

#define NVIC_BASE 				0xE000E100UL

#define NVIC						((NVIC_Type   *) NVIC_BASE      )

typedef struct{
			
	volatile uint32 ISER[5U];   
	volatile uint32 RESRVED[27U];
  volatile uint32 ICER[5U] ;	
	
} NVIC_Type;


void NVIC_EnableIRQ(uint8 IRQn);	
void NVIC_DisableIRQ(uint8 IRQn);