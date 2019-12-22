/*==========================================================================================
 **** Routines for raw data transfer b/n MCU and MMC/SD cards through SPI bus ****
      This is A driver for MMC/SD cards using any MCU (i.e these functions are platform independant)
            
SOURCE FILE : SD.c

Controller: Any. (tested on ARM7/9/Cortex_M/Cortex_A, Microchip PIC, Atmel AVR)

Compiler  : IAR EWB V. 5.4 or higher / also GCC for ARM can be used. use CCS PCHW for PIC, AVR studio for AVR
Version   : 3.1
Developer : Daniel D.      
Date      : March 2009

Note: 1. The native mode for SD/MMC is SD bus protocol. SPI is secondary and is inferior to SD mode
      2. See SD.h for available commands
      3. See the SanDisk SD card user manual in "../datasheets"
===========================================================================================*/

#include "lib/SD.h"
#include "lib/spi.h"

/*------------------------------------------------------------------
Function : to initialize the SD card in SPI mode

Returns  : 0 = success; 1 = ERROR

Arguments: None
-------------------------------------------------------------------*/
char SD_init(void)
{
  char j,response;
  unsigned int i; 
  
  for(j=0;j<10;j++)            //at least 74 clocks to take card in to SPI mode
     SPI_write(0xff);
     
  i=0;
     
  do{
      response = SD_sendCommand(GO_IDLE_STATE, 0); //send 'reset & go idle' command untill card becomes ready
    }while(response!=0x01 && i++ < 0xfffe);
    
  if(i > 0xfffe) return 1;     //loop was exited due to timeout, card is not responding (too bad)
  
  SD_CS_DEASSERT;              //deselect card
  SPI_write(0xff);
  SPI_write(0xff);

  i = 0;
  
  do
  {
    response = SD_sendCommand(SEND_OP_COND, 0); //activate card's initialization process
    response = SD_sendCommand(SEND_OP_COND, 0); //same command sent again for compatibility with some cards 
  }while(response!=0x00 && i++ < 0xfffe);
    
  if(i > 0xfffe) return 1;                      // loop was exited due to timeout (not again!)

  SD_sendCommand(SET_BLOCK_LEN, 512); //set block size to 512
  
  SD_CS_DEASSERT;                     //deselect card
          
  return 0;                          //Yay! Successfully initialized
}

/*------------------------------------------------------------------
Function :  To send a standard command to SD/MMC 

Returns  : Card response

Arguments: Command to transmit & command arguments

Note:      See available commands in "SD.h"
-------------------------------------------------------------------*/
char SD_sendCommand(char cmd, unsigned long arg)
{
  char response; 
  unsigned long retry = 0;

  SD_CS_ASSERT;             //select card (activate bus)

  SPI_write(cmd | 0x40);    //send command, first two bits always '01'
  SPI_write(arg>>24);       //send command argument (32 bit) a byte at a time starting from MSB
  SPI_write(arg>>16);
  SPI_write(arg>>8);
  SPI_write(arg);
  SPI_write(0x95);          //pre-calculated checksum(see SD Card manual)
  
  while(((response = SPI_read()) == 0xff) && (retry++ < 0x0fffffff));  //wait response
  
  SPI_read();                                                          //extra 8 CLK
  
  return response;                                                     //return card response
}

/*---------------------------------------------------------------------------------
Function : to read a single block from SD/MMC (512 Bytes)

Returns  : 0 for success, 1 for error

Arguments: Address on the card from where data is to be read, and buffer where read data will be stored

Note     : Card access is Block-wise and each block has 512 Bytes (just like a Hard disk)
-----------------------------------------------------------------------------------*/
char SD_readSingleBlock(unsigned long startBlock, char *buffer)
{
  char  response;
  unsigned long i, retry = 0;

  response = SD_sendCommand(READ_SINGLE_BLOCK, startBlock*512); //read a Block command
  
  if(response != 0x00)            //check for SD status: 0x00 - OK (No flags set)
     return response;             //at least one Error flag is up, return error

  retry = 0;
  
  while((SPI_read() != 0xfe) && retry++ < 0x00ffffff);       //wait for start block token 
  if(retry > 0x00ffffff) { SD_CS_DEASSERT; return 1;}        //return error if time-out
    
  for(i=0; i<512; i++)           //read 512 bytes in to buffer
    buffer[i] = SPI_read();
  
  SPI_read();                    //receive incoming CRC (16-bit), evenif CRC is ignored
  SPI_read();

  SPI_read();                    //extra 8 clock pulses
  SD_CS_DEASSERT;                //deselect card
  return 0;                      //512 bytes read successfully
}

/*----------------------------------------------------------------------
Function : to write a single block of data to SD/MMC

Returns  : 0 for success, 1 for error

Arguments: block address on the card where data is to be written and pointer to the write buffer

Note     : Card access is Block-wise and each block has 512 Bytes (just like Hard disk)
----------------------------------------------------------------------*/
char SD_writeSingleBlock(unsigned long startBlock, char *buffer)
{
  char response;
  unsigned long i, retry=0;

  response = SD_sendCommand(WRITE_SINGLE_BLOCK, startBlock*512); //write Block command
  if(response != 0x00)                          //check for SD status: 0x00 - OK (No flags set)
     return response;                           //at least one Error flag is up, return error
  
  SPI_write(0xfe);           //Send start block token 0xfe (0x11111110)

  for(i=0; i<512; i++)       //send 512 bytes data
    SPI_write(buffer[i]);
  
  SPI_write(0xff);           //transmit dummy CRC (16-bit), evenif CRC is ignored in SPI mode
  SPI_write(0xff);
  
  response = SPI_read();     //read card response
  
  if((response & 0x1f) != 0x05)    //response= XXX0AAA1 ; AAA='010' - data accepted
  {                                //AAA='101'-data rejected due to CRC error
    SD_CS_DEASSERT;                //AAA='110'-data rejected due to write error
    return response;
  }

  while((!SPI_read()) && (retry++ < 0x0fffffff));  //wait for SD card to complete writing and get idle
  if(retry>0x0fffffff){SD_CS_DEASSERT; return 1;}

  SD_CS_DEASSERT;
  SPI_write(0xff);     //just spend 8 clock cycle delay before reasserting the CS line
  SD_CS_ASSERT;       //re-asserting the CS line to verify if card is still busy

  while((!SPI_read()) && (retry++ < 0x00ffffff ));  //wait for SD card to complete writing and get idle
  if(retry > 0x00ffffff){SD_CS_DEASSERT; return 1;}

  SD_CS_DEASSERT;

  return 0;         //512 bytes successfully written on card 
}
