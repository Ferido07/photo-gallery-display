/********************************************************************************************************
- FB-EDU-PARM-LPC2138 Configuration file
-
- Author: Kruck Wang(Wang RongHua)
-
- Date: 2009-04-23
-
- Guangzhou Windway Electronic Technology Co., Ltd.
********************************************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//type define
typedef unsigned char  uint8;         
typedef signed   char  int8;            
typedef unsigned short uint16;          
typedef signed   short int16;                 
typedef unsigned int   uint32;                
typedef signed   int   int32;               
typedef float          fp32;                   
typedef double         fp64;                    


/*************************************/
/*      LPC2138 Register definition  */
/*************************************/

#include    <lpc214x.h>

/*********************************************/
/* System setup, Fosc¡¢Fcclk¡¢Fcco¡¢Fpclk    */	
/*********************************************/
#define Fosc            5000000                    //Osc freq,10MHz~25MHz, should be the same with the target board
#define Fcclk           (Fosc * 2)                  //System freq, must <=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO freq£¬valid value must between 156MHz and 320MHz
#define Fpclk           (Fcclk / 4) * 2             //VPB freq

#include    "target.h"

#include    <stdio.h>

#endif
