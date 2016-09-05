#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <SoftwareSerial.h>
#include "enes100.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor_front_right = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor_front_left = AFMS.getMotor(2);
Adafruit_DCMotor *myMotor_back_left = AFMS.getMotor(3);
Adafruit_DCMotor *myMotor_back_right = AFMS.getMotor(4);

SoftwareSerial sSerial(4,12);

enes100::RfClient<SoftwareSerial> rf(&sSerial);
enes100::Marker marker;

int S0 = 8;//pinB
int S1 = 9;//pinA
int S2 = 7;//pinE
int S3 = 6;//pinF
int taosOutPin = A0;//pinC
int LED = 1;//pinD


int number = 108;
float green;
int counter1=0;
double tolerance = 0.032;
double north_val = 0;
double south_val = 0;
double east_val = 0;
double west_val = 0;
double exit_y = 0;

//Front Sensor(0)

int trigPin0 = 13;
int echoPin0 = 5;

//SideParallel(1)
int trigPin1 = 11;
int echoPin1 = 3;

//SideTop(2)
int trigPin2 = 10;
int echoPin2 = 2;

int numberPlate = 108;
int mm0, mm1, mm2;
int steps;
long secs1;
long height, dim, ground;
int markerLength[2];
int lengthRock; //
int once1, once2, start;
int mark1, mark2;

long seconds1, seconds2;

void setup() {
  pinMode(trigPin0, OUTPUT);
  pinMode(echoPin0, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  TCS3200setup();
  
  sSerial.begin(9600);
  Serial.begin(9600);    // set up Serial library at 9600 bps

  rf.resetServer();
  rf.sendMessage("Connected.");
  AFMS.begin();  // create with the default frequency 1.6KHz
  
  
  // Set the speed to start, from 0 (off) to 255 (max speed)

  myMotor_front_right->setSpeed(125);
  myMotor_front_left->setSpeed(125);
  myMotor_back_left->setSpeed(125);
  myMotor_back_right->setSpeed(125);
  

  delay(500);
}

void loop() {
  if (start == 0){
    phaseOne();
    phaseTwo();
    start++;
  }
  else{ 
    sensingF();
    sensingSideP();
    sensingSideT();
    rf.receiveMarker(&marker, numberPlate);
    switch(steps){
    myMotor_front_right->setSpeed(90);
    myMotor_front_left->setSpeed(90);
    myMotor_back_left->setSpeed(90);
    myMotor_back_right->setSpeed(90);
      default:
      rf.receiveMarker(&marker, numberPlate);
      while(mm1 > 200 && steps != 4){
        startForward();
        sensingF();
        sensingSideP();
        sensingSideT();
        //Serial.println("running default");
        //rf.sendMessage("going through while");
      }
      stopMoving();
//      rf.sendMessage("first stop");
//      Serial.println("default stop");
      steps++;
      break;

      case 1:
      //Serial.println("case1 Height");
     // rf.sendMessage("case 1 Height+++++++++++++");
      stopMoving();
      calcHeight(mm2);
      rf.sendMessage("Height is: ");
      rf.sendMessage(height);
      rf.sendMessage(" mm  ");
      steps++;
      break;

      case 2:
      //Serial.println("case 2 Length");
      calcLength(mm1);
      stopMoving();
      rf.sendMessage("Length is: ");
      rf.sendMessage(lengthRock);
      rf.sendMessage(" mm");
      rf.sendMessage("Dimension is: ");
      rf.sendMessage(dim);
      rf.sendMessage(" mm");
      steps++;
      break;

    case 3:
    //Serial.println("case 3 Length");
    stopMoving();
    myMotor_front_right->setSpeed(125);
    myMotor_front_left->setSpeed(125);
    myMotor_back_left->setSpeed(125);
    myMotor_back_right->setSpeed(125);
    backupcolor();
    while(1==1){
      stopMoving();
    }
  
    break;
    }
    }
}

////////////////////////////////////////////////////////////////
void backupcolor(){

 if(counter1==0){

 rf.receiveMarker(&marker, number);

  if(marker.y>.4){
  startForward();
   while(marker.y>.4){



  rf.receiveMarker(&marker,number);

   }
  stopMoving();
 delay(100);


  }
turnWest();

rf.receiveMarker(&marker, number);
startForward();
while(marker.x>2.05){

rf.receiveMarker(&marker, number);
}
stopMoving();
delay(100);
rf.receiveMarker(&marker, number);
turnNorth();
rf.receiveMarker(&marker, number);
startForward();
while(marker.y<.78){//this is if the left edge of the marker is at .8. If the center of marker is at .8 change to like .80ish

rf.receiveMarker(&marker, number);

}
stopMoving();
delay(100);
turnWest();
rf.receiveMarker(&marker, number);
startBackward();
delay(4500);
stopMoving();
detectColor(taosOutPin);
Serial.print("\n\n\n");
delay(1000);

counter1++;
 }
}

////////////////////////////////////////////////////////////////
void startForward()
{
  myMotor_front_right->run(FORWARD);
  myMotor_front_left->run(FORWARD);
  myMotor_back_right->run(FORWARD);
  myMotor_back_left->run(FORWARD);
}

void startBackward()
{
  myMotor_front_right->run(BACKWARD);
  myMotor_front_left->run(BACKWARD);
  myMotor_back_right->run(BACKWARD);
  myMotor_back_left->run(BACKWARD);
}

void stopMoving()
{
  myMotor_front_right->run(RELEASE);
  myMotor_front_left->run(RELEASE);
  myMotor_back_right->run(RELEASE);
  myMotor_back_left->run(RELEASE);
}

void turnRight()
{
  myMotor_front_left->run(FORWARD);
  myMotor_back_left->run(FORWARD);
  myMotor_front_right->run(BACKWARD);
  myMotor_back_right->run(BACKWARD);
}

void turnLeft()
{
  myMotor_front_left->run(BACKWARD);
  myMotor_back_left->run(BACKWARD);
  myMotor_front_right->run(FORWARD);
  myMotor_back_right->run(FORWARD);
}


/////////////////////////////////////////////////////////////////
void turnNorth()
{
  rf.receiveMarker(&marker, number);
  north_val = marker.theta - 1.570795;

  if (north_val < 0)
  {
    north_val = -north_val;
  }

  while ( north_val > tolerance )
  {

    if ( marker.theta > 0 )
    {

      if (marker.theta < 1.570795)
      {
        turnLeft();
      }

      else
      {
        turnRight();
      }

    }

    else
    {
      if (marker.theta > -1.570795)
      {
        turnLeft();
      }

      else
      {
        turnRight();
      }


    }

    rf.receiveMarker(&marker, number);
    north_val = marker.theta - 1.570795;
    if (north_val < 0)
    {
      north_val = -north_val;
    }

  }

  stopMoving();
  rf.receiveMarker(&marker, number);


}

/////////////////////////////////////////////////////////////////
void turnSouth()
{
  rf.receiveMarker(&marker, number);

  south_val = -1.570795 - marker.theta;
  if (south_val < 0)
  {
    south_val = -south_val;
  }

  while ( south_val > tolerance )
  {

    if ( marker.theta > 0 )
    {

      if (marker.theta < 1.570795)
      {
        turnRight();
      }

      else
      {
        turnLeft();
      }

    }

    else
    {
      if (marker.theta > -1.570795)
      {
        turnRight();
      }

      else
      {
        turnLeft();
      }

    }
    rf.receiveMarker(&marker, number);
    south_val = -1.570795 - marker.theta;
    if (south_val < 0)
    {
      south_val = -south_val;
    }

  }

  stopMoving();
  rf.receiveMarker(&marker, number);

}

////////////////////////////////////////////////////////////////////
void turnEast()
{

  rf.receiveMarker(&marker, number);

  east_val = marker.theta;

  if (east_val < 0)
  {
    east_val = -east_val;
  }


  while ( east_val > tolerance )
  {

    if ( marker.theta > 0 )
    {

      turnRight();

    }

    else
    {

      turnLeft();

    }

    rf.receiveMarker(&marker, number);
    east_val = marker.theta;

    if (east_val < 0)
    {
      east_val = -east_val;
    }


  }

  stopMoving();
  rf.receiveMarker(&marker, number);


}

/////////////////////////////////////////////////////////////////
void turnWest()
{

  rf.receiveMarker(&marker, number);

  west_val = marker.theta;
  if (west_val < 0)
  {
    west_val = -west_val;
  }


  while ( (3.14159 - west_val) > tolerance )
  {

    if ( marker.theta > 0 )
    {

      turnLeft();

    }

    else
    {

      turnRight();

    }

    rf.receiveMarker(&marker, number);
    west_val = marker.theta;

    if (west_val < 0)
    {
      west_val = -west_val;
    }

  }

  stopMoving();
  rf.receiveMarker(&marker, number);

}

/////////////////////////////////////////////////////////////////
void isSomethingThere() {

  //Loop until sensor detects something

  int counter = 0;
  double current;

  while  (counter < 3)
  {
    current = getDistance(trigPin1, echoPin1);
    rf.sendMessage("Distance Detected:");
    rf.sendMessage(current);
    if (getDistance(trigPin1, echoPin1) < 500) {
      counter ++;
    }

  }
  rf.sendMessage("There at");
  rf.sendMessage(current);
  counter = 0;

}
/////////////////////////////////////////////////////////////////
void isSomethingNotThere() {

  //Loop until sensor no longer detects anything

  int counter = 0;
  double current;

  while  (counter < 3)
  {
    current = getDistance(trigPin1, echoPin1);
    rf.sendMessage("Distance Detected:");
    rf.sendMessage(current);
    if (getDistance(trigPin1, echoPin1) > 500) {
      counter ++;
    }

  }
  rf.sendMessage("Not there at");
  rf.sendMessage(current);
  counter = 0;

}
////////////////////////////////////////////////////////////////
int detectColor(int taosOutPin){

green = colorRead(taosOutPin,3,1);

Serial.println(colorBG());
Serial.print(green);
rf.sendMessage("************************");
rf.sendMessage(colorBG());
rf.sendMessage(colorBG());
rf.sendMessage(colorBG());
}
/////////////////////////////////////////////////////////////////
String colorBG(){
  if(green <= 34){
    return "Green";
  } else{
    return "Black";
  }
}
/////////////////////////////////////////////////////////////////
float colorRead(int taosOutPin, int color, boolean LEDstate){ 
taosMode(1);
int sensorDelay = 100;

digitalWrite(S3, HIGH); //S3
digitalWrite(S2, HIGH); //S2

// create a variable where the pulse reading from sensor will go
float readPulse;

//  turn LEDs on or off, as directed by the LEDstate var
if(LEDstate == 0){
    digitalWrite(LED, LOW);
}
if(LEDstate == 1){
    digitalWrite(LED, HIGH);
}

// wait a bit for LEDs to actually turn on, as directed by sensorDelay var
delay(sensorDelay);

// now take a measurement from the sensor, timing a low pulse on the sensor's "out" pin
readPulse = pulseIn(taosOutPin, LOW, 80000);

//if the pulseIn times out, it returns 0 and that throws off numbers. just cap it at 80k if it happens
if(readPulse < .1){
readPulse = 80000;
}

//turn off color sensor and LEDs to save power 
taosMode(0);

// return the pulse value back to whatever called for it... 
return readPulse;

}
/////////////////////////////////////////////////////////////////
void taosMode(int mode){
    
    if(mode == 0){
    //power OFF mode-  LED off and both channels "low"
    digitalWrite(LED, LOW);
    digitalWrite(S0, LOW); //S0
    digitalWrite(S1, LOW); //S1
    //  Serial.println("mOFFm");
    
    }else if(mode == 1){
    //this will put in 1:1, highest sensitivity
    digitalWrite(S0, HIGH); //S0
    digitalWrite(S1, HIGH); //S1
    // Serial.println("m1:1m");
    
    }else if(mode == 2){
    //this will put in 1:5
    digitalWrite(S0, HIGH); //S0
    digitalWrite(S1, LOW); //S1
    //Serial.println("m1:5m");
    
    }else if(mode == 3){
    //this will put in 1:50
    digitalWrite(S0, LOW); //S0
    digitalWrite(S1, HIGH); //S1 
    //Serial.println("m1:50m");
    }
    
    return;

}
/////////////////////////////////////////////////////////////////
void TCS3200setup(){

    //initialize pins
    pinMode(LED,OUTPUT); //LED pinD
    
    //color mode selection
    pinMode(S2,OUTPUT); //S2 pinE
    pinMode(S3,OUTPUT); //s3 pinF


    //(since input) It can go to the analogue pin and be treated as a ditial pin
    //color response pin (only actual input from taos)
    pinMode(taosOutPin, INPUT); //taosOutPin pinC
    
    //communication freq (sensitivity) selection
    pinMode(S0,OUTPUT); //S0 pinB
    pinMode(S1,OUTPUT); //S1 pinA 
    
    return;

}


/////////////////////////////////////////////////////////////////
void sensingF(){
  mm0 = getDistance(trigPin0, echoPin0);
  Serial.print(mm0); 
  Serial.println(" mm0");
  delay(200); //200
}
void sensingSideP(){
  mm1 = getDistance(trigPin1, echoPin1);
  Serial.print(mm1); 
  Serial.println(" mm1");
  delay(200);  //200
}

void sensingSideT(){
  mm2 = getDistance(trigPin2, echoPin2);
  Serial.print(mm2); 
  Serial.println(" mm2");
  delay(200); //200
}

int getDistance (int trigPin, int echoPin){
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(15); 
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(15);
 unsigned long dist = pulseIn(echoPin, HIGH); 
 int distance = (dist/2)/2.9412;
 return distance;
 
}

////////////////////////////////////////////////////////////////
int calcLength(int sideRock){
  if(sideRock < 48){
    startForward();
    if(once2 == 0){
      seconds1 = millis();
      once2++;
    }
    seconds2 = millis();
//    Serial.println("recursion 1 calcLength");
//    Serial.println(seconds2);
//    rf.sendMessage("recursion 1 calcLength ");
//    rf.sendMessage(seconds2);
//    rf.sendMessage("seconds <- before");
//    rf.sendMessage(seconds1); //changed library for this to print
    sensingSideP();
    sensingSideT();
//    rf.sendMessage("_______+++___");
//    rf.sendMessage(sideRock);
    calcLength(mm1);
  } else if(sideRock > 300 && seconds2 > 100){
    //rf.sendMessage("Got here");
    stopMoving();
    secs1 = timing(seconds1, seconds2);
    Serial.print("Length is: ********************* ");
    lengthRock = (.0719*secs1); //comes out in millimeters
    dim = lengthRock * height;
    Serial.println(lengthRock);
    Serial.println("dim: ");
    Serial.print(dim);
    Serial.print("*****");
    rf.sendMessage("Length is: ");
    rf.sendMessage(lengthRock);
    rf.sendMessage(" mm  ");
    rf.sendMessage("Dimension: ");
    rf.sendMessage(dim);
    rf.sendMessage(" mm  ");
    return lengthRock; 
    
  }
  //rf.sendMessage("returned rf");
  return lengthRock;
}
////////////////////////////////////////////////////////////////
long calcHeight(long topSide){
  ground = 316;   //normalized. Sand could throw the sensor off
  if(once1 == 0){
    height = ground - (topSide + 10);  //usually off by 10mm
    once1++;
  }
//  Serial.print("Height is: ");
//  Serial.println(height);
  rf.sendMessage("Height is: ");
  rf.sendMessage(height);
  rf.sendMessage(" mm  ");
  return height;
  
}

///////////////////////////////////////////////////////////////////
long timing(long milSeconds1, long milSeconds2){
  long startT;
  long endT;
  if(milSeconds1 > 0){
    startT = milSeconds1;
  }
  if(milSeconds2 > startT){
    endT = milSeconds2;
  }
  Serial.println(endT - startT);
  return endT - startT;
}


////////////////Nathan////////////////////

//double tolerance = 0.035;
double obstacle_one_start = 0;
double obstacle_one_end = 0;
int counter = 0;
int current = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void phaseOne()
{

  //Driving to X bound

  turnEast();
  startForward();
  do {

    rf.receiveMarker(&marker, number);

  } while ( marker.x < 0.82);
  //1.32
  //1.25-0.92= 0.33
  //1.12-0.79=
  //Higher = more to the right 

  stopMoving();

  rf.sendMessage("X bound reached at: ");
  rf.sendMessage(marker.x);

  //Driving to Y bound

  turnSouth();
  startForward();

  counter = 0;
  do {

    rf.receiveMarker(&marker, number);
    counter ++;

    if (counter == 10)
    {
      turnSouth();
      counter = 0;
    }

    startForward();

  } while ( marker.y > 0.33);
  stopMoving();
  delay(500); 
  counter = 0;
  rf.sendMessage("Y bound reached at: ");
  rf.sendMessage(marker.y);

  rf.sendMessage(" Starting Scan");

  startBackward();
  isSomethingThere();
  stopMoving();
  rf.receiveMarker(&marker, number);
  obstacle_one_start = marker.y;
  delay(500);

  rf.receiveMarker(&marker, number);


  startBackward();
  isSomethingNotThere();
  stopMoving();
  rf.receiveMarker(&marker, number);
  obstacle_one_end = marker.y;
  delay(500);

  rf.sendMessage("There is something from ");
  rf.sendMessage(obstacle_one_start);
  rf.sendMessage(" to ");
  rf.sendMessage(obstacle_one_end);


  if ( (1.2 - obstacle_one_end) > obstacle_one_start)
  {
    startBackward();
    do {

      rf.receiveMarker(&marker, number);

    } while ( marker.x < (obstacle_one_end + 0.157));
    stopMoving();
    turnEast();
  }

  else
  {

    turnSouth();
    startForward();

    rf.receiveMarker(&marker, number);

    while (marker.y > (obstacle_one_start / 2))
    {
      rf.receiveMarker(&marker, number);
    }

    stopMoving();
    turnEast();

  }

  stopMoving();

  rf.receiveMarker(&marker, number);
  exit_y = marker.y;

  startForward();
  do {

    rf.receiveMarker(&marker, number);

  } while ( marker.x < 1.7);
  stopMoving();

}


//////////////////////////////////////////////////////////////////////////////////////////////////
void phaseTwo()
{

  rf.receiveMarker(&marker, number);

  if ( marker.y < 1.2 )
  {

    turnNorth();
    startForward();
    do {

      rf.receiveMarker(&marker, number);

    } while ( marker.y < 1.2);
    stopMoving();

  }

  //0.92-1.2

  else
  {

    turnSouth();
    startForward();
    do {

      rf.receiveMarker(&marker, number);

    } while ( marker.y > 1.2);
    stopMoving();

  }

  turnEast();
  startForward();
  do {

    rf.receiveMarker(&marker, number);

  } while ( marker.x < 2.09);
  // This the x bound for the obstacle, greater = more to the right 
  //2.40-2.15
  
  stopMoving();

  turnSouth();

  rf.sendMessage("Site Reached");
  rf.sendMessage(" Starting Main Scan");



}
