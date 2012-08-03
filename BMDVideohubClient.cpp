/*
Copyright 2012 Kasper Skårhøj, SKAARHOJ, kasperskaarhoj@gmail.com

This file is part of the Blackmagic Design Videohub Client library for Arduino

The BMDVideohubClient library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The BMDVideohubClient library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the BMDVideohubClient library. If not, see http://www.gnu.org/licenses/.

*/



// CURRENTLY ONLY SUPPORT FOR VIDEOHUB MICRO


#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "BMDVideohubClient.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
BMDVideohubClient::BMDVideohubClient(){}
BMDVideohubClient::BMDVideohubClient(IPAddress ip){
	begin(ip);
}

/**
 * Setting up IP address for the videohub (and local port to open telnet connection to)
 */
void BMDVideohubClient::begin(IPAddress ip){

	// Initialize the Ethernet client library
	EthernetClient client;
	_client = client;
	
	_videoHubIP = ip;	// Set video hub IP address
	_localPort = 9990;	// Set local port
	
	_serialOutput = false;
	memset(_linebuffer,0,40-1);
	_linebufferPointer = 0;
	
	_section = 0;
	
	_devicePresent = false;
	_numInputs = 0;
	_numOutputs = 0;
	_numMonOutputs = 0;
	_numProcUnits = 0;
	_serialPorts = 0;	
}

/**
 * Initiating connection handshake to the Videohub
 */
void BMDVideohubClient::connect() {

  // if you get a connection, report back via serial:
  if (_client.connect(_videoHubIP, _localPort)) {
    Serial.println(PSTR("connected"));
  } else {
    // if you didn't get a connection to the server:
    Serial.println(PSTR("connection failed"));
  }
}

/**
 * Reads information from the videohub as it arrives, does the parsing, stores in memory
 */
void BMDVideohubClient::runLoop() {
  	// if there are incoming bytes available 
	// from the server, read them and print them:
	while (_client.available()) {
		char c = _client.read();

		if (c==10)	{	// Line break:
			_parseline();
			memset(_linebuffer,0,40-1);
			_linebufferPointer=0;
		} else if (_linebufferPointer<40-1)	{	// one byte for null termination
			_linebuffer[_linebufferPointer] = c;
			_linebufferPointer++;
		} else {
			Serial.println(PSTR("ERROR: Buffer overflow."));
		}
	}
	
	// if the server's disconnected, stop the client:
	if (!_client.connected()) {
		Serial.println();
		Serial.println(PSTR("disconnecting."));
		_client.stop();
	// do nothing:
		while(true);
	}
}


void BMDVideohubClient::_parseline()	{
	int inputNum;
	int outputNum;
	int portNum;
	
	if (!strcmp(_linebuffer,""))	{
		_section = 0;
	} else if (!strcmp_P(_linebuffer,PSTR("PROTOCOL PREAMBLE:")))	{		//strcmp_P("RAM STRING", PSTR("FLASH STRING"));
		_section = 1;
	} else if (!strcmp_P(_linebuffer,PSTR("VIDEOHUB DEVICE:")))	{
		_section = 2;
	} else if (!strcmp_P(_linebuffer,PSTR("INPUT LABELS:")))	{
		_section = 3;
	} else if (!strcmp_P(_linebuffer,PSTR("OUTPUT LABELS:")))	{
		_section = 4;
	} else if (!strcmp_P(_linebuffer,PSTR("VIDEO OUTPUT LOCKS:")))	{
		_section = 5;
	} else if (!strcmp_P(_linebuffer,PSTR("VIDEO OUTPUT ROUTING:")))	{
		_section = 6;
	} else {
		_linebufferParsePointer=0;
		switch(_section)	{
			case 2: 	// VIDEOHUB DEVICE
				if (_isFirstPartOfLinebuffer("Device present:"))	{
					_linebufferParsePointer+=15+1;
					_devicePresent = _isFirstPartOfLinebuffer("true");
				}
				if (_isFirstPartOfLinebuffer("Video inputs:"))	{
					_linebufferParsePointer+=13+1;
					_numInputs = _parseInt();
				}
				if (_isFirstPartOfLinebuffer("Video processing units:"))	{
					_linebufferParsePointer+=23+1;
					_numProcUnits = _parseInt();
				}
				if (_isFirstPartOfLinebuffer("Video outputs:"))	{
					_linebufferParsePointer+=14+1;
					_numOutputs = _parseInt();
				}
				if (_isFirstPartOfLinebuffer("Video monitoring outputs:"))	{
					_linebufferParsePointer+=25+1;
					_numMonOutputs = _parseInt();
				}
				if (_isFirstPartOfLinebuffer("Serial ports:"))	{
					_linebufferParsePointer+=13+1;
					_serialPorts = _parseInt();
				}
			break;
			case 3: 	// INPUT LABELS
			case 4: 	// OUTPUT LABELS
				portNum = _parseInt();
				if (portNum>=0 && portNum<=15)	{
					_linebufferParsePointer++;
					while(_linebufferParsePointer < _linebufferPointer)	{
						// Here: Traverse every char ofthe label.
						// For future implementation when I know where to store this information! Too little memory.
						//						Serial.print(_linebuffer[_linebufferParsePointer]);
						_linebufferParsePointer++;
					}
				}
			break;
			case 5: 	// VIDEO OUTPUT LOCKS
				outputNum = _parseInt();
				if (outputNum>=0 && outputNum<=15)	{
					_outputLocks[outputNum] = _linebuffer[_linebufferParsePointer+1];
				}
			break;
			case 6: 	// VIDEO OUTPUT ROUTING
				outputNum = _parseInt();
				if (outputNum>=0 && outputNum<=15)	{
					_linebufferParsePointer++;	// Advance 
					inputNum = _parseInt();
					if (inputNum>=0 && inputNum<=15)	{
						_outputRouting[outputNum] = inputNum;
					}
				}
			break;
		}
		
		
		
		
		
		
	}
	
		// Content parsing:
		Serial.print(_section);
		Serial.print(": ");
		Serial.println(_linebuffer);
	
}

int BMDVideohubClient::_parseInt()	{
	int output = -1;
	while(_linebufferParsePointer < _linebufferPointer)	{
		if (_linebuffer[_linebufferParsePointer]>=48 && _linebuffer[_linebufferParsePointer]<=57)	{
			if (output==-1)	output=0;
			output = output*10 +_linebuffer[_linebufferParsePointer]-48;
		} else break;
		_linebufferParsePointer++;
	}
	return output;
}

bool BMDVideohubClient::_isFirstPartOfLinebuffer(char *firstPartStr)	{
	for (uint8_t i=0; i<strlen(firstPartStr); i++)	{
		if (_linebufferParsePointer+i >= _linebufferPointer || firstPartStr[i]!=_linebuffer[_linebufferParsePointer+i])	{
			return false;
		}
	}
	return true;
}



/*
A serial port = 1
A video input port = 2
A video output port = 3
A video monitoring output port = 4
*/

void BMDVideohubClient::setRoute(uint8_t outputPortType, uint8_t outputPortIndex, uint8_t inputPortIndex)	{
	if (outputPortType==3)	{
		_client.println("VIDEO OUTPUT ROUTING:"); 
		_client.print(outputPortIndex,DEC); 
		_client.print(" "); 
		_client.print(inputPortIndex,DEC); 
		_client.println(""); 
	}
}
void BMDVideohubClient::send()	{
	_client.println(""); 
}
void BMDVideohubClient::setLock(uint8_t portType, uint8_t portIndex, char newState)	{
		// Missing implementation... TODO
}
uint8_t BMDVideohubClient::getRoute(uint8_t portType, uint8_t outputPortIndex)	{
	if (portType==3)	{
		return _outputRouting[outputPortIndex];	// TODO: Check port index is 0-15
	}
}
void BMDVideohubClient::getLock(uint8_t portType, uint8_t portIndex)	{
		// Missing implementation... TODO
}
