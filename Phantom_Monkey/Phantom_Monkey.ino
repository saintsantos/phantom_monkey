/*
 * Phantom_Monkey.ino
 * Version: 8.4
 * 
 * Author: Chase D.H. Brooks
 * Date: 16 May 2016
 * 
 * The following code is not to be repurposed for other than its original intended use.
 * This code controls the monkey prop for the Networks national tour of Phantom of the Opera.
 * 
 */

#include <Servo.h>

//Relay control constants (RELAY CONTROL ONLY)
#define ON LOW
#define OFF HIGH

//INPUT LIST
int openRelay = 13;     //Relay (Button/DMX) to open prop
int closeRelay = 2;     //Relay (Button/DMX) to close prop
int sitUpSwitch = 3;    //Normally HIGH, goes LOW when monkey sits up fully
int layDownSwitch = 4;  //Normally HIGH, goes LOW when monkey lays down fully
int armsOpen = 6;       //Normally HIGH, goes LOW when arms are in safe close position
int lidSwitch = 5;      //LOW when lid closed, HIGH when lid open
//Input on A1 is RESET BUTTON

//OUTPUT LIST
int sitUpMotor = 7;     //Relay to DC motor, sits monkey up
int layDownMotor = 8;   //Relay to DC motor, lays monkey down
Servo lidServo;         //Servo, controls arms to open lid
int lidServoPin = 9;    //output pin for lid servo
Servo neckServo;        //Servo, controls head position
int neckServoPin = 10;  //output pin for neck servo
int clapMotor = 11;     //Relay to DC motor, runs arm clapping motion
int led = 12;           //LED
//Output on A0 is for SERVO POWER RELAY

//PROGRAM VARIABLES
int lidServoOpen = 0;     //open position of lid servo
int lidServoClose = 180;  //close position of lid servo
int lidServoPos;          //current position of lid servo
int lidTime = 8000;       //time to open lid in milliseconds

int sitUpTime = 10000;        //LONGER THAN time to sit up in milliseconds (backup)
int layDownTime = 10000;      //LONGER THAN time to lay down in milliseconds (backup)
int timeCounter = 0;          //counter for time checks

int neckServoUp = 180;    //up position of neck servo
int neckServoDown = 0;   //down position of neck servo
int neckServoPos;         //current position of lid servo
int neckTime = 5000;      //time to lift head in milliseconds

int resetCounter = 0;

//SETUP FUNCTION This code only runs once, upon startup
void setup() { 
  pinMode(openRelay, INPUT_PULLUP);     //initiate openRelay as an input
  pinMode(closeRelay, INPUT_PULLUP);    //initiate closeRelay as an input 
  pinMode(sitUpSwitch, INPUT_PULLUP);   //initiate sitUpSwitch as an input
  pinMode(layDownSwitch, INPUT_PULLUP); //initiate layDownSwich as an input
  pinMode(armsOpen, INPUT_PULLUP);      //initiate armsOpen as an input
//  pinMode(lidSwitch, INPUT_PULLUP);     //initiate lidSwitch as an input
  pinMode(A1, INPUT_PULLUP);            //initiate A1 as input for RESET SWITCH
  lidServo.write(0);
  lidServo.attach(lidServoPin);         //attach lidServo to lidServoPin0
  neckServo.write(0);
  neckServo.attach(neckServoPin);       //attach neckServo to neckServoPin
  pinMode(sitUpMotor, OUTPUT);          //initiate sitUpMotor as an output
  pinMode(layDownMotor, OUTPUT);        //initiate layDownMotor as an output
  pinMode(led, OUTPUT);                 //initiate led as an output
  pinMode(clapMotor, OUTPUT);           //initiate clapMotor as an output
  pinMode(A0, OUTPUT);                  //initiate A0 as output for SERVO POWER RELAY
  timeCounter = 0;                      //reset timeCounter to 0
  delay(1000);                          //1 second delay for good measure
  
  digitalWrite(A0, HIGH);         //initialize Servo Power Relay HIGH
  digitalWrite(sitUpMotor, HIGH);   //initialize sitUpMotor HIGH
  digitalWrite(layDownMotor, HIGH); //initialize layDownMotor HIGH
  digitalWrite(clapMotor, HIGH);  //initialize clapMotor HIGH

}//END SETUP

//LOOP FUNCTION This code loops continuously
void loop() {   
 // if (/*digitalRead(closeRelay)==HIGH && digitalRead(lidSwitch) == LOW && */ digitalRead(openRelay) == LOW) sitUp();
 // if (digitalRead(closeRelay)==LOW /* && digitalRead(lidSwitch) == HIGH && digitalRead(openRelay) == HIGH */) layDown();

  if(digitalRead(openRelay) == LOW) {
    sitUp();
  }
  if(digitalRead(closeRelay) == HIGH) {
    layDown();
  }
  
/*Cutting Out Extras  
  if (digitalRead(A1) == LOW) {
    bool marker;
    while(marker = resetPressed) {
      delay(10);
      resetCounter+=10;
      if (resetCounter == 5000) {
        marker = false;
        resetLayDown();
//        while (true) {
//          digitalWrite(led, HIGH);
//          delay(200);          
//          digitalWrite(led, LOW);
//          delay(200);
//        }
      }//ENF IF resetCounter == 5000
    }//END WHILE resetPressed
  }//END IF resetPressed
*/

  delay(1000);    //Added delay for good practice

}//END LOOP

/*
 * sitUp()
 * Triggered by openRelay
 * Runs through cycle of opening lid, sitting monkey up, lifting head, LED on, clapping start
 * 
 * return void
 * return to loop() and shut down sitUpMotor if timer expires on sitting up motion
 */
void sitUp() {
  digitalWrite(A0, ON);         //connects servo power
  for (lidServoPos = lidServoClose; lidServoPos > lidServoOpen; lidServoPos--){
    lidServo.write(lidServoPos);                 //open lid
    delay(lidTime/abs(lidServoOpen-lidServoClose));               //wait for lid to open, divides time evenly across positional change
  }
  delay(250);
  digitalWrite(sitUpMotor, ON);                //activate sitUpMotor
  while(digitalRead(sitUpSwitch) == HIGH /* && timeCounter < layDownTime */) {   //wait for sitUpSwitch (with timer backup)
    delay(10);         //checks sitUpSwitch every 10 milliseconds
    //timeCounter+=10;     //increment timeCounter by 10 milliseconds //Time counter is not necessary here
  }

/*
  if (timeCounter == sitUpTime) {             //if sitUpSwitch didn't trigger, stop motor and exit layDown
    digitalWrite(sitUpMotor, OFF);
    return;                                     
  }
*/

  digitalWrite(sitUpMotor, OFF);                //sitUpSwitch has gone low, break out of loop and turn off sitUpMotor
  //timeCounter = 0;                              //reset timeCounter (deprecated)
  digitalWrite(led, HIGH);              //turns on LED to proper voltage
  for (neckServoPos = neckServoDown; neckServoPos < neckServoUp; neckServoPos++){
    neckServo.write(neckServoPos);              //tilt head up
    delay(neckTime/abs(neckServoUp-neckServoDown));            //wait for neck to complete
  }
  delay(500);
  digitalWrite(clapMotor, ON);                //activate clapping motor
  delay(500);
  digitalWrite(A0, OFF);        //cuts power to servos
  delay(50);
}

/*
 * layDown()
 * Triggered by closeRelay
 * Checks for proper arm position
 * Cycles through stopping claps, LED off, tilt head down, lay monkey down, close lid
 * 
 * return void
 * return to loop() and shut down layDownMotor if timer expires on laying down motion
 */
void layDown() {
  bool b = true;
  digitalWrite(A0, ON);      //connects servo power
  while(b){
    if (digitalRead(armsOpen) == LOW) {            //only runs when arms are ready to close
      digitalWrite(clapMotor, OFF);                 //stop clapping
      for (neckServoPos = neckServoUp; neckServoPos > neckServoDown-1; neckServoPos--){
          neckServo.write(neckServoPos);              //lower head
          delay(neckTime/abs(neckServoUp-neckServoDown));            //wait for neck to complete (value never changes, is 27.78ms every time
      }
      delay(250);
      analogWrite(led, LOW);                        //turn off LED
      digitalWrite(layDownMotor, ON);             //activate layDownMotor
      while(digitalRead(layDownSwitch) == HIGH /* && timeCounter < layDownTime*/) {   //wait for layDownSwitch (with timer backup)
        delay(10);         //checks layDownSwitch every 10 milliseconds                   
       // timeCounter+=10;    //increment timeCounter by 10 milliseconds (deprecated)
      }
/*
      if (timeCounter == layDownTime) {             //if layDownSwitch didn't trigger, stop motor and exit layDown
        digitalWrite(layDownMotor, OFF);
        return;                                     
      }
*/
      digitalWrite(layDownMotor, OFF);              //stop layDownMotor
     // timeCounter = 0;                              //reset timeCounter (deprecated)
      for (lidServoPos = lidServoOpen; lidServoPos < lidServoClose; lidServoPos++){
        lidServo.write(lidServoPos);                 //close lid
        delay(lidTime/abs(lidServoOpen-lidServoClose));               //wait for lid to close, divides time evenly across positional change
      }
      b = false;
    }//END IF (armsOpen == LOW)
  }//END WHILE (b)
  delay(100);
  digitalWrite(A0, OFF);     //cuts power to servos
  delay(50);
}//END layDown()

/*

bool resetPressed() {
  if (digitalRead(closeRelay)==LOW && digitalRead(lidSwitch)== HIGH && digitalRead(openRelay) == LOW)  {
    return true;
  }
  return false;
}

void resetLayDown() {
  analogWrite(led, LOW);
  if (digitalRead(armsOpen) == HIGH){
    while(digitalRead(armsOpen) == HIGH) {
      digitalWrite(clapMotor, ON);
    }
    digitalWrite(clapMotor, OFF);
    for (neckServoPos = neckServoUp; neckServoPos > neckServoDown-1; neckServoPos--){
        neckServo.write(neckServoPos);              //lower head
        delay(neckTime/abs(neckServoUp-neckServoDown));            //wait for neck to complete
    }
  }
  if(digitalRead(layDownSwitch) == HIGH) {
      digitalWrite(layDownMotor, ON);             //activate layDownMotor
      while(digitalRead(layDownSwitch) == HIGH && timeCounter < layDownTime) {   //wait for layDownSwitch (with timer backup)
        delay(10);         //checks layDownSwitch every 10 milliseconds                   
        timeCounter+=10;    //increment timeCounter by 10 milliseconds
      }
      if (timeCounter == layDownTime) {             //if layDownSwitch didn't trigger, stop motor and exit layDown
        digitalWrite(layDownMotor, OFF);
        return;                                     
      }
      digitalWrite(layDownMotor, OFF);              //stop layDownMotor
      timeCounter = 0;                              //reset timeCounter  
  }
  if(digitalRead(lidSwitch) == HIGH) {
    for (lidServoPos = lidServoOpen; lidServoPos < lidServoClose; lidServoPos++){
      lidServo.write(lidServoPos);                 //close lid
      delay(lidTime/abs(lidServoOpen-lidServoClose));               //wait for lid to close, divides time evenly across positional change
    }
  }
}
*/
/*
 * Changes from v4.0:
 *    Open and Close limits for lid servo
 *    Up and Down limtis for neck servo
 *    Lid switch operation 
 * Changes from v5.0:
 *    Reset functionality
 *    Fixed backup timer counts to match delay milliseconds
 *    Change log moved to end of file
 * Changes from v6.0:
 *    Defined ON and OFF for relays
 *    LOW turns relay ON
 *    HIGH turns relay OFF
 * Changes from v7.1:
 *    Cut out timer backups
 *    Cut out reset
 *    Cut out lidSwitch 
 * Changes from v8.0:
 *    Lengthened neck time for testing
 *    shortened delay before situp/laydown motor runs
 *    added delay at end of stiup() and laydown()
 * Changes from v8.1:
 *    swapped armsOpen and lidSwitch
 */

