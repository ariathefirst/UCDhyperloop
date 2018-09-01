#include <CAN.h>
#include <stdbool.h> //not sure if boolean data type is already included in CAN or Ardiuno

int currentState = 0;
int nextState;

int errorCount = 0;

float propEncoderVel;
float propEncoderDist;

int togglePneumaticsInput(bool frictionBrakeToggle, bool eddyBrakeToggle, int pneumaticsState) { //input the current state and pick which brakes to toggle

	bool pneumaticsBool[4];
	int pneumaticsNextState;
	int i;
	int begin = 2; //set 2 as the default
	int end = 1; //set 1 as the default

	for(i = 0; i <= 3; i++) {
		 pneumaticsState % 2 == 1 ? pneumaticsBool[i] = true : pneumaticsBool[i] = false;
		 pneumaticsState /= 2;
	}

	if(frictionBrakeToggle == true) {
		begin = 0;
	}

	if(eddyBrakeToggle == true) {
		end = 3;
	}

	for(i = begin; i <= end; i++) {
		pneumaticsBool[i] == true ? pneumaticsBool[i] = false : pneumaticsBool[i] = true;
	}

	pneumaticsNextState = (8 * pneumaticsBool[0]) + (4 * pneumaticsBool[1]) + (2 *pneumaticsBool[2]) + (pneumaticsBool[3]);
	return pneumaticsNextState;
}

int sendPacketInt(long id, int data1, int data2, int data3, int data4) { //used to send integers along the CAN bus
	int begin;
	int end;

	begin = CAN.beginPacket(id);
	CAN.write(data1, 2); //integers are constrained to 2 bytes so they must be between 32768 and -32768
	CAN.write(data2, 2);
	CAN.write(data3, 2);
	CAN.write(data4, 2);
	end = CAN.endPacket(id);

	if(begin != 1 || end != 1) { //verify that data is sent correctly
    	Serial.print("ERROR: Failed to send packet ");
    	Serial.println(id);
    	return 0;
  	} else {
		return 1;
	}
}

int sendPacketFloat(long id, float data1, float data2) { //used to send floats across the CAN bus
	int begin;
	int end;

	begin = CAN.beginPacket(id);
	CAN.write(data1, 4);
	CAN.write(data2, 4);
	end = CAN.endPacket(id);

	if(begin != 1 || end != 1) { //verify that data is sent correctly
    	Serial.print("ERROR: Failed to send packet ");
    	Serial.println(id);
    	return 0;
  	} else {
		return 1;
	}
}

int sendRtr(long id) { //request certain types data from different subsystems based on address
	int begin;
	int end;

	begin = CAN.beginPacket(id, 1, true);
	end = CAN.endPacket();

	if(begin != 1 || end != 1) { //verify that data is sent correctly
    	Serial.print("ERROR: Failed to send remote transmission request ");
    	Serial.println(id);
    	return 0;
  	} else {
		return 1;
	}
}

int receivePacketInt(long id, int *data1, int *data2, int *data3, int *data4) { //used to receive integers from the CAN bus 
	int packetSize = CAN.parsePacket();

	if(packetSize && !CAN.packetRtr() && CAN.packetId() == id) {
		*data1 = CAN.read();
		*data2 = CAN.read();
		*data3 = CAN.read();
		*data4 = CAN.read();
		return 1;
	}

	return 0;
}

int receivePacketFloat(long id, float *data1, float *data2) { //used to receive floats from the CAN bus
	int packetSize = CAN.parsePacket();

	if(packetSize && !CAN.packetRtr() && CAN.packetId() == id) {
		*data1 = CAN.read();
		*data2 = CAN.read();
		return 1;
	}

	return 0;
}

int receiveRtr(long id) {
	int packetSize = CAN.parsePacket();

	if(packetSize && CAN.packetRtr() && CAN.packetId() = id) {
		return 1;
	} else {
		return 0;
	}
}

void setup() {
	Serial.begin(9600); //halt operation until serial connection opens
	while(!Serial);

	if(!CAN.begin(500E3)) { //halt operation if CAN won't start
		Serial.println("ERROR: Starting CAN failed");
		while(1);
	}
}

void loop() {
	switch(currentState) {
		case 0:
			if(Serial.available() > 0) {
				//parse serial input
				if(inputPneumatics != currentPneumatics) { //placeholder
					//send pneumatics
					nextState = 3;
				} else if(inputVelocity != currentVelocity) { //placeholder
					sendPacketFloat(0xA1, inputVelocity);
					//currentVelocity = inputVelocity; put this in state 6
					nextState = 2;
				} else {
					sendRtr(0xA3);
					nextState = 1;
				}
			} else {
				sendRtr(0xA3);
				nextState = 1;
			}
			break;

		case 1:
			while(!receivePacketFloat(0xA3, &propEncoderVel, &propEncoderDist)) {
				errorCount < 1000 ? errorCount++ : goto end1;
			}
			
			Serial.print("Velocity: ");
			Serial.print(propEncoderVel);
			Serial.print("          Distance: ");
			Serial.println(propEncoderDist);
			propErrorCount = 0;
			nextState = 0;
			break;

			end1:
			Serial.println("ERROR: Propulsion failure");
			sendPacketInt(0xA2, 1);
			errorCount = 0;
			nextState = 4;
			break;

		case 2:
			while(!receiveRtr(0xA1)) {
				errorCount < 1000 ? errorCount++ : goto end2;
			}

			errorCount = 0;
			nextState = 0;
			break;

			end2:
			Serial.println("ERROR: Propulsion failure");
			sendPacketInt(0xA2, 1);
			errorCount = 0;
			nextState = 4;
			break;

		case 3:
			while(!receiveRtr(0xB1)) {
				errorCount < 1000 ? errorCount++ : goto end3;
			}

			errorCount = 0;
			nextState = 0;
			break;

			end3:
			propErrorCount = 0;
			nextState = 0;
			break;

	}
	
	currentState = nextState;
}
