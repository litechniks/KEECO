void configureAPSettings() {
  #ifdef AP
  if (!wifiIsConnected) {
   WiFi.mode(WIFI_AP_STA); 
   WiFi.softAPConfig(apIP, apIP, netMsk);
   WiFi.softAP(AP_SSID, AP_Password);
   webserver.on("/", handleRoot);
   webserver.on("/wifisave", handleWifiSave);
   webserver.begin();
  }
  #endif
}

void stopAPServices() {
  #ifdef AP
    webserver.stop();
    WiFi.mode(WIFI_STA);
  #endif
}

void webserverInLoop() {
  #ifdef AP
   if (!wifiIsConnected) {
    webserver.handleClient(); 
   }
  #endif
}

/** Is this an IP? */
boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

void handleRoot() {
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webserver.sendContent(
    "<html><head></head><body>"
    "<h1>WiFi Configuration</h1>"
  );
  if (webserver.client().localIP() == apIP) {
    webserver.sendContent(String("<p>You are connected to this AP: ") + AP_SSID + "</p>");
  } else {
    webserver.sendContent(String("<p>You are connected to this WiFi network: ") + WiFi_SSID + "</p>");
  }
  webserver.sendContent(
    "\r\n<br />"
    "<table><tr><th align='left'>SoftAP configuration</th></tr>"
  );
  webserver.sendContent(String() + "<tr><td>SSID " + String(AP_SSID) + "</td></tr>");
  webserver.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.softAPIP()) + "</td></tr>");
  webserver.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN settings</th></tr>"
  );
  webserver.sendContent(String() + "<tr><td>SSID " + String(WiFi_SSID) + "</td></tr>");
  webserver.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.localIP()) + "</td></tr>");
  webserver.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN lista (refresh page to update list)</th></tr>"
  );
  #ifdef DEBUG
   Serial.println("scan start");
  #endif
  int n = WiFi.scanNetworks();
  #ifdef DEBUG
   Serial.println("scan done");
  #endif
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      webserver.sendContent(String() + "\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : " *") + " (" + WiFi.RSSI(i) + ")</td></tr>");
    }
  } else {
    webserver.sendContent(String() + "<tr><td>No WLAN found</td></tr>");
  }
  webserver.sendContent(
    "</table>"
    "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network::</h4>"
    "<input type='text' placeholder='SSID' name='n'/>"
    "<br /><input type='password' placeholder='password' name='p'/>"
    "<br /><input type='submit' value='Connect/Disconnect'/></form>"
    "</body></html>"
  );
  webserver.client().stop(); // Stop is needed because we sent no content length
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  webserver.arg("n").toCharArray(WiFi_SSID, sizeof(WiFi_SSID) - 1);
  webserver.arg("p").toCharArray(WiFi_Password, sizeof(WiFi_Password) - 1);
  webserver.sendHeader("Location", "/", true);
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webserver.client().stop(); // Stop is needed because we sent no content length
  storeWifiCredentials();
  if (strlen(WiFi_SSID) > 0) {
    WiFi.disconnect();
    WiFi.begin ( WiFi_SSID, WiFi_Password ); 
  }
}
