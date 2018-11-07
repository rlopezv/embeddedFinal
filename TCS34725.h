// TCS34725 RGB Color I2C Sensor breakout library for F030R8 Nucleo board
#ifndef MBED_TCS34725_H
#define MBED_TCS34725_H

#include "mbed.h"

//These pins should be the only piece specific to the F030R8
#define SCL            PB_8
#define SDA            PB_9

//I2C Address

#define SENSOR_ADDR (41<<1)
#define COMMAND_BIT 0x80

//Important Registers

#define TCS34725_ENABLE                  (0x00) //for turning on the device
#define TCS34725_ATIME                   (0x01)  //for integration time
#define TCS34725_CONTROL                 (0x0F)  //for setting the gain
#define TCS34725_ID                      (0x12)
#define TCS34725_CDATAL                  (0x14)    /* Clear channel data */
#define TCS34725_CDATAH                  (0x15)
#define TCS34725_RDATAL                  (0x16)    /* Red channel data */
#define TCS34725_RDATAH                  (0x17)
#define TCS34725_GDATAL                  (0x18)    /* Green channel data */
#define TCS34725_GDATAH                  (0x19)
#define TCS34725_BDATAL                  (0x1A)    /* Blue channel data */
#define TCS34725_BDATAH                  (0x1B)

//Configuration Bits

#define TCS34725_ENABLE_AEN              (0x02)    /* RGBC Enable - Writing 1 actives the ADC, 0 disables it */
#define TCS34725_ENABLE_PON              (0x01) /* Power on - Writing 1 activates the internal oscillator, 0 disables it */
#define TCS34725_INTEGRATIONTIME_2_4MS    0xFF   /**<  2.4ms - 1 cycle    - Max Count: 1024  */
#define TCS34725_INTEGRATIONTIME_24MS     0xF6   /**<  24ms  - 10 cycles  - Max Count: 10240 */
#define TCS34725_INTEGRATIONTIME_50MS     0xEB   /**<  50ms  - 20 cycles  - Max Count: 20480 */
#define TCS34725_INTEGRATIONTIME_101MS    0xD5   /**<  101ms - 42 cycles  - Max Count: 43008 */
#define TCS34725_INTEGRATIONTIME_154MS    0xC0   /**<  154ms - 64 cycles  - Max Count: 65535 */
#define TCS34725_INTEGRATIONTIME_700MS    0x00   /**<  700ms - 256 cycles - Max Count: 65535 */
#define TCS34725_GAIN_1X                  0x00   /**<  No gain  */
#define TCS34725_GAIN_4X                  0x01   /**<  4x gain  */
#define TCS34725_GAIN_16X                 0x02   /**<  16x gain */
#define TCS34725_GAIN_60X                 0x03   /**<  60x gain */

/** TCS34725 control class.
 *
 * Example:
 * @code
 * //Send rgb data to the serial port
 * #include "TCS34725.h"
 * #include "mbed.h"
 *
 * TCS34725 colorSens(p9, p10); //I2C sda and scl
 * Serial pc(USBTX, USBRX); //USB serial
 *
 * int main() {
 *   uint16_t r,g,b,c;
 *       
 *   if(!colorSens.init(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_60X)){
 *       pc.printf("ERROR\n"); //check to see if i2c is responding
 *   }
 *
 *   while(1) {
        colorSens.getColor(r,g,b,c); //pass variables by reference...
 *      pc.printf("DATA: r%d g%d b%d c%d", r, g, b, c);
 *      wait(0.5);
 *  }
 * }
 * @endcode
 */

class TCS34725 {
    private:
        I2C i2c;
        uint8_t t_intTime;
        uint8_t t_gain;
        void i2cWrite8(uint8_t addr, char reg, char data);
        uint8_t i2cRead8(uint8_t addr, char reg);
        uint16_t i2cRead16(uint8_t addr, char reg);
    public:
        /** Initialize object with default i2c pins */
        TCS34725();
        
        /** Initialize object with specific i2c pins
        * 
        * @param i2c_sda SDA pin
        * @param i2c_scl SCL pin
        */
        TCS34725(PinName i2c_sda, PinName i2c_scl);
        
        /** Boot up the sensor and checks if acking (see header for param defines)
        * 
        * @param intTime Integration time for reading (will delay accordingly)
        * @param i2c_scl Gain value
        */
        bool init(char intTime, char gain);
        
        /** Configure after initial boot (will restart sensor)
        * 
        * @param intTime Integration time for reading (will delay accordingly)
        * @param i2c_scl Gain value
        * @return 
        *   1 if failed
        *`  0 if successful
        */
        void config(char intTime, char gain);
        
        /** Returns rgbc reading from the sensor.
        * 
        * @param r Red value (passed by reference)
        * @param g Green value (passed by reference)
        * @param b Blue value (passed by reference)
        * @param c Clear value (all wavelengths - essentially shade) (passed by reference)
        */
        void getColor(uint16_t &r, uint16_t &g, uint16_t &b, uint16_t &c);
        
        /** Debug function... probably not useful unless youre debugging your i2c line
        * 
        * @param deb Serial object for debugging (passed by reference)
        */
        void DEBUG(Serial *deb);
};

#endif
