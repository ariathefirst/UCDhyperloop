#include <CAN.h>
#include <stdbool.h> //not sure if boolean data type is already included in CAN or Ardiuno


int togglePneumaticsInput(bool frictionBrakeToggle, bool eddyBrakeToggle, int pneumaticsState) {

	bool pneumaticsBool[4];
	int pneumaticsNextState;
	int i;
	int j; //maybe I can combine i and j to save memory
	int begin = 2; //set 2 as the default
	int end = 1; //set 1 as the default

	for(i = 0; i <= 3; i++) {
		 pneumaticsState % 2 == 1 ? pneumaticsBool[i] = true : pneumaticsBool[i] = false;
		 pneumaticsState = pneumaticsState / 2;
	}

	if(frictionBrakeToggle == true) {
		begin = 0;
	}

	if(eddyBrakeToggle == true) {
		end = 3;
	}

	for(j = begin; j <= end, i++) {
		pneumaticsBool[i] == true ? pneumaticsBool[i] = false : pneumaticsBool[i] = true;
	}

	pneumaticsNextState = (8 * pneumaticsBool[0]) + (4 * pneumaticsBool[1]) + (2 *pneumaticsBool[2]) + (pneumaticsBool[3]);
	return pneumaticsNextState;
}

void setup() {
	
	Serial.begin(9600); //open serial port
	while(!Serial); //halt operation until serial connection opens

	if(!CAN.begin(500E3)) { //halt operation if CAN won't start
		Serial.println("Starting CAN failed!");
		while(1);
	}

}

void loop() {

	if(Serial.available() > 0) {
		serialBuffer = Serial.read();

	}



}
