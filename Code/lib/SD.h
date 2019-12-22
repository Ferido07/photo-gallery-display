#ifndef _SD_H_
#define _SD_H_

#include    <lpc214x.h>

//Macros                                                         
#define SD_CS_ASSERT     (IO0CLR |= 0x800)
#define SD_CS_DEASSERT   (IO0SET |= 0x800)
#define FAST_SPI         (S0SPCCR -= 120)      //fast SPI speed (6Mbps)  

//SD/MMC standard commands
#define GO_IDLE_STATE            0
#define SEND_OP_COND             1
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38

//function prototypes
char SD_init(void);
char SD_sendCommand(char cmd, unsigned long arg);
char SD_readSingleBlock(unsigned long startBlock,char *buffer);
char SD_writeSingleBlock(unsigned long startBlock,char *buffer);

#endif
