//Libraries
#include <math.h>

//INPUT pins
//location control
#define xPin A0
#define yPin A1

//other controls
#define sonarPin 6
#define activatePin 7

//OUTPUT pins
#define speaker 8

//control variables 
int PosX = 5;
int PosY = 5;
int pulse;

//calculated variables
int tPosX = 0;
int tPosY = 0;
const double pi = atan2(0, -10);
const double beginning = atan2(10, -10);
double tAzimuth = 0;
int xDistance;
int yDistance;
double tDistance;

//Serial comm varaibles
int serialvalue; 
int started = 0;

enum submarine{START, MOVE, SONAR, FAIL, SUCCESS};
submarine state = START; 

void setup()
{
  Serial.begin(9600); // open the arduino serial port
  //INPUT
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(sonarPin, INPUT);
  pinMode(activatePin, INPUT);
  
  //OUTPUT
  pinMode(speaker, OUTPUT);

}

void loop()
{

    switch (state) {
    case START:
      starting();
      break;
    
    case MOVE:
      moving();
      break;
    
    case SONAR:
      sonarPulse();
      break;
    
    case FAIL:
      lose();
      break;
    
    case SUCCESS:
      win();
      break;
  }

  //if(Serial.available()) // check to see if there's serial data in the buffer
  //{
    //serialvalue = Serial.read(); // read a byte of serial data
    //started = 1; // set the started flag to on
  //}

  //if(started) {
}

//-------------- START state --------------//
void starting() {
    Serial.println("Please calibrate");
    Serial.println("your control set x, y = 0");
    if (analogRead(xPin) > 450 && analogRead(xPin) < 550 && 
        analogRead(yPin) > 450 && analogRead(yPin) < 550 && 
        digitalRead(activatePin) == LOW) {
        pulse = 3;
          state = MOVE;
    }
}


//-------------- MOVE state --------------//
void moving() {
    
    locate();
      
    locationDebug();

    handleMove();
  
    if (digitalRead(activatePin) == LOW) {
      handleActivate();   
    }
    
    if (digitalRead(sonarPin) == LOW && pulse > 0) {
        pulse--;
      state = SONAR;   
    } else if (digitalRead(sonarPin) == LOW && pulse == 0) {
        tone(speaker, 150, 500);
        Serial.println("No more pulse");
        delay(501);
    }
}

void locate() {
    xDistance = abs(tPosX-PosX);
    yDistance = abs(tPosY-PosY);
    tDistance = sqrt(pow(xDistance, 2) + pow(yDistance, 2));
    tAzimuth = atan2(yDistance, xDistance);
    if (tAzimuth== pi) {
        tAzimuth = -tAzimuth;
    } else if (tAzimuth > beginning) {
        tAzimuth = tAzimuth - 2 *pi;
    } 
}

void handleMove() {
    PosX = (1023-analogRead(xPin)) / 50 - 10;
    PosY = (1023-analogRead(yPin)) / 50 - 10;
}

void locationDebug() {
    Serial.print(PosX);
    Serial.print(" ");
    Serial.print(PosY);
    Serial.print(" ");
    Serial.print(tAzimuth); // angle in radians
    Serial.print(" ");
    Serial.print(tDistance); // distance
    Serial.println();
    delay(500); // pause
}

void handleActivate() {
    if (xDistance <= 2 && yDistance <= 2) {
        state = SUCCESS;
    } else {
        state = FAIL;
    }
}

//-------------- SONAR state --------------//
void sonarPulse() {
    Serial.println("Pulsing");
    Serial.print(tAzimuth); // angle
    Serial.print(" ");
    Serial.print(tDistance); // distance
    Serial.println();
    state = MOVE;
    delay(2000);
}


//-------------- SUCCESS state --------------//
// Play sound, seed new location, increase counter
void win() {
    Serial.println("WIN");
    tone(speaker, 450, 250);
  PosX = 3;
    PosY = -5;
    state = MOVE;
    delay(251);
}


//-------------- FAIL state --------------//
// Play sound, show score, reset counter
void lose() {
    Serial.println("LOSE");
  tone(speaker, 50, 500);
    state = START;
    delay(501);
}
  
// ------------ AUX ------------

