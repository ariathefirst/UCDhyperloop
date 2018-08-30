#include <CAN.h>
#include <stdbool.h> //not sure if boolean data type is already included in CAN or Ardiuno

int currentState = 1;
int nextState;

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

int sendPacketInt(long id, int data) { //used to send integers along the CAN bus
	int begin;
	int end;
	
	begin = CAN.beginPacket(id);
	CAN.write(data);
	end = CAN.endPacket();

	if(begin != 1 || end != 1) { //verify that data is sent correctly
    	Serial.print("ERROR: Failed to send packet ");
    	Serial.println(id);
    	return 0;
  	} else {
		return 1;
	}
}

int receivePacketInt(long id, int data) { //used to receive integers from the CAN bus 
	int packetSize = CAN.parsePacket();

	if(packetSize && CAN.packetRtr() != true && CAN.packetId() == id) { //only receive packets if they have nonzero size
		if(CAN.available() && CAN.peek() != -1) {
			return CAN.read();
		} else {
			return NULL;
		}
	}
}

int sendRtr(long id) { //request data from different subsystems based on address
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

int receiveCallback() {

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
		case 1:
			if(Serial.available() > 0) {
				//parse serial input
				if(inputPneumatics != currentPneumatics) { //placeholder
					//send pneumatics
					nextState = 4;
				} else if(inputVelocity != currentVelocity) {
					sendPacketInt(0x1B, inputVelocity);
					//currentVelocity = inputVelocity; put this in state 6
					nextState = 6;
				} else {
					sendRtr(0x1A);
					nextState = 2;
				}
			} else {
				sendRtr(0x1A);
				nextState = 2;
			}
	}
	currentState = nextState;
}
