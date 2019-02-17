#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>
#include "gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {DHTXX_OK = 0, DHTXX_NO_CONN = 1, DHTXX_CS_ERROR = 2,
			  DHTXX_TIMEOUT = 0x30,
			  DHTXX_UNKNOWN_DEVICE = 0x40,
			}
DHTxx_errors;

typedef enum {DHT_Unknown = 0, DHT11 = 1, DHT22 = 2, DHT21 = 3} DHTxx_types;

typedef struct
{
	uint32_t 	  last_read_time; // Time from last physical read
	DHTxx_types   type;
	//DHTxx_errors  last_error;
	GPIO_TypeDef *data_port;
	uint16_t 	  data_pin;
	uint8_t 	  buf[5];

} DHTxx_hygrometer_t;

DHTxx_errors init_DHTxx(DHTxx_hygrometer_t* conf, DHTxx_types type,
						uint16_t data_pin, 		  GPIO_TypeDef *data_port);

//! Do nothing if too frequent calls, and force = false
DHTxx_errors read_raw_DHTxx(DHTxx_hygrometer_t* conf, int force);

//! No I/O! Just interprets results of previous read_raw_DHTxx()
//! Temperature, multiplied by 10
//! So, integer part = res/10, fraction = res%10
int get_temperature_DHTxx(DHTxx_hygrometer_t* conf);
int get_humidity_DHTxx(DHTxx_hygrometer_t* conf);

inline double fget_temperature_DHTxx(DHTxx_hygrometer_t* conf)
{
	int r = get_temperature_DHTxx(conf);
	return (double)r/10;
}

inline double fget_humidity_DHTxx(DHTxx_hygrometer_t* conf)
{
	int r = get_humidity_DHTxx(conf);
	return (double)r/10;
}


int ms_before_first_read(DHTxx_hygrometer_t* conf);
int ms_before_next_read(DHTxx_hygrometer_t* conf);

#ifdef __cplusplus
}
#endif
#endif /* __DHT11_H */
