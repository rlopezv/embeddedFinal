#include "mbed.h"
#include "rtos.h"
#include "project_config.h"
#include "SerialGPS.h"

SerialGPS gps_sensor(PA_9,PA_10, 9600);

gps_data gps_info;

Thread threadGPS(osPriorityNormal, 1024); // 1K stack size

extern EventFlags gps_event;

void GPS_thread(); 

void GPS_thread() {
   

	 while(1){
		  gps_event.wait_all(0x01);
			//Si hay disponible
		  //Change time zone
		  //Wait for signal
			if (gps_sensor.sample()) {
				//printf("------------------------------------------\r\n");
				//printf("%s\r\n",gps_sensor.msg);
				//printf("sats %d, long %f, lat %f, alt %f, geoid %f, time %f\n\r", gps_sensor.sats, gps_sensor.longitude, gps_sensor.latitude, gps_sensor.alt, gps_sensor.geoid, gps_sensor.time);

				gps_info.satellites = gps_sensor.sats; 
				gps_info.longitude = gps_sensor.longitude; 
				gps_info.latitude = gps_sensor.latitude;
				gps_info.altitude = gps_sensor.alt;
				//Add hour diference
				gps_info.time= gps_sensor.time + 10000;
				//out of range
				if (gps_info.time>240000) {
					gps_info.time-=240000;
				}
				//printf("#sats %d, long %f, lat %f, alt %f, time %f\n\r", gps_info.satellites, gps_info.longitude, gps_info.latitude, gps_info.altitude, gps_info.time);
    } else {
//			pc.printf("-----------------NO VALUE-----------------\r\n");
		}
		gps_event.set(0x02);
		gps_event.set(0x03);
		//		gps_thread.wait(1000);
					
					//Raise signal for other sensors
		}			
}

