#include "mbed.h"
#ifndef __MAIN_CONFIG_STATION_DATA__
#define __MAIN_CONFIG_STATION_DATA__

#define MMA8451Q_I2C_ADDRESS (0x1d<<1)

#define TCS34725_I2C_ADDRESS (0x41<<1)

#define TEST_MODE 0
#define NORMAL_MODE 1
#define LOWPOWER_MODE 2
#define ADVANCED_MODE 3

#define NORMAL_MODE_LABEL "NORMAL"
#define TEST_MODE_LABEL "TEST"
#define LOWPOWER_MODE_LABEL "LOWPOWER"
#define ADVANCED_MODE_LABEL "ADVANCED"

//Colors definition
#define NA "N/A"
#define RED "RED"
#define GREEN "GREEN"
#define BLUE "BLUE"

#define TEST_SAMPLING 2
#define NORMAL_SAMPLING 3

#define TEST_SAMPLING_MS 2000
#define NORMAL_SAMPLING_MS 30000

#define TIME_OFFSET 2
#define SAMPLING_TIME 2000
#define STATISTIC_TIME 12000
#define STATISTIC_TIME_MS 3600000
#define MAX_MODES 2


//Ranges
#define TEMP_MIN_VALUE -10
#define TEMP_MAX_VALUE 50
#define HUMIDITY_MIN_VALUE 25
#define HUMIDITY_MAX_VALUE 75
#define LIGHT_MIN_VALUE 0
#define LIGHT_MAX_VALUE 100
#define SM_MIN_VALUE 0
#define SM_MAX_VALUE 100
#define ZPOS_MIN_VALUE 0.9
#define ZPOS_MAX_VALUE 1.0



typedef struct {
  float max, min, mean;
	bool bounds_error;
} sensor_sumary_data;

typedef struct {
  float xmax, xmin, ymax, ymin, zmax, zmin;
	bool bounds_error;
} acc_sumary_data;


typedef struct {
  uint8_t hour, minute, seconds;
  float latitude, longitude, altitude, time;
  int satellites;
} gps_data;


//Data Structure for RGB

typedef struct {
  uint16_t red,green,blue,clear;
  char* color;
} rgb_data;

//Data Structure for Accelerometer

typedef struct {
  float xPos, yPos, zPos;
  bool fall;
} acc_data;

typedef struct {
  int sm, light, humidity, temperature, acc;
	int red, green, blue;
} sensor_alert_data;

#endif
