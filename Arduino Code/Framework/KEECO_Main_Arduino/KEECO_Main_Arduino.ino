#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <EEPROM.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#define DEBUG //to enable debug purpose serial output 
#define OTA
#define AP

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#ifdef AP
 ESP8266WebServer webserver(80);
 IPAddress apIP(192, 168, 4, 1);
 IPAddress netMsk(255, 255, 255, 0);
#endif

//Wifi Credentials - 32 and 64 lengths are defined by standard. Values are loaded from EEPROM
char WiFi_SSID[33] = "";
char WiFi_Password[65] = "";

char *AP_SSID = "KEECO_Node";
char *AP_Password = "1234";

//#DeviceUUID - identifies the device at the server application
char Device_UUID[] = "e398d8b1-036f-4059-9bdd-de1e25967e79"; 

bool wsIsConnected = false;
bool wifiIsConnected = false;



void setup() {  
  loadWifiCredentials();
  WiFiMulti.addAP(WiFi_SSID, WiFi_Password);
  WiFi.onEvent(WiFiEvent);
  
  checkIfConfigModeReq(60000, true);
  configureAPSettings();
  
}

void loop() {
  delay(1000);
  webserverInLoop();
  Serial.println("Running...");
}
