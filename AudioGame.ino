//Libraries
#include <math.h>

//INPUT pins
//location controls
#define xPin A0
#define yPin A1

//other controls
#define sonarPin 2
#define activatePin 3

//OUTPUT pins
#define speaker 9

//control variables 
int PosX = 0;
int PosY = 0;
int pulse;
int score;
int alreadyStart = false;
int alreadyMove = false;

//calculated variables
int tPosX = 5;
int tPosY = 5;
double angle;
int xDistance;
int yDistance;
double tDistance;

//for calculated data for Max
const double pi = atan2(0, -1);
const double beginning = atan2(1, -1);

enum submarine{START, MOVE, SONAR, FAIL, SUCCESS};
submarine state = START; 

void setup() {
  Serial.begin(9600);
  
  //INPUT
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(sonarPin, INPUT);
  pinMode(activatePin, INPUT);
  
  //OUTPUT
  pinMode(speaker, OUTPUT);

}

void loop() {       
    switch (state) {
        case START:
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
}

//-------------- START state ----------------------------//
void starting() {

        //Send serial msg to Max to staart instructions
        if (!alreadyStart) {
            Serial.println("start 1");
            alreadyStart = true;
        }

        //Calibrate controls and activate to start
        if (analogRead(xPin) > 400 && analogRead(xPin) < 600 && 
            analogRead(yPin) > 400 && analogRead(yPin) < 600 &&
            digitalRead(activatePin) == HIGH) {
                pulse = 3;
                score = 0;
                alreadyMove = false;
                state = MOVE;
                delay(500);
        }
}


//-------------- MOVE state ----------------------------//
void moving() {

    //Send serial msg to Max to start playing ambient sound
    if (!alreadyMove) {
        Serial.println("move 1");
        alreadyMove = true;
    }
  
    triangulate();
      
    locationDebug(); 

    handleMove();

    if (digitalRead(activatePin) == HIGH) {
      handleActivate();   
    }
    
    if (digitalRead(sonarPin) == HIGH && pulse > 0) {
        pulse--;
        state = SONAR;   
    } else if (digitalRead(sonarPin) == HIGH && pulse == 0) {
        tone(speaker, 150, 500);
        Serial.println("No more pulse");
        delay(501);
    }
}


//Calculate distance and angle to target
void triangulate() {
    xDistance = tPosX-PosX;
    yDistance = tPosY-PosY;
    tDistance = sqrt(pow(xDistance, 2) + pow(yDistance, 2));
    angle = atan2(yDistance, xDistance);
    if (angle == pi) {
        angle = -angle;
    } else if (angle > beginning) {
        angle = angle - 2 * pi;
    }
}

//map xy control to between -5 to 5
void handleMove() {
    PosX = map(analogRead(xPin), 0, 1023, -5, 5);
    PosY = map(analogRead(yPin), 0, 1023, 5, -5);
}

//for debugging only
void locationDebug() {

    Serial.println(String(PosX) + " " + String(PosY) + " " + String(angle) + " " + String(tDistance));
    delay(500); // pause
}

//Win or lose depending on distance to target when activate
void handleActivate() {
    if (abs(xDistance) <= 2 && abs(yDistance) <= 2) {
        state = SUCCESS;
    } else {
        state = FAIL;
    }
}

//-------------- SONAR state ----------------------------//

//Send msg to Max to play sonar sound based on quad spatialization, then switch back to MOVE state
void playSonar() {
    Serial.println("sonar 1 " + String(angle) + " " + String(tDistance));
    alreadyMove = false;
    state = MOVE;
    delay(12000);
}


//-------------- SUCCESS state ----------------------------//

//Send msg to Max to play winning sound, then seed new target location
void win() {
    Serial.println("win 1");
    tone(speaker, 450, 250);
    tPosX = 3;
    tPosY = -5;
    pulse = 3;
    score++;
    state = MOVE;
    delay(251);
}


//-------------- FAIL state ----------------------------//

//Send msg to Max to play losing sound, then reset the game
void lose() {
    Serial.println("lose 1");
    tone(speaker, 50, 500);
    alreadyStart = false;
    state = START;
    delay(501);
}
  
// ------------ AUX ------------



