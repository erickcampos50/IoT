/*
  This a simple example of the aREST Library for the Arduino/Genuino MKR1000 board.
  See the README file for more details.

  Written in 2016 by Marco Schwartz under a GPL license.
*/

// Libraries
#include <SPI.h>
#include "Ethernet.h"
#include <PubSubClient.h>
#include <aREST.h>
#include <avr/wdt.h>

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x40 };

// Clients
EthernetClient ethClient;
PubSubClient client(ethClient);

// Create aREST instance
aREST rest = aREST(client);

// Device ID for the device on the cloud (should be 6 characters long)
char * device_id = "erick0";

// Variables to be exposed to the API
int temperature;
int humidity;
String local_ip = "";

// Declare functions to be exposed to the API
int ledControl(String command);

// Callback function for the cloud connection
void callback(char* topic, byte* payload, unsigned int length);

// IP address in case DHCP fails
IPAddress ip(192,168,2,2);

// Ethernet server
EthernetServer server(80);

void setup(void)
{
    // Start the Ethernet connection and the server
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  // Start Serial
  Serial.begin(115200);

  // Set callback
  client.setCallback(callback);

  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);
  rest.variable("local_ip", &local_ip);

  // Function to be exposed
  rest.function("led",ledControl);

  // Give name and ID to device (ID should be 6 characters long)
  rest.set_id(device_id);
  rest.set_name("megaethernet");

  

  // Start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

 
}

void loop() {

  // Connect to the cloud
  rest.handle(client);

  
  

  // Handle REST calls
  EthernetClient localClient = server.available();
  if (!localClient) {
    return;
  }
  while(!localClient.available()){
    delay(1);
  }
  rest.handle(localClient);
 
}

// Custom function accessible by the API
int ledControl(String command) {
  Serial.print("FUNCAO FUNCIONOU");
  // Get state from command
  int state = command.toInt();

  digitalWrite(6,state);
  digitalWrite(5,HIGH);
  pinMode(5, OUTPUT);
  return 1;
}


// Handles message arrived on subscribed topic(s)
void callback(char* topic, byte* payload, unsigned int length) {

  // Handle
  rest.handle_callback(client, topic, payload, length);

}

// Convert IP address to String
String ipToString(IPAddress address)
{
  return String(address[0]) + "." +
    String(address[1]) + "." +
    String(address[2]) + "." +
    String(address[3]);
}
