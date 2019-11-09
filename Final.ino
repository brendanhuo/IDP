//Defining all sensor pins
#define trigPin 12
#define echoPin 11
#define trigPin2 6
#define echoPin2 5
#define hallEffect A0
#define irFront A1
#define irBack A2
#define lineFollowerPin1 A3
#define lineFollowerPin2 A5
#define redFlashingPin 2
#define amberFlashingPin 7
#define greenContinuousPin 3
#define amberContinuousPin 4

//Including necessary libraries
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>

unsigned long time;
int pos = 0; 
int minesPickedUp = 0;

//Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

//Sets up the 2 motors
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

//Sets up servo object
Servo myservo;

//Variable to store running mean average
long runningMean8;

//Side Ultrasonic
long readUltraSonic(){
  long duration, distance;

  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);

  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);
  return distance;
}

//Back ultrasonic
long readUltraSonic2(){
  long duration, distance;

  digitalWrite(trigPin2, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin2, HIGH);

  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin2, LOW);
  duration = pulseIn(echoPin2, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);
  return distance;
}

long readlineFollower() {
  long lineFollowerValue1
  for (int i = 0; i<8; i++) {
     lineFollowerValue1 += analogRead(lineFollowerPin1);
     delay(5);
  }
  lineFollowerValue1 = lineFollowerValue1 / 8;
  
  Serial.println(lineFollowerValue1);
  
  return lineFollowerValue1; 
}

bool readHallEffect() {
  long hallEffectReading = 0;

  for (int i = 0; i<10; i++) {
     hallEffectReading += analogRead(hallEffect);
     delay(5);
  }
  hallEffectReading = hallEffectReading/10;
 
  Serial.println(hallEffectReading);
  delay(10);

  if (hallEffectReading <= 250) {
      return 1;
    }
  return 0;
}

// 0 is front, 1 is back
bool readIR(int a = 0) {
  long irReading = 0 ;
  if (a == 0) {
    for (int i = 0; i < 8; i++) {
      irReading  += analogRead(irFront);
      delay(1);
    }
      irReading = irReading / 8;
      Serial.println(irReading);
      delay(1);
    if (irReading >=48) {
      return 1;
    }
  }
  else {
    for (int i = 0; i < 8; i++) {
      irReading  += analogRead(irBack);
      delay(1);
    }
      irReading = irReading / 8;
      Serial.println(irReading);
      delay(1);
    
    if (irReading >=35) {
      return 1;
    }
  }
  return 0; 
}

void setup() {
  Serial.begin(9600);
  
  pinMode(13, OUTPUT);          
  pinMode(amberFlashingPin, OUTPUT);
  pinMode(amberContinuousPin, OUTPUT);
  pinMode(greenContinuousPin, OUTPUT);
  pinMode(redFlashingPin, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  myservo.attach(10);  // attaches the servo on pin 10 to the servo object
  
  AFMS.begin();  // create with the default frequency 1.6KHz

  //Required to ground all pins to prevent floatingness causing LEDs to light when not required
  digitalWrite(amberFlashingPin, LOW);
  digitalWrite(amberContinuousPin, LOW);
  digitalWrite(greenContinuousPin, LOW);
  digitalWrite(redFlashingPin, LOW);

  //Delay to allow time for hands to get away
  delay(1000);

  //Amber flashing LED turns on as about to begin moving
  digitalWrite(amberFlashingPin, HIGH);
  
  //delay(5000);
  
//  myMotor->setSpeed(100);
//  myOtherMotor->setSpeed(100);
//  myMotor->run(FORWARD);
//  myOtherMotor->run(BACKWARD);
//  // turn on motor
//  delay(50);
//  myMotor->run(RELEASE);
//  myOtherMotor->run(RELEASE);

  Serial.println("Programme starting");
  
  //Moves servo from default position to front position
  for(int i = 90; i>=0; i -= 1){
    myservo.write(i);
    delay(20);
  }

  Serial.println(runningMean8);

  // Releasing motors in case not released last time the programme was run
  myMotor->run(RELEASE);
  myOtherMotor->run(RELEASE);
  
//  for (pos = 35; pos <= 180; pos += 10) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(300);                       // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  moveBackward(10000);
//  moveForward(4000);
//  delay(20);
//  turnLeft(2700);

  time = millis();
  Serial.println(time);
  moveForward(2500);
  
  for (int i = 0; i < 300; i++) {
    myMotor->setSpeed(104);
    myOtherMotor->setSpeed(100);
    myMotor->run(FORWARD);
    myOtherMotor->run(FORWARD);
    delay(10);
    if (readlineFollower() > 25) {
      moveForward(1000);
      myMotor->setSpeed(0);
      myOtherMotor->setSpeed(0);
      myMotor->run(RELEASE);
      myOtherMotor->run(RELEASE);
      turnLeft(2950); //Slightly more than 90 degree turn as some inaccuracy in movement up to that point
      delay(20);
      break;
    }
  }  
  moveBackward(500);
  for(int i = 0; i<100; i++) {
    runningMean8 += readUltraSonic();
    delay(10);
  }
  runningMean8 = runningMean8 / 100 * 8;
}

void pickUp() {
  for (int i = 0; i<=90; i++) {
    myservo.write(i);   
    delay(50); 
  }
}

void dropOff() {
  for (int i = 90; i<=180; i++) {
    myservo.write(i);   
    delay(15); 
  }
}

// 2700 is 90 degrees
void turnRight(long timeTurned) {

// .  Acceleration function
//    for (int i = 0; i<=100; i++) {
//      myMotor->setSpeed(i);
//      myOtherMotor->setSpeed(i);
//      myMotor->run(FORWARD);
//      myOtherMotor->run(BACKWARD);
//      delay(3);
//    }

    myMotor->setSpeed(100);
    myOtherMotor->setSpeed(100);
    delay(10);
    myMotor->run(FORWARD);
    myOtherMotor->run(BACKWARD);

    delay(timeTurned);
    
//    Deceleration function
//    for (int i = 100; i>=0; i--) {
//      myMotor->setSpeed(i);
//      myMotor->run(FORWARD);
//      delay(3);
//    }

    myMotor->setSpeed(0);
    myOtherMotor->setSpeed(0);
    myMotor->run(RELEASE);
    myOtherMotor->run(RELEASE);
}

void turnLeft(long timeTurned) {
  
//    for (int i = 0; i<=100; i++) {
//      myMotor->setSpeed(i);
//      myOtherMotor->setSpeed(i);
//      myMotor->run(BACKWARD);
//      myOtherMotor->run(BACKWARD);
//      delay(3);
//    }

    myMotor->setSpeed(100);
    myOtherMotor->setSpeed(100);
    delay(10);
    myMotor->run(BACKWARD);
    myOtherMotor->run(FORWARD);

    delay(timeTurned);

//    for (int i = 100; i>=0; i--) {
//      myMotor->setSpeed(i);
//      myMotor->run(BACKWARD);
//      delay(3);
//    }

    myMotor->setSpeed(0);
    myOtherMotor->setSpeed(0);
    myMotor->run(RELEASE);
    myOtherMotor->run(RELEASE);
}

void moveBackward(long timeToMove) {
  
//    for (int i = 0; i<=100; i++) {
//      myMotor->setSpeed(i);
//      myOtherMotor->setSpeed(i);
//      myMotor->run(BACKWARD);
//      myOtherMotor->run(BACKWARD);
//      delay(3);
//    }

    myMotor->setSpeed(204);
    myOtherMotor->setSpeed(200);
    delay(1);
    myMotor->run(BACKWARD);
    myOtherMotor->run(BACKWARD);

    delay(timeToMove/2); //Half as doubled speed 
    
//    for (int i = 100; i>=0; i--) {
//      myMotor->setSpeed(i);
//      myOtherMotor->setSpeed(i);
//      myMotor->run(BACKWARD);
//      myOtherMotor->run(BACKWARD);
//      delay(3);
//    }

    myMotor->setSpeed(0);
    myOtherMotor->setSpeed(0);
    myMotor->run(RELEASE);
    myOtherMotor->run(RELEASE); 
}

//Function that wasn't used in the end that makes the robot orientate itself towards a mine
//void moveForward(long timeToMove) {
//  
////    for (int i = 0; i<=100; i++) {
////      myMotor->setSpeed(i);
////      myOtherMotor->setSpeed(i);
////      myMotor->run(FORWARD);
////      myOtherMotor->run(FORWARD);
////      delay(3);
////    }
//
//    myMotor->setSpeed(204);
//    myOtherMotor->setSpeed(200);
//    delay(1);
//    myMotor->run(FORWARD);
//    myOtherMotor->run(FORWARD);
//
//    delay(timeToMove/2);//Half as doubled speed 
//    
////    for (int i = 100; i>=0; i--) {
////      myMotor->setSpeed(i);
////      myOtherMotor->setSpeed(i);
////      myMotor->run(FORWARD);
////      myOtherMotor->run(FORWARD);
////      delay(3);
////    }
//
//    myMotor->setSpeed(0);
//    myOtherMotor->setSpeed(0);
//    myMotor->run(RELEASE);
//    myOtherMotor->run(RELEASE);
//}
//
//long testWhichDirection(long timing) {
//  long leftDistance, centreDistance, rightDistance;
//  int minimum;
//  turnLeft(timing);
//  delay(200);
//  for (int i; i<32; i++){
//  leftDistance += readUltraSonic();
//  delay(30);
//  }
//  leftDistance = leftDistance / 32;
//  turnRight(timing);
//  delay(200);
//  for (int i; i<32; i++){
//  centreDistance += readUltraSonic();
//  delay(30);
//  }
//  centreDistance = centreDistance / 32;
//  turnRight(timing);
//  delay(200);
//  for (int i; i<32; i++){
//  rightDistance += readUltraSonic();
//  delay(30);
//  }
//  rightDistance = rightDistance/32;
//  delay(100);
//  turnLeft(timing);
//  delay(100);
//  
//  if (leftDistance < centreDistance && leftDistance < rightDistance) {
//    minimum = 1;
//    turnLeft(timing);
//    delay(500);
//    turnLeft(timing);
//  }
//  else if (centreDistance < rightDistance) {
//    minimum = 2;
//    turnLeft(timing);
//    delay(100);
//  }
//  else {
//    minimum = 3;
//  }
//
//  delay(500);
//  return minimum;
//}
//
//void moveTowardsBrick() {
//  for (int i = 600; i >= 75;i = i/2){
////    if(testWhichDirection(i) == 1) {
////      delay(500);
////      turnLeft(i);
////    }
////    else if (testWhichDirection(i) == 3) {
////      delay(500);
////      turnRight(i);
////    }
//      testWhichDirection(i);
//  }
//  moveForward(1000);
//
//  delay(250);
//}

//Minimum distance at which the robot will turn for a mine
long minDistance = 100;

void loop() {
  
  long distance = readUltraSonic();
  int runningCounter = 0;
  
//  runningMean8 = runningMean8 + readUltraSonic() - runningMean8/8;
  
  Serial.println("Running Mean");
  Serial.println(distance);
  
  if (distance > minDistance){

    myMotor->setSpeed(204);
    myOtherMotor->setSpeed(200);
    myMotor->run(FORWARD);
    myOtherMotor->run(FORWARD);
    //delay(5);
    Serial.println("Moving");
  }
  
  else{
//    int runningCounter = 0;

      //Attempted function to make robot turn 
//    for (int i = 0; i < 100; i++) {
//      delay(30);
//      runningMean8 = runningMean8 + readUltraSonic() - runningMean8/8;
//      if (runningMean8 / 8 > minDistance) {
//        runningCounter = i;
//        break; 
//      }
//    }

    Serial.println("Started turning");
//    Serial.println(runningCounter);
    
    minesPickedUp += 1;
    
    moveForward(1200);
    turnRight(2950);

    int liveMineOrNot = 0;
    
//    moveForward(runningMean8*15);

    int runningCounter = 0;
    //While the front IR has not been triggered by a mine
    while(readIR() == 0) {
      //Break statement in case a mine is not on the path
      if(runningCounter >=1000) {
        break;
      }
      //If hall effect detects a magnet assign 1 to the variable to say that the mine is live
      if(readHallEffect()==1){
      liveMineOrNot = 1;
      }
      runningCounter += 1;
      myMotor->setSpeed(204);
      myOtherMotor->setSpeed(200);
      myMotor->run(FORWARD);
      myOtherMotor->run(FORWARD);
      delay(10);
    }
    
    myMotor->setSpeed(0);
    myOtherMotor->setSpeed(0);
    myMotor->run(RELEASE);
    myOtherMotor->run(RELEASE);
    if(readHallEffect()==1){
      liveMineOrNot = 1;
    }
    
    moveForward(100);

    digitalWrite(amberFlashingPin, LOW);
    digitalWrite(amberContinuousPin, HIGH);

    //turnRight(100);
    runningCounter = 0;

    //Use back IR to position robot well for hall effect sensors
    while(readIR(1) == 0){
      if(runningCounter >=25) {
        break;
      }
      if(readHallEffect()==1){
        liveMineOrNot = 1;
        break;
      }
      runningCounter +=1;
      myMotor->setSpeed(60);
      myOtherMotor->setSpeed(50);
      myMotor->run(FORWARD);
      myOtherMotor->run(FORWARD);
      delay(5);
    }

      delay(3000);

    myMotor->setSpeed(0);
    myOtherMotor->setSpeed(0);
    myMotor->run(RELEASE);
    myOtherMotor->run(RELEASE);

    //Turn on required LEDs depending on whether the mine is live or not
    if(liveMineOrNot==0){
      digitalWrite(amberContinuousPin, LOW);
      digitalWrite(greenContinuousPin, HIGH);
      delay(5000);
      digitalWrite(greenContinuousPin, LOW);
    }
    else{
      digitalWrite(amberContinuousPin, LOW);
      //delay(5000);
      digitalWrite(redFlashingPin, HIGH);
      
    }
    
    pickUp();

    runningCounter = 0;

    digitalWrite(amberFlashingPin, HIGH);
    
    while(readlineFollower() <=25) {
    if(runningCounter >=1000) {
        moveForward(4000);
        break;
      }
    myMotor->setSpeed(204);
    myOtherMotor->setSpeed(200);
    myMotor->run(BACKWARD);
    myOtherMotor->run(BACKWARD);
    runningCounter += 1;
    delay(20);
    Serial.println(readlineFollower());
    }
    
    myMotor->setSpeed(0);
    myOtherMotor->setSpeed(0);
    myMotor->run(RELEASE);
    myOtherMotor->run(RELEASE);
    turnRight(50);
    moveForward(2200);
    
    turnLeft(2700);

    //Prepares back ultrasonic sensors for readings
    for(int i = 0; i<100; i++) {
    runningMean8 += readUltraSonic2();
    delay(10);
    }
    
    runningMean8 = runningMean8 / 100 * 8;

    //Robot keeps running until it is sufficiently far from the wall
    while(runningMean8 / 8 <=105) {
      myMotor->setSpeed(204);
      myOtherMotor->setSpeed(200);
      myMotor->run(FORWARD);
      myOtherMotor->run(FORWARD); 
      delay(20);
      //Uses running mean average to smooth readings
      runningMean8 = runningMean8 + readUltraSonic2() - runningMean8/8;  
      delay(5);
    }

    //Moves into the wall to straighten
    moveForward(9000);

    //Reverses different distances depending on the type of mine to move to different boxes
    if (liveMineOrNot == 1) {
      moveBackward(3000);
    }
    else{
      moveBackward(11500);
    }

    //Turns towards box and moves forward
    turnLeft(1350);
    moveForward(2500);

    digitalWrite(amberFlashingPin, LOW);
    delay(100);
    dropOff();
    delay(100);
    digitalWrite(amberFlashingPin, HIGH);
    
    digitalWrite(greenContinuousPin, LOW);
    digitalWrite(redFlashingPin, LOW);
    
    moveBackward(1500);

    //Reset servo to front position
    for(int i = 170; i>=0; i -= 1){
    myservo.write(i);
    delay(20);
  }
    
    turnRight(1350);

    //Move forward to hit wall to straighten robot
    if (liveMineOrNot == 1) {
      moveForward(8000);
    }
    else{
      moveForward(15000);
    }
    
    //Move backward to original position to detect other mines
    moveBackward(20000);

    //If more than 5 minutes and 20 seconds have passed then return to starting position
    if (millis() - time > 320000) {
      moveBackward(11000);
      moveForward(2000);
      turnRight(2700);
      moveBackward(7000);
      moveForward(2000);
      delay(200000);
    }
  }
}
