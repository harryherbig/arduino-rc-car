#include <MotorDriver.h>
#include <Eventually.h>
#include <Servo.h>

MotorDriver m;
EvtManager mgr;
Servo servo1;

const int trigPin = A1;
const int echoPin = A0;
#define START_STOP_PIN A5
//const int START_STOP_PIN = A5;

const int speedLimit = 150;
const int turnSpeed = 100;
const int safeDistance = 40;

const int leftAngle = 10;
const int neutralAngle = 90;
const int rightAngle = 170;

const int directionLeft = -1;
const int directionNeutral = 0;
const int directionBack = 10;
const int directionRight = 1;


long duration;
int distance;

bool goingForwards = false;
bool goingBackwards = false;
bool turning = false;

bool shouldMove = false;

void setup()
{

  Serial.begin(115200);
  Serial.println("Serial at 115200 ");

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(START_STOP_PIN, INPUT_PULLUP);

  servo1.attach(10);
  lookLeft();
  delay(1200);
  lookRight();
  delay(1200);
  lookFront();
  mgr.addListener(new EvtPinListener(START_STOP_PIN, 40, LOW, (EvtAction)switchDrivingState));
//  mgr.addListener(new EvtPinListener(startButtonPin, (EvtAction)switchDrivingState));
  mgr.addListener(new EvtTimeListener(100, true, (EvtAction)doKIstuff));
  mgr.addListener(new EvtTimeListener(1000, true, (EvtAction)debugPrint));
}

bool switchDrivingState() {
  shouldMove = !shouldMove;
  Serial.print("Is driving: ");
  Serial.println(shouldMove ? "YES" : "NO");
}



USE_EVENTUALLY_LOOP(mgr) // Use this instead of your loop() function.

boolean debugPrint() {
  Serial.print("Distance: ");
  Serial.println(distance, DEC);
//  Serial.print("Turning: ");
//  Serial.println(turning ? "YES" : "NO");
    Serial.print("Is driving: ");
  Serial.println(shouldMove ? "YES" : "NO");
//  Serial.print("goingForwards: ");
//  Serial.println(goingForwards ? "YES" : "NO");
}

bool measure() {
  distance = getDistance();
  return true;
}

int getDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  return distance;
}
int getFreeDirection() {
  lookFront();
  delay(5000);
  lookLeft();
  delay(5000);
  int leftDistance = getDistance();
  Serial.println("leftDistance: " + leftDistance);
  lookFront();
  delay(5000);
  lookRight();
  delay(2000);
  int rightDistance = getDistance();
  Serial.println("rightDistance: " + rightDistance);
  lookFront();
  
  delay(2000);
  if (rightDistance < 80 && leftDistance > 80 ) {
    Serial.println("go right");
    return directionRight;
  } else if (rightDistance > 80 && leftDistance < 80 ) {
    Serial.println("go left");
    return directionLeft;
  } else {
    Serial.println("go right");
    return directionBack;
    }
  
}

bool release() {
  if (turning || goingForwards) {
    Serial.println("Stopping Car");
    m.motor(1, RELEASE, 0);
    m.motor(2, RELEASE, 0);
    m.motor(3, RELEASE, 0);
    m.motor(4, RELEASE, 0);
    goingForwards = false;
    turning = false;
  } else {
    Serial.print(".");
  }
  return true;
}
void lookRight() {
  servo1.write(rightAngle);
//  Serial.println("Looking right");
}
void lookLeft() {
  servo1.write(leftAngle);
//  Serial.println("Looking left");
}
void lookFront() {
  servo1.write(neutralAngle);
//  Serial.println("Looking front");
}

bool backwards() {
  if (!goingBackwards && shouldMove) {
    Serial.println("Starting going backwards");
    goingBackwards = true;
    goingForwards = false;
    turning = false;
    for (int i = 10; i <= speedLimit; i = i + 5) {
      m.motor(1, BACKWARD, i);
      m.motor(2, BACKWARD, i);
      m.motor(3, BACKWARD, i);
      m.motor(4, BACKWARD, i);
      delay(5);
    }
    delay(3000);
  }
  return true;
}

boolean forwards() {
  if (goingForwards && shouldMove) {
    // keep goingForwards
  } else {
    Serial.println("Starting going forwards");
    goingForwards = true;
    turning = false;
    for (int i = 10; i <= speedLimit; i = i + 10) {
      m.motor(1, FORWARD, i);
      m.motor(2, FORWARD, i);
      m.motor(3, FORWARD, i);
      m.motor(4, FORWARD, i);
      delay(10);
    }
  }
  return true;
}


boolean left() {
  if (!turning && shouldMove) {
    Serial.println("Starting left turn");
    if (goingForwards) {
      release();
    }
    turning = true;
    for (int i = 10; i <= turnSpeed; i = i + 10) {
      m.motor(1, FORWARD, i);
      m.motor(3, FORWARD, i);
      m.motor(2, BACKWARD, i);
      m.motor(4, BACKWARD, i);
    }
  }
  return true;
}

boolean right() {
  if (!turning && shouldMove) {
    Serial.println("Starting right turn");
    if (goingForwards) {
      release();
    }
    turning = true;
    for (int i = 10; i <= turnSpeed; i = i + 10) {
      m.motor(1, BACKWARD, i);
      m.motor(3, BACKWARD, i);
      m.motor(2, FORWARD, i);
      m.motor(4, FORWARD, i);
    }
  }
  return true;
}

// the loop
bool doKIstuff() {
  measure();
  reactToDistance();
  return true;
}

int chooseDirection() {
  int direction = getFreeDirection();
  if (direction == directionLeft) {
    left();
  } else if (direction == directionRight) {
    right();
  } else if (direction == directionBack) {
    backwards();
  }
  return direction;
}

bool reactToDistance()
{
  if (distance < 5 ) {
    // error reading of "0" or emergency stop
    release();
  } else if (distance > safeDistance && !goingForwards) {
    Serial.println("Start goingForwards");
    forwards();
  } else if (distance > safeDistance && goingForwards) {
    // keep goingForwards
  } else if (distance <= safeDistance) {
    if (turning) {
      // keep turning
    } else { //
      Serial.println("Choosing direction");
      if (chooseDirection() == directionBack); {
        chooseDirection(); // after driving back choose again
      }
    }

  } else {
    release(); // should not happen
  }
  delay(1000);
  return true;
}
