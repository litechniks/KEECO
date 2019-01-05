void WiFiEvent(WiFiEvent_t event) {
  #ifdef DEBUG
    Serial.printf("[WiFi-event] event: %d\n", event);
  #endif
    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            stopAPServices();
          #ifdef DEBUG
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
          #endif
            wifiIsConnected = true;
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
          wifiIsConnected = false;
          configureAPSettings();
          #ifdef DEBUG
            Serial.println("WiFi lost connection");
          #endif
            break;
    }
}
