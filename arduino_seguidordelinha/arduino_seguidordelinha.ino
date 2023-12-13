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

#define DEFSPEED 150
#define HALFSPEED 120
#define LOWSPEED 0
#define INFRAWHITE 0
#define INFRABLACK 1

#define CLKDEBOUNCETIME 500

#define STARTNODE 14
#define STARTDIR 1

struct MapNodeDir {
    int node;
    int directionArrival;
};

const MapNodeDir* MAP[15][4] = {
    {&MapNodeDir{1, 2}, nullptr, nullptr, nullptr},
    {&MapNodeDir{4, 2}, &MapNodeDir{2, 2}, &MapNodeDir{0, 0}, &MapNodeDir{8, 1}},
    {&MapNodeDir{3, 2}, nullptr, &MapNodeDir{1, 1}, &MapNodeDir{5, 1}},
    {nullptr, nullptr, &MapNodeDir{2, 0}, nullptr},
    {&MapNodeDir{5, 2}, nullptr, &MapNodeDir{1, 0}, &MapNodeDir{9, 1}},
    {&MapNodeDir{6, 2}, &MapNodeDir{2, 3}, &MapNodeDir{4, 0}, &MapNodeDir{10, 1}},
    {nullptr, nullptr, &MapNodeDir{5, 0}, nullptr},
    {&MapNodeDir{8, 2}, nullptr, nullptr, nullptr},
    {&MapNodeDir{9, 2}, &MapNodeDir{1, 3}, &MapNodeDir{7, 0}, &MapNodeDir{12, 1}},
    {&MapNodeDir{10, 2}, &MapNodeDir{4, 3}, &MapNodeDir{8, 0}, &MapNodeDir{13, 1}},
    {&MapNodeDir{11, 2}, &MapNodeDir{5, 3}, &MapNodeDir{9, 0}, &MapNodeDir{14, 1}},
    {nullptr, nullptr, &MapNodeDir{10, 0}, nullptr},
    {nullptr, &MapNodeDir{8, 3}, nullptr, nullptr},
    {nullptr, &MapNodeDir{9, 3}, nullptr, nullptr},
    {nullptr, &MapNodeDir{10, 3}, nullptr, nullptr} };


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

    Serial.begin(9600, SERIAL_8N1);
    Serial.println("Starting");
}

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



int* path = nullptr;
int pathLen = 0;

bool isturning = false;
int pathIndex = -1; // index of current item in path
int turningStep = 0; // step of the turning state machine

int nexStartNode = STARTNODE;
int nexStartDir = STARTDIR;

bool followPath(int* path, int pathSize) {
    // Directions:
    // 0 - Right
    // 1 - Left
    // 2 - Straight
    // 3 - Back
    // 4 - Stall
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


// Directions:
// 0 - Right
// 1 - Left
// 2 - Straight
// 3 - Back
// 4 - Stall

void processPath(String pathStr) {
    int* newPath = (int*)malloc(sizeof(int) * (pathLen + pathStr.length()));
    int i;
    for (i = 0; i < pathLen; i++) newPath[i] = path[i];
    int curNode = nexStartNode;
    for (i = 0; i < pathStr.length(); i++) {
        int absDir = pathStr.charAt(i) - '0';
        if (absDir == 4) newPath[i + pathLen] = 4; // Stall
        else {
            int relDir = (nexStartDir + 4 - MAP[curNode][absDir]->directionArrival) % 4;
            newPath[i + pathLen] = relDir;
            curNode = MAP[curNode][absDir]->node;
            nexStartDir = MAP[curNode][relDir]->directionArrival;
        }
    }
    pathLen += pathStr.length();
    if (path != nullptr) free(path);
    path = newPath;
}

bool walking = false;
String readSerial = "";
void loop() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
            if (readSerial.length() > 0) {
                switch (readSerial.charAt(0)) {
                case 'd':
                    Serial.print("DEBUG: ");
                    Serial.println(readSerial.substring(1));
                    break;
                case 'r':
                    processPath(readSerial.substring(1));
                    break;
                case 'k':
                    // kill
                    walking = false;
                    isturning = false;
                    setMotorForce(0, 0);
                    if (path != nullptr) {
                        free(path);
                        path = nullptr;
                    }
                    pathLen = 0;
                    break;
                default:
                    break;
                }
                readSerial = "";


                // if (path != nullptr) free(path);
                // path = (int*)malloc(sizeof(int) * readSerial.length());
                // pathLen = readSerial.length();
                // for (int i = 0; i < pathLen; i++) {
                //     path[i] = readSerial.charAt(i) - '0';
                // }
                // walking = true;
                // isturning = false;
                // turningStep = 0;
                // readSerial = "";
            }
        }
        else readSerial += c;
    }

    if (walking) walking = followPath();
    else setMotorForce(0, 0);

    digitalWrite(BUILTIN_LED, walking ? HIGH : LOW);

}