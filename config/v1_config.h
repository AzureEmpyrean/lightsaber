// This config file contains all the things should be the same for all
// V1 teensysaber configurations. In most cases there should be no need
// to change anything in this file.

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define GYRO_CLASS FXAS21002
#define ACCEL_CLASS FXOS8700

// Teensy 3.2 pin map:
// A lot of these can be changed, but be careful, because:
//   o The pins used by the prop shield cannot be easily changed.
//   o Pins that are going to control normal LEDs (not neopixels) need PWM capability,
//     and not all teensy pins can do PWM.
//   o Touch input is not available on all pins.
//   o Sdcard chip select pin depends on what shield you use.
//   o Battery level and blade identification needs analog input, which is not possible
//     on all pins.
//
// See the teensy 3.2 pinout diagram for more info: https://www.pjrc.com/teensy/pinout.html
enum SaberPins {
  // Bottom edge (in pin-out diagram)
  rx = 0,            		//
  tx = 1,                   // FREE
  motionSensorInterruptPin = 2,   // motion sensor interrupt (prop shield)
  freePin3 = 3,                   // FREE
  freePin4 = 4,                   // FREE (make this sdCardSelectPin if you're using a Wiz820+SD shield)
  amplifierPin = A14,             // Amplifier enable pin (prop shield)
  serialFlashSelectPin = 6,       // serial flash chip select (prop shield)
  freePin7 = 7,                   // FREE
  freePin8 = 8,                   // FREE
  spiLedDataOut = 11,
  spiLedClock = 13,
  bladePin = 9,                  // blade control, either WS2811 or PWM
  bladeIdentifyPin = 9,          // blade identify input / FoC
  sdCardSelectPin = 15,           //
  spiDataOut = 11,                // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card

  // Top edge
  spiClock = 14,                  // spi clock, flash, spi led & sd card
  //batteryLevelPin = 14,           // battery level input
  auxPin = 19,                    // AUX button
  powerButtonPin = 18,            // power button
  aux2Pin = ,                   // AUX2 button
  i2cDataPin = 17,                // I2C bus, Used by motion sensors
  i2cClockPin = 16,               // I2C bus, Used by motion sensors
  freePin20 = 20,				  // free
  bladePowerPin1 = 21,            // blade power control
  bladePowerPin2 = 22,            // blade power control
  bladePowerPin3 = 23,            // blade power control
};
