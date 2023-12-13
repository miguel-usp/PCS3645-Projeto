#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define MAXROUTE 32;  // max number of directions to store in memory

// Serial

const int SERIALBAUD = 9600;

const char* ssid = "Virus";          // Enter SSID here
const char* password = "miguel314";  //Enter Password here

// const char* ssid = "Wi-Fi Gauss";        // Enter SSID here
// const char* password = "miguelerebeca";  //Enter Password here


const char* hostname = "central-esp8266";

// WIFI
ESP8266WebServer server(80);

// Directions
std::vector<std::vector<int>> directions;
std::vector<int> direction;
unsigned long stop_direction_time = 0;  // time of last executed direction

void setup() {

    // Serial
    Serial.begin(SERIALBAUD, SERIAL_701);
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

    server.begin();
    Serial.println("HTTP server started");

    directions = std::vector<std::vector<int>>();
    direction = std::vector<int>();
}

bool forcedOn = false;
void loop() {
    server.handleClient();
}

handle_OnConnect() {
    String args = server.arg("plain");
    Serial.println(args);
}