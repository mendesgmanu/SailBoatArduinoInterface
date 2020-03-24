#include <BNO055.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void BNO055::init(ros::NodeHandle* n){
	if(!bno.begin())
	  {
	    /* There was a problem detecting the BNO055 ... check your connections */
	    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
	    while(1);
	  } else 
	{
	Serial.println("IMU BNO055 has been detected");	
	IMU::init(n);
	}



	
}

void BNO055::updateMeasure(){
	//Wire.beginTransmission(address);
	//Wire.write(0x02);
	//Wire.endTransmission();
	//Wire.requestFrom((uint8_t)address, (uint8_t)22);


	imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
	int yaw = euler.x();
	int pitch = euler.y();
	int roll = euler.z();

	/*
	Serial.println("BNO IMU x-rot: "+String(yaw));
	Serial.println("BNO IMU y-rot: "+String(pitch));
	Serial.println("BNO IMU z-rot: "+String(roll));

	if(bno.isFullyCalibrated()){
		
	} else {
		Serial.println("BNO Not fully calibrated");
	}
	*/

	imu::Vector<3> magneto = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
	int magx = magneto.x();
	int magy = magneto.y();
	int magz = magneto.z();

	imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
	int accx = accel.x();
	int accy = accel.y();
	int accz = accel.z();

	imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

	int rotx = gyro.x();
	int roty = gyro.y();
	int rotz = gyro.z();

	angles[0] = yaw*0.001745329;
	angles[1] = pitch*0.01745329;
	angles[2] = roll*0.01745329;

	double cy = cos(-angles[0]*0.5);
	double sy = sin(-angles[0]*0.5);
	double cp = cos(-angles[2]*0.5);
	double sp = sin(-angles[2]*0.5);
	double cr = cos(-angles[1]*0.5);
	double sr = sin(-angles[1]*0.5);

	quat[0] = cy*cp*cr+sy*sp*sr;
	quat[1] = cy*cp*sr-sy*sp*cr;
	quat[2] = sy*cp*sr+cy*sp*cr;
	quat[3] = sy*cp*cr-cy*sp*sr;

	mag[0]= magx;
	mag[1]= magy;
	mag[2]= magz;

	accel[0]= -accx*0.01;
	accel[1]= -accy*0.01;
	accel[2]= accz*0.01;

	rot[0]= rotx;
	rot[1]= roty;
	rot[2]= rotz;


	
	
	heading = angles[0];
	/*
	uint8_t system, d_gyro, d_accel, d_magneto = 0;
	bno.getCalibration(&system, &d_gyro, &d_accel, &d_magneto);
	Serial.print(" Gyro=");
	Serial.print(d_gyro, DEC);
	Serial.print(" Accel=");
	Serial.print(d_accel, DEC);
	Serial.print(" Mag=");
	Serial.println(d_magneto, DEC);
	*/
	delay(BNO055_SAMPLERATE_DELAY_MS);

/* PREVIOUS SETUP / ORIGINAL CODE HERE

	int yaw = Wire.read();
	yaw <<= 8;
	yaw += Wire.read();

	int pitch = (char)Wire.read();
	int roll = (char)Wire.read();

	int magx = Wire.read();
	magx <<= 8;
	magx += Wire.read();

	int magy = Wire.read();
	magy <<= 8;
	magy += Wire.read();

	int magz = Wire.read();
	magz <<= 8;
	magz += Wire.read();

	int accx = Wire.read();
	accx <<= 8;
	accx += Wire.read();

	int accy = Wire.read();
	accy <<= 8;
	accy += Wire.read();

	int accz = Wire.read();
	accz <<= 8;
	accz += Wire.read();

	int rotx = Wire.read();
	rotx <<= 8;
	rotx += Wire.read();

	int roty = Wire.read();
	roty <<= 8;
	roty += Wire.read();

	int rotz = Wire.read();
	rotz <<= 8;
	rotz += Wire.read();
*/

	
}

void BNO055::updateTest(){
	quat[0] = 0.980;
	quat[1] = 0.001;
	quat[2] = 0.189;
	quat[3] = -0.063;

	rot[0] = 0.0;
	rot[1] = 0.0;
	rot[2] = 0.0;

	accel[0] = 0.0;
	accel[1] = 0.0;
	accel[2] = 0.0;
}
