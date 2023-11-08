#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// PINS
const int MA1 = 5;   // Define the pin to which the LED is connected
const int MA2 = 4;   // Define the pin to which the LED is connected
const int MB1 = 14;  //
const int MB2 = 12;
const int BTN = 13;
const int LED = 15;

// Serial

const int SERIALBAUD = 9600;

const char* ssid = "Virus";          // Enter SSID here
const char* password = "miguel314";  //Enter Password here


// const char* ssid = "Wi-Fi Gauss";          // Enter SSID here
// const char* password = "miguelerebeca";  //Enter Password here

// WIFI
ESP8266WebServer server(80);

// Directions
std::vector<std::vector<int>> directions;
std::vector<int> direction;
unsigned long stop_direction_time = 0;  // time of last executed direction

void setup() {

    // PINS
    pinMode(MA1, OUTPUT);
    pinMode(MA2, OUTPUT);
    pinMode(MB1, OUTPUT);
    pinMode(MB2, OUTPUT);
    pinMode(BTN, INPUT);
    pinMode(LED, OUTPUT);

    // Serial
    Serial.begin(SERIALBAUD);
    delay(100);

    // Wifi
    Serial.println("Connecting to ");
    Serial.println(ssid);

    //connect to your local wi-fi network
    WiFi.begin(ssid, password);

    //check wi-fi is connected to wi-fi network
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected..!");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handle_OnConnect);
    server.on("/directions", handle_directions);
    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP server started");

    directions = std::vector<std::vector<int>>();
    direction = std::vector<int>();
}

// bool motorOn = false;
// void loop() {
//     if (checkBtnClick()) {
//         motorOn = !motorOn;
//         digitalWrite(LED, motorOn ? HIGH : LOW);
//         setMotorForce(motorOn ? 255 : 0, motorOn ? 255 : 0);
//     }
// }

bool forcedOn = false;
void loop() {
    // analogWrite(MA1, 255);
    // analogWrite(MA2, 0);
    // setMotorForce(255,255);
    // return;

    server.handleClient();

    if (checkBtnClick()) {
        Serial.println(forcedOn ? "Clicked to OFF" : "Click to ON");
        forcedOn = !forcedOn;
        setMotorForce(forcedOn ? 255 : 0, forcedOn ? 255 : 0);
    }

    // Check if there are any directions in the list
    if (!forcedOn) {
        unsigned long current_time = millis();
        if (directions.size() > 0) {
            // Get the current direction
            direction = directions[0];

            // Check if it's time to execute the direction
            if (stop_direction_time == 0 || (current_time >= stop_direction_time)) {
                Serial.print("Previous: ");
                Serial.println(stop_direction_time);
                setMotorForce(direction[0], direction[1]);
                stop_direction_time = current_time + direction[2];
                directions.erase(directions.begin());

                Serial.print("Running (");
                Serial.print(direction[0]);
                Serial.print(", ");
                Serial.print(direction[1]);
                Serial.print(", ");
                Serial.print(direction[2]);
                Serial.print(") at ");
                Serial.print(current_time);
                Serial.print(" until");
                Serial.println(stop_direction_time);
            }
        }
        else if (direction.size() > 2 && current_time >= stop_direction_time) {
            stop_direction_time = 0;
            setMotorForce(0, 0);
            direction = std::vector<int>();
        }
    }
}

void handle_OnConnect() {
    server.send(200, "text/plain", "Hello to the M&M car!");
}


void handle_directions() {
    // directions.clear();
    String csv_data = server.arg("plain");
    Serial.println("Received CSV data:");
    Serial.println(csv_data);

    // Parse CSV data and save to dynamic list in memory
    std::vector<int> row;
    String value;
    for (int i = 0; i < csv_data.length(); i++) {
        char c = csv_data.charAt(i);
        if (c == ',' || c == '\n') {
            row.push_back(value.toInt());
            value = "";
            if (c == '\n') {
                Serial.print("Row: ");
                for (int j = 0; j < row.size(); j++) {
                    Serial.print(row[j]);
                    Serial.print(", ");
                }
                Serial.println();
                directions.push_back(row);
                row.clear();
            }
        }
        else {
            value += c;
        }
    }
    server.send(200, "text/plain", "Received CSV data");
}

void handle_NotFound() {
    server.send(404, "text/plain", "Not found");
}

bool clicked = false;
long nextClick = 0;
int DEBOUNCETIME = 300;
bool checkBtnClick() {
    bool clickedNow = digitalRead(BTN) == HIGH;
    // digitalWrite(LED, clickedNow ? HIGH : LOW);
    long currentTime = millis();
    if (clickedNow != clicked && nextClick <= currentTime) {
        clicked = !clicked;
        nextClick = currentTime + DEBOUNCETIME;
        return clicked;
    }
    return false;
}

void doTurn(int duration, bool direction) {
    for (int i = 0; i < duration / 4; i++) {
        if (direction) setMotorForce(map(i, 0, duration / 2, 0, 255), map(i, 0, duration / 2, 0, -253));
        else setMotorForce(map(i, 0, duration / 2, 0, -255), map(i, 0, duration / 2, 0, 253));
        delay(1);
    }
    if (direction) setMotorForce(255, -253);
    else setMotorForce(-255, 253);
    delay(duration / 2);
    for (int i = 0; i < duration / 4; i++) {
        if (direction) setMotorForce(map(i, 0, duration / 2, 255, 0), map(i, 0, duration / 2, -253, 0));
        else setMotorForce(map(i, 0, duration / 2, -255, 0), map(i, 0, duration / 2, 253, 0));
        delay(1);
    }
    setMotorForce(0, 0);
}

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



void stopAll() {
    setMotorForce(0, 0);
}