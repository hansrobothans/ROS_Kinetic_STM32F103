#include "gy85.h"

void Gy85::init()
{
	Wire.begin();
}

void Gy85::send_value(int dev_addr, uint8_t value)
{
	Wire.beginTransmission(dev_addr);
	Wire.write(value);
	Wire.endTransmission();
}

uint8_t Gy85::check_id(int dev_addr, uint8_t reg_addr)
{
	Wire.beginTransmission(dev_addr);
	Wire.write(reg_addr);
	Wire.endTransmission();
	Wire.requestFrom(dev_addr, 1);
	return Wire.read();
}

void Gy85::write_to_register(int dev_addr, uint8_t reg_addr, uint8_t reg_value)
{
	Wire.beginTransmission(dev_addr);
	Wire.write(reg_addr);
	Wire.write(reg_value);
	Wire.endTransmission();
}

bool Gy85::check_gyroscope()
{
	if((check_id(ITG3205_GYRO_ADDRESS, ITG3205_WHO_AM_I) & 0x7E) == ITG3205_GYRO_ADDRESS){
		write_to_register(ITG3205_GYRO_ADDRESS, ITG3205_PWR_MGM, ITG3205_RESET);
		write_to_register(ITG3205_GYRO_ADDRESS, ITG3205_DLPF_FS, 0x1B);
		write_to_register(ITG3205_GYRO_ADDRESS, 0x15, 0x13);
		write_to_register(ITG3205_GYRO_ADDRESS, ITG3205_PWR_MGM, 0x03);
		return true;
	} else {
		return false;
	}

}

void Gy85::measure_gyroscope()
{
	uint8_t gyro_read = 0;
	send_value(ITG3205_GYRO_ADDRESS, 0x1D);
	Wire.requestFrom(ITG3205_GYRO_ADDRESS, 6);
	while(Wire.available()){
		gyro_buffer[gyro_read] = Wire.read();
		gyro_read++;
	}
	raw_rotation.x = (float)(GYRO_X_INVERT*(int16_t)(((int)gyro_buffer[2*GYRO_X_AXIS] <<8) | gyro_buffer[2*GYRO_X_AXIS+1])) * ITG3205_SCALE;  //rad/s
	raw_rotation.y = (float)(GYRO_Y_INVERT*(int16_t)(((int)gyro_buffer[2*GYRO_Y_AXIS] <<8) | gyro_buffer[2*GYRO_Y_AXIS+1])) * ITG3205_SCALE;
	raw_rotation.z = (float)(GYRO_Z_INVERT*(int16_t)(((int)gyro_buffer[2*GYRO_Z_AXIS] <<8) | gyro_buffer[2*GYRO_Z_AXIS+1])) * ITG3205_SCALE;
}

bool Gy85::check_accelerometer()
{
	if (check_id(ADXL345_ACCELEROMETER_ADDRESS, ADXL345_DEVID) == ADXL345_DEVICE_ID){
		write_to_register(ADXL345_ACCELEROMETER_ADDRESS,ADXL345_POWER_CTL,0x08);  //D3, enables measuring
		write_to_register(ADXL345_ACCELEROMETER_ADDRESS,ADXL345_DATA_FORMAT,0x09); //D3 and D0, enables FULL_RES and +/-4g   
		write_to_register(ADXL345_ACCELEROMETER_ADDRESS,ADXL345_BW_RATE,0x09); //Set the bw to 0 Hz
		return true;
	}
	else
		return false;
}

void Gy85::measure_acceleration()
{
	uint8_t acc_reads = 0;
	send_value(ADXL345_ACCELEROMETER_ADDRESS, ADXL345_DATAX0);
	Wire.requestFrom(ADXL345_ACCELEROMETER_ADDRESS, 6);
	while(Wire.available()){
		acc_buffer[acc_reads] = Wire.read();
		acc_reads++;
	}

	raw_acceleration.x =  ((float)ACC_X_INVERT*(int16_t)((int)acc_buffer[2*ACC_X_AXIS+1]<<8 | acc_buffer[2*ACC_X_AXIS]) / ADXL345_SCALE);
	raw_acceleration.y =  ((float)ACC_Y_INVERT*(int16_t)((int)acc_buffer[2*ACC_Y_AXIS+1]<<8 | acc_buffer[2*ACC_Y_AXIS]) / ADXL345_SCALE);
	raw_acceleration.z =  ((float)ACC_Z_INVERT*(int16_t)((int)acc_buffer[2*ACC_Z_AXIS+1]<<8 | acc_buffer[2*ACC_Z_AXIS]) / ADXL345_SCALE);

}

bool Gy85::check_magnetometer()
{
	write_to_register(HMC5883L_MAG_ADDRESS,HMC5883L_MAG_REG_B,HMC5883L_MAG_GAIN);  //Sets the gain
	write_to_register(HMC5883L_MAG_ADDRESS,HMC5883L_MAG_REG_A,0x18); //75Hz output
	write_to_register(HMC5883L_MAG_ADDRESS,HMC5883L_MAG_MODE,0x01); //Single-Measurement Mode
	return true;;
}

void Gy85::measure_magnetometer()
{
	uint8_t mag_reads = 0;
	send_value(HMC5883L_MAG_ADDRESS,HMC5883L_MAG_DATAX0);
	Wire.requestFrom(HMC5883L_MAG_ADDRESS,6);
	while(Wire.available()){
		mag_buffer[mag_reads] = Wire.read();
		mag_reads++;
	}
	raw_magnetic_field.x =  (float)(MAG_X_INVERT * ((int16_t)((int)mag_buffer[2*MAG_X_AXIS] << 8) | (mag_buffer[2*MAG_X_AXIS+1]))) * HMC5883L_MAG_SCALE;
	raw_magnetic_field.y =  (float)(MAG_Y_INVERT * ((int16_t)((int)mag_buffer[2*MAG_Y_AXIS] << 8) | (mag_buffer[2*MAG_Y_AXIS+1]))) * HMC5883L_MAG_SCALE;
	raw_magnetic_field.z =  (float)(MAG_Z_INVERT * ((int16_t)((int)mag_buffer[2*MAG_Z_AXIS] << 8) | (mag_buffer[2*MAG_Z_AXIS+1]))) * HMC5883L_MAG_SCALE;
	if(raw_magnetic_field.x == 0.0 )
		raw_magnetic_field.x = -28514.00;
	if(raw_magnetic_field.y == 0.0)
        raw_magnetic_field.y = -28302.00;
	if(raw_magnetic_field.z ==0.0)
		raw_magnetic_field.z = -28412.00;
	write_to_register(HMC5883L_MAG_ADDRESS,HMC5883L_MAG_MODE,0x01);
}

