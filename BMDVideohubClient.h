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


/**
  Version 1.0.0
	(Semantic Versioning)
**/


#ifndef BMDVideohubClient_h
#define BMDVideohubClient_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Ethernet.h>
#include <avr/pgmspace.h>

class BMDVideohubClient
{
  private:
	EthernetClient _client;		// Object for communication, see constructor.
	uint16_t _localPort; 		// local port to send from
	IPAddress _videoHubIP;		// IP address of the video hub
	boolean _serialOutput;		// If set, the library will print status/debug information to the Serial object
	
	char _linebuffer[40];		// Buffer for incoming lines.
	uint16_t _linebufferPointer;
	uint8_t _linebufferParsePointer;

	uint8_t _section;
	
	
  public:
    BMDVideohubClient();
    BMDVideohubClient(IPAddress ip);
    void begin(IPAddress ip);
    void connect();
    void runLoop();

  private:
	void _parseline();
	int _parseInt();
	bool _isFirstPartOfLinebuffer(char *firstPartStr);

	char _outputLocks[16];
	uint8_t _outputRouting[16];
	bool _devicePresent;
	uint8_t _numInputs;
	uint8_t _numOutputs;
	uint8_t _numMonOutputs;
	uint8_t _numProcUnits;
	uint8_t _serialPorts;
	
	
	
  public:
	void setRoute(uint8_t outputPortType, uint8_t outputPortIndex, uint8_t inputPortIndex);
	void setLock(uint8_t portType, uint8_t portIndex, char newState);
	uint8_t getRoute(uint8_t portType, uint8_t outputPortIndex);
	void getLock(uint8_t portType, uint8_t portIndex);
	void send();
	
/********************************
 * General Getter/Setter methods
 ********************************/




/********************************
* BMDVideohubClient Switcher state methods
* Returns the most recent information we've 
* got about the switchers state
 ********************************/



/********************************
 * BMDVideohubClient Switcher Change methods
 * Asks the switcher to changes something
 ********************************/

};

#endif

