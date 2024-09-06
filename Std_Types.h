/**************************************************
 *	FILE DESCRIPTION
 * ------------------------------------------------
 *		File: Std_Types.h
 *
 *
 *
 *		Description: contains Standard Types
 *
 *
 **************************************************/
 #ifndef STD_TYPES_H
 #define STD_TYPES_H
 /*************************************************
 * INCLUDES
 **************************************************/
 #include "Platform_Types.h"
 /*************************************************
 * GLOBAL MACROS
 *************************************************/
 #define NULL (void*)0
 
 
 #define TRUE   			1
 #define FALSE  			0
 
 #define ENABLE 			1
 #define DISABLE 			0
 
 #define STD_HIGH   	1
 #define STD_LOW			0
 
 #define STD_ACTIVE	 	1
 #define STD_IDLE			0
 
 #define STD_ON				1
 #define STD_OFF	   	0
 
 /*************BIT MATH***********************/
#define SET_BIT(Reg,Bit)		( (Reg) |=  ( (1) << (Bit) ) )
#define CLR_BIT(Reg,Bit)		( (Reg) &= ~( (1) << (Bit) ) )
#define TGL_BIT(Reg,Bit)		( (Reg) ^=  ( (1) << (Bit) ) )
#define GET_BIT(Reg,Bit)		( (Reg) >>   (Bit)&( 1 ) )
/******************END BIT MATH **************/ 
 
#endif /*STD_TYPES_H*/ 