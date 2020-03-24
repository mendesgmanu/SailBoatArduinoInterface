#include <WindSensor.h>

void WindSensor::initialiseWindSensor(){
	
Serial.println("Init Custom WindSensor");
#ifdef CUSTOM_WIND_VANE
	//Serial.println("USING CUSTOM WIND VANE: " + String(WIND_SENSOR_CSN_PIN) + "," + String(WIND_SENSOR_CLK_PIN) + "," + String(WIND_SENSOR_DO_PIN));
	
	pinMode(WIND_SENSOR_CSN_PIN, OUTPUT);
	pinMode(WIND_SENSOR_CLK_PIN, OUTPUT);	
	pinMode(WIND_SENSOR_DO_PIN, INPUT);

  	digitalWrite(WIND_SENSOR_CSN_PIN, HIGH); 
  	digitalWrite(WIND_SENSOR_CLK_PIN, HIGH);
#else
	pinMode(WIND_SENSOR_PIN, INPUT);
#endif

#ifdef WIND_ANEMOMETER_PIN
	pinMode(WIND_ANEMOMETER_PIN, INPUT);
#endif
	Serial.println("Wind sensor has been configured");

}

void WindSensor::updateAnemometer(){
#ifdef WIND_ANEMOMETER_PIN
	if ((millis() - contactBounceTime) > 15 ) { // debounce the switch contact.
		anemometerRevolution++;
		contactBounceTime = millis();
	}
	if(millis() - timeAnemometer > 2000){
		windSpeed = anemometerRevolution * 1.061/2.0;
		anemometerRevolution = 0;
		timeAnemometer = millis();
	}
#endif
}

void WindSensor::updateMeasures(){
//	Logger::Log(1, F("SetupWindSensor()"), F(""));  // Done in the Setup
	// Safety:

#ifdef CUSTOM_WIND_VANE
	value=0;

	digitalWrite(WIND_SENSOR_CSN_PIN, LOW);
	delayMicroseconds(1); //Waiting for Tclkfe

	//Passing 10 times, from 0 to 9
	for(int x=0; x<10; x++){
		digitalWrite(WIND_SENSOR_CLK_PIN, LOW);
		delayMicroseconds(1); //Tclk/2
		digitalWrite(WIND_SENSOR_CLK_PIN, HIGH);
		delayMicroseconds(1); //Tdo valid, like Tclk/2
		value = (value << 1) | digitalRead(WIND_SENSOR_DO_PIN);   //shift all the entering data to the left and past the pin state to it. 1e bit is MSB
	}

	digitalWrite(WIND_SENSOR_CSN_PIN, HIGH); //

#else
	value = analogRead(WIND_SENSOR_PIN);
	// If no value / no sensor:
	if ((value < WIND_SENSOR_MIN) || (value > WIND_SENSOR_MAX)){
		// This test might not detect if the signal wire is not linked but if it is activated,
		//   there is 100% chances that it's true !
//		Logger::Warning(F("WindAngle"), F("No Wind Sensor/Value read or sensor in bad state!"));
	}
	else {
		// Feedback initialization:
//		Logger::Message(F("##\t"), F("Wind Sensor seems to be OK"), F(""), 1);
	}
#endif
#ifdef WIND_ANEMOMETER_PIN
	if(millis() - timeAnemometer > 4000 && digitalRead(WIND_ANEMOMETER_PIN)==HIGH){
		windSpeed = 0;
		anemometerRevolution=0;
		timeAnemometer = millis();
	}
#endif
//	Logger::Log(0, F("Wind sensor corrected value :"), String(value));

	// returns the angle, with reference to the boat:
	angle = mapf(value, WIND_SENSOR_MIN, WIND_SENSOR_MAX, WIND_ANGLE_MIN, WIND_ANGLE_MAX);  // The angle is now in the [0;+360] interval

#ifndef CUSTOM_WIND_VANE
	Serial.println("Running default wind vane");
	angle = fmod(angle + 180,360);
    	if (angle < 0)
        	angle += 360;
  	angle -= 180;
	angle = -angle;
	angle = kf.updateEstimate(angle);

#endif
	angle = angle*DEG_TO_RAD;

}

void WindSensor::updateTest(){
	angle = 120;
}

void WindSensor::communicateData(){
	msg.x = 0;
	msg.y = 0;
#ifdef WIND_ANEMOMETER_PIN
	msg.x = windSpeed*cos(angle);
	msg.y = windSpeed*sin(angle);
#endif
	msg.theta = angle;
	//Serial.println("Wind vane angle: " + String(msg.theta));

	pub.publish(&msg);
}
