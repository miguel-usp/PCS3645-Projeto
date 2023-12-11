#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Serial

const int SERIALBAUD = 9600;

// const char* ssid = "Virus";          // Enter SSID here
// const char* password = "miguel314";  //Enter Password here


const char* ssid = "Wi-Fi Gauss";          // Enter SSID here
const char* password = "miguelerebeca";  //Enter Password here

// WIFI
ESP8266WebServer server(80);

// Directions
std::vector<std::vector<int>> directions;
std::vector<int> direction;
unsigned long stop_direction_time = 0;  // time of last executed direction

void setup() {

    // Serial
    Serial.begin(SERIALBAUD);
    delay(100);

    // Wifi
    Serial.println("d Connecting to ");
    Serial.println(ssid);

    //connect to your local wi-fi network
    WiFi.begin(ssid, password);

    //check wi-fi is connected to wi-fi network
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("d WiFi connected..!");
    Serial.print("d Got IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handle_OnConnect);
    server.on("/directions", handle_directions);
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


void handle_directions() {
    // directions.clear();
    String csv_data = server.arg("plain");
    Serial.print("d Received CSV data: ");
    Serial.println(csv_data);

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