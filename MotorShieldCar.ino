#include <MotorDriver.h>
#include <SoftwareSerial.h>

MotorDriver m;
SoftwareSerial BTserial(0, 1); // RX | TX
const long baudRate = 38400; 
char c=' ';
void setup()
{
 
BTserial.begin(baudRate);  
    Serial.print("BTserial started at "); Serial.println(baudRate);
    Serial.println(" ");

}

void debug() {
  release();
  delay(500);
  forwards();
  delay(3000);
  backwards();
  delay(3000);
  release();
}

void release() {
  m.motor(1, RELEASE, 0);
  m.motor(2, RELEASE, 0);
  m.motor(3, RELEASE, 0);
  m.motor(4, RELEASE, 0);
}


void backwards() {
  m.motor(1, BACKWARD, 80);
  m.motor(2, BACKWARD, 80);
  m.motor(3, FORWARD, 80);
  m.motor(4, FORWARD, 80);
}

void forwards() {
  m.motor(1, FORWARD, 80);
  m.motor(2, FORWARD, 80);
  m.motor(3, BACKWARD, 80);
  m.motor(4, BACKWARD, 80);
}

void loop()
{
 if (BTserial.available())
    {
        c = BTserial.read();
        BTserial.write(c);
        m.motor(1, FORWARD, 80);
        delay(100);
        m.motor(1, RELEASE, 0);
    }
}
