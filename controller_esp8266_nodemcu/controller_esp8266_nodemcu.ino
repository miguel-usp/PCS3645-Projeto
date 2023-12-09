#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
extern "C" {
  #include<user_interface.h>
}
//#include <ESP8266WebServer.h>

const char* ssid = "Virus";          // Enter SSID here
const char* password = "miguel314";  //Enter Password here

// Serial
const int SERIALBAUD = 9600;

// Variables
std::vector<String> cars {"", "", "", ""};
int current_devices = 0;

void setup() {
    Serial.begin(SERIALBAUD);
    Serial.println("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
}

void loop() {
    current_devices = WiFi.softAPgetStationNum();
    if(current_devices != cars.size()) {
        cars_status();
    }
    
    delay(1000);
}

void cars_status() {
    struct station_info *stat_info; 
    struct ip4_addr *IPaddress;
    IPAddress address;
    
    stat_info = wifi_softap_get_station_info();
    
    Serial.print("Total connected devices are = ");
    Serial.println(current_devices);
    
    while (stat_info != NULL) {
        IPaddress = &stat_info->ip;
        address = IPaddress->addr;

        Serial.print("Device address is ");
        Serial.println(address.toString());

        String response = httpGETRequest("http://" + address.toString());
        
        if(response == "{}") {
          Serial.println("Non-car connected");
          return;
        }

        Serial.println("Request done");
        
        JSONVar responseJSON = JSON.parse(response);

        int id = (int)responseJSON["ID"];

        Serial.println(address.toString());
        
        cars[id] = address.toString();

        Serial.println("Car ");
        Serial.print(id);
        Serial.print(" with IP ");
        Serial.print(cars[id]);
        Serial.println(" added to connected cars");

        stat_info = STAILQ_NEXT(stat_info, next);
    }
}

String httpGETRequest(String address) {
    WiFiClient wifiClient;
    HTTPClient http;
    http.begin(wifiClient, address);
    int httpResponseCode = http.GET();
    String payload = "{}"; 
        
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
        Serial.println(payload);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }

    http.end();
    return payload;
}
