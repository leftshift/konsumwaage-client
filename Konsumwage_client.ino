/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include "HX711.h"

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
HX711 scale(A1, A0);

void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("SSID", "PASSWORD");
    USE_SERIAL.println("[SETUP] Connected.");
    
    USE_SERIAL.println("[SETUP] Setting up Scale...");

    scale.set_scale(2280.f);
    scale.tare();

    USE_SERIAL.print("[SETUP] Scale set up. Reading: ")

}

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        USE_SERIAL.print("[SCALE] taking measurement: ");
        weight = scale.get_units(10);
        USE_SERIAL.println(String(weight));

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://localhost:5000/put"); //HTTP


        USE_SERIAL.print("[HTTP] PUT...\n");
        // start connection and send HTTP header
        json = "{ \"value\" : " + String(weight) + "}";
        int httpCode = http.PUT(json);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(10000);
}

