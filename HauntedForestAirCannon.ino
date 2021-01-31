/*
Remote controlled air cannons for the Haunted Forest At Grand Farms controleld via wifi

<ip>/open -- Opens the relay
<ip>/close -- Closes the relay
<ip>/pulse -- Opens the relay for 100ms, then closes it

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Timer.h"

//wifi credentials
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

//Relay pin
#define RELAY 12

//start a server on port 80
ESP8266WebServer server(80);

//Timer for the pulse function
Timer t;

//Sends back the uptime of the air cannon, mainly so we know if its working or not.
void handleRoot()
{
    String toReturn = "Hello World.</br>Uptime: ";
    toReturn = toReturn + millis();
    toReturn += "</br></br>/on</br>/off</br>";
    server.send(200, "text/html", toReturn); //Send web page
}

//Opens up the relay, and sends back 200 OK
void handleOpen() 
{
    openRelay();
    send200OK();
}

//Closes the relay, and sends back 200 OK
void handleClose() 
{
    closeRelay();
    send200OK();
}

//Opens the relay, the closes it after 100ms
void handlePulse() 
{
    send200OK();
    openRelay();
    t.after(100, closeRelay);
}

//Utility function to send 200OK as a reponse
void send200OK() 
{
    server.send(200, "text/html", "OK");
}

//Utility function to close the relay
void closeRelay() 
{
    digitalWrite(RELAY, LOW);
}

//Utility function to open the relay
void openRelay() 
{
    digitalWrite(RELAY, HIGH);
}

//Initial setup process
void setup()
{
    //We need to make sure the ESP doesn't go into deep sleep
    WiFi.mode(WIFI_STA);
    wifi_set_sleep_type(NONE_SLEEP_T);

    pinMode(RELAY, OUTPUT);
    digitalWrite(RELAY, LOW);

    //Needs a bit of a delay to initalize the serial. Not exactly sure why.
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println();
    Serial.print("Trying to connect to: ");
    Serial.println(ssid);

    //This is so we can identify the air cannons on our network
    WiFi.hostname("ESP-AirCannon");

    //connect to wifi
    WiFi.begin(ssid, password);

    //Prints dots until it is connected
    Serial.println("Connecting...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    //Prints the IP of the device
    Serial.println("");
    Serial.println("Connected!");
    Serial.println("Your IP address is: ");
    Serial.println(WiFi.localIP());

    //Routes
    server.on("/", handleRoot);
    server.on("/open", handleOpen);
    server.on("/close", handleClose);
    server.on("/pulse", handlePulse);

    //start the HTTP server
    server.begin();
    Serial.println("HTTP server started");
}

//Handle the loop. This needs to run as fast as possable
void loop()
{
    t.update();
    server.handleClient();
}
