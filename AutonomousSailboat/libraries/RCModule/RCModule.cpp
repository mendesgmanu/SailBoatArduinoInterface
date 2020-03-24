#include <RCModule.h>
#include <Arduino.h>
#include <Sailboat.h>

void RC::updateMeasures(){
	if(millis() - watchdog > 10000 && controlling){
		controlling = false;
		Sailboat::Instance()->setController(previousController);
		Serial.println("Setting mode to Previous controller: " + String(previousController));
	}
}

void RC::interruptCH(uint8_t channel, uint8_t pin){
	//Serial.println(channel + String(", ") + String(pin) + String(" Interrupted"));
	if (digitalRead(pin) == HIGH) {
		rc_start[channel] = micros();
		//Serial.println("HIGH");
	} else {
		//Serial.println("LOW");
		uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
		switch(channel) {
			case 0:
				if (rc_compare > RC_1_MAX) {
					rc_compare = RC_1_MAX-1;
				}
				if (rc_compare < RC_1_MIN) {
					rc_compare = RC_1_MIN+1;
				}
				break;
			case 1:		
				if (rc_compare > RC_2_MAX) {
					rc_compare = RC_2_MAX-1;
				}
				if (rc_compare < RC_2_MIN) {
					rc_compare = RC_2_MIN+1;
				}
				break;
		}
		
		rc_values[channel] = rc_compare;	
		if (counter[channel] > sizeof(rc_prevValues[channel])-1) {
			counter[channel] = 0;
		} else {
			counter[channel] = counter[channel] + 1;
		}
		//Serial.println(sizeof(rc_prevValues[channel]));
		//Serial.println(counter[channel]);
		//Serial.println("");
	
		rc_prevValues[channel][counter[channel]%sizeof(rc_prevValues[channel])-1] = rc_values[channel];
		if (counter[channel] > sizeof(rc_prevValues[channel])-1) {
			uint16_t total = 0;
			for(int i = 0; i < sizeof(rc_prevValues[channel]); ++i){
				total = total + rc_prevValues[channel][i];
			}
			float mean = total/sizeof(rc_prevValues[channel]);
		
			
			if (abs(prevMean[channel] - mean) > 50) {
				Serial.println("Changing to controlling");
				if(!controlling){
					previousController = Sailboat::Instance()->actualControllerIndex();
					Sailboat::Instance()->setController(RC_CONTROLLER);
					Serial.println("Setting mode to RC");
				}
				controlling = true;
				watchdog = millis();
			}
			prevMean[channel] = mean;
		}
	}
	
		
}

void(* resetFunc) (void) = 0;

void RC::resetArduino(uint8_t channel, uint8_t pin) {
	

	if (digitalRead(pin) == HIGH) {
		rc_start[channel] = micros();
		//Serial.println("HIGH");
	} else {
		
		uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
		rc_values[channel] = rc_compare;

		if (rc_values[channel] > 1900){
			Serial.println("reset Arduino called");
			Serial.println(rc_values[channel]);
			delay(1000);
			resetFunc();
			delay(100);
		}
	}
}
