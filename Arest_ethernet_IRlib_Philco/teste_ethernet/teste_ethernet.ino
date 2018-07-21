/*
 * Uses Arduino Due to redirect terminal commands to the ESP8266
 * USB cable connected to PROGRAMMING port of Due
 * ESP8266 connected to UART 1 of Due
 * 
 * http://startingelectronics.org/articles/ESP8266-testing/
 * 
 * Date: 5 December 2016    Author: W. Smith
 */

void setup() {
  Serial.begin(115200);   // USB serial port of Due (PROGRAMMING)
  Serial1.begin(115200);  // ESP8266 on Serial Port 1 of Due (UART 1)
}

char rx_byte = 0;

void loop() {
  // send terminal byte to ESP8266
  if (Serial.available() > 0) {
    rx_byte = Serial.read();
    Serial1.print(rx_byte);
  }
  // send ESP8266 byte to terminal
  if (Serial.available() > 0) {
    rx_byte = Serial.read();
    Serial.print(rx_byte);
  }
}

