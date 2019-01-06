#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <EEPROM.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <timer.h>

//for OTA https://tttapa.github.io/ESP8266/Chap14%20-%20WebSocket.html

#define DEBUG //to enable debug purpose serial output 
#define OTA
#define AP
#define AUT_IO

#define websocketSendTimer 5000 //#Data send rate - here to adjust how often the data should be sent to the server

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
int ioValues[48]; //Assign 3 bytes per I/O item, max 16 I/O items per node. 1st-2nd byte = 16 bit I/O value, 3rd byte = future usage

//#DeviceUUID - identifies the device at the server application
char Device_UUID[] = "e398d8b1-036f-4059-9bdd-de1e25967e79"; 

bool wsIsConnected = false;
bool wifiIsConnected = false;

auto timer = timer_create_default();
 
/*#IO_Header to be sent to the WebSocket Server that announces the I/O items in the ioValues[48] array in the format of IO_Name:byteOffsetInArray_IO_Type,IO_Name:byteOffsetInArray:IO_Type...
IO_Type can be
  - AI - Analog Input
  - AO - Analog Output
  - DI - Digital Input
  - DO - Digital Output*/
char *IO_Header = "IO_Name1:0:DI,IO_Name2:3:AI,IO_Name3:6:AO";  

void setup() {  
  loadWifiCredentials();
  WiFiMulti.addAP(WiFi_SSID, WiFi_Password);
  WiFi.onEvent(WiFiEvent);
  
  checkIfConfigModeReq(60000, true);
  configureAPSettings();
  configureWebSocket();
  timer.every(websocketSendTimer, valuesSendWebsocket);
}

void loop() {
  delay(1000);
  webserverInLoop();
  websocketInLoop();
  handleIO();
  timer.tick();
  Serial.println("Running...");
}
