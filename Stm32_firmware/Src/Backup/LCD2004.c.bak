/*
 * LCD2004.c
 *
 */


#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "gpio.h"
#include "main.h"

#define PIN_RS    (1 << 0)																// Reset work of I2C display
#define PIN_EN    (1 << 2)
#define BACKLIGHT (1 << 3)																// Display works with backlight
#define NOBACKLIGHT (0x00)																// Display work without backlight
#define I2C_variable (hi2c1)															// Initialise hi2c variable
#define LCD_DELAY_MS 5

volatile int LIGHT = BACKLIGHT;															// initialse that display works with backlight
volatile char current_symbols[80];														// old data, that was printed last time
volatile char new_symbols[80];															// new data to print
volatile uint8_t LCD_ADDR = (uint8_t)(0x27 << 1);

HAL_StatusTypeDef LCD_SendInternal(uint8_t data, uint8_t flags) {
	/* Functon that send data to display. At start it check if there connection with display.
	 * Then form data to send. And at the end it send data with help of build Hal function. */
    HAL_StatusTypeDef res;
    for(;;) {
        res = HAL_I2C_IsDeviceReady(&I2C_variable, LCD_ADDR, 1,
                                    HAL_MAX_DELAY);
        if(res == HAL_OK)
            break;
    }

    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;

    uint8_t data_arr[4];
    data_arr[0] = up|flags|LIGHT|PIN_EN;
    data_arr[1] = up|flags|LIGHT;
    data_arr[2] = lo|flags|LIGHT|PIN_EN;
    data_arr[3] = lo|flags|LIGHT;

    res = HAL_I2C_Master_Transmit(&I2C_variable, LCD_ADDR, data_arr,
                                  sizeof(data_arr), HAL_MAX_DELAY);
    HAL_Delay(LCD_DELAY_MS);
    return res;
}

void search_I2C_bus(I2C_HandleTypeDef i2c){
	// This function scan I2C bus
	// It need semihosting or another tool for printf
	printf("Scanning I2C bus:\r\n");
	HAL_StatusTypeDef result;
	uint8_t i;
	 	for (i=1; i<128; i++)
	 	{
	 	  result = HAL_I2C_IsDeviceReady(&i2c, (uint16_t)(i<<1), 2, 2);
	 	  if (result != HAL_OK)
	 	  {
	 		  printf(".");
	 	  }
	 	  if (result == HAL_OK)
	 	  {
	 		  printf("0x%X", i);
	 	  }
	 	}
	 	printf("\r\n");
}

void LCD_SendCommand(uint8_t cmd) {
	/* Send command to display. Pin Rs is 0 */
    LCD_SendInternal(cmd, 0);
}

void LCD_SendData(uint8_t data) {
	/* Send data to display. Pin Rs is 1 */
    LCD_SendInternal(data, PIN_RS);
}

void LCD_Init(uint8_t lcd_addr, I2C_HandleTypeDef i2c_variable) {
	/* Initialise i2c display. Initialise i2c variable. Clear array. */
	I2C_variable = i2c_variable;
	LCD_ADDR = lcd_addr;
    // 4-bit mode, 2 lines, 5x7 format
    LCD_SendCommand(0b00110000);
    // display & cursor home (keep this!)
    LCD_SendCommand(0b00000010);
    // display on, right shift, underline off, blink off
    LCD_SendCommand(0b00001100);
    // clear display (optional here)
    LCD_SendCommand(0b00000001);
    for(int i = 0; i < 80; i++){
    	current_symbols[i] = ' ';
    	new_symbols[i] = ' ';
    }
}

void LCD_Clear(){
	/* Clear display and clear two arrays */
	LCD_SendCommand(0b00000001);
	for(int i = 0; i < 80; i++){
	    current_symbols[i] = ' ';
	    new_symbols[i] = ' ';
	}
}

void Array_Clear(){
	/* Clear all new array */
	for(int i = 0; i < 80; i++){
		new_symbols[i] = ' ';
	}
}

void Array_Clear_first_row(){
	/* Clear first row of new array */
	for(int i = 0; i < 20; i++){
		new_symbols[i] = ' ';
	}
}

void Array_Clear_second_row(){
	/* Clear second row of new array */
	for(int i = 20; i < 40; i++){
		new_symbols[i] = ' ';
	}
}

void Array_Clear_third_row(){
	/* Clear third row of new array */
	for(int i = 40; i < 60; i++){
		new_symbols[i] = ' ';
	}
}

void Array_Clear_fourth_row(){
	/* Clear fourth row of new array */
	for(int i = 60; i < 80; i++){
		new_symbols[i] = ' ';
	}
}

void Array_Clear_first_two_rows(){
	/* Clear first two rows of new array */
	for(int i = 0; i < 40; i++){
		new_symbols[i] = ' ';
	}
}

void Array_Clear_third_and_fourth_rows(){
	/* Clear second two rows of new array */
	for(int i = 40; i < 80; i++){
		new_symbols[i] = ' ';
	}
}

void LCD_Print(){
	/* Function for print new array on display */
	void SmallPrint(int start, int end){
		/* Small function for print. At start it checks if new data not like old. Then pass it or
		 * There some magic, for example i, i + 44 .. It connected with adreess of cells in display,
		 *  you can read about it in datasheet */
		int state = 1;
		for(int i = start; i < end; i++){
			if(new_symbols[i] != current_symbols[i]){
				if(state){
					if(i < 20){
						LCD_SendCommand(0b10000000|i);
					}
					if(i >= 20 && i < 40){
						LCD_SendCommand(0b10000000|(i + 44));
					}
					if(i >= 40 && i < 60){
						LCD_SendCommand(0b10000000|(i - 20));
					}
					if(i >= 60 && i < 80){
						LCD_SendCommand(0b10000000|(i + 24));
					}
				}
				LCD_SendData(new_symbols[i]);
				state = 0;
				current_symbols[i] = new_symbols[i];
			}else{
				state = 1;
			}
		}
	}

	SmallPrint(0, 20);
	SmallPrint(40, 60);
	SmallPrint(20, 40);
	SmallPrint(60, 80);
}

void LCD_SendString(int row, int column, char *str) {
	/* Write String to new cell. */
	int index = row * 20 + column;
	while(*str && index < 80) {
		new_symbols[index] = *str;
		index++;
		str++;
	}
}

void LCD_SendChar(int row, int column, char str) {
	/* Write Char to new cell. */
	int index = row * 20 + column;
	if(index < 80) {
		new_symbols[index] = str;
	}
}

void LCD_SendInt(int row, int column, int number, int base){
	/* Write Integer to new cell. */
	char sentence[10];
	itoa(number, sentence, base);
	LCD_SendString(row, column, sentence);
}

void LCD_SendFloat(int row, int column, float number){
	/* Write Float to new cell. */
	char sentence[10];
	sprintf(sentence, "%g", number);
	LCD_SendString(row, column, sentence);
}

void LCD_SendDouble(int row, int column, double number){
	/* Write Double to new cell. */
	char sentence[10];
	sprintf(sentence, "%g", number);
	LCD_SendString(row, column, sentence);
}

void search_I2C_bus_without_semihosting(I2C_HandleTypeDef i2c_Variable){
	// This function scan I2C bus and print address on display
	// You can work only with one I2C controller
		HAL_StatusTypeDef result;
		uint8_t i;
		 for (i=1; i<128; i++)
		 	{
		 	  result = HAL_I2C_IsDeviceReady(&i2c_Variable, (uint16_t)(i<<1), 2, 2);
		 	  if (result == HAL_OK)
		 	  {
			 	 LCD_Init(i<<1, i2c_Variable);
			 	 LCD_ADDR = i << 1;
			 	 LCD_SendString(0, 0, "Adress is: 0x");
			 	 LCD_SendInt(0, 12, i, 16);
			 	 LCD_Print();
		 	  }
		 	}
		 	printf("\r\n");
}

void LCD_PowerOn(){
	/* Mode LCD when it works on full power */
	LIGHT = BACKLIGHT;
	LCD_SendCommand(0b00001100);
}

void LCD_PowerOff(){
	/* Mode LCD when it don`t work */
	LIGHT = NOBACKLIGHT;
	LCD_SendCommand(0b00001000);
}

void LCD_POWER_SAVING_MODE(){
	/* Mode LCD when it works on low power, without backlight */
	LIGHT = NOBACKLIGHT;
	LCD_SendCommand(0b00000000);
}

void LCD_NOPOWER_SAVING_MODE(){
	LIGHT = BACKLIGHT;
	LCD_SendCommand(0b00000000);
}

void LCD_PrintRTC(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day_of_week, uint8_t day_of_month, uint8_t month, uint8_t year){
	/* Special function for printing data from RTC on display */
	Array_Clear_first_row();
	LCD_SendChar(0, 0, (char) ((day_of_month/10)%10) + 0x30);
	LCD_SendChar(0, 1, (char) (day_of_month%10) + 0x30);
	LCD_SendString(0, 2, "/");
	LCD_SendChar(0, 3,  (char) ((month/10)%10) + 0x30);
	LCD_SendChar(0, 4,  (char) (month%10) + 0x30);
	LCD_SendString(0, 5, "/");
	LCD_SendChar(0, 6, (char) ((year/10)%10) + 0x30);
	LCD_SendChar(0, 7, (char) (year%10) + 0x30);
	LCD_SendString(0, 8, "/");
	switch(day_of_week){
		case 1: LCD_SendString(0, 9, "Mo"); break;
		case 2: LCD_SendString(0, 9, "Tu"); break;
		case 3: LCD_SendString(0, 9, "We"); break;
		case 4: LCD_SendString(0, 9, "Th"); break;
		case 5: LCD_SendString(0, 9, "Fr"); break;
		case 6: LCD_SendString(0, 9, "Sa"); break;
		case 7: LCD_SendString(0, 9, "Su"); break;
	}
	LCD_SendString(0, 11, "/");
	LCD_SendChar(0, 12, (char) ((hours/10)%10) + 0x30);
	LCD_SendChar(0, 13, (char) (hours%10) + 0x30);
	LCD_SendString(0, 14, ":");
	LCD_SendChar(0, 15, (char) ((minutes/10)%10) + 0x30);
	LCD_SendChar(0, 16, (char) (minutes%10) + 0x30);
	LCD_SendString(0, 17, ":");
	LCD_SendChar(0, 18, (char) ((seconds/10)%10) + 0x30);
	LCD_SendChar(0, 19, (char) (seconds%10) + 0x30);
	LCD_Print();
}
