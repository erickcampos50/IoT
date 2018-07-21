/*
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Ethernet library (for example to be used with the Ethernet shield).
  This example illustrate the cloud part of aREST that makes the board accessible from anywhere
  See the README file for more details.

  Written in 2015 by Marco Schwartz under a GPL license.
*/

// Libraries
#include <SPI.h>
#include "Ethernet.h"
#include <PubSubClient.h>
#include <aREST.h>

#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
IRsendRaw mySender;

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x40 };

// Clients
EthernetClient ethClient;
PubSubClient client(ethClient);

// Create aREST instance
aREST rest = aREST(client);

// Variables to be exposed to the API
//int temperature;
//int humidity;

//Variaveis armazenando os codigos das funcoes
#define RAW_DATA_LEN 68
uint16_t power[RAW_DATA_LEN] ={9000,4550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,1700,580,1700,580,1700,580,1700,580,1700,580,1700,580,550,580,1700,580,550,580,550,580,550,580,1700,580,1700,580,550,580,550,580,550,580,1700,580,1700,580,1700,580,550,580,550,580,1700,580,1700,580,1700,580 };
uint16_t vol_mais[RAW_DATA_LEN]= {9000,4550,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,580,550,1700,550,580,550,580,550,580,550,1700,550,580,550,580,550,1700,550,580,550,1700,550,1700,550,1700,550 };
uint16_t vol_menos[RAW_DATA_LEN]= {9000,4550,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,580,550,580,550,580,550,1700,550,580,550,1700,550,580,550,580,550,1700,550,1700,550,1700,550 };
uint16_t canal_mais[RAW_DATA_LEN]= {9000,4550,550,580,550,550,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,580,1700,550,1700,550,580,550,1700,550,580,550,580,550,1700,550,580,550,1700,550,580,550,580,550,580,550,1700,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,1700,550};
uint16_t canal_menos[RAW_DATA_LEN]={9000,4550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,1700,580,1700,580,1700,580,1700,580,1700,580,1700,580,550,580,1700,580,550,580,550,580,550,580,1700,580,1700,580,550,580,550,580,550,580,1700,580,1700,580,1700,580,550,580,550,580,1700,580,1700,580,1700,580 };

// ID to identify the device on the cloud (should be 6 characters long)
char* device_id = "091193";

void setup(void)
{
  // Start Serial
  Serial.begin(115200);
  Serial.println("Iniciando device_id=091193");
  // Set callback
  client.setCallback(callback);

// Init variables and expose them to REST API
//  temperature = 24;
//  humidity = 40;
//  rest.variable("temperature",&temperature);
//  rest.variable("humidity",&humidity);

//Declares the function in the Arduino sketch for Arest
  rest.function("power",tv_power);
  rest.function("vol+",tv_vol_mais);
  rest.function("vol-",tv_vol_menos);
  rest.function("ch+",tv_canal_mais); 
  rest.function("ch-",tv_canal_menos);


  // Give name & ID to the device
  rest.set_id(device_id);
  rest.set_name("ethernet");

  // Start the Ethernet connection and the server
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  // Set output topic
  char* out_topic = rest.get_topic();

}

void loop() {

  // Connect to the cloud
  rest.handle(client);

}

// Handles message arrived on subscribed topic(s)
void callback(char* topic, byte* payload, unsigned int length) {

  rest.handle_callback(client, topic, payload, length);

}

int tv_power(){
  mySender.send(power,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println(F("Sent signal."));
  return 0;
}

int tv_canal_mais(){
  mySender.send(canal_mais,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println(F("Sent signal."));
  return 0;
}

int tv_canal_menos(){
  mySender.send(canal_menos,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println(F("Sent signal."));
  return 0;
}

int tv_vol_mais(){
  mySender.send(vol_mais,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println(F("Sent signal."));
  return 0;
}

int tv_vol_menos(){
  mySender.send(vol_menos,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println(F("Sent signal."));
  return 0;
}


