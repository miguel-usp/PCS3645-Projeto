#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define MAXROUTE 32;  // max number of directions to store in memory

// Serial

const int SERIALBAUD = 9600;

const char* ssid = "Virus";          // Enter SSID here
const char* password = "miguel314";  //Enter Password here

// const char* ssid = "Wi-Fi Gauss";        // Enter SSID here
// const char* password = "miguelerebeca";  //Enter Password here


const char* hostname = "car-esp8266-0";

// WIFI
ESP8266WebServer server(80);

// Directions
std::vector<std::vector<int>> directions;
std::vector<int> direction;
unsigned long stop_direction_time = 0;  // time of last executed direction

void setup() {

    // Serial
    Serial.begin(SERIALBAUD, SERIAL_8N1);
    delay(100);

    // Wifi
    Serial.println("d Contectando a ");
    Serial.println(ssid);

    WiFi.hostname(hostname);
    Serial.println("Added hostname");
    //connect to your local wi-fi network
    WiFi.begin(ssid, password);
    Serial.println("Begun");

    //check wi-fi is connected to wi-fi network
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println(".");
    }
    Serial.println("");
    Serial.println("d WiFi connected..!");
    Serial.print("d Got IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handle_OnConnect);
    server.on("/dirs", handle_directions);
    server.on("/kill", handle_kill);
    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP server started");

    directions = std::vector<std::vector<int>>();
    direction = std::vector<int>();
}

bool forcedOn = false;
void loop() {
    server.handleClient();
}

void handle_OnConnect() {
    server.send(200, "text/plain", "Hello to the M&M car!");
}

void handle_kill() {
    Serial.println("k");
    server.send(200, "text/plain", "Killed");
}


void handle_directions() {
    // directions.clear();
    String csv_data = server.arg("plain");
    Serial.print("d Received directions data: ");
    Serial.println(csv_data);
    Serial.print("r");
    Serial.println(csv_data);

    // String value = "";
    // int car = -1, startPos = -1, curDirInd = 0;
    // int* directions[MAXROUTE];
    // for (int i = 0; i < csv_data.length(); i++) {
    //     char c = csv_data.charAt(i);
    //     if (c == ',') {
    //         if (car == -1) {
    //             car = value.toInt();
    //         }
    //         else if (startPos == -1) {
    //             startPos = value.toInt();
    //         }
    //         else {
    //             directions[curDirInd] = value.toInt();
    //             curDirInd++;
    //         }
    //     }
    //     else {
    //         value += c;
    //     }
    // }



    // Parse CSV data and save to dynamic list in memory
    // std::vector<int> row;
    // String value;
    // for (int i = 0; i < csv_data.length(); i++) {
    //     char c = csv_data.charAt(i);
    //     if (c == ',' || c == '\n') {
    //         row.push_back(value.toInt());
    //         value = "";
    //         if (c == '\n') {
    //             Serial.print("Row: ");
    //             for (int j = 0; j < row.size(); j++) {
    //                 Serial.print(row[j]);
    //                 Serial.print(", ");
    //             }
    //             Serial.println();
    //             directions.push_back(row);
    //             row.clear();
    //         }
    //     }
    //     else {
    //         value += c;
    //     }
    // }


    server.send(200, "text/plain", "Received CSV data");
}

void handle_NotFound() {
    server.send(404, "text/plain", "Not found");
}