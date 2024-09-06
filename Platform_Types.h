/**************************************************
 *	FILE DESCRIPTION
 * ------------------------------------------------
 *		File: Platform_Types.h
 *
 *
 *
 *		Description: Contains types that dependent on platform - CortexM4
 *
 **************************************************/
 
#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/**************************************************
* GLOBAL DATA TYPED AND STRUCTURES
**************************************************/

typedef	unsigned  char 				uint8 ;
typedef signed    char				sint8 ;

typedef unsigned short int	  uint16;
typedef signed   short int 		sint16;

typedef unsigned long  int 		uint32;
typedef signed   long  int 		sint32;

typedef float 								float32;
typedef double 							  float64;

#ifdef PLATFORM_SUPPORT_SINT64_UNIT64 /*Valid only for gnu and c99*/
/*this section has no configuration yet*/
typedef signed   long long   sint64; 
typedef unsigned long long   uint64;
#endif


/***********************************
*END OF FILE :Platform.h
**********************************/

#endif /* PLATFORM_TYPES_H */