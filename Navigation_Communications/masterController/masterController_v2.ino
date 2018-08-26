#include <CAN.h>
#include <stdbool.h> //not sure if boolean data type is already included in CAN or Ardiuno

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

int sendPacketInt(int data, long id) { //used to send integers along the CAN bus
	int begin;
	int end;
	
	begin = CAN.beginPacket(id);
	CAN.write(data);
	end = CAN.endPacket();

	if(begin != 1 || end != 1) { //verify that data is sent correctly
    		Serial.print("Failed to send packet: ");
    		Serial.println(id);
    		return 0;
  	} else {
		return 1;
	}
}

int receivePacketInt(long desiredId) { //used to receive integers from the CAN bus 
	int packetSize = CAN.parsePacket();

	if(packetSize && CAN.packetRtr() != true && CAN.packetId() == desiredId) { //only receive packets if they have nonzero size
		if(CAN.available() && CAN.peek() != -1) {
			return CAN.read();
		} else {
			return NULL;
		}
	}
}

int sendRtr(long desiredId) { //request data from different subsystems based on address
	int begin;
	int end;

	begin = CAN.beginPacket(desiredId, 1, true);
	end = CAN.endPacket();

	if(begin != 1 || end != 1) { //verify that data is sent correctly
    		Serial.print("Failed to send remote transmission request: ");
    		Serial.println(desiredId);
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
		Serial.println("Starting CAN failed!");
		while(1);
	}
}

void loop() {

	if(Serial.available() > 0) {
		serialBuffer = Serial.read();
	}

	switch(dataTransferState) { //data transfer FSM for sequencing data sent along the CAN bus
		case 1:

		case 2:

		case 3:

		default:
	}
}
