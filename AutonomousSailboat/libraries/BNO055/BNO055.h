#ifndef BNO055_SENSOR_H
#define BNO055_SENSOR_H

#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <IMU.h>
#include <sensor_msgs/Imu.h>

class BNO055 : public IMU{
	public:
		BNO055(uint8_t address = 0x28) : IMU("IMU", 10, 10), address(address){}

		void init(ros::NodeHandle* n);
		void updateMeasure();
		void updateTest();

		float* getQuat(){return quat;}
		float* getAccel(){return accel;}
		float* getMag(){return mag;}
		float* getRot(){return rot;}

	private:
		uint8_t address;
};

#endif
