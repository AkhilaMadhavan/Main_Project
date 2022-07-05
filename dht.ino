#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;

int dht = A3;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(dht, INPUT);

}

void loop() {
DHT.read(dht);  
int dhtvalue = DHT.temperature;
Serial.print(dhtvalue);
delay(2000);
}
