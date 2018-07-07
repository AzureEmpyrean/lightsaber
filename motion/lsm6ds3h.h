#ifndef MOTION_LSM6DS3H_H
#define MOTION_LSM6DS3H_H

class LSM6DS3H : public I2CDevice, Looper, StateMachine {
public:
  const char* name() override { return "LSM6DS3H"; }
  enum Registers {
    FUNC_CFG_ACCESS = 0x1,
    SENSOR_SYNC_TIME_FRAME = 0x4,
    FIFO_CONTROL1 = 0x6,
    FIFO_CONTROL2 = 0x7,
    FIFO_CONTROL3 = 0x8,
    FIFO_CONTROL4 = 0x9,
    FIFO_CONTROL5 = 0xA,
    ORIENT_CFG_G = 0xB,
    INT1_CTRL = 0xD,
    INT2_CTRL = 0xE,
    WHO_AM_I = 0xF,
    CTRL1_XL = 0x10,
    CTRL2_G = 0x11,
    CTRL3_C = 0x12,
    CTRL4_C = 0x13,
    CTRL5_C = 0x14,
    CTRL6_C = 0x15,
    CTRL7_G = 0x16,
    CTRL8_XL = 0x17,
    CTRL9_XL = 0x18,
    CTRL10_C = 0x19,
    MASTER_CONFIG = 0x1A,
    WAKE_UP_SRC = 0x1B,
    TAP_SRC = 0x1C,
    D6D_SRC = 0x1D,
    STATUS_REG = 0x1E,
    STATUS_SPIAux = 0x1E,
    OUT_TEMP_L = 0x20,
    OUT_TEMP_H = 0x21,
    OUTX_L_G = 0x22,
    OUTX_H_G = 0x23,
    OUTY_L_G = 0x24,
    OUTY_H_G = 0x25,
    OUTZ_L_G = 0x26,
    OUTZ_H_G = 0x27,
    OUTX_L_XL = 0x28,
    OUTX_H_XL = 0x29,
    OUTY_L_XL = 0x2A,
    OUTY_H_XL = 0x2B,
    OUTZ_L_XL = 0x2C,
    OUTZ_H_XL = 0x2D,
    SENSORHUB1_REG = 0x2E,
    SENSORHUB2_REG = 0x2F,
    SENSORHUB3_REG = 0x30,
    SENSORHUB4_REG = 0x31,
    SENSORHUB5_REG = 0x32,
    SENSORHUB6_REG = 0x33,
    SENSORHUB7_REG = 0x34,
    SENSORHUB8_REG = 0x35,
    SENSORHUB9_REG = 0x36,
    SENSORHUB10_REG = 0x37,
    SENSORHUB11_REG = 0x38,
    SENSORHUB12_REG = 0x39,
    FIFO_STATUS1 = 0x3A,
    FIFO_STATUS2 = 0x3B,
    FIFO_STATUS3 = 0x3C,
    FIFO_STATUS4 = 0x3D,
    FIFO_DATA_OUT_L = 0x3E,
    FIFO_DATA_OUT_H = 0x3F,
    TIMESTAMP0_REG = 0x40,
    TIMESTAMP1_REG = 0x40,
    TIMESTAMP2_REG = 0x41,
    STEP_TIMESTAMP_L = 0x49,
    STEP_TIMESTAMP_H = 0x4A,
    STEP_COUNTER_L = 0x4B,
    STEP_COUNTER_H = 0x4C,
    SENSORHUB13_REG = 0x4D,
    SENSORHUB14_REG = 0x4E,
    SENSORHUB15_REG = 0x4F,
    SENSORHUB16_REG = 0x50,
    SENSORHUB17_REG = 0x51,
    SENSORHUB18_REG = 0x52,
    FUNC_SRC = 0x53,
    TAP_CFG = 0x58,
    TAP_THS_6D = 0x59,
    INT_DUR2 = 0x5A,
    WAKE_UP_THS = 0x5B,
    WAKE_UP_DUR = 0x5C,
    FREE_FALL = 0x5D,
    MD1_CFG = 0x5E,
    MD2_CFG = 0x5F,
    OUT_MAG_RAW_X_L = 0x66,
    OUT_MAG_RAW_X_H = 0x67,
    OUT_MAG_RAW_Y_L = 0x68,
    OUT_MAG_RAW_Y_H = 0x69,
    OUT_MAG_RAW_Z_L = 0x6A,
    OUT_MAG_RAW_Z_H = 0x6B,
    CTRL_SPIAux = 0x70
  };

  LSM6DS3H() : I2CDevice(106) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();

    while (1) {
      first_motion_ = true;
      first_accel_ = true;
      
      unsigned char databuffer[6];

      STDOUT.print("Motion setup ... ");

      writeByte(CTRL1_XL, 0x88);  // 1.66kHz accel, 4G range
      writeByte(CTRL2_G, 0x8C);   // 1.66kHz gyro, 2000 dps
      writeByte(CTRL3_C, 0x44);   // ?
      writeByte(CTRL4_C, 0x00);
      writeByte(CTRL5_C, 0x00);
      writeByte(CTRL6_C, 0x00);
      writeByte(CTRL7_G, 0x00);
      writeByte(CTRL8_XL, 0x00);
      writeByte(CTRL9_XL, 0x38);  // accel xyz enable
      writeByte(CTRL10_C, 0x38);  // gyro xyz enable
      if (readByte(WHO_AM_I) == 105) {
        STDOUT.println("done.");
      } else {
        STDOUT.println("failed.");
      }

      while (SaberBase::MotionRequested()) {
        YIELD();
        int status_reg = readByte(STATUS_REG);
        if (status_reg == -1) {
          // motion fail, reboot motion chip.
          STDOUT.println("Motion chip timeout, reboot motion chip!");
          writeByte(CTRL3_C, 1);
          delay(20);
          break;
        }
	// Do the accel data first to make clashes as fast as possible.
        if (status_reg & 0x4) {
          // accel data available
          if (readBytes(OUTX_L_XL, databuffer, 6) == 6) {
            SaberBase::DoAccel(
              Vec3::FromData(databuffer, 4.0 / 32768.0,   // 4 g range
			     Vec3::BYTEORDER_LSB, Vec3::ORIENTATION),
	      first_accel_);
	    first_accel_ = false;
	  }
	}
	if (status_reg & 0x2) {
	  // gyroscope data available
	  if (readBytes(OUTX_L_G, databuffer, 6) == 6) {
	    SaberBase::DoMotion(
	      Vec3::FromData(databuffer, 2000.0 / 32768.0,  // 2000 dps
			     Vec3::BYTEORDER_LSB, Vec3::ORIENTATION),
	      first_motion_);
	    first_motion_ = false;
          }
        }
        if (status_reg & 0x1) {
          // Temp data available
          int16_t temp_data;
          if (readBytes(OUT_TEMP_L, (uint8_t*)&temp_data, 2) == 2) {
            float temp = 25.0f + temp_data * (1.0f / 16.0f);
            if (monitor.ShouldPrint(Monitoring::MonitorTemp)) {
              STDOUT.print("TEMP: ");
              STDOUT.println(temp);
            }
          }
        }
      }

      STDOUT.println("Motion disable.");

      writeByte(CTRL9_XL, 0x0);  // accel xyz disable
      writeByte(CTRL10_C, 0x0);  // gyro xyz disable

      while (!SaberBase::MotionRequested()) YIELD();
    }
    STATE_MACHINE_END();
  }

  bool first_motion_;
  bool first_accel_;
};

#endif
