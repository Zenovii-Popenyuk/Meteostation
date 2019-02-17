/*
 * ZS-042.h
 *
 *  Created on: 19 ???. 2019 ?.
 *      Author: popenyuk
 */


uint8_t Buffer[8];

void I2C_WriteBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf);
void I2C_ReadBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf);
uint8_t RTC_ConvertFromDec(uint8_t c);
uint8_t RTC_ConvertFromBinDec(uint8_t c);
void RTC_SetData(int second, int minute, int hour, int day_of_week, int day_of_month, int month, int year);
void RTC_ReadData(uint8_t *seconds, uint8_t* minutes, uint8_t* hours, uint8_t* day_of_week, uint8_t* day_of_month, uint8_t* month, uint8_t* year);

