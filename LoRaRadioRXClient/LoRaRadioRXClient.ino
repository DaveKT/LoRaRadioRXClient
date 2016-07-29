/*
* Author: David Kolet-Tassara
* Date:   July 26, 2016
* Rev:    1.0
* Description:
*   This sketch uses the RadioHead library to send and receive messages via the
*   RFM95W LoRa Packet Radio sold by Adafruit. More information on the radio may
*   be found here: https://www.adafruit.com/product/3072
*   Uses the Serial Monitor of the Arduino IDE as the message console.
* Part List:
*   Arduino/Genuino Uno
*   RFM95W LoRa Packet Radio
*   Edge-Launch SMA Connector for 1.6mm / 0.062" Thick PCBs
*   Right-angle Mini GSM/Cellular Quad-Band Antenna - 2dBi SMA Plug
* Attribution:
*   Parts of the code were taken from the tutorial for the RFM9x LoRa Packet Radio
*   written by Limor Fried (ladyada).
*   RadioHead Libraries were written by Mike McCauley.
*   http://www.airspayce.com/mikem/arduino/RadioHead/index.html
*   
*   //TODO
*   1.  Review code comments
*/

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0 //acceptable values are 868 or 915 MHz
#define RF95_PWR 23 //acceptable range is between 5 and 23 dBm
String OPERATOR = "DKT"; //name of the user sending from this console

RH_RF95 rf95(RFM95_CS, RFM95_INT);

String message="";

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("Booting LoRa Radio");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa Radio Init Failed");
    while (1);
  }
  Serial.println("LoRa Radio OK!");

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Freq set to: "); Serial.print(RF95_FREQ); Serial.println("MHz");
  
  rf95.setTxPower(RF95_PWR, false);
  Serial.print("TXPower set to: "); Serial.print(RF95_PWR); Serial.println("dBm");

  Serial.println("Ready!");
}

void loop() {
  
  message=Serial.readString();

  if (message != "") {

    message = OPERATOR+": "+message;
     
    //explode string to char array for sending
    int messageSize = message.length()+1;
    char radiopacket[messageSize];
    message.toCharArray(radiopacket, messageSize);

    //send the message OTA
    Serial.println("Sending..."); delay(10);
    rf95.send((uint8_t *)radiopacket, messageSize);
    delay(10);
    rf95.waitPacketSent();

    //report and clean up
    Serial.println(message);
    message="";
    
  } else {

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: "); Serial.println((char*)buf);
      Serial.print("RSSI: "); Serial.println(rf95.lastRssi(), DEC);
    }
  }
}
