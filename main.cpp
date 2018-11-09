#include "mbed.h"
#include "rtos.h"
#include "project_config.h"

Serial pc(USBTX,USBRX,19200);
InterruptIn userButton (PB_2);
BusOut monitoringTypeLed(LED1, LED2, LED3);
DigitalOut red(PH_0);
DigitalOut green(PH_1);
DigitalOut blue(PB_13);

const char *MODES [] = {TEST_MODE_LABEL, NORMAL_MODE_LABEL, LOWPOWER_MODE_LABEL, ADVANCED_MODE_LABEL};

const int SAMPLING [] = {TEST_SAMPLING,NORMAL_SAMPLING, 0, 0};

const char *COLORS [] = {NA, RED, GREEN, BLUE};

short int analogTicks;
short int i2cTicks;

extern Thread threadANALOG;
extern Thread threadGPS;
extern Thread threadI2C;

extern void ANALOG_thread();
extern void GPS_thread();
extern void I2C_thread();

extern float smValue;
extern float lightValue;
extern float tempValue;
extern float humidityValue;
extern gps_data gps_info;
extern rgb_data rgb_info;
extern acc_data acc_info;


extern sensor_sumary_data sm_summary_info;
extern sensor_sumary_data light_summary_info;
extern sensor_sumary_data temp_summary_info;
extern sensor_sumary_data humidity_summary_info;
extern acc_sumary_data acc_summary_info;


sensor_alert_data sensor_alert_info;

EventFlags gps_event;

bool modeChanged = true;
int mode = TEST_MODE;

//Timer for printing statistics
Timer st_timer;

//Ticker for defining sampling instant
Ticker app_ticker;
bool ticker_fired = false;

//Ticker for blinking rgb
Ticker rgbTicker; 
bool rgb_blink_fired = false;


void sample() {
  ticker_fired = true; 
}

//Handle test change
void userButtonHandler(void) {
	mode+=1;
  if (mode>=MAX_MODES) {
		mode = TEST_MODE;
	}
	if (mode==NORMAL_MODE) {
		st_timer.start();
	} else if (mode==TEST_MODE) {
		st_timer.stop();
		st_timer.reset();
	}
	//$ TICKER ON
	app_ticker.detach();
	app_ticker.attach(sample,SAMPLING[mode]);
	
	modeChanged = true;
}

int getLedStatus(int mode) {
	int result = 0;
	switch(mode) {
		case TEST_MODE: result = 1;
			break;
		case NORMAL_MODE: result = 2;
			break;
		case LOWPOWER_MODE: result = 4;
			break;
		case ADVANCED_MODE: result = 8;
			break;
	}
	return result;
}

void rbgBlink() {
	if (!rgb_blink_fired) {
		red = 0;
		green = 0;
		blue= 0;
	} else if (rgb_blink_fired) {
		red = 1;
		green = 1;
		blue = 1;
	}
	rgb_blink_fired = !rgb_blink_fired;
}

void lightLed(bool sm, bool light, bool temp, bool humidity, bool acc) {
	//YELLOW
	if (sm) {
					red = 0;																													
					green = 0;																												
					blue = 1;																													
	//WHITE
	} else if (light) {
					red = 0;																													
					green = 0;																												
					blue = 0;
	//PURPLE
	} else if (temp) {
					red = 0;																													
					green = 1;																												
					blue = 0;
	//TURQUOISE
	} else if (humidity) {
					red = 1;																													
					green = 0;																												
					blue = 0;
	
	} else if (acc) {
					red = 0;																													
					green = 0;																												
					blue = 0;
	}	else {
					red = 1;																													
					green = 1;																												
					blue = 1;
					rgbTicker.attach(&rbgBlink,0.5);
	}
}

void handle_data() {
		pc.printf("*******************************************\n\r");
		pc.printf("Mode:%s\n\r",MODES[mode]);
		pc.printf("Soil Moisture-> %6.3f%%\r\n", smValue);
		pc.printf("Light -> %6.3f%%\r\n", lightValue);
		pc.printf("Accelerometer-> xAxis:%f, yAxis:%f, zAxis:%f\n\r", acc_info.xPos, acc_info.yPos, acc_info.zPos);
		pc.printf("Color-> Clear:%d, Red:%d,Green:%d,Blue:%d -- Predominant:%s\n\r", rgb_info.clear,  rgb_info.red, rgb_info.green, rgb_info.blue, rgb_info.color);
		pc.printf("Temp/Hummidity-> Temperature:%0.3fºC, Relative Humidity:%0.3f%\n\r", tempValue, humidityValue );
		pc.printf("GPS-> #Sats:%d, Lat(UTC):%f, Long(UTC):%f, Altitude:%f, GPS Time: %d%d:%d%d:%d%d\r\n", gps_info.satellites,gps_info.latitude,gps_info.longitude, gps_info.altitude, 
			(int) gps_info.time / 100000 % 10, (int) gps_info.time / 10000 % 10, 
			(int) gps_info.time / 1000 % 10, (int) gps_info.time / 1000 % 10, 
			(int) gps_info.time / 10 % 10, (int) gps_info.time % 10);
	
}

void light_led_color(char *color) {
				if (strncmp(color,RED,2) == 0){														
					red = 0;																													
					green = 1;																												
					blue = 1;																													
			}else if (strncmp(color,BLUE,2) == 0){										
					red = 1;																													
					green = 1;																												
					blue = 0;																													
				}else {																															
					red = 1;																													
					green = 0;																												
					blue = 1;																													
			}		
}

void handle_statistical_led() {
				//If errors
				if (sm_summary_info.bounds_error || light_summary_info.bounds_error || temp_summary_info.bounds_error || humidity_summary_info.bounds_error || acc_summary_info.bounds_error) {
					if (sensor_alert_info.sm>sensor_alert_info.light && sensor_alert_info.sm>sensor_alert_info.temperature && sensor_alert_info.sm>sensor_alert_info.humidity && sensor_alert_info.sm>sensor_alert_info.acc) {
						lightLed(true,false,false,false, false);
					} else if (sensor_alert_info.light>sensor_alert_info.sm && sensor_alert_info.light>sensor_alert_info.temperature && sensor_alert_info.light>sensor_alert_info.humidity && sensor_alert_info.light>sensor_alert_info.acc) {
						lightLed(false,true,false,false, false);
					} else if (sensor_alert_info.temperature>sensor_alert_info.sm && sensor_alert_info.temperature>sensor_alert_info.light && sensor_alert_info.temperature>sensor_alert_info.humidity && sensor_alert_info.temperature>sensor_alert_info.acc) {
						lightLed(false,false,true,false,false);
					} else if (sensor_alert_info.humidity>sensor_alert_info.sm && sensor_alert_info.humidity>sensor_alert_info.light && sensor_alert_info.humidity>sensor_alert_info.temperature && sensor_alert_info.humidity>sensor_alert_info.acc) {
						lightLed(false,false,false,true,false);
					} else if (sensor_alert_info.acc>sensor_alert_info.sm && sensor_alert_info.acc>sensor_alert_info.light && sensor_alert_info.acc>sensor_alert_info.temperature && sensor_alert_info.acc>sensor_alert_info.humidity) {
						lightLed(false,false,false,false,true);
					} else {
						lightLed(false,false,false,false,false);
					}
				}
				//Premodimant color in period
				else {
					if (sensor_alert_info.blue>sensor_alert_info.red && sensor_alert_info.blue>sensor_alert_info.green) {
						light_led_color(BLUE);
					} else if (sensor_alert_info.red>sensor_alert_info.blue && sensor_alert_info.red>sensor_alert_info.green) {
						light_led_color(RED);
					} else {
						light_led_color(GREEN);
					}
				} 				
}

//Init data
void init_statistical_data() {
				light_summary_info.bounds_error = false;
				humidity_summary_info.bounds_error = false;
				sm_summary_info.bounds_error = false;
				temp_summary_info.bounds_error = false;
				acc_summary_info.bounds_error = false;
      	light_summary_info.max=0, light_summary_info.mean=0, light_summary_info.min=0;
				temp_summary_info.max=0, temp_summary_info.mean=0, temp_summary_info.min=0;
				humidity_summary_info.max=0, humidity_summary_info.mean=0, humidity_summary_info.min=0;
				sm_summary_info.max = 0, sm_summary_info.mean = 0, sm_summary_info.min = 0;
				acc_summary_info.xmax=0, acc_summary_info.ymax=0, acc_summary_info.zmax=0,
				acc_summary_info.xmin=0, acc_summary_info.ymin=0, acc_summary_info.zmin=0;
				sensor_alert_info.green =0, sensor_alert_info.blue =0, sensor_alert_info.red =0;
				sensor_alert_info.sm =0, sensor_alert_info.humidity =0, sensor_alert_info.light =0, sensor_alert_info.temperature =0, sensor_alert_info.acc=0;
				i2cTicks = 1;
	      analogTicks = 1;
	
}

void handle_statistical_data() {
				rgbTicker.detach();
      	pc.printf("****************STATISTICS*****************\n\r");
				pc.printf("Light-> Max:%0.3fºC, Mean:%0.3fºC, Min:%0.3fºC\n\r", light_summary_info.max, light_summary_info.mean, light_summary_info.min );
				pc.printf("Temperature-> Max:%0.3fºC, Mean:%0.3fºC, Min:%0.3fºC\n\r", temp_summary_info.max, temp_summary_info.mean, temp_summary_info.min );
				pc.printf("Humidity-> Max:%6.3f%%, Mean:%6.3f%%, Min:%6.3f%%\n\r", humidity_summary_info.max, humidity_summary_info.mean, humidity_summary_info.min );
				pc.printf("Soil Moisture-> Max:%6.3f%%, Mean:%6.3f%%, Min:%6.3f%%\n\r", sm_summary_info.max, sm_summary_info.mean, sm_summary_info.min );
				pc.printf("Accelerometer-> Max(x,y,z):(%6.3f,%6.3f,%6.3f) Min(x,y,z):%6.3f,%6.3f,%6.3f \n\r", acc_summary_info.xmax, acc_summary_info.ymax, acc_summary_info.zmax,
				acc_summary_info.xmin, acc_summary_info.ymin, acc_summary_info.zmin );
				pc.printf("Alerts-> sm:%d, light:%d, temp:%d, humidity:%d, acc:%d\n\r", sensor_alert_info.sm, sensor_alert_info.light, sensor_alert_info.temperature , sensor_alert_info.humidity);
	      pc.printf("Predominant Color-> RGB:(%d,%d,%d)\n\r", sensor_alert_info.red, sensor_alert_info.green, sensor_alert_info.blue);
				handle_statistical_led();
				init_statistical_data();
}



// main() runs in its own thread in the OS
int main() {

	  monitoringTypeLed.write(getLedStatus(mode));
	  //Registers user button pullup for changing mode
		userButton.mode(PullUp);
	  userButton.rise(userButtonHandler);
	  //Start threads
    threadANALOG.start(ANALOG_thread);
    threadGPS.start(GPS_thread);
	  threadI2C.start(I2C_thread);
	  init_statistical_data();
// $TICKER_ON	
		app_ticker.attach(sample,SAMPLING[mode]);
		pc.printf("mbed-os-rev: %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);	

 	while (true) {
// $TICKER_ON		
				if (ticker_fired) {
					gps_event.set(0x01);
					handle_data();
					ticker_fired = !ticker_fired;
				}

// $TICKER OFF
//					gps_event.set(0x01);
//					handle_data();

		if (modeChanged) {
					monitoringTypeLed.write(getLedStatus(mode));
					modeChanged = !modeChanged;
					init_statistical_data();
				}
				if (mode==NORMAL_MODE) {
						if (st_timer.read_ms()>=STATISTIC_TIME) {
								handle_statistical_data();
								st_timer.reset();
						}
				} else if (mode==TEST_MODE) {
						light_led_color(rgb_info.color);	
				}
			Thread::wait(1);
// $TICKER OFF
//				Thread::wait(SAMPLING[mode]*1000);
	}
}


