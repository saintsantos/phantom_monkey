/*
 * Phantom_Monkey.ino
 * Version: 8.4
 * 
 * Author: Chase D.H. Brooks
 * Date: 16 May 2016
 * 
 * Modified: Edwin Santos
 * Date: May 31, 2016
 * 
 * The following code is not to be repurposed for other than its original intended use.
 * This code controls the monkey prop for the Networks national tour of Phantom of the Opera.
 * 
 */

#include <Servo.h>

//Relay control constants (RELAY CONTROL ONLY)
#define ON HIGH
#define OFF LOW 


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
int clapMotor = 11;     //Relay to DC motor, runs arm clapping motion
int led = 12;           //LED


//Servos
Servo lidServo;         //Servo, controls arms to open lid
int lidServoPin = 9;    //output pin for lid servo
Servo neckServo;        //Servo, controls head position
int neckServoPin = 10;  //output pin for neck servo
//Output on A0 is for SERVO POWER RELAY


//Servo Positions
int lidServoOpen = 180;     //open position of lid servo
int lidServoClose = 0;  //close position of lid servo
int lidServoPos;          //current position of lid servo

int neckServoUp = 180;    //up position of neck servo
int neckServoDown = 1;   //down position of neck servo
int neckServoPos;         //current position of lid servo


// Time variables (Deprecated)
int neckTime = 5000;      //time to lift head in milliseconds
int sitUpTime = 10000;        //LONGER THAN time to sit up in milliseconds (backup)
int layDownTime = 10000;      //LONGER THAN time to lay down in milliseconds (backup)
int lidTime = 8000;       //time to open lid in milliseconds

int timeCounter = 0;          //counter for time checks
int resetCounter = 0;

//Sitting boolean
boolean sittingUp;



//SETUP FUNCTION This code only runs once, upon startup
void setup() { 
  Serial.begin(9600);
  pinMode(openRelay, INPUT_PULLUP);            //initiate openRelay as an input
  pinMode(closeRelay, INPUT_PULLUP);           //initiate closeRelay as an input 
  pinMode(sitUpSwitch, INPUT_PULLUP);   //initiate sitUpSwitch as an input
  pinMode(layDownSwitch, INPUT_PULLUP); //initiate layDownSwich as an input
  pinMode(armsOpen, INPUT_PULLUP);      //initiate armsOpen as an input
//  pinMode(lidSwitch, INPUT_PULLUP);     //initiate lidSwitch as an input (remove)
 // pinMode(A1, INPUT_PULLUP);            //initiate A1 as input for RESET SWITCH (No reset)
  lidServo.attach(lidServoPin);         //attach lidServo to lidServoPin0
  lidServo.write(0);                  //set to 180
  neckServo.attach(neckServoPin);       //attach neckServo to neckServoPin
  neckServo.write(0);                 //set to zero
  pinMode(sitUpMotor, OUTPUT);          //initiate sitUpMotor as an output
  pinMode(layDownMotor, OUTPUT);        //initiate layDownMotor as an output
  pinMode(led, OUTPUT);                 //initiate led as an output
  pinMode(clapMotor, OUTPUT);           //initiate clapMotor as an output
  pinMode(A0, OUTPUT);                  //initiate A0 as output for SERVO POWER RELAY
  timeCounter = 0;                      //reset timeCounter to 0
  delay(1000);                          //1 second delay for good measure
  
  digitalWrite(A0, LOW);         //initialize Servo Power Relay HIGH
  digitalWrite(sitUpMotor, LOW);   //initialize sitUpMotor HIGH
  digitalWrite(layDownMotor, LOW); //initialize layDownMotor HIGH
  digitalWrite(clapMotor, LOW);  //initialize clapMotor HIGH
  sittingUp = false;
  lidServo.detach();
  neckServo.detach();

}//END SETUP

//LOOP FUNCTION This code loops continuously
void loop() {   
 // if (/*digitalRead(closeRelay)==HIGH && digitalRead(lidSwitch) == LOW && */ digitalRead(openRelay) == LOW) sitUp();
 // if (digitalRead(closeRelay)==LOW /* && digitalRead(lidSwitch) == HIGH && digitalRead(openRelay) == HIGH */) layDown();

  if(digitalRead(openRelay) == LOW) {
    Serial.println("Open Pressed");
    sitUp();
  }
  if(digitalRead(closeRelay) == LOW) {
    Serial.println("Closed Pressed");
    layDown();
  }
  Serial.println("Waiting...");
  //sitUp();
  //layDown();
  


  delay(50);    //Added delay for good practice

}//END LOOP

/*
 * sitUp()
 * Triggered by openRelay
 * Will only trigger successfully when the sittingUp boolean is false (meaning the monkey is currently lying down)
 * If monkey is sitting up and sittingUp boolean is triggered, sitUp() does nothing
 * Runs through cycle of opening lid, sitting monkey up, lifting head, LED on, clapping start
 * 
 * return void
 * return to loop() and shut down sitUpMotor if timer expires on sitting up motion
 */
void sitUp() {
  if (sittingUp == false) {
    lidServo.attach(lidServoPin);
    neckServo.attach(neckServoPin);
     digitalWrite(A0, ON);         //connects servo power
    for (lidServoPos = lidServoClose; lidServoPos < lidServoOpen; lidServoPos++){
      lidServo.write(lidServoPos);                 //open 
      Serial.println(lidServoPos);
      delay(lidTime/180);               //wait for lid to open, divides time evenly across positional change (Servo set to 180 difference)
    }
    lidServo.detach();
    delay(250);
    digitalWrite(sitUpMotor, ON);                //activate sitUpMotor
    while(digitalRead(sitUpSwitch) == HIGH /* && timeCounter < layDownTime */) {   //wait for sitUpSwitch (with timer backup)
      Serial.println(digitalRead(sitUpSwitch));
      delay(10);         //checks sitUpSwitch every 10 milliseconds
      //timeCounter+=10;     //increment timeCounter by 10 milliseconds //Time counter is not necessary here
    }

    Serial.println("sitUp Switch triggered");
    digitalWrite(sitUpMotor, OFF);                //sitUpSwitch has gone low, break out of loop and turn off sitUpMotor
    //timeCounter = 0;                              //reset timeCounter (deprecated)
    digitalWrite(led, HIGH);              //turns on LED to proper voltage
    for (neckServoPos = neckServoDown; neckServoPos < neckServoUp; neckServoPos++){
      neckServo.write(neckServoPos);              //tilt head up
      delay(neckTime/180);            //wait for neck to complete (Servo set to 180 difference)
    }
    neckServo.detach();
    delay(500);
    digitalWrite(clapMotor, ON);                //activate clapping motor
    delay(500);
    digitalWrite(A0, OFF);        //cuts power to servos
    delay(50);
    sittingUp = true;
  }
}

/*
 * layDown()
 * Triggered by closeRelay
 * Only runs when the sittingUp is true (monkey is currently in the sitting Position)
 * If sittingUp boolean is false, layDown does nothing
 * Checks for proper arm position
 * Cycles through stopping claps, LED off, tilt head down, lay monkey down, close lid
 * 
 * return void
 * return to loop() and shut down layDownMotor if timer expires on laying down motion
 */
void layDown() {
  if (sittingUp == true) {
    lidServo.attach(lidServoPin);
    neckServo.attach(neckServoPin);
    bool b = true;
    digitalWrite(A0, ON);      //connects servo power
    while(b == true){
      if (digitalRead(armsOpen) == LOW) {            //only runs when arms are ready to close
        Serial.println("Arms Open Fully");
        digitalWrite(clapMotor, OFF);                 //stop clapping
        for (neckServoPos = neckServoUp; neckServoPos > neckServoDown-1; neckServoPos--){
            neckServo.write(neckServoPos);              //lower head
            Serial.println(neckServoPos);
            delay(neckTime/180);            //wait for neck to complete (Servo set to 180 difference)
        }
        neckServo.detach();
        delay(250);
        analogWrite(led, LOW);                        //turn off LED
        Serial.println("Monkey lays down");
        digitalWrite(layDownMotor, ON);             //activate layDownMotor
        while(digitalRead(layDownSwitch) == HIGH) {   //wait for layDownSwitch (with timer backup)
          delay(10);         //checks layDownSwitch every 10 milliseconds                   
          
        }
        Serial.println("Monkey Laying Down");
  
        digitalWrite(layDownMotor, OFF);              //stop layDownMotor
        for (lidServoPos = lidServoOpen; lidServoPos > lidServoClose; lidServoPos--){
          lidServo.write(lidServoPos);                 //close lid
          delay(lidTime/180);               //wait for lid to close, divides time evenly across positional change (Servo set to 180 difference)
        }
        lidServo.detach();
        sittingUp = false;
        b = false;
      }//END IF (armsOpen == LOW)
      Serial.println(b);
      Serial.println("Switch not triggered");
    }//END WHILE (b)
    delay(100);
    digitalWrite(A0, OFF);     //cuts power to servos
    delay(50);
  }//END if sittingUp
}//END layDown()


/*TODO
 * Prevent re-enable of situp function once completed (Completed)
 * Set inputs properly for relay
 * remove resetLayDown() (Completed)
 * final testing
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

