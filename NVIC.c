/*-------------------------------------------------------
*		FILE DESCRIPTION
*--------------------------------------------------------
*				File: NVIC.c
*				Description: NVIC Functionality 
*
*-------------------------------------------------------*/


#include "NVIC.h"




void NVIC_EnableIRQ(uint8 IRQn){
		NVIC->ISER[IRQn/32] = (1UL << (IRQn));
}

void NVIC_DisableIRQ(uint8 IRQn){
		NVIC->ICER[IRQn/32] = (1UL << (IRQn));
}