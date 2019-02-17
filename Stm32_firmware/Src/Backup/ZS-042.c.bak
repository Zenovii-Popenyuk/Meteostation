/*
 * ZS-042.c
 *
 */


#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "gpio.h"
#include "main.h"
#include "ZS-042.h"
#include "stdlib.h"
#include "LCD2004.h"

extern uint8_t Buffer[8]; // Buffer for read data from read time clock

void I2C_WriteBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf){
	/* Write data to RTC*/
	while(HAL_I2C_Master_Transmit(&hi, (uint16_t)DEV_ADDR,(uint8_t*) &Buffer, (uint16_t)sizebuf, (uint32_t)1000)!= HAL_OK)
	{
		if (HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{
			Array_Clear_first_row();
			LCD_SendString(0, 0, "Buffer error");
			LCD_Print();
		}
	}
}

void I2C_ReadBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf){
	/* Read data from RTC. */
	while(HAL_I2C_Master_Receive(&hi, (uint16_t)DEV_ADDR, (uint8_t*) &Buffer, (uint16_t)sizebuf, (uint32_t)1000)!= HAL_OK)        {
		if (HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF){
			Array_Clear_first_row();
			LCD_SendString(0, 0, "Buffer error");
			LCD_Print();
		}
	}
}

uint8_t RTC_ConvertFromDec(uint8_t c){
	/* Function to convert data. You can read about it in datasheet for ZS-042. */
	uint8_t ch = ((c>>4)*10+(0x0F&c));
	return ch;
}

uint8_t RTC_ConvertFromBinDec(uint8_t c){
	/* Function to convert data. You can read about it in datasheet for ZS-042. */
	uint8_t ch = ((c/10)<<4)|(c%10);
	return ch;
}

void RTC_SetData(int second, int minute, int hour, int day_of_week, int day_of_month, int month, int year){
	/* Send new data to RTC. */
	Buffer[0]=RTC_ConvertFromBinDec(00);
	Buffer[1]=RTC_ConvertFromBinDec(second);
	Buffer[2]=RTC_ConvertFromBinDec(minute);
	Buffer[3]=RTC_ConvertFromBinDec(hour);
	Buffer[4]=RTC_ConvertFromBinDec(day_of_week);
	Buffer[5]=RTC_ConvertFromBinDec(day_of_month);
	Buffer[6]=RTC_ConvertFromBinDec(month);
	Buffer[7]=RTC_ConvertFromBinDec(year);
	I2C_WriteBuffer(hi2c3, (uint16_t)0xD0, sizeof(Buffer));
}

void RTC_ReadData(uint8_t *seconds, uint8_t* minutes, uint8_t* hours, uint8_t* day_of_week, uint8_t* day_of_month, uint8_t* month, uint8_t* year){
	/* Get new data from RTC. */
	Buffer[0]=0;
    I2C_WriteBuffer(hi2c3, (uint16_t)0xD0, 1);
    I2C_ReadBuffer(hi2c3, (uint16_t)0xD0, sizeof(Buffer));
    *seconds = RTC_ConvertFromDec(Buffer[0]);
    *minutes = RTC_ConvertFromDec(Buffer[1]);
    *hours = RTC_ConvertFromDec(Buffer[2]);
    *day_of_week = RTC_ConvertFromDec(Buffer[3]);
    *day_of_month = RTC_ConvertFromDec(Buffer[4]);
    *month = RTC_ConvertFromDec(Buffer[5]);
    *year = RTC_ConvertFromDec(Buffer[6]);
}
