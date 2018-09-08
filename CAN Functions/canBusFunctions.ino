#include <CAN.h>

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

	if(packetSize && CAN.packetRtr() && CAN.packetId() = id) {
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
