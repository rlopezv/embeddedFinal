#include "TCS34725.h"

TCS34725::TCS34725() : i2c(SDA,SCL) {}
TCS34725::TCS34725(PinName i2c_sda, PinName i2c_scl) : i2c(i2c_sda,i2c_scl) {}

void TCS34725::i2cWrite8(uint8_t addr, char reg, char data){
    char packet[2] = {reg | COMMAND_BIT ,data & 0xFF}; 
    i2c.write(addr,packet,2,false);
    wait(0.01);
}

uint8_t TCS34725::i2cRead8(uint8_t addr, char reg){
    char packet[1] = {reg | COMMAND_BIT};
    char data[1] = {0};
    i2c.write(addr,packet,1, true);
    i2c.read(addr,data,1,false);
    return (uint8_t)data[0];
}

uint16_t TCS34725::i2cRead16(uint8_t addr, char reg){
    char packet[1] = {reg | COMMAND_BIT};
    char data[2] = {0,0};
    i2c.write(addr,packet,1, true);
    i2c.read(addr,data,2, false);
    return ((uint16_t)data[1] << 8) | (uint16_t)data[0];
}

bool TCS34725::init(char intTime, char gain){
    i2c.frequency(100000);
    
    uint8_t id = i2cRead8(SENSOR_ADDR, TCS34725_ID);
    if(id != 0x44)return false;
    
    i2cWrite8(SENSOR_ADDR,TCS34725_ATIME, intTime);
    t_intTime = intTime;
    i2cWrite8(SENSOR_ADDR,TCS34725_CONTROL, gain);
    t_gain = gain;
    i2cWrite8(SENSOR_ADDR,TCS34725_ENABLE, TCS34725_ENABLE_PON);
    wait(0.003);
    i2cWrite8(SENSOR_ADDR,TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    return true;
}

void TCS34725::config(char intTime, char gain){
    uint8_t reg = i2cRead8(SENSOR_ADDR, TCS34725_ENABLE);
    i2cWrite8(SENSOR_ADDR,TCS34725_ENABLE, reg | ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
    init(intTime,gain);
}

void TCS34725::getColor(uint16_t &r, uint16_t &g, uint16_t &b, uint16_t &c){
    c = i2cRead16(SENSOR_ADDR, TCS34725_CDATAL);
    r = i2cRead16(SENSOR_ADDR, TCS34725_RDATAL);
    g = i2cRead16(SENSOR_ADDR, TCS34725_GDATAL);
    b = i2cRead16(SENSOR_ADDR, TCS34725_BDATAL);
    switch(t_intTime){
        case TCS34725_INTEGRATIONTIME_2_4MS:
            wait(0.003);
            break;
        case TCS34725_INTEGRATIONTIME_24MS:
            wait(0.024);
            break;
        case TCS34725_INTEGRATIONTIME_50MS:
            wait(0.05);
            break;
        case TCS34725_INTEGRATIONTIME_101MS:
            wait(0.101);
            break;
        case TCS34725_INTEGRATIONTIME_154MS:
            wait(0.154);
            break;
        case TCS34725_INTEGRATIONTIME_700MS:
            wait(0.7);
            break;
    }
}

void TCS34725::DEBUG(Serial * deb){
    deb->printf("ATIME:%d ENABLE:%d CONTROL:%d ID:%d\n",
    i2cRead8(SENSOR_ADDR, TCS34725_ATIME),
    i2cRead8(SENSOR_ADDR, TCS34725_ENABLE),
    i2cRead8(SENSOR_ADDR, TCS34725_CONTROL),
    i2cRead16(SENSOR_ADDR, TCS34725_ID));
}