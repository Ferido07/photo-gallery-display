#include "lib/Config.h"
#include "lib/Target.h"
#include "lib/T6963C.h"
#include "lib/images.h"
#include "lib/spi.h"
#include "lib/SD.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

char buffer1[512];	 
char buffer2[512]; 
char buffer3[512];
char buffer4[512];
unsigned char imageFromSD[2048]; 
uint8 NumberOfImages = 0;

void displayPic(int i);
void write_card(void);
void read_card(int);


 
int main (){ 
	 
	 int imageNo = 0;
	 CCR = 1; // Use external clock

	 PINSEL0 = 0x00000000;// All GPIO
	 PINSEL1 = 0x00000000;// All GPIO
	 IO0DIR =	0xffffffff; 
	 IO1DIR |=	1<<16; // Output for next LED
	 IO1DIR |=	1<<19; // Output for previous LED
	 IO1DIR |=	1<<22; // Output for slideshow LED 
	 
	 // Initialization
	 SPI_init();
	 SD_init();
	 init_lcd ();
    
	 //Only needed for first time storage of pics. for first run.
	 //write_card();
	 NumberOfImages = 5;
	
	 //Display hello world image which is never repeated again
   disp_img(0,16,128,Hello);

	 while(1){
			// show next 
			if(!(IO1PIN & 1<<17)){	
				//Turn on LED
				IO1SET |= 1<<16;
				
				imageNo++;
				if(imageNo > NumberOfImages){
						 imageNo = 1;
				}
				displayPic(imageNo);	
			
				//Turn off LED
				IO1CLR |= 1<<16;
			}
			//show previous
			if(!(IO1PIN & 1<<18)){
				 //Turn on LED
				 IO1SET |= 1<<19;
				
				 imageNo--;
					if(imageNo < 1){
						imageNo = NumberOfImages;
					}
					displayPic(imageNo);

					//Turn off LED
					IO1CLR |= 1<<19;
			}			
			// begin slide show
			if(!(IO1PIN & 1<<21)){
				int i = 1;
				IO1SET |= 1<<22;				
				
				for(; i <= NumberOfImages; i++){
					displayPic(i);
					delay1(20);
				}
					
				IO1CLR |= 1<<22;			
			}		
	 }
}  
 
 void displayPic(int i){
	 // Clear the screen	
	 clrram();  
   //reduce 1 becasue SD blocks start from 0 and multiply by 4 beccause each Image takes 4 blocks i.e. 2048 bytes 	 
	 read_card((i-1)<<2);
	 //Display Image
	 disp_img(0,16,128,imageFromSD);
}
 

//writes 2048 byte long data to 4 buffers
void write_to_buffers(unsigned char image[]){
	int i = 0;
	for(i = 0; i < 512; i++){
			buffer1[i] = image[i];
	}
	for(i = 0; i < 512; i++){
			buffer2[i] = image[i + 512];
	}
	for(i = 0; i < 512; i++){
			buffer3[i] = image[i + 1024];	
	}
	for(i = 0; i < 512; i++){
			buffer4[i] = image[i + 1536];
	}
}
//write the 4 buffers to SD
void write_buffers_to_SD(int startBlock){
	//write buffers to card
	SD_writeSingleBlock(startBlock, buffer1);	
	SD_writeSingleBlock(startBlock + 1, buffer2);
	SD_writeSingleBlock(startBlock + 2, buffer3);	
	SD_writeSingleBlock(startBlock + 3, buffer4);
}

 void write_card(void){ 
	 //first write to buffer then copy to SD
	 
	 write_to_buffers(p2);
   write_buffers_to_SD(0);
	 NumberOfImages++;
	 
	 write_to_buffers(p3);
   write_buffers_to_SD(4);
	 NumberOfImages++;
				
	 write_to_buffers(p4);
   write_buffers_to_SD(8);
	 NumberOfImages++;

	 write_to_buffers(p5);
   write_buffers_to_SD(12);
	 NumberOfImages++;
				
	 write_to_buffers(p1);
   write_buffers_to_SD(16);
	 NumberOfImages++;

}
 
void read_card(int block){
	int i=0;
	
	//Read first block to buffer
	SD_readSingleBlock(block, buffer1); 
	//Copy buffer to imageFromSD
	for(i=0;i<512;i++ )
		imageFromSD[i]=buffer1[i];
	
  //Read second block to buffer2
	SD_readSingleBlock(block + 1, buffer2); 
	//Copy buffer2 to imageFromSD
	for(i=0;i<512;i++ )
		imageFromSD[i + 512]=buffer2[i];
	
	
	//Read third block to buffer3
	SD_readSingleBlock(block + 2, buffer3); 
	//Copy buffer3 to imageFromSD
	for(i=0;i<512;i++ )
		imageFromSD[i+1024]=buffer3[i];
	
 //Read fourth block to buffer4
	SD_readSingleBlock(block+3, buffer4); 
	//Copy buffer2 to imageFromSD
	for(i=0;i<512;i++ )
		imageFromSD[i+1536]=buffer4[i];	
}
