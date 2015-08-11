/*****************
 * Example: Video Hub
 *
 * - kasper & tom
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an BlackMagic Design Videohub (Micro for instance) connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// <= SETUP" is found!
 */




#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>


// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the switcher is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE8, 0xE9 };		// <= SETUP
IPAddress ardip(192, 168, 0, 22);		// <= SETUP

IPAddress vhip(192, 168, 0, 50);                // <= SETUP (the IP address of the Videohub)

// This sketch will write debug info. to the serial port and will cycle the first 12 outputs through each of the first 12 inputs.
uint8_t i = 1;
uint8_t j = 1;
//counters for the loop

#include <BMDVideohubClient.h>

BMDVideohubClient Videohub;

void setup() {

  // start the Ethernet connection:
  Ethernet.begin(mac, ardip);

  // start the serial library:
  Serial.begin(9600);
  
  Videohub.serialOutput = true;

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  Videohub.begin(vhip);
  Videohub.connect();
}

void loop() {
  
  Videohub.runLoop();
  
  delay(3000);
  
  Videohub.setRoute(i, j);
  j++;
  if (j > 12) {
    i++;
    j = 1;
  }
  if (i > 12) {
    i = 1;
  }
}



