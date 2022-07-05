#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5);

void setup()
{

  Serial.begin(9600);
  // Setting the baud rate of GSM Module

  mySerial.begin(9600);
  mySerial.write("AT\r");             // because of SIM800L autobounding mode
  delay(100);
 
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1 second
  mySerial.println("AT+CMGS=\"+919947980023\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("Name");// The SMS text you want to send
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module
  delay(1000);
  Serial.print(mySerial.readString());


}


void loop()
{


}
