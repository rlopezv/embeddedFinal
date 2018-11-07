#include "mbed.h"
#include "rtos.h"
#include "project_config.h"
#include "MMA8451Q.h"
#include "TCS34725.h"
#include "Si7021.h"

MMA8451Q accelerometer(PB_9,PB_8,MMA8451Q_I2C_ADDRESS);
TCS34725 rgb_sensor(PB_9,PB_8);
Si7021 th_sensor(PB_9,PB_8);

float tempValue;
float humidityValue;
rgb_data rgb_info;
acc_data acc_info;
sensor_sumary_data temp_summary_info;
sensor_sumary_data humidity_summary_info;
acc_sumary_data acc_summary_info;

extern short int i2cTicks;
extern EventFlags gps_event;
extern sensor_alert_data sensor_alert_info;

Thread threadI2C(osPriorityNormal, 1024); // 1K stack size

void I2C_thread(); 

void I2C_thread() {

	if(!rgb_sensor.init(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X)){

	}
	
	for (;;) {
		//Wait for signal
		gps_event.wait_all(0x03);	
		//Accelerometer
		acc_info.xPos = accelerometer.getAccX() ;
    acc_info.yPos = accelerometer.getAccY() ;
    acc_info.zPos = accelerometer.getAccZ() ;
		if (i2cTicks==1 || acc_summary_info.xmax<acc_info.xPos) {
					acc_summary_info.xmax=acc_info.xPos;
		}
		if (i2cTicks==1 || acc_summary_info.xmin>acc_info.xPos) {
					acc_summary_info.xmin=acc_info.xPos;
		}
		if (i2cTicks==1 || acc_summary_info.xmax<acc_info.yPos) {
					acc_summary_info.ymax=acc_info.yPos;
		}
		if (i2cTicks==1 || acc_summary_info.ymin>acc_info.yPos) {
					acc_summary_info.ymin=acc_info.yPos;
		}
		if (i2cTicks==1 || acc_summary_info.zmax<acc_info.zPos) {
					acc_summary_info.zmax=acc_info.zPos;
		}
		if (i2cTicks==1 || acc_summary_info.zmin>acc_info.zPos) {
					acc_summary_info.zmin=acc_info.zPos;
		}
		
		//Temperature Hummidity
		if (th_sensor.check()) {
			if (th_sensor.measure()) {
				tempValue = th_sensor.get_temperature();
				if (tempValue<TEMP_MIN_VALUE && tempValue>TEMP_MAX_VALUE) {
					temp_summary_info.bounds_error = true;	
					sensor_alert_info.temperature++;
				}
				
				if (i2cTicks==1 || temp_summary_info.max<tempValue) {
					temp_summary_info.max=tempValue;
				}
				if (i2cTicks==1 || temp_summary_info.min>tempValue) {
					temp_summary_info.min=tempValue;
				}
				temp_summary_info.mean=((temp_summary_info.mean*(i2cTicks-1))+tempValue)/i2cTicks;
				
				humidityValue = th_sensor.get_humidity();
				if (humidityValue<HUMIDITY_MIN_VALUE && humidityValue>HUMIDITY_MAX_VALUE) {
					humidity_summary_info.bounds_error = true;	
					sensor_alert_info.humidity++;
				}
				if (i2cTicks==1 || humidity_summary_info.max<humidityValue) {
					humidity_summary_info.max=humidityValue;
				}
				if (i2cTicks==1 || humidity_summary_info.min>humidityValue) {
					humidity_summary_info.min=humidityValue;
				}
				humidity_summary_info.mean=((humidity_summary_info.mean*(i2cTicks-1))+humidityValue)/i2cTicks;

			}
		}
		
		
		//RGD
		rgb_sensor.getColor(rgb_info.red,rgb_info.green,rgb_info.blue,rgb_info.clear); //pass variables by reference...
		if (rgb_info.red>rgb_info.green && rgb_info.red>rgb_info.blue) {
			rgb_info.color = RED;
			sensor_alert_info.red++;
		} else if (rgb_info.green>rgb_info.red && rgb_info.green>rgb_info.blue) {
			rgb_info.color = GREEN;
			sensor_alert_info.green++;
		} else if (rgb_info.blue>rgb_info.red && rgb_info.blue>rgb_info.green) {
			rgb_info.color = BLUE;
			sensor_alert_info.blue++;
		} else {
					rgb_info.color =  NA;
		}
		i2cTicks++;
	}
}
