#include <Sailboat.h>
#include <TimeLib.h>
#include <Wire.h>

Sailboat* Sailboat::sailboat = NULL;

Sailboat::~Sailboat(){
	for(int i = 0; i < NB_SENSORS; ++i){
		if(sensors[i] != NULL)
			delete sensors[i];
	}
	
	for(int i = 0; i < NB_ACTUATORS; ++i){
		if(actuators[i] != NULL)
			delete actuators[i];
	}
	
	for(int i = 0; i < NB_CONTROLLERS; ++i){
		if(controllers[i] != NULL)
			delete controllers[i];
	}
}

void Sailboat::setController(ControllerInterface* control){
	if(controller != NULL)
		controller->setActivated(false);
	controller = control;
	controller->init();
	controller->setActivated(true);
	actualControllerI = -1;
}
void Sailboat::setController(int index){
	if(controller != NULL)
		controller->setActivated(false);
	if(index < NB_CONTROLLERS){
		actualControllerI = index;
		controller = controllers[index];
		controller->init();
		controller->setActivated(true);
		if(LOGGER)
			Logger::Instance()->Toast("Changed to :", String(controllerNames[index]), 5000);
	}
}

void Sailboat::cmdCallback(const geometry_msgs::Twist& msg){
	cmd = msg;
}

void Sailboat::msgCallback(const std_msgs::String& msg){
	switch(msg.data[0]){
	case 'C':
		setController(msg.data[1] - '0');
		break;
	case 'M':
		break;
	case 'P':
		if(LOGGER)
			Logger::Instance()->Toast("From PC :", String(msg.data+1), 5000);
		break;
	} 
	watchdogROS = minute();
}

void Sailboat::init(ros::NodeHandle* n){
	Wire.begin();
	
	sensors[SENSOR_WINDSENSOR] = new WindSensor();
	sensors[SENSOR_GPS] = new GPS(Serial1);
	sensors[SENSOR_IMU] = new XSens();
	
	sens[SENSOR_RC] = new RC();
	
	actuators[ACTUATOR_RUDDER] = new Rudder();
	actuators[ACTUATOR_SAIL] = new Sail();
	
	for(int i = 0; i < NB_SENSORS; ++i)
		sensors[i]->init(n);
	
	for(int i = 0; i < NB_ACTUATORS; ++i)
		actuators[i]->init(n);
	
	watchdog = minute();
	watchdogROS = minute();
	if(watchdog > 58)
		watchdog = -1;
	if(watchdogROS > 58)
		watchdogROS = -1;
	
	if(LOGGER)
		Logger::Instance()->Toast("Sailboat is", "Ready!!", 0);
}

void Sailboat::updateSensors(){
	for(int i = 0; i < NB_SENSORS; ++i){
		sensors[i]->update();
	}
	
	for(int i = 0; i < NB_SENSORS_NOT_ROS; ++i){
		sens[i]->update();
	}
}

void Sailboat::updateTestSensors(){
	for(int i = 0; i < NB_SENSORS; ++i)
		sensors[i]->updateT();
}

void Sailboat::communicateData(){
	if(millis() - timerMillisCOM > 2){
		for(int i = 0; i < NB_SENSORS; ++i)
			sensors[i]->communicate();
		timerMillisCOM = millis();
	}
	if(millis() - timerMillisCOMAct > 10){
		for(int i = 0; i < NB_ACTUATORS; ++i)
			actuators[i]->communicateData();
		timerMillisCOMAct = millis();
	}
}


void Sailboat::Control(){
	if(millis() - timerMillis > 100){
		if(controller != NULL){
			controller->Control(cmd);
			watchdog = minute();
		}else{
			if(minute() - watchdog > 1)
				setController(RETURNHOME_CONTROLLER);
		}
		
		if(minute() - watchdogROS > 5){
			if(LOGGER)
				Logger::Instance()->Toast("ROS DEAD??", "ROS DEAD??", 0);
			setController(RETURNHOME_CONTROLLER);
		}
		
		for(int i =0; i < NB_CONTROLLERS; ++i){
			if(controllers[i] != NULL && !controllers[i]->isActivated()){
				controllers[i]->updateBackground();
			}
		}
	}
}