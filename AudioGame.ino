//Libraries
#include <math.h>

//INPUT pins
//location control
#define xPin A0
#define yPin A1

//other controls
#define sonarPin 2
#define activatePin 4

//OUTPUT pins
#define speaker 9

//control variables 
int PosX = 0;
int PosY = 0;
int pulse;
int alreadyStart = false;
int alreadyMove = false;

//calculated variables
int tPosX = 5;
int tPosY = 5;
const double pi = atan2(0, -5);
const double beginning = atan2(5, -5);
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
  pinMode(sonarPin, INPUT_PULLUP);
  pinMode(activatePin, INPUT_PULLUP);
  
  //OUTPUT
  pinMode(speaker, OUTPUT);

}

void loop() {       
    switch (state) {
        case START:
          delay(500);
          starting();
          break;
        
        case MOVE:
          moving();
          break;
        
        case SONAR:
          playSonar();
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

//-------------- START state ----------------------------//
void starting() {
    //if(Serial.available()){
      //  serialvalue = Serial.read(); // read a byte of serial data
        //started = 1; // set the started flag to on
     //}

    //if (started) {
        if (!alreadyStart) {
            Serial.print("start");
            Serial.print(" ");
            Serial.print(1);
            Serial.println();
            alreadyStart = true;
        }

        if (analogRead(xPin) > 400 && analogRead(xPin) < 500 && 
            analogRead(yPin) > 400 && 
            digitalRead(activatePin) == LOW) {
                pulse = 3;
                alreadyMove = false;
                state = MOVE;
                delay(300);
        }
    //}
}


//-------------- MOVE state ----------------------------//
void moving() {
    if (!alreadyMove) {
        Serial.print("move");
        Serial.print(" ");
        Serial.print(1);
        Serial.println();
        alreadyMove = true;
    }
  
    triangulate();
      
    //locationDebug();

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

void triangulate() {
    xDistance = tPosX-PosX;
    yDistance = tPosY-PosY;
    tDistance = sqrt(pow(xDistance, 2) + pow(yDistance, 2));
    tAzimuth = atan2(yDistance, xDistance);
    if (tAzimuth == pi) {
        tAzimuth = -tAzimuth;
    } else if (tAzimuth > beginning) {
        tAzimuth = tAzimuth - 2 * pi;
    }
}

void handleMove() {
    PosX = map(analogRead(xPin), 0, 1023, 5, -5);
    PosY = map(analogRead(yPin), 0, 1023, 5, -5);
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
    if (abs(xDistance) <= 2 && abs(yDistance) <= 2) {
        state = SUCCESS;
    } else {
        state = FAIL;
    }
}

//-------------- SONAR state ----------------------------//
void playSonar() {
    Serial.print("sonar");
    Serial.print(" ");
    Serial.print(1);
    Serial.print(" ");
    Serial.print(tAzimuth); // angle
    Serial.print(" ");
    Serial.print(tDistance); // distance
    Serial.println();
    alreadyMove = false;
    state = MOVE;
    delay(8000);
}


//-------------- SUCCESS state ----------------------------//
// Play sound, seed new location, increase counter
void win() {
    Serial.println("WIN");
    tone(speaker, 450, 250);
    PosX = 3;
    PosY = -5;
    state = MOVE;
    delay(251);
}


//-------------- FAIL state ----------------------------//
// Play sound, show score, reset counter
void lose() {
    Serial.println("LOSE");
    tone(speaker, 50, 500);
    alreadyStart = false;
    state = START;
    delay(501);
}
  
// ------------ AUX ------------

