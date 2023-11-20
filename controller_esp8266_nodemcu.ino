#include <ESP8266WiFi.h>
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
std::vector<char*> cars;
int current_cars = 0;

void setup() {
    Serial.begin(SERIALBAUD);
    Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
}

void loop() {
    current_cars = WiFi.softAPgetStationNum();
    if(current_cars != cars.size()) {
        cars_status();
    }
    
    delay(1000);
}

void cars_status() {
    int i=0;
    struct station_info *stat_info; 
    struct ip_addr *IPaddress;
    IPAddress address;
    
    stat_info = wifi_softap_get_station_info();
    
    Serial.print("Total connected cars are = ");
    Serial.println(current_cars);
    
    while (stat_info != NULL) {
        IPaddress = &stat_info->ip;
        address = IPaddress->addr;

        String response = httpGETRequest("http://" + address.toString());
        JSONVar responseJSON = JSON.parse(response);

        int id = responseJSON["ID"].toInt();
        cars[id] = address;

        stat_info = STAILQ_NEXT(stat_info, next);
        i++;
    }
}

void httpGETRequest(String address) {
    HTTPClient http;
    http.begin(address);
    int httpResponseCode = http.GET();
    String payload = "{}"; 
        
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }

    http.end()
    return payload;
}