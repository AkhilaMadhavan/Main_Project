#include <Wire.h>
#include <TinyGPS++.h>
#include "MAX30105.h"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
SoftwareSerial myserial(D7,D8);
SoftwareSerial Myserial(D3,D4);
#include "heartRate.h"
static const int RXPin = 4, TXPin = 5;   // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
static const uint32_t GPSBaud = 9600; //if Baud rate 9600 didn't work in your case then use 4800

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);

MAX30105 particleSensor;


char auth[] = "Hkutsq2mDk7HIh75bl9advim-ab-P6Bq";
char ssid[] = "JioFi_245E218";
char pass[] = "g9r6ca6s7i";

int x, y, z, k,l,i,t,h,m,d,a;
String str, temp,lab1,lab2,lab3,lab4,lab5,lab6,lab7;

int xflag, yflag, zflag, bflag;
float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;
unsigned int move_index = 1; 


const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
int avgbpm;
int slot,slot1;

BLYNK_WRITE(V3)
{
  slot = param.asInt();
}

BLYNK_WRITE(V4)
{
  slot1 = param.asInt();
}
void setup()
{
  Serial.begin(9600);
myserial.begin(9600);
Myserial.begin(9600);
 Serial.begin(GPSBaud);
 timer.setInterval(5000L, checkGPS); 
  Blynk.begin(auth, ssid, pass);
  Blynk.notify("NodeMCU connected to Blynk !");
Serial.println("Initializing...");
 if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

 
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
  Blynk.run();
    while (myserial.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(myserial.read()))
        displayInfo();
  }
 long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      avgbpm = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
    Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  avgbpm=beatAvg;
  //Serial.print(avgbpm);
  Blynk.virtualWrite(V2,avgbpm);
   //Blynk.virtualWrite(V1,random(91,100));
//  if ((avgbpm >= 40)and(avgbpm <=100) and irValue > 50000)
//  {
//    Serial.println("normal");
//    Blynk.virtualWrite(V0,avgbpm);
//  }

   
  if (Serial.available())
  {
    str = Serial.readStringUntil('#');
    Serial.print("str = ");
    Serial.println(str);
    
    //temperature

    if (str[0] == 'X')
    {
      Serial.print("t = ");
      i = 1;
      temp = "";
      while (str[i] != '\0')
      {
        temp = temp + str[i];
        i++;
      }
      Serial.println(temp);
      t = temp.toInt();
      Serial.print("T = ");
      Serial.println(t);
     Blynk.virtualWrite(V0, t);


     if(t> 34)
     {
      Blynk.notify("Temperature increased");

      emergency1();
      delay(1000);
      emergency2();
      delay(1000);
      emergency3();
      delay(1000);
     }

     

    }

    //position

    if (str[0] == 'Y')
    {
      Serial.print("h = ");
      i = 1;
      temp = "";
      while (str[i] != '\0')
      {
        temp = temp + str[i];
        i++;
      }
      Serial.println(temp);
      h = temp.toInt();
      Serial.print("H = ");
      Serial.println(h);
      if(h > 100)
      {
        Blynk.notify("Fall detected");
      emergency1();
      delay(1000);
      emergency2();
      delay(1000);
      emergency3();
      delay(1000);
      }


     

    }

  

  
}
if(slot == 1)
{
  lab1 = "Name: SANKARANTAMBI";
  lab2 = "Age : 82";
  lab3 = "Cardiovascular : High blood pressure,Heart Attack, Dizziness"; 
  lab4 = "Respiratory: Asthma, Emphysema";
  lab5 = "Blood Group : A+";
  lab6 = "Doctor's Name: Dr. Sunny";
  lab7 = "Phone number: 4466 ";
  Blynk.virtualWrite(V5, lab1);
  Blynk.virtualWrite(V5, lab2);
  Blynk.virtualWrite(V5, lab5);
  Blynk.virtualWrite(V5, lab3);
  Blynk.virtualWrite(V5, lab4);
  Blynk.virtualWrite(V5, lab6);
  Blynk.virtualWrite(V5, lab7);
  
}
if(slot1 == 1)
{
  Blynk.notify("Message sent");
SendMessage();
}
}
void SendMessage()
{
  Serial.println("atverycritical");
  Myserial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
   Serial.println("at");
  Myserial.println("AT+CMGS=\"+917902939979\"\r"); // Replace x with mobile number
  delay(1000);
   Serial.println("at");
 Myserial.println(" Hi Dr.Sunny, Your patient is in critical situation. Please go through the details --- Name: SANKARANTAMBI - Age : 82 - Cardiovascular : High blood pressure,Heart Attack, Dizziness -Respiratory: Asthma, Emphysema - Blood Group : A+ - Location :- Latitude: "+latitude +"Longitude :"+ longitude);// The SMS text you want to send
  delay(100);
   Serial.println("at");
   Myserial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);

}
void emergency1()
{
  Serial.println("atverycritical");
  Myserial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
   Serial.println("at");
  Myserial.println("AT+CMGS=\"+916282360321\"\r"); // Replace x with mobile number
  delay(1000);
   Serial.println("at");
  Myserial.println(" Hi Neema, Your Father Sankaranthambi needs your help.FALL DETECTED.Please visit him asap.");// The SMS text you want to send
  delay(100);
   Serial.println("at");
   Myserial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);

}

void emergency2()
{
  Serial.println("atverycritical");
  Myserial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
   Serial.println("at");
  Myserial.println("AT+CMGS=\"+917736510228\"\r"); // Replace x with mobile number
  delay(1000);
   Serial.println("at");
  Myserial.println("Hi Afeefa, Your neighbour Sankaranthambi needs your help.FALL DETECTED.Please visit him asap.");// The SMS text you want to send
  delay(100);
   Serial.println("at");
   Myserial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);

}

void emergency3()
{
  Serial.println("atverycritical");
  Myserial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
   Serial.println("at");
  Myserial.println("AT+CMGS=\"+919847587202\"\r"); // Replace x with mobile number
  delay(1000);
   Serial.println("at");
  Myserial.println("Hi Abel, Your Uncle Sankaranthambi needs your help.FALL DETECTED.Please visit him asap.");// The SMS text you want to send
  delay(100);
   Serial.println("at");
   Myserial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);

}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
     // Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
  }
}

void displayInfo()
{ 
  if (gps.location.isValid() ) 
  {    
    float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
    float longitude = (gps.location.lng()); 
    
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6);
     
     
    myMap.location(move_index, latitude, longitude, "GPS_Location");
    spd = gps.speed.kmph();               //get speed
//       Blynk.virtualWrite(V3, spd);
//       
       sats = gps.satellites.value();    //get number of satellites
      // Blynk.virtualWrite(V4, sats);

       bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
       //Blynk.virtualWrite(V5, bearing);                   
  }
  
 Serial.println();
}
