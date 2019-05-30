#ifndef _IMU_H_
#define _IMU_H_

#include "config.h"
#include "Wire.h"
#include <geometry_msgs/Vector3.h>

/** ITG3205 config **/
#define GYRO_X_AXIS 1
#define GYRO_Y_AXIS 0
#define GYRO_Z_AXIS 2
#define GYRO_X_INVERT 1
#define GYRO_Y_INVERT 1
#define GYRO_Z_INVERT 1

#define ITG3205_GYRO_ADDRESS 0x68
#define ITG3205_WHO_AM_I 0x00
#define ITG3205_PWR_MGM 0x3E
#define ITG3205_RESET 0x80
#define ITG3205_DLPF_FS 0x16
#define ITG3205_SCALE 0.00121414209  //rad/s

/** ADXL345 config**/
#define ACC_X_AXIS 0
#define ACC_Y_AXIS 1
#define ACC_Z_AXIS 2
#define ACC_X_INVERT 1
#define ACC_Y_INVERT 1
#define ACC_Z_INVERT 1

#define ADXL345_DEVID 0x00
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_ACCELEROMETER_ADDRESS 0x53
#define ADXL345_DEVICE_ID 0xE5
#define ADXL345_SCALE 25.6000

/** HMC5883L config**/
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_INVERT 1
#define MAG_Y_INVERT 1
#define MAG_Z_INVERT 1

#define HMC5883L_MAG_ADDRESS 0x1E
#define HMC5883L_MAG_ID 0x10
#define HMC5883L_MAG_REG_A 0x00
#define HMC5883L_MAG_REG_B 0x01
#define HMC5883L_MAG_MODE 0x02
#define HMC5883L_MAG_DATAX0 0x03
#define HMC5883L_MAG_GAIN 0x20  //Default gain
#define HMC5883L_MAG_SCALE 0.92  // mG/LSb

typedef struct {
	float x;
	float y;
	float z;
} Vector;

class Gy85 {
	public:
		void init();
		void send_value(int dev_addr, uint8_t value);
		uint8_t check_id(int dev_addr, uint8_t res_addr);
		void write_to_register(int dev_addr, uint8_t reg_addr, uint8_t reg_value);

		bool check_gyroscope();
		void measure_gyroscope();

		bool check_accelerometer();
		void measure_acceleration();

		bool check_magnetometer();
		void measure_magnetometer();

		geometry_msgs::Vector3 raw_acceleration, raw_rotation, raw_magnetic_field;

	private:
		uint8_t gyro_buffer[6];
		uint8_t acc_buffer[6];
		uint8_t mag_buffer[6];
		TwoWire Wire;
};

#endif //_IMU_H_
