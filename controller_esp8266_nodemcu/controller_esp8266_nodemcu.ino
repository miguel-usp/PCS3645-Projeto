#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <Arduino_JSON.h>
extern "C" {
  #include<user_interface.h>
}
//#include <ESP8266WebServer.h>

const char* ssid = "Virus";          // Enter SSID here
const char* password = "miguel314";  //Enter Password here

// Serial
const int SERIALBAUD = 9600;

// Timings
const int TIMING_BRANCH = 20;
const int TIMING_NODE = 10;

// Server
ESP8266WebServer server(80);

// Variables
std::vector<String> cars {"", "", "", ""};
int current_devices = 0;

void setup() {
    Serial.begin(SERIALBAUD, SERIAL_7O1);
    //Serial.println("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    //Serial.print("AP IP address: ");
    //Serial.println(IP);
    server.on("/dirs", handle_inputDirections);
}

void handle_inputDirections() {
    String args = server.arg("plain");

    if(args.length() > 0) {
        int i = 0;
        int vehicleId;
        int startNode;
        int direction;

        while(args.charAt(i) != '\0') {
            vehicleId = ((int) args.charAt(i)) - 48;
            startNode = ((int) args.charAt(i + 2)) - 48;
            
            i += 3;

            if(args.charAt(i) != ',') {
                startNode *= 10;
                startNode += ((int) args.charAt(i)) - 48;
                i += 2;
            } else {
                i++;
            }

            transmit_serial(0);
            transmit_serial(startNode);

            while(args.charAt(i) != ';') {
                direction = ((int) args.charAt(i)) - 48;

                i++;
                if(args.charAt(i) != ',') {
                    direction *= 10;
                    direction += ((int) args.charAt(i)) - 48;
                    i += 2;
                } else {
                    i++;
                }

                transmit_serial(direction);
            }

            i++;
        }
    }

    while(Serial.available()) {

    }
}

int receive_serial() {
    char receivedWord;
    int word[7];

    receivedWord = Serial.read();
    int_to_bin((int) receivedWord, 7, word);
}

void transmit_serial(int data) {
    int word[7];

    int lowerBinaryData[4];
    int upperBinaryData[2];

    int sum;

    char finalWord;

    if(data < 16) {
        int_to_bin((unsigned) data, 4, word);

        generateParity(word);

        sum = bin_to_int(word, 7);
        finalWord = (char) sum;

        Serial.print(finalWord);
    }
    else {
        int lowerBits, upperBits;
        lowerBits = (unsigned) (data << ((sizeof(int) * 8) - 4)) >> ((sizeof(int) * 8) - 4);
        upperBits = (unsigned) (data << ((sizeof(int) * 8) - 6)) >> ((sizeof(int) * 8) - 2);
        int_to_bin((unsigned) lowerBits, 4, lowerBinaryData);
        int_to_bin((unsigned) upperBits, 2, upperBinaryData);

        word[0] = lowerBits[0];
        word[1] = lowerBits[1];
        word[2] = lowerBits[2];
        word[3] = lowerBits[3];

        generateParity(word);

        sum = bin_to_int(word, 7);
        finalWord = (char) sum;

        Serial.print(finalWord);

        word[0] = upperBits[0];
        word[1] = upperBits[1];
        word[2] = 0;
        word[3] = 0;

        generateParity(word);

        sum = bin_to_int(word, 7);
        finalWord = (char) sum;

        Serial.print(finalWord);
    }
}

void generateParity(int* word) {
        if(word[0] != word[1]) {
            if(word[2] == 1) word[4] = 0;
            else word[4] = 1;
        } else {
            if(word[2] == 1) word[4] = 1;
            else word[4] = 0;
        }

        if(word[0] != word[1]) {
            if(word[3] == 1) word[5] = 0;
            else word[5] = 1;
        } else {
            if(word[3] == 1) word[5] = 1;
            else word[5] = 0;
        }

        if(word[0] != word[2]) {
            if(word[3] == 1) word[6] = 0;
            else word[6] = 1;
        } else {
            if(word[3] == 1) word[6] = 1;
            else word[6] = 0;
        }
}

void int_to_bin(unsigned int in, int count, int* out)
{
    /* assert: count <= sizeof(int)*CHAR_BIT */
    unsigned int mask = 1U << (count-1);
    int i;
    for (i = 0; i < count; i++) {
        out[i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
}

int bin_to_int(int* in, int count) {
    int i, res = 0, pow = 1;
    for (i = 0; i < count; i++) {
        if(in[i] == 1) {
            res += pow;
        }
        pow *= 2;
    }
    return res;
}

void loop() {
    current_devices = WiFi.softAPgetStationNum();
    if(current_devices != cars.size()) {
        cars_status();
    }
    server.handleClient();
    delay(1000);
}

void cars_status() {
    struct station_info *stat_info; 
    struct ip4_addr *IPaddress;
    IPAddress address;
    
    stat_info = wifi_softap_get_station_info();
    
    //Serial.print("Total connected devices are = ");
    //Serial.println(current_devices);
    
    while (stat_info != NULL) {
        IPaddress = &stat_info->ip;
        address = IPaddress->addr;

        //Serial.print("Device address is ");
        //Serial.println(address.toString());

        String response = httpGETRequest("http://" + address.toString());
        
        if(response == "{}") {
          //Serial.println("Non-car connected");
          return;
        }

        //Serial.println("Request done");
        
        JSONVar responseJSON = JSON.parse(response);

        int id = (int)responseJSON["ID"];

        //Serial.println(address.toString());
        
        cars[id] = address.toString();

        //Serial.println("Car ");
        //Serial.print(id);
        //Serial.print(" with IP ");
        //Serial.print(cars[id]);
        //Serial.println(" added to connected cars");

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
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        payload = http.getString();
        //Serial.println(payload);
    }
    else {
        //Serial.print("Error code: ");
        //Serial.println(httpResponseCode);
    }

    http.end();
    return payload;
}
