//Definição dos pinos de controle do motor

#define MA1 3   // IN1
#define MA2 9   // IN2
#define MB1 5   // IN3
#define MB2 11  // IN4
#define BUILTIN_LED 13
#define BTN 4

//Definição dos pinos dos sensores
#define pin_S1 6
#define pin_S2 7

void setMotorForce(int motorA, int motorB) {
    if (motorA < 0) {
        analogWrite(MA1, 0);
        analogWrite(MA2, -motorA);
    }
    else {
        analogWrite(MA1, motorA);
        analogWrite(MA2, 0);
    }
    if (motorB < 0) {
        analogWrite(MB1, 0);
        analogWrite(MB2, -motorB);
    }
    else {
        analogWrite(MB1, motorB);
        analogWrite(MB2, 0);
    }
}




void setup() {
    //Setamos os pinos de controle dos motores como saída
    pinMode(MA1, OUTPUT);
    pinMode(MA2, OUTPUT);
    pinMode(MB1, OUTPUT);
    pinMode(MB2, OUTPUT);

    //Setamos os pinos dos sensores como entrada
    pinMode(pin_S1, INPUT);
    pinMode(pin_S2, INPUT);

    // Botão
    pinMode(BTN, INPUT);

    // Setamos o pino do LED como saída e escrevemos baixo
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);

    Serial.begin(9600);
}

#define DEFSPEED 150
#define HALFSPEED 120
#define LOWSPEED 0
#define INFRAWHITE 0
#define INFRABLACK 1

// void turn(bool right) {
//     while (true) {
//         bool s1_black = digitalRead(pin_S1) == INFRABLACK;
//         bool s2_black = digitalRead(pin_S2) == INFRABLACK;
//         if (!s1_black && !s2_black) {
//             setMotorForce(0, 0);
//             break;
//         }
//         setMotorForce(right ? (s2_black ? DEFSPEED : LOWSPEED) : (s1_black ? LOWSPEED : DEFSPEED),
//             right ? (s1_black ? LOWSPEED : DEFSPEED) : (s2_black ? DEFSPEED : LOWSPEED));
//     }
// }

#define CLKDEBOUNCETIME 500

long nextClick = 0;
bool checkClick() {
    bool clickedNow = digitalRead(BTN) == HIGH;
    long currentTime = millis();
    if (clickedNow && nextClick <= currentTime) {
        nextClick = currentTime + CLKDEBOUNCETIME;
        return true;
    }
    return false;
}




bool isturning = false;
int pathIndex = -1; // index of current item in path
int turningStep = 0; // step of the turning state machine
bool followPath(int* path, int pathSize) {
    // Directions:
    // 0 - Right
    // 1 - Left
    // 2 - Straight
    // 3 - Back
    // returns true if it will continue the path
    bool s1_black = digitalRead(pin_S1) == INFRABLACK;
    bool s2_black = digitalRead(pin_S2) == INFRABLACK;
    int turningDirection;
    if (isturning) {
        turningDirection = path[pathIndex];
        Serial.println(pathIndex);
        Serial.println(turningDirection);
        if (turningDirection < 2) {
            // Turn to either side
            if (!s1_black && !s2_black) isturning = false;
            else {
                if (turningDirection == 0)
                    setMotorForce(s2_black ? DEFSPEED : -HALFSPEED, s2_black ? -HALFSPEED : DEFSPEED);
                else
                    setMotorForce(s1_black ? -HALFSPEED : DEFSPEED, s1_black ? DEFSPEED : -HALFSPEED);
            }
        }
        else if (turningDirection == 2) {
            // Go straight
            if (!s1_black || !s2_black) isturning = false;
            else setMotorForce(DEFSPEED, DEFSPEED);
        }
        else {
            // Go back
            if (turningStep == 3 && !s1_black) {
                isturning = false;
                turningStep = 0;
            }
            else {
                turningStep = ((turningStep == 0) && s1_black) ? 1 : ((turningStep == 1) && !s1_black) || ((turningStep == 0) && !s1_black) ? 2
                    : ((turningStep == 2) && s1_black) ? 3
                    : turningStep;
                setMotorForce(-DEFSPEED, turningStep == 0 ? -DEFSPEED : DEFSPEED);
            }
        }
    }
    if (!isturning) {
        if (s1_black && s2_black) {
            setMotorForce(0, 0);
            delay(200);
            if (digitalRead(pin_S1) == INFRABLACK && digitalRead(pin_S2) == INFRABLACK) {
                pathIndex += 1;
                if (pathIndex >= pathSize) {
                    pathIndex = -1;
                    return false;
                }
                isturning = true;
            }
        }
        else setMotorForce(s1_black ? LOWSPEED : DEFSPEED, s2_black ? LOWSPEED : DEFSPEED);
    }
    return true;
}

int* path = nullptr;
int pathLen = 0;

// Directions:
// 0 - Right
// 1 - Left
// 2 - Straight
// 3 - Back
bool walking = false;
void loop() {
    if (checkClick()) {
        for (int i = 0; i < 3; i++) {
            digitalWrite(BUILTIN_LED, HIGH);
            delay(200);
            digitalWrite(BUILTIN_LED, LOW);
            delay(800);
        }

        walking = !walking;
        isturning = false;
        // setMotorForce(walking ? DEFSPEED : 0, walking ? DEFSPEED : 0); 
        turningStep = 0;
        if (path != nullptr) free(path);
        path = (int*)malloc(sizeof(int) * 5);
        pathLen = 5;
        path[0] = 1;
        path[1] = 2;
        path[2] = 0;
        path[3] = 0;
        path[4] = 1;
    }
    if (walking) walking = followPath(path, pathLen);
    else setMotorForce(0, 0);
    digitalWrite(BUILTIN_LED, walking ? HIGH : LOW);

}