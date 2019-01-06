void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            #ifdef DEBUG
             Serial.println("WebSocket Disconnected");
            #endif
            wsIsConnected = false;
            break;
        case WStype_CONNECTED:
            {
            #ifdef DEBUG
             Serial.println("WebSocket Connected");
            #endif
            wsIsConnected = true;
            webSocket.sendTXT("WELCOME");
            webSocket.sendTXT(Device_UUID);
            }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);

            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);
            //setOutputs()
            break;
    }
}

void configureWebSocket() {
      webSocket.setReconnectInterval(5000); //try to reconnect every 5 sec
      webSocket.beginSocketIO("192.168.0.123", 81);  //#webSocketServerIP
      webSocket.onEvent(webSocketEvent);
  }

void websocketInLoop() {
    webSocket.loop();
}

bool valuesSendWebsocket(void *) {   //argument because of timer requirement
  if (wsIsConnected){
   webSocket.sendBIN((uint8_t *) ioValues, sizeof(ioValues));
  }
   return true;  //true for the timer means it will be repeated
}

void headerSendWebSocket() {
  if (wsIsConnected){
   webSocket.sendTXT(IO_Header); 
  }
}
