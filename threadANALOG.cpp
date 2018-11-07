#include "mbed.h"
#include "rtos.h"
#include "project_config.h"

class SoilMoisture {
	public:
SoilMoisture(PinName pin) : _pin(pin)
{
// _pin(pin) means pass pin to the AnalogIn constructor
}
 
float read()
{
//convert sensor reading to temperature in degrees C
    return _pin.read()*100;
}
//overload of float conversion (avoids needing to type .read() in equations)
operator float ()
{
//convert sensor reading to temperature in degrees C
    return _pin.read()*100;
}
private:
//sets up the AnalogIn pin
    AnalogIn _pin;
};


class LightSensor {
	public:
LightSensor(PinName pin) : _pin(pin)
{
// _pin(pin) means pass pin to the AnalogIn constructor
}
 
float read()
{
//convert sensor reading to light
		return _pin.read()*100;
}
//overload of float conversion (avoids needing to type .read() in equations)
operator float ()
{
//convert sensor reading to temperature in degrees C
    return _pin.read()*100;
}
private:
//sets up the AnalogIn pin
    AnalogIn _pin;
};

Thread threadANALOG(osPriorityNormal, 512); // 1K stack size

void ANALOG_thread(); 

//Sensors
SoilMoisture soil_moisture(PA_0);
LightSensor light_sensor(PA_4);

//Values
float smValue;
float lightValue;

sensor_sumary_data light_summary_info;
sensor_sumary_data sm_summary_info;
extern sensor_alert_data sensor_alert_info;

extern short int analogTicks;

extern EventFlags gps_event;

void ANALOG_thread() {
		while (true) {
			  gps_event.wait_all(0x02);
			  //Wait for signal
				smValue = soil_moisture.read();
				if (smValue<SM_MIN_VALUE && smValue>SM_MAX_VALUE) {
					sm_summary_info.bounds_error = true;	
					sensor_alert_info.sm++;
				}
				if (analogTicks==1 || sm_summary_info.max<smValue) {
					sm_summary_info.max=smValue;
				}
				if (analogTicks==1 || sm_summary_info.min>smValue) {
					sm_summary_info.min=smValue;
				}
				sm_summary_info.mean=((sm_summary_info.mean*(analogTicks-1))+smValue)/analogTicks;
				
				lightValue = light_sensor.read();
				if (lightValue<LIGHT_MIN_VALUE && lightValue>LIGHT_MAX_VALUE) {
					light_summary_info.bounds_error = true;	
					sensor_alert_info.light++;
				}
				if (analogTicks==1 || light_summary_info.max<lightValue) {
					light_summary_info.max=lightValue;
				}
				if (analogTicks==1 || light_summary_info.min>lightValue) {
					light_summary_info.min=lightValue;
				}
				light_summary_info.mean=((light_summary_info.mean*(analogTicks-1))+lightValue)/analogTicks;
				analogTicks++;
    }
}
