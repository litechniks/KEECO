void(* resetFunc) (void) = 0;//declare reset function at address 0

void checkIfConfigModeReq(unsigned int startupTimeOut, bool reset) {  //returns true if configuration happened
  bool configOverSerial = false;
  char serialData[100] = "";
  char terminationChar = '\r';
  unsigned int startupTimeStamp;
  
  Serial.setTimeout(1000);
  Serial.begin(115200);

  Serial.println(Device_UUID);
  
  startupTimeStamp = millis();
  while((WiFiMulti.run() != WL_CONNECTED)&&(millis()<startupTimeStamp+startupTimeOut)) {
       #ifdef DEBUG
        Serial.println("CONF_AVAIL");
       #endif
       if (Serial.readBytesUntil(terminationChar,serialData,100)){
        configOverSerial = true;
        break;
       }
  }

  if (configOverSerial){
    int8_t separator_position = -1;
    uint8_t search_index = 0;
    while ((separator_position == -1)&&(search_index<sizeof(serialData))) {
      if (serialData[search_index]==',') {
        separator_position = search_index;
      }
      else {
        search_index++;
      }
    }
    
    if (separator_position != -1) {
      for (int i = 0; i < separator_position; i++) {
            WiFi_SSID[i] = serialData[i]; 
            WiFi_SSID[i+1] = '\0'; 
      }
      
      for (int i = (separator_position + 1); i < sizeof(serialData)-23; i++) {
            WiFi_Password[i-(separator_position + 1)] = serialData[i]; 
            if (WiFi_Password[i-(separator_position + 1)]== 10 ) {
              WiFi_Password[i-(separator_position + 1)] = '\0'; //to remove \r from the end of serial string
            }
            WiFi_Password[i-(separator_position + 1)+1] = '\0'; 
      }
    }
     #ifdef DEBUG
      Serial.print("WiFi_SSID:");
      Serial.println(WiFi_SSID);
      Serial.print("WiFi_Password:");
      Serial.println(WiFi_Password);
     #endif
      Serial.println("CONFIG_ACK");   
      storeWifiCredentials();
      if (reset){
        resetFunc(); //call reset 
      }
    }   
    else {
    }
    Serial.println("CONF_NOT_AVAIL");  
}

 
