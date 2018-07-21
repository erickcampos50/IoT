/*
  This a simple example of the aREST Library for the ESP8266 WiFi chip.
  This example illustrate the cloud part of aREST that makes the board accessible from anywhere
  See the README file for more details.

  Written in 2016 by Marco Schwartz under a GPL license.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <aREST.h>


#include <IRremoteESP8266.h>
#include <IRsend.h>

#define IR_LED 4  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.
// Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Create aREST instance
aREST rest = aREST(client);

// Unique ID to identify the device for cloud.arest.io
char* device_id = "091193";

// WiFi parameters
const char* ssid = "Josiele";
const char* password = "NYF71900";

// Variables to be exposed to the API
int temperature;
int humidity;

// Functions
void callback(char* topic, byte* payload, unsigned int length);

//Variaveis armazenando os codigos das funcoes
#define RAW_DATA_LEN 68
uint16_t power[RAW_DATA_LEN] ={9000, 4550, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 580, 1700, 550, 1700, 550, 580, 550, 1700, 550, 1700, 550, 1700, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1711, 550, 1700, 550 };
uint16_t vol_mais[RAW_DATA_LEN]= {9000,4550,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,580,550,1700,550,580,550,580,550,580,550,1700,550,580,550,580,550,1700,550,580,550,1700,550,1700,550,1700,550 };
uint16_t vol_menos[RAW_DATA_LEN]= {9000,4550,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,580,550,580,550,580,550,1700,550,580,550,1700,550,580,550,580,550,1700,550,1700,550,1700,550 };
uint16_t canal_mais[RAW_DATA_LEN]= {9000,4550,550,580,550,550,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,580,1700,550,1700,550,580,550,1700,550,580,550,580,550,1700,550,580,550,1700,550,580,550,580,550,580,550,1700,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,1700,550};
uint16_t canal_menos[RAW_DATA_LEN]={9000,4550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,1700,580,1700,580,1700,580,1700,580,1700,580,1700,580,550,580,1700,580,550,580,550,580,550,580,1700,580,1700,580,550,580,550,580,550,580,1700,580,1700,580,1700,580,550,580,550,580,1700,580,1700,580,1700,580 };

#define RAW_DATA_LEN2 198 //Valor exclusivo para o AC Rheem
uint16_t power_ac[RAW_DATA_LEN2] ={6100,7400,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,580,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,580,550,1700,550,1700,550,1700,550,1700,550,1700,550,580,550,1700,550,1700,550,580,550,580,550,580,550,580,550,580,550,1700,550,580,550,580,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,580,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,580,550,1700,550,1700,550,7400,550}

int tv_power(String a){
  irsend.sendRaw(power,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println("Sent signal.");
  return 0;
}

int tv_canal_mais(String b){
  irsend.sendRaw(canal_mais,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println("Sent signal.");
  return 0;
}

int tv_canal_menos(String c){
  irsend.sendRaw(canal_menos,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println("Sent signal.");
  return 0;
}

int tv_vol_mais(String d){
  irsend.sendRaw(vol_mais,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println("Sent signal.");
  return 0;
}

int tv_vol_menos(String e){
  irsend.sendRaw(vol_menos,RAW_DATA_LEN,38);//Pass the buffer,length, optionally frequency
  Serial.println("Sent signal.");
  return 0;
}

int ac_power(String a){
  irsend.sendRaw(power_ac,RAW_DATA_LEN2,38);//Pass the buffer,length, optionally frequency
  Serial.println("Sent signal.");
  return 0;
}
void setup(void)
{
  // Start Serial
  Serial.begin(115200);
  // Set callback
  client.setCallback(callback);


  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

  irsend.begin();
  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id(device_id);
  rest.set_name("node1");

//Declares the function in the Arduino sketch for Arest
  rest.function("power",tv_power); //o primeiro termo é como arest vai ler essa funcao e o segundo é o nome da função declarada dentro do sketch
  rest.function("vol_mais",tv_vol_mais);
  rest.function("vol_menos",tv_vol_menos);
  rest.function("canal_mais",tv_canal_mais); 
  rest.function("canal_menos",tv_canal_menos);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

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





