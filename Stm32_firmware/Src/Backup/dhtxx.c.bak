// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "dhtxx.h"
#include "us_timing.h"
#include <stdio.h>
#include <assert.h>

//! Do not tested for DHT21 -- info from datasheets
#define ZERO_LENGTH_DHT11 30 // Max 26-29
#define ONE_LENGTH_DHT11  70
#define ZERO_LENGTH_DHT22 30 // Max 26-29
#define ONE_LENGTH_DHT22  70
#define ZERO_LENGTH_DHT21 30 // Max 26-29
#define ONE_LENGTH_DHT21  70

//! Мінімальний час між вимірами, мілісекунди
#define DHT11_MIN_MEASURES_PAUSE 2000
#define DHT22_MIN_MEASURES_PAUSE 2000
#define DHT21_MIN_MEASURES_PAUSE 2000
#define DHT_Unknown_MIN_MEASURES_PAUSE DHT22_MIN_MEASURES_PAUSE

//! Час перед першим виміром після включення, мілісекунди
#define DHT11_FIRST_MEASURES_PAUSE 2000
#define DHT22_FIRST_MEASURES_PAUSE 2000
#define DHT21_FIRST_MEASURES_PAUSE 2000
#define DHT_Unknown_FIRST_MEASURES_PAUSE DHT22_FIRST_MEASURES_PAUSE

//! Length of zero signal to start measurements
//! 20000 us is  minimal for my DHT11, (18000 us -- Datasheet)
//! and DHT22 is ok with 600 us, (500 us -- Datasheet)
//! But, contrary to datasheets DHT22 do not works after so long
//! "start call" as for DHT11, so no "unknown" safe define.
#define DHT11_START_LENGTH 20000
#define DHT22_START_LENGTH 600
#define DHT21_START_LENGTH 600


//! Може не бути особливо точним, тому неохота розділяти на N
//! варіантів
#define global_timeout_us 1000

//#define STDIO_DEBUG
//=DHT11=params==============================================

//=DHT22=params==============================================

//===========================================================

#define WEAK_SYMBOL  __attribute__((weak))
// https://en.wikipedia.org/wiki/Weak_symbol


//===========================================================
//=MCU=abstractions==========================================
//===========================================================

static inline int read_pin(DHTxx_hygrometer_t* conf)
{
	return HAL_GPIO_ReadPin(conf->data_port, conf->data_pin);
}

static inline void write_pin(DHTxx_hygrometer_t* conf, int pinstate)
{
	HAL_GPIO_WritePin(conf->data_port, conf->data_pin, pinstate);
}

static inline uint32_t local_get_ms()
{
	return HAL_GetTick();
}

static inline void set_as_open_drain_output(DHTxx_hygrometer_t* conf)
{
	//! Мені не подобається цей код! Але готових функцій для зміни
	//! ролі піна на льоту в HAL немає, використовувати два піни --
	//! ще більше збочення, а лізти в регістри неохота -- тоді буде
	//! купа ifdef, навіть для STM32F3 i STM32F1
	GPIO_InitTypeDef GPIO_InitStructOut;
	GPIO_InitStructOut.Pin  = conf->data_pin;
	GPIO_InitStructOut.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStructOut.Pull = GPIO_NOPULL;
	GPIO_InitStructOut.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(conf->data_port, &GPIO_InitStructOut);
}

static inline void set_as_input(DHTxx_hygrometer_t* conf)
{
	//! Див. комент в тілі set_open_drain_output
	GPIO_InitTypeDef GPIO_InitStructIn;
	GPIO_InitStructIn.Pin  = conf->data_pin;
	GPIO_InitStructIn.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructIn.Pull = GPIO_NOPULL;
	GPIO_InitStructIn.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(conf->data_port, &GPIO_InitStructIn);
}
//===========================================================

static int read_pulse(DHTxx_hygrometer_t* conf){
	us_reset_counter();

	 while(read_pin(conf) == GPIO_PIN_RESET &&
			 get_us() < global_timeout_us ){}
	uint16_t before = get_us();
	if(before > global_timeout_us)
	{
#ifdef STDIO_DEBUG
		printf("before: %d\n", before);
#endif
		return -1;
	}
	while(read_pin(conf) == GPIO_PIN_SET &&
			 get_us() < global_timeout_us ){}
	uint16_t after = get_us();
	if(after > global_timeout_us)
	{
#ifdef STDIO_DEBUG
		printf("before: %d\n", before);
		printf("after: %d\n", after);
#endif
		return -1;
	}
	return after-before;
}


//========================================================
//=User=code==============================================
//========================================================
#pragma weak init_DHTxx // Перестраховка. :-)
DHTxx_errors WEAK_SYMBOL io_init_DHTxx(DHTxx_hygrometer_t* conf)
{
	//! Here is special pin/port/other initialization
	return DHTXX_OK;
}

//! Ну не люблю я макросів! :-) Ерзац constexpr-функції з С++
static inline int buf_size(DHTxx_hygrometer_t* conf)
{
	return sizeof(conf->buf)/sizeof(conf->buf[0]);
}

DHTxx_errors init_DHTxx(DHTxx_hygrometer_t* conf, DHTxx_types type,
						uint16_t data_pin, 		  GPIO_TypeDef *data_port)
{
	conf->data_pin  = data_pin;
	conf->data_port = data_port;
	conf->type		= type;
	// conf->last_error = DHTXX_OK;
	conf->last_read_time  = 0;
	for(int i = 0; i<buf_size(conf); ++i)
	{
		conf->buf[i] = 0;
	}
	return io_init_DHTxx(conf);
}

//! pulse_length -- for debug
static int8_t read_bit(DHTxx_hygrometer_t* conf, int *pulse_length)
{
	int res = read_pulse(conf);
	if(pulse_length)
		*pulse_length = res;

	if(res == -1)
		return -1;
	//! Не став розділяти часи для DHT11 і DHT22/DHT21 -- поки немає сенсу,
	//! вони рівні. Але перевірку вставив!
	assert(ZERO_LENGTH_DHT11 == ZERO_LENGTH_DHT22 &&
			"Code expects 0 bit pulse width to be the same");
	assert(ONE_LENGTH_DHT11 == ONE_LENGTH_DHT22 &&
			"Code expects 1 bit pulse width to be the same");
	if( res > 0 && res < ZERO_LENGTH_DHT11 )
		return 0;
	else
		return 1;
}

int ms_before_first_read(DHTxx_hygrometer_t* conf)
{
	switch(conf->type)
	{
	case DHT11:
		return DHT11_FIRST_MEASURES_PAUSE;
		break;
	case DHT22:
		return  DHT22_FIRST_MEASURES_PAUSE;
		break;
	case DHT21:
		return  DHT21_FIRST_MEASURES_PAUSE;
		break;
	default:
		return DHT_Unknown_FIRST_MEASURES_PAUSE;
	}
}

int ms_before_next_read(DHTxx_hygrometer_t* conf)
{
	return local_get_ms() - conf->last_read_time;
}

static inline int get_min_measures_pause(DHTxx_hygrometer_t* conf)
{
	switch(conf->type)
	{
	case DHT11:
		return DHT11_MIN_MEASURES_PAUSE;
		break;
	case DHT22:
		return  DHT22_MIN_MEASURES_PAUSE;
		break;
	case DHT21:
		return  DHT21_MIN_MEASURES_PAUSE;
		break;
	default:
		return DHT_Unknown_MIN_MEASURES_PAUSE;
	}
}

static inline int get_start_signal_length(DHTxx_hygrometer_t* conf)
{
	switch(conf->type)
	{
	case DHT11:
		return DHT11_START_LENGTH;
		break;
	case DHT22:
		return DHT22_START_LENGTH;
	case DHT21:
		return DHT21_START_LENGTH;
		break;
	default:
		//Unknown device. Cannot proceed without start impulse time.
		return -1;
	}
}


DHTxx_errors read_raw_DHTxx(DHTxx_hygrometer_t* conf, int force)
{
	if(conf->last_read_time - local_get_ms() < get_min_measures_pause(conf)
				&& !force)
		return DHTXX_OK;

	int start_time = get_start_signal_length(conf);
	if(start_time < 0 )
		return DHTXX_UNKNOWN_DEVICE;

	set_as_open_drain_output(conf);
	write_pin(conf, SET);   // Get ready
	//udelay(250);            // More or less arbitrary
	write_pin(conf, RESET); // Put to zero -- command DHT to start
	udelay(start_time);
	write_pin(conf, SET);   // Release line part one -- it will be pulled-up
	//udelay(5);              // Part of necessary time.
	set_as_input(conf);

	uint16_t dt[42];
	int8_t res;

#ifdef STDIO_DEBUG
	int dtl[42];
	int idx_dtl = 0;
	res =  read_bit(conf, dtl + idx_dtl++); // Wait for line to go up
    //start reading
	res =  read_bit(conf, dtl + idx_dtl++); // Read acknowledge from DHT
#else
	//! Wait for line to go up -- not a bit
	res = read_bit(conf, 0);
	if(res == -1)
		return DHTXX_NO_CONN;
	//! Not really a bit -- just acknowledge
	res = read_bit(conf, 0);
#endif
	if(res == -1)
		return DHTXX_NO_CONN;
	for(int i = 0 ; i<40; i++){
#ifdef STDIO_DEBUG
		res =  read_bit(conf, dtl + idx_dtl++);
#else
		res =  read_bit(conf, 0);
#endif
		if(res == -1)
			return DHTXX_NO_CONN;
		dt[i] = res;
	}

#ifdef STDIO_DEBUG
 	for(int i = 0; i<42; ++i)
	{
		if(i>0 && i%8 == 0)
			printf(" ");
		printf("%01x", dt[i]);
	}
 	printf("\n");
#endif

	//convert data
 	for(int i = 0; i<buf_size(conf); i++){
 		conf->buf[i] = 0;
 		for(int j = 0; j<8; j++){
 			conf->buf[i] <<= 1;
 			conf->buf[i] |= dt[i*8+j];
 		}
 	}

	//calculate checksum
	uint8_t check_sum = 0;
	for(int i = 0; i<4; i++){
		check_sum += *(conf->buf+i);
	}

#ifdef STDIO_DEBUG
 	for(int i = 0; i<5; ++i)
	{
		printf("%02x ", *(conf->buf+i));
	}
	printf("%02x\n", check_sum);

	printf("%2d | ", dtl[0]);
	printf("%2d | ", dtl[1]);
 	for(int i = 0; i<40; ++i)
	{
		if(i>0 && i%8 == 0)
			printf(" | ");
		printf("%2d ", dtl[i+2]);
	}
 	printf("\n");
#endif
	if (conf->buf[4] != check_sum )
		return DHTXX_CS_ERROR;

	conf->last_read_time = local_get_ms();
	return DHTXX_OK;
}

int get_temperature_DHTxx(DHTxx_hygrometer_t* conf)
{
	uint16_t data;
	switch(conf->type)
	{
	case DHT11:
		return 10 * conf->buf[2];
		break;
	case DHT22:
	case DHT21:
		// Clear sign for shift
		data = ( (conf->buf[2] & 0x7F) << 8 ) + conf->buf[3];
		return (conf->buf[2] & 0x80) ? -data : data;
		break;
	default:
		return -1;
	}
}

int get_humidity_DHTxx(DHTxx_hygrometer_t* conf)
{
	switch(conf->type)
	{
	case DHT11:
		return 10 * conf->buf[0];
		break;
	case DHT22:
	case DHT21:
		return (conf->buf[0] << 8) + conf->buf[1];
		break;
	default:
		return -1;
	}
}

//========================================================
