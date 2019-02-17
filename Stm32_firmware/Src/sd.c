#include "sd.h"
#include "LCD2004.h"
#include "main.h"

extern volatile uint16_t Timer1;
extern int USE_SEMIHOSTING;
extern SPI_HandleTypeDef hspi2;
sd_info_ptr sdinfo;
uint8_t sect[512];

uint8_t SPIx_WriteRead(uint8_t Byte){
	uint8_t receivedbyte = 0;
	if(HAL_SPI_TransmitReceive(&hspi2,(uint8_t*) &Byte,(uint8_t*) &receivedbyte,1,0x1000)!=HAL_OK)
	{
	  return 0;
	}
	return receivedbyte;
}

void SD_PowerOn(void){
	Timer1 = 0;
	while(Timer1<2);
}

void SPI_SendByte(uint8_t bt){
	SPIx_WriteRead(bt);
}

uint8_t SPI_ReceiveByte(void){
	uint8_t bt = SPIx_WriteRead(0xFF);
	return bt;
}

void SPI_Release(void){
	/* RElease SPI line*/
	SPIx_WriteRead(0xFF);
}

uint8_t SPI_wait_ready(void){
	/* Function that wait until SD card return some answer*/
	uint8_t res;
	uint16_t cnt;
	cnt=0;
	do {
	res=SPI_ReceiveByte();
	cnt++;
	} while ( (res!=0xFF)&&(cnt<0xFFFF) );
	if (cnt>=0xFFFF) return 1;
	return res;
}

uint8_t SD_cmd (uint8_t cmd, uint32_t arg){
	/* Send command to SD card */
	uint8_t n, res;
	// ACMD<n> is the command sequense of CMD55-CMD<n>
	if (cmd & 0x80)
	{
	  cmd &= 0x7F;
	  res = SD_cmd(CMD55, 0);
	  if (res > 1) return res;
	}
	// Select the card
	SS_SD_DESELECT();
	SPI_ReceiveByte();
	SS_SD_SELECT();
	SPI_ReceiveByte();

	// Send a command packet
	SPI_SendByte(cmd); // Start + Command index
	SPI_SendByte((uint8_t)(arg >> 24)); // Argument[31..24]
	SPI_SendByte((uint8_t)(arg >> 16)); // Argument[23..16]
	SPI_SendByte((uint8_t)(arg >> 8)); // Argument[15..8]
	SPI_SendByte((uint8_t)arg); // Argument[7..0]
	n = 0x01; // Dummy CRC + Stop
	if (cmd == CMD0) {n = 0x95;} // Valid CRC for CMD0(0)
	if (cmd == CMD8) {n = 0x87;} // Valid CRC for CMD8(0x1AA)
	SPI_SendByte(n);// Receive a command response
	n = 10; // Wait for a valid response in timeout of 10 attempts
	do {
	    res = SPI_ReceiveByte();
	} while ((res & 0x80) && --n);
	return res;

}

uint8_t SD_Read_Block (uint8_t *buff, uint32_t lba){
	/* Read a block of data from SD card */
	uint8_t result;
	uint16_t cnt;
	result=SD_cmd (CMD17, lba);
	if (result!=0x00) return 5;
	SPI_Release();
	cnt=0;
	do{
	  result=SPI_ReceiveByte();
	  cnt++;
	} while ( (result!=0xFE)&&(cnt<0xFFFF) );
	if (cnt>=0xFFFF) return 5;
	for (cnt=0;cnt<512;cnt++) buff[cnt]=SPI_ReceiveByte();
	SPI_Release();
	SPI_Release();
	return 0;
}

uint8_t SD_Write_Block (uint8_t *buff, uint32_t lba){
	/* Write a data in block of memory */
	uint8_t result;
	uint16_t cnt;
	result=SD_cmd(CMD24,lba);
	if (result!=0x00) return 6;
	SPI_Release();
	SPI_SendByte (0xFE);
	for (cnt=0;cnt<512;cnt++) SPI_SendByte(buff[cnt]);
	SPI_Release();
	SPI_Release();
	result=SPI_ReceiveByte();
	if ((result&0x05)!=0x05) return 6;
	cnt=0;
	do {
	  result=SPI_ReceiveByte();
	  cnt++;
	} while ( (result!=0xFF)&&(cnt<0xFFFF) );
	if (cnt>=0xFFFF) return 6;
	return 0;
}

uint8_t SD_Init(void){
	/* Initialise SD card with SPI */
	uint8_t i, cmd;;
	uint8_t ocr[4];
	int16_t tmr;
	uint32_t temp;
	sdinfo.type = 0;
	temp = hspi2.Init.BaudRatePrescaler;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; //156.25 kbbs
	HAL_SPI_Init(&hspi2);
	SS_SD_DESELECT();
	hspi2.Init.BaudRatePrescaler = temp;
	HAL_SPI_Init(&hspi2);
	for(i=0;i<10;i++)
	SPI_Release();
	SS_SD_SELECT();
	if (SD_cmd(CMD0, 0) == 1) // Enter Idle state
	{
	  SPI_Release();
		if (SD_cmd(CMD8, 0x1AA) == 1) // SDv2
		{
			for (i = 0; i < 4; i++) ocr[i] = SPI_ReceiveByte();
			Array_Clear();
			if(USE_SEMIHOSTING){
				LCD_SendString(0, 0, "OCR: 0x   0x   0x");
				printf("OCR: 0x%02X 0x%02X 0x%02X 0x%02X\n",ocr[0],ocr[1],ocr[2],ocr[3]);
				LCD_SendString(1, 0, "0x");
				LCD_SendInt(0, 7, ocr[0], 16);
				LCD_SendInt(0, 12, ocr[1], 16);
				LCD_SendInt(0, 17, ocr[2], 16);
				LCD_SendInt(1, 2, ocr[3], 16);
			}
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) // The card can work at vdd range of 2.7-3.6V
			{
				for (tmr = 12000; tmr && SD_cmd(ACMD41, 1UL << 30); tmr--)
				; // Wait for leaving idle state (ACMD41 with HCS bit)
				if (tmr && SD_cmd(CMD58, 0) == 0) { // Check CCS bit in the OCR
				  for (i = 0; i < 4; i++) ocr[i] = SPI_ReceiveByte();
					sdinfo.type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; // SDv2 (HC or SC)
					if(USE_SEMIHOSTING){
						LCD_SendString(2, 0, "OCR: 0x   0x   0x");
						printf("OCR: 0x%02X 0x%02X 0x%02X 0x%02X\n",ocr[0],ocr[1],ocr[2],ocr[3]);
						LCD_SendString(3, 0, "0x");
						LCD_SendInt(2, 7, ocr[0], 16);
						LCD_SendInt(2, 12, ocr[1], 16);
						LCD_SendInt(2, 17, ocr[2], 16);
						LCD_SendInt(3, 2, ocr[3], 16);
					}
				}
			}
		}
		else //SDv1 or MMCv3
		{
			if (SD_cmd(ACMD41, 0) <= 1){
				sdinfo.type = CT_SD1; cmd = ACMD41; // SDv1
			}else{
				sdinfo.type = CT_MMC; cmd = CMD1; // MMCv3
			}
			for (tmr = 25000; tmr && SD_cmd(cmd, 0); tmr--) ; // Wait for leaving idle state
			if (!tmr || SD_cmd(CMD16, 512) != 0) // Set R/W block length to 512
				sdinfo.type = 0;
		}
	}else{
	  return 1;
	}
	if(USE_SEMIHOSTING){
	  LCD_SendString(3, 4, "Type SD: 0x");
	  LCD_SendInt(3, 15, sdinfo.type, 16);
	  LCD_Print();
	  printf("Type SD: 0x%02X\n", sdinfo.type);
	}
	return 0;
}
