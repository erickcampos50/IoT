/*
  This a simple example of the aREST Library for the ESP8266 WiFi chip.
  This example illustrate the cloud part of aREST that makes the board accessible from anywhere
  See the README file for more details.

  Written in 2016 by Marco Schwartz under a GPL license.

  VERSÃO ADAPTADA PARA FUNCIONAR TANTO NA CLOUD QUANTO LOCALMENTE
  INFORMAÇÕES SÃO EXIBIDAS SOBRE NOME DA REDE (CASO SEJA NECESSÁRIO CRIAR UMA), O IP PARA ACESSO LOCAL E O DEVICE_ID PARA CRIAÇÃO DE NOVAS DASHBOARDS NO AREST.IO
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <aREST.h>

//Bibliotecas referentes a emissão de IR
#include <IRremoteESP8266.h>
#include <IRsend.h>

//Gerenciamento do WIFI
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//Biblioteca para viabilizar atualizações OTA
#include <ArduinoOTA.h>

//para simplificar a função de sinalização do LED
#include <Ticker.h> //https://github.com/esp8266/Arduino/blob/master/libraries/Ticker/examples/TickerBasic/TickerBasic.ino
Ticker ticker;
#define LED_Status 0 //LED de sinalização para os eventos (D3)

//Porta utilizada para emissão de IR, essa é a melhor até o momento. Não usar em hipotese alguma D4 ou irá causar um bug fisico e o ESP entrará em loop
#define IR_LED 4  // ESP8266 GPIO pin to use for IR. Recommended: 4 (D2).

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.
// Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Create aREST instance
aREST rest = aREST(client);

// Unique ID to identify the device for cloud.arest.io
char* device_id = "UFJFnode01";

// Variables to be exposed to the API
String local_ip = "";

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Functions
void callback(char* topic, byte* payload, unsigned int length);

void tick() //Utilizada para piscar complementar o LED de sinalização, ainda em testes
{
  //toggle state
  int state = digitalRead(LED_Status);  // get the current state of GPIO1 pin
  digitalWrite(LED_Status, !state);     // set pin to the opposite state
}

//Variaveis armazenando os codigos das funcoes
#define RAW_DATA_LEN2 198 //Valor exclusivo para o AC Rheem
uint16_t rheem_power[RAW_DATA_LEN2] = {6100, 7400, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 1700, 550, 580, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 1700, 550, 580, 550, 1700, 550, 1700, 550, 580, 550, 580, 550, 580, 550, 580, 550, 580, 550, 1700, 550, 580, 550, 580, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 580, 550, 1700, 550, 1700, 550, 7400, 550};

int rheem_power_on(String a) {
  irsend.sendRaw(rheem_power, RAW_DATA_LEN2, 38); //Pass the buffer,length, optionally frequency
  Serial.println("rheem_power_on");
  ticker.attach(0.05, tick);
  delay(300);
  ticker.detach();
  return 0;
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.5, tick);
}

int esp_reset (String a) { //para resetar o ESP via Wifi
  Serial.println("Reiniciando o módulo");
  ticker.attach(0.2, tick);
  delay(2000);
  ESP.reset();
  return 0;
}

int wifi_reset (String a) { //para limpar as configurações salvas de credenciais e forcar o modo AP
  Serial.println("Resetando as configurações de credenciais de Wifi");
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  esp_reset("string");
  return 0;
}

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Set callback
  client.setCallback(callback);

  //LED de sinalização para os eventos
  pinMode(LED_Status, OUTPUT);

  //Ativando o emissor de IR
  irsend.begin();

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id(device_id);
  rest.set_name("node1");

  //Declares the function in the Arduino sketch for Arest
  rest.function("rheem_power_on", rheem_power_on);
  rest.function("wifi_reset", wifi_reset);
  rest.function("esp_reset", esp_reset);


  // Init variables and expose them to REST API
  rest.variable("local_ip", &local_ip);


  WiFiManager wifiManager;  //funcao que irá iniciar a configuração do wifi baseado nas ultimas credenciais fornecidas

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  //wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("UFJFnode01", "nodemcu01");

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("UFJFnode01", "nodemcu01")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("Conectado a uma rede WiFi com sucesso. Prosseguindo.");
  ticker.attach(0.5, tick);
  delay(3000);
  ticker.detach();
  digitalWrite(LED_Status, LOW);

  // Iniciar servidor local
  server.begin();
  Serial.println("Local server started on IP:");

  // Print the IP address
  Serial.println(WiFi.localIP());
  local_ip = ipToString(WiFi.localIP());

  //Exibe informações importantes para criação de interface e rede wireless
  Serial.print("Device id: ");
  Serial.println(device_id);

  // Set output topic
  char* out_topic = rest.get_topic();

  //Seção referente ao OTA
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  //  ArduinoOTA.setPassword((const char *)"nodemcu01"); //Esse é o password para fazer uplado via OTA, deve ser o mesmo do AP para evitar confusões
  
  ArduinoOTA.begin();
}

void loop() {


  // Connect to the cloud
  rest.handle(client);

  // GERENCIAR CHAMADAS LOCAIS
  WiFiClient clientLocal = server.available();
  if (!clientLocal) {
    return;
  }
  while (!clientLocal.available()) {
    delay(1);
  }
  rest.handle(clientLocal);

  //Função referente ao OTA
  ArduinoOTA.handle();
}

// Handles message arrived on subscribed topic(s)
void callback(char* topic, byte* payload, unsigned int length) {
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
