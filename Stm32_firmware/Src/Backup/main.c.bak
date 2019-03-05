
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "LCD2004.h"
#include <stdlib.h>
#include "dhtxx.h"
#include "us_timing.h"
#include "bmp280.h"
#include "math.h"
#include "ZS-042.h"
#include "sd.h"
#include "user_diskio.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define USE_TIM11_TIMING 1																								// declarate that we use 11 timer

volatile uint16_t Timer1 = 0;																							// resert the timer counter
volatile int USE_SEMIHOSTING = 0;																						// to use semihosting or another tool, install this variable to 1

BMP280_HandleTypedef bmp280;																							// variable for bmp280

FATFS SDFatFs;																											// variables for SD card
FATFS *fatfs;
FIL MyFile;
FRESULT res;

DHTxx_errors DHT_res;																									// variables for hygrometer
DHTxx_hygrometer_t hr11_1;

float pressure, temperature, humidity , total_BAR_tem = 0, total_BAR_press = 0;

int DHT_temp = 0, DHT_press = 0, pressed = 0, last_pressed = 0, number_of_measurements = 0;								// variables for using all sensors
int number_of_barometer = 150, last_read_time = 0, last_error_time = 0;
int last_id = 0;
uint8_t seconds=0, minutes = 0, hours = 0, day_of_week = 0, day_of_month = 0, month = 0, year = 0, sect[512], result, ip_first = 0, ip_second = 0, ip_third = 0, ip_fourth = 0;	// variables for using clock
uint8_t information[4] = {0, 0, 0, 0};
uint32_t byteswritten, bytesread;

extern char USERPath[4]; /* logical drive path */
char sentence[100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

FRESULT ReadLongFile(void){
	/*Function for reading long file from SD card. Actually you can read short file.*/
  uint16_t i=0, i1=0;
  uint32_t ind=0;
  uint32_t f_size = MyFile.obj.objsize;
  if(USE_SEMIHOSTING){
	  printf("fsize: %lu\n", (unsigned long)f_size);
  }
  ind=0;
  do
  {
    if(f_size<512)
    {
      i1=f_size;
    }else{
      i1=512;
    }
    f_size-=i1;
    f_lseek(&MyFile,ind);
    f_read(&MyFile,sect,i1,(UINT *)&bytesread);
    for(i=0;i<bytesread;i++)
    {
    	if(USE_SEMIHOSTING){
    		printf("%c", (char)(sect[i]));
    	}
    }
    ind+=i1;
  }while(f_size>0);
  if(USE_SEMIHOSTING){
	  printf("\n");
  }
  return FR_OK;
}

void WriteLongFile(const char* sentence){
	/*Function for writing long files from SD card. At start it check your SD card, then search your file. If on
	 * SD card no file with input name, then it will create it. At finish it close file.*/
	if(f_mount(&SDFatFs,(TCHAR const*)USERPath,0)!=FR_OK)
	  {
	    Error_Handler();
	  }else{
			if(f_open(&MyFile,"RESULT.TXT", FA_OPEN_APPEND | FA_WRITE)!=FR_OK)
			{
			  Error_Handler();
			}else{
			  res = f_write(&MyFile, sentence, strlen(sentence), (void*)&byteswritten);
			  if((byteswritten==0)||(res!=FR_OK))
			  {
				Error_Handler();
			  }
			  f_close(&MyFile);
			  f_mount(&SDFatFs,(TCHAR const*)USERPath,0);
			}
	  }
}

void init_timing()
{
	#ifdef  USE_HAL_DELAY_AND_ASM
	 return;
	#elif defined USE_DWT_TIMING
	 DWT_Init();
	#elif defined USE_TIM11_TIMING
	 TIM11_reinit();
	#else
	#error "Unknown timing method."
	#endif
}

void fullpower(){
	/* It start FULL POWER MODE. BMP280 turn on normal mode, I2C display turn on backlighting.
	 * And change number of reading data from BMP280 from 15 to 150. Write information, that you turn on mode on SD card.*/
	LCD_PowerOn();
	LCD_NOPOWER_SAVING_MODE();
	bmp280.params.mode = BMP280_MODE_NORMAL;
	number_of_barometer = 150;
	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = 0x76;
	bmp280.i2c = &hi2c2;
    if (!bmp280_init(&bmp280, &bmp280.params)) {
    	Array_Clear_third_and_fourth_rows();
	    LCD_SendString(2, 0, "BMP280 initialization failed");
	    LCD_Print();
	}
    if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "FuLL POWER MODE enabled");
		WriteLongFile(sentence);
    }
}

void lowpower(){
	/* It start LOW POWER MODE. BMP280 turn on forced mode, I2C display turn off backlighting.
	 * And change number of reading data from BMP280 from 150 to 15. Write information, that you turn on mode on SD card. */
	LCD_POWER_SAVING_MODE();
	bmp280.params.mode = BMP280_MODE_FORCED;
	number_of_barometer = 15;
	bmp280_init_default_params(&bmp280.params);
    bmp280.addr = 0x76;
    bmp280.i2c = &hi2c2;
    if (!bmp280_init(&bmp280, &bmp280.params)) {
    	Array_Clear_third_and_fourth_rows();
		LCD_SendString(2, 0, "BMP280 initialization failed");
		LCD_Print();
    }
    if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "LOW POWER MODE enabled");
		WriteLongFile(sentence);
    }
}

void poweroff(){
	/* It power off meteostation, except WIFI module. BMP280 turn on sleep mode, I2C display turn off backlighting.
	 * Write information, that you turn on mode on SD card. */
	LCD_PowerOff();
	bmp280.params.mode = BMP280_MODE_SLEEP;
	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = 0x76;
	bmp280.i2c = &hi2c2;
	if (!bmp280_init(&bmp280, &bmp280.params)) {
		Array_Clear_third_and_fourth_rows();
	    LCD_SendString(2, 0, "BMP280 initialization failed");
	    LCD_Print();
	}
	if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "Meteorological is powered off");
		WriteLongFile(sentence);
	}
}

void DHT_Error(char *str){
	/* Print sentence on I2C display, that hygrometer isn`t ok. */
	Array_Clear_second_row();
	LCD_SendString(1, 0, str);
	LCD_Print();
}

void TransmitBarometerData(int temperatureG, int humidity, float pressure, float temperatureB){
	/* Transmit all possible available data to I2C display, WIFI module and  SD card. To send data to WIFI module it cut data. */
	void TransmitMiniData(int id, int value, int CRF){
		uint8_t buffer[3] = {id, (uint8_t)value, (uint8_t)CRF};
		HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), HAL_MAX_DELAY);
	}
	LCD_SendString(2, 0, "Pressure:");
	LCD_SendFloat(2, 10, pressure);
	LCD_SendString(3, 0, "Temperature:");
	LCD_SendFloat(3, 13, temperatureB);
	LCD_SendString(2, 18, "Pa");
	LCD_SendString(3, 18, " C");
	LCD_Print();
	int first_temp = temperatureG/10;
	int second_temp = (temperatureG - first_temp * 10) / 1;
	int first_hum = humidity/10;
	int second_hum = (humidity - first_hum * 10) / 1;
	int first_two = pressure / 10000;
	int second_two = (pressure - first_two * 10000) / 100;
	if(second_two < 10){
		second_two *= 10;
	}
	int third = (pressure - first_two * 10000 - second_two * 100) / 1;
	if(third < 10){
		third *= 10;
	}
	int after_coma = ((pressure - first_two * 10000 - second_two * 100 - third) * 100);
	int first_temperature = temperatureB / 1;
	int second_temperature = (first_temperature - temperatureB) * 100;
	TransmitMiniData(0, first_temperature, first_temperature);
	TransmitMiniData(1, second_temp, second_temp);
	TransmitMiniData(2, first_hum, first_hum);
	TransmitMiniData(3, second_hum, second_hum);
	TransmitMiniData(4, first_two, first_two);
	TransmitMiniData(5, second_two, second_two);
	TransmitMiniData(6, third, third);
	TransmitMiniData(7, after_coma, after_coma);
	TransmitMiniData(8, first_temperature, first_temperature);
	TransmitMiniData(9, second_temperature, second_temperature);
	TransmitMiniData(10, seconds, seconds);
	TransmitMiniData(11, minutes, minutes);
	TransmitMiniData(12, hours, hours);
	TransmitMiniData(13, day_of_week, day_of_week);
	TransmitMiniData(14, day_of_month, day_of_month);
	TransmitMiniData(15, month, month);
	TransmitMiniData(16, year, year);

	if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "\tBarometer readings:\n\t\tTemperature: %f C\n\t\tPressure: %f Pa\n", total_BAR_tem, total_BAR_press);
		WriteLongFile(sentence);
	}
}

void BMP_Error(BMP280_HandleTypedef* bmp280){
	/* Write information that Baramoter is disconnected. */
	Array_Clear_third_and_fourth_rows();
	LCD_SendString(2, 0, "Key: Temperature/pressure reading failed");
	bmp280_init_default_params(&bmp280->params);
	(*bmp280).addr = 0x76;
	(*bmp280).i2c = &hi2c2;
	bmp280_init(bmp280, &bmp280->params);
	LCD_Print();
	if ((HAL_GetTick() - last_error_time) >= 1000){
		if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
			sprintf(sentence, "\tWarning: No Barometer Connection!\n");
			WriteLongFile(sentence);
		}
		last_error_time = HAL_GetTick();
	}
}

char* Str_day_of_week(int number){
	/* Change from number type day of week to String */
	switch(number){
	  case 1: return "Monday";
	  case 2: return "Tuesday";
	  case 3: return "Wednesday";
	  case 4: return "Thursday";
	  case 5: return "Friday";
	  case 6: return "Saturday";
	  case 7: return "Sunday";
	  default: return "Unknown day!";
	  }
}

void clear_sentence(char* sentence){
	for(int index = 0; index < 100; index++){
		  sentence[index] = ' ';
	  }
}

void RTC_WriteData(){
	/* Write time to I2C display and SD card. */
	RTC_ReadData(&seconds, &minutes, &hours, &day_of_week, &day_of_month, &month, &year);
	last_read_time = HAL_GetTick();
	LCD_PrintRTC(seconds, minutes, hours, day_of_week, day_of_month, month, year);
	clear_sentence(sentence);
	if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "\tData: %d:%d:%d, %s, %d/%d/20%d\n", hours, minutes, seconds, Str_day_of_week(day_of_week), day_of_month, month, year);
		WriteLongFile(sentence);
	}
}

void WritetHygrometerData(){
	Array_Clear_second_row();
	DHT_temp = get_temperature_DHTxx(&hr11_1);
	DHT_press = get_humidity_DHTxx(&hr11_1);
	clear_sentence(sentence);
	if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "\tHygrometer readings:\n\t\tTemperature: %d C\n\t\tHumidity: %d %%\n", (int)DHT_temp/10, (int)DHT_press/10);
		WriteLongFile(sentence);
	}
	LCD_SendInt(1, 10, DHT_press/10, 10);
	LCD_SendString(1, 0, "Humidity:");
	LCD_SendString(1, 13, "%");
	LCD_Print();
}

void HygrometerCSError(){
	DHT_Error("Key: Checksum ERROR");
	if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "\tWarning: Hygrometer Checksum ERROR!\n");
		WriteLongFile(sentence);
	}
}

void HygrometerNOConnection(){
	DHT_Error("Key: NO CONNECTION");
	if(disk_status(SDFatFs.drv) != STA_NOINIT && res == FR_OK){
		sprintf(sentence, "\tWarning: Hygrometer NO CONNECTION!\n");
		WriteLongFile(sentence);
	}
}

void CheckAvailableData(){
	HAL_UART_Receive(&huart1, information, 4, 1);
	uint8_t id = information[0];
	uint8_t CRF_id = information[1];
	uint8_t value = information[2];
	uint8_t CRF = information[3];
	if(id == CRF_id){
		if (id <= 1){
			pressed = value;
			if (last_pressed != pressed){
				  switch(pressed){
					  case 0: fullpower(); break;
					  case 1: lowpower(); break;
					  case 2: poweroff(); break;
				  }
				  last_pressed = pressed;
			}
		}else{
			if(id != last_id){
				RTC_ReadData(&seconds, &minutes, &hours, &day_of_week, &day_of_month, &month, &year);
				 if(value == CRF){
					switch(id){
						case 2: year = value; break;
						case 3: month = value; break;
						case 4: day_of_month = value; break;
						case 5: hours = value; break;
						case 6: minutes = value; break;
						case 7: seconds = value; break;
						case 8: day_of_week = value; break;
						case 9: ip_first = value; break;
						case 10: ip_second = value; break;
						case 11: ip_third = value; break;
						case 12: ip_fourth = value; break;
					}
				 }
				 RTC_SetData(seconds, minutes, hours, day_of_week, day_of_month, month, year);
				 last_id = id;
			}
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  if(USE_SEMIHOSTING){
	  extern void initialise_monitor_handles(void);
	  initialise_monitor_handles();
  }
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  /* Initialize all user configured peripherals */
  bmp280_init_default_params(&bmp280.params);
  bmp280.addr = 0x76;
  bmp280.i2c = &hi2c2;
  bmp280_init(&bmp280, &bmp280.params);
  LCD_Init(0x27<<1, hi2c1);
  LCD_PowerOn();
  init_timing();
  HAL_TIM_Base_Start_IT(&htim2);
  init_DHTxx(&hr11_1, DHT11, GPIO_PIN_4, GPIOC);
  HAL_Delay(ms_before_first_read(&hr11_1));
  Array_Clear();
  disk_initialize(SDFatFs.drv);
  clear_sentence(sentence);
  if (!bmp280_init(&bmp280, &bmp280.params)) {
	  BMP_Error(&bmp280);
  }
  if(disk_status(SDFatFs.drv) != STA_NOINIT){
	  sprintf(sentence, "Results of work of the meteorological station:\n");
	  WriteLongFile(sentence);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int start_time = HAL_GetTick();
  while (1)
  {
	  CheckAvailableData();

	  /* Check if there time to read data from different sensors. */
	  if(pressed != 2){
		  if((HAL_GetTick() - start_time) > 10000){
			  if((HAL_GetTick() - last_read_time) > 1000){
				  RTC_WriteData();
			  }
		  }else{
			  Array_Clear_first_row();
			  LCD_SendInt(0, 0, ip_first, 10);
			  LCD_SendString(0, 3, ".");
			  LCD_SendInt(0, 4, ip_second, 10);
			  LCD_SendString(0, 7, ".");
			  LCD_SendInt(0, 8, ip_third, 10);
			  LCD_SendString(0, 10, ".");
			   LCD_SendInt(0, 11, ip_fourth, 10);
			  LCD_Print();
		  }
		  if(ms_before_next_read(&hr11_1) >= 2000){
			  /* Read data from hygrometer. */
			  DHT_res = read_raw_DHTxx(&hr11_1, 0);
			  if (DHT_res==DHTXX_OK)
			  {
				  WritetHygrometerData();
			  }
			  if (DHT_res==DHTXX_CS_ERROR)
			  {
				  HygrometerCSError();
			  }
			  if (DHT_res==DHTXX_NO_CONN)
			  {
				  HygrometerNOConnection();
			  }
		  }
		    if(bmp_ms_before_next_read(&bmp280) > (1000 / number_of_barometer)){
				  /* Read data from barometer. */
				if (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity)) {
					BMP_Error(&bmp280);

				}else{
					if (number_of_measurements < number_of_barometer){
						total_BAR_tem += temperature;
						total_BAR_press += pressure;
						number_of_measurements++;
					}else{
						Array_Clear_third_and_fourth_rows();
						total_BAR_tem = total_BAR_tem / number_of_measurements;
						total_BAR_press = total_BAR_press / number_of_measurements;
						TransmitBarometerData(DHT_temp, DHT_press, total_BAR_press, total_BAR_tem);
						total_BAR_tem = 0;
						total_BAR_press = 0;
						number_of_measurements = 0;
					}
				}
		    }
	  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
