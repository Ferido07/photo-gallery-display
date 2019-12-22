/* ==================================================================================================================
                              A driver for the SPI module
	 
       The Serial Peripheral Interface (SPI) is a synchronous serial communication interface. The communication is 
       Carried out by using Four lines (serial data out (SDO), serial data in(SDI), serial clock (SCK) and 
       Slave select (SSEL)). These signals are also termed as:
                SDO = Master out slave in (MOSI)---------- data line from master to slave
                SDI = Master In Slave Out (MOSI)---------- data line from slave to master
                SCK = Serial clock              ---------- Always from master to slave
                SSEL = slave select or cheap select (CS) --- Always from master to slave
   
       SPI is also known as Motorola bus. For more details, please refer to the SPI specification 

       please refer to chapter-12 on the manual
     		
    $$ ECEG-4501 Microcomputers & Interfacing, Lab exercise.  Prepared by: Daniel D. @ ECED of AAiT $$
=================================================================================================================== */
#include    <lpc214x.h>

/*---------------------------------------------------------------------------
  Function: SPI_init

  Parameters: None

  Returns:   None

  Description: Initializes the SPI module as a master on the bus  
----------------------------------------------------------------------------*/
unsigned int SPI_init(void)
{
  unsigned int status;
  
  PINSEL0|= 1<<8|  1<<10|  1<<12;     //SPI SCK function selected on pin
 // PINSEL0| = 1<<10;    //SPI MISO function selected on pin
  //PINSEL0| = 1<<12;    //SPI MOSI function selected on pin
  
//  IO0DIR| = 1<<14;      //make SSEL output
	IO0DIR |= 1<<7; 
  IO0DIR|= 1<<11;      //make SSEL output
  IO0SET|= 1<<11;      //and drive it high (disable slave)
  
  S0SPCR = 0x20;           //Master mode, clock active-high, data 8-bit MSB first, no interrupt
  
  status = S0SPSR;         //reading the status register will clear it
  
  //S0SPCCR = 8;            //SPI_CLK = PCLK/8 = 48M/8 = 6MHz (i.e SPI data rate = 6Mbps)
  
  S0SPCCR = 128;            //SPI_CLK = PCLK/128 = 48M/128 = 375KHz (for compatibility with MMC initialization)
  
  return status;            //dummy
  
} 

/*---------------------------------------------------------------------------------
    Function: SPI_write

    Purpose: Sends a data byte on the SPI bus as a master.

    Parameters:  the data byte to be transmitted
-------------------------------------------------------------------------------*/
char SPI_write(char data) 
{       
    S0SPDR = data;                  //start transfer
    while((S0SPSR & 1<<7) == 0);    //wait for transfer complete
       
    return S0SPDR;                  //dummy: read the data register to clear the status flag
} 

/*---------------------------------------------------------------------------------
    Function: SPI_read

    Purpose: Reads a data byte from the SPI bus. 

    Parameters: None

    Returns: The data byte read from the SPI bus    
-------------------------------------------------------------------------------*/
char SPI_read(void) 
{   
    S0SPDR = 0xff;                  //dummy write
    while((S0SPSR & 1<<7)== 0);    //wait for data to arrive
       
    return S0SPDR;                  //read the data register to clear the status flag
} 

