#include <CAN.h>

#define CHECK_INPUT 0
#define AWAIT_PROP_DATA 1
#define AWAIT_PROP_CONFIRM 2
#define AWAIT_PNEUM_CONFIRM 3
#define CHECK_INPUT_PROP_FAIL 4
#define AWAIT_WHEEL_DATA 5
#define AWAIT_WHEEL_CONFIRM 6
#define AWAIT_PNEUM_CONFIRM_PROP_FAIL 7

const int maxChars = 20;
const int maxResponseTime = 1500; //delay time (milliseconds) until a subsystem is deemed malfunctional

char inputString[maxChars];
char inputType;
char inputParam[maxChars - 2];

int currentState = 0;
int nextState;

int startTime;

float propEncoderVel, propEncoderDist;
float wheelEncoderVel, wheelEncoderDist;

float inputVelocity;

void parseSerial(char *receivedChars[maxChars], int *charIndex) {
	char charBuffer;
  
	*charIndex = 0;

	if(Serial.available() > 0) {
		while(Serial.available() > 0 && *charIndex < maxChars) {
			charBuffer = Serial.read();

			if(*charIndex == maxChars - 1) {
				*receivedChars[*charIndex] = '\n';
				*charIndex++;

			} else if(charBuffer != '\n') {
				*receivedChars[*charIndex] = charBuffer;
				*charIndex++;
			}
		}
	*receivedChars[*charIndex] = '\n';
  }
	return;
}

int sendRtr(long id) { //request certain types data from different subsystems based on id
	int begin, end;

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

int receiveRtr(long id) { //use to check if certain ids are requested
	int packetSize = CAN.parsePacket();

	if(packetSize && CAN.packetRtr() && CAN.packetId() == id) {
		return 1;

	} else {
		return 0;
  }
}

int sendPacketInt(long id, int int0, int int1, int int2, int int3) { //used to send ints across the CAN
	int begin, end;
	int i, j; //data union array indexing

	union data {
		int num;
		byte bytes[sizeof(num)];
	};

	union data input[4];

	input[0].num = int0;
	input[1].num = int1;
	input[2].num = int2;
	input[3].num = int3;
   
	begin = CAN.beginPacket(id);

	for(i = 0; i < 4; i++) { //cycle through the 4 inputs
		for(j = 0; j < sizeof(input[i].num); j++) { //cycle through the 2 bytes of each of the 4 inputs
			CAN.write(input[i].bytes[j]);
		}
	}

	end = CAN.endPacket();
  
	if (begin != 1 || end != 1) { //verify that data is sent correctly
		Serial.print("ERROR: Failed to send packet ");
		Serial.println(id);
		return 0;

	} else {
		return 1;
	}
}

int receivePacketInt(long id, int *int0, int *int1, int *int2, int *int3) { //used to receive ints from the CAN
	int packetSize = CAN.parsePacket();
	int i, j; //data union array indexing

	union data {
		int num;
		byte bytes[sizeof(num)];
	};

	union data output[4];

	if(packetSize && !CAN.packetRtr() && CAN.packetId() == id) {
		for(i = 0; i < 4; i++) { //cycle through the 4 outputs
			for(j = 0; j < sizeof(output[i].num); j++) { //cycle through the 2 bytes of each of the 4 outputs
				output[i].bytes[j] = CAN.read();
			}
		}
		
		*int0 = output[0].num;
		*int1 = output[1].num;
		*int2 = output[2].num;
		*int3 = output[3].num;
		return 1;

	} else {
		return 0;
	}
}

int sendPacketFloat(long id, float float0, float float1) { //used to send floats across the CAN
	int begin, end;
	int i, j; //data union array indexing

	union data {
		float num;
		byte bytes[sizeof(num)];
	};

	union data input[2];

	input[0].num = float0;
	input[1].num = float1;
   
	begin = CAN.beginPacket(id);

	for(i = 0; i < 2; i++) { //cycle through the 2 inputs
		for(j = 0; j < sizeof(input[i].num); j++) { //cycle through the 4 bytes of each of the 2 inputs
			CAN.write(input[i].bytes[j]);
		}
	}
  
	end = CAN.endPacket();
  
	if (begin != 1 || end != 1) { //verify that data is sent correctly
		Serial.print("ERROR: Failed to send packet ");
		Serial.println(id);
		return 0;

	} else {
		return 1;
	}
}

int receivePacketFloat(long id, float *float0, float *float1) { //used to receive floats from the CAN
	int packetSize = CAN.parsePacket();
	int i, j; //data union array indexing

	union data {
		float num;
		byte bytes[sizeof(num)];
	};

	union data output[2];

	if(packetSize && !CAN.packetRtr() && CAN.packetId() == id) {
		for(i = 0; i < 2; i++) { //cycle through the 2 outputs
			for(j = 0; j < sizeof(output[i].num); j++) { //cycle through the 4 bytes of each of the 2 outputs
				output[i].bytes[j];
			}
		}

		*float0 = output[0].num;
		*float1 = output[1].num;
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
		case CHECK_INPUT:
			if(Serial.available() > 0) {
				
				if(inputType = 'p') {
					//send pneumatics
					nextState = AWAIT_PNEUM_CONFIRM;
				} else if(inputType = 'v') {
		  			sendPacketFloat(0xA1, inputVelocity, 0);
		  			//currentVelocity = inputVelocity; put this in state 6
		  			nextState = AWAIT_PROP_CONFIRM;
				} else {
		  			sendRtr(0xA3);
		  			nextState = AWAIT_PROP_DATA;
				}
	  		} else {
				sendRtr(0xA3);
				nextState = AWAIT_PROP_DATA;
	  		}
	  		break;

		case AWAIT_PROP_DATA:
			startTime = millis();
	  		while(!receivePacketFloat(0xA3, &propEncoderVel, &propEncoderDist)) {
				if(millis() - startTime >= maxResponseTime) {
					Serial.println("ERROR: Propulsion failure");
	  				sendPacketInt(0xA2, 1, 0, 0, 0);
	  				nextState = CHECK_INPUT_PROP_FAIL;
	  				goto END1;
				}
	  		}
	  
	  		Serial.print("Velocity: ");
	  		Serial.print(propEncoderVel);
	 		Serial.print("          Distance: ");
	  		Serial.println(propEncoderDist);
	  		nextState = CHECK_INPUT;

	  		END1:
	  		break;

		case AWAIT_PROP_CONFIRM:
			startTime = millis();
	  		while(!receiveRtr(0xA1)) {
				if(millis() - startTime >= maxResponseTime) {
					Serial.println("ERROR: Propulsion failure");
	  				sendPacketInt(0xA2, 1, 0, 0, 0);
	  				nextState = CHECK_INPUT_PROP_FAIL;
	  				goto END2;
				}
	  		}

	  		nextState = CHECK_INPUT;

	  		END2:
	  		break;

		case AWAIT_PNEUM_CONFIRM:
			startTime = millis();
	  		while(!receiveRtr(0xB1)) {
				if(millis() - startTime >= maxResponseTime) {
					Serial.println("ERROR: Pneumatics failure");
					//pneumatics error procedure goes here (not sure yet)
					goto END3;
				}
	  		}

	  		//nextState = ?;

	  		END3:
	  		break;
  	}
  
  currentState = nextState;
}
