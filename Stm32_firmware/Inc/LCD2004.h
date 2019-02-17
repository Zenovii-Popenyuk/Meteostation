/*
 * LCD2004.h
 *
 *  Created on: 19 ???. 2019 ?.
 *      Author: popenyuk
 */

#ifndef LCD2004_H_
#define LCD2004_H_

HAL_StatusTypeDef LCD_SendInternal(uint8_t data, uint8_t flags);
void search_I2C_bus(I2C_HandleTypeDef i2c_variable);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(uint8_t lcd_addr, I2C_HandleTypeDef i2c_variable);
void LCD_Clear();
void Array_Clear();
void Array_Clear_first_two_rows();
void Array_Clear_first_row();
void Array_Clear_second_row();
void Array_Clear_third_row();
void Array_Clear_fourth_row();
void Array_Clear_third_and_fourth_rows();
void LCD_Print();
void search_I2C_bus_without_semihosting(I2C_HandleTypeDef i2c_variable);
void LCD_SendString(int row, int column, char *str);
void LCD_SendChar(int row, int column, char str);
void LCD_SendInt(int row, int column, int number, int base);
void LCD_SendFloat(int row, int column, float number);
void LCD_SendDouble(int row, int column, double number);
void LCD_PrintRTC(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day_of_week, uint8_t day_of_month, uint8_t month, uint8_t year);
void LCD_PowerOn();
void LCD_PowerOff();
void LCD_POWER_SAVING_MODE();
void LCD_NOPOWER_SAVING_MODE();

#endif /* LCD2004_H_ */
