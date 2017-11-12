#include <math.h>
//INPUT pins
#define xPin A0
#define yPin A1

//control variables 
int PosX = 0;
int PosY = 0;

//calculated variables
int tPosX = -10;
int tPosY = 10;
const double pi = atan2(0, -10);
const double beginning = atan2(10, -10);
double tAzimuth = 0;
int xDistance;
int yDistance;
double tDistance;
int lastValX = 0;
int lastValY = 0;

//Serial comm varaibles
int serialvalue; 
int started = 0;

enum abduction{START, MOVE, ERR, LOCATE, FAIL, SUCCESS};
abduction state = MOVE; 

void setup()
{
  Serial.begin(9600); // open the arduino serial port
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  lastValX = analogRead(xPin);
  lastValY = analogRead(yPin);
  Serial.println(lastValX);
  Serial.println(lastValY);
}

void loop()
{

    switch (state) {
    case START:
      break;
    
    case MOVE:
      moving();
      break;
    
    case LOCATE:
      break;
    
    case ERR:
      break;
    
    case FAIL:
      break;
    
    case SUCCESS:
      break;
  }

  //if(Serial.available()) // check to see if there's serial data in the buffer
  //{
    //serialvalue = Serial.read(); // read a byte of serial data
    //started = 1; // set the started flag to on
  //}

  //if(started) {
}



//-------------- MOVE state --------------//
void moving() {
    xDistance = tPosX-PosX;
    yDistance = tPosY-PosY;
    tDistance = abs(sqrt(pow(xDistance, 2) + pow(yDistance, 2)));
    tAzimuth = atan2(yDistance, xDistance);
    if (tAzimuth== pi) {
      tAzimuth = -tAzimuth;
    } else if (tAzimuth > beginning) {
        tAzimuth = tAzimuth - 2 *pi;
    }
    //to Max MSP
    Serial.print(PosX);
  Serial.print(" ");
  Serial.print(PosY);
    Serial.print(" ");
    Serial.print(tAzimuth); // angle
    Serial.print(" ");
    Serial.print(tDistance); // distance
    Serial.println();
    delay(1000); // pause

    if (analogRead(xPin) > lastValX) {
        PosX++;
    } else if (analogRead(xPin) < lastValX) {
        PosX--;
    }
    if (analogRead(yPin) > lastValY) {
        PosY++;
    } else if (analogRead(yPin) < lastValY) {
        PosY--;
    }
    
    lastValX = analogRead(xPin);
    lastValY = analogRead(yPin);
}

void grab() {
    if (tDistance <= 5) {
        state = SUCCESS;
    } else if (tDistance >= 5) {
        state = FAIL;
    }
}

//-------------- LOCATE state --------------//

//-------------- ERR state --------------//

//-------------- FAIL state --------------//

//-------------- SUCCESS state --------------//
// Play sound, seed new location 


