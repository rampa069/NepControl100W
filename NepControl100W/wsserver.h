//----------------------------------------------------------------------------------------------
// webserver and websockets
//----------------------------------------------------------------------------------------------

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

//-----------------------------------------------------------------------------------------
// Get Sensor Readings and return JSON object
String getSensorReadings(){
  readings["ptt"] = bool(!digitalRead(pttPIN) || !digitalRead(ptt2PIN));
  readings["bias"] = bool(digitalRead(biasPIN));
  readings["rxtxrelais"] = bool(digitalRead(txPIN));
  readings["bv"] = String(Read_BV(bvHLPIN));
  readings["band"] = String(G_bandcode);
  readings["bandMode"] = String(G_bandMode == 0 ? "AUTO" : "MANUAL");
  readings["ssid"] = String(WiFi.SSID());
  readings["rssi"] = String(WiFi.RSSI());
  readings["rst"] = dbm2smeter[get_SWert(WiFi.RSSI())];
  readings["time"] = String(timestamp());
  String jsonString = JSON.stringify(readings);
  return jsonString;
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Build a JSON response for a command acknowledgement
String buildCmdResponse(const char* cmd, bool ok, const char* msg) {
  JSONVar resp;
  resp["cmd"] = String(cmd);
  resp["ok"] = ok;
  resp["msg"] = String(msg);
  return JSON.stringify(resp);
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Validate that a band value is a valid LPF band
bool isValidBand(int band) {
  return (band == 160 || band == 80 || band == 60 || band == 40 ||
          band == 30 || band == 20 || band == 17 || band == 15 ||
          band == 12 || band == 10 || band == 6);
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Process a single JSON command from a WebSocket client
//
// Commands:
//   {"cmd":"setBand","band":20}        — switch to MANUAL mode, select LPF for 20m
//   {"cmd":"setAutoBand"}              — switch back to AUTO mode (HL2 bandvoltage)
//   {"cmd":"setPTT","state":true}      — remote PTT override on/off
//   {"cmd":"getStatus"}                — request immediate telemetry push
//
String processCommand(JSONVar& doc) {
  String cmd = (const char*) doc["cmd"];

  if (cmd == "setBand") {
    int band = (int)(double) doc["band"];
    if (!isValidBand(band)) {
      return buildCmdResponse("setBand", false, "invalid band");
    }
    G_bandMode = 1;
    G_remoteBand = band;
    Serial.print("CMD setBand: "); Serial.print(band); Serial.println("m (MANUAL mode)");
    return buildCmdResponse("setBand", true, String("band set to " + String(band) + "m, MANUAL mode").c_str());
  }

  if (cmd == "setAutoBand") {
    G_bandMode = 0;
    G_remoteBand = 0;
    Serial.println("CMD setAutoBand: AUTO mode");
    return buildCmdResponse("setAutoBand", true, "AUTO mode");
  }

  if (cmd == "setPTT") {
    G_remotePTT = (bool) doc["state"];
    Serial.print("CMD setPTT: "); Serial.println(G_remotePTT ? "ON" : "OFF");
    return buildCmdResponse("setPTT", true, G_remotePTT ? "remote PTT on" : "remote PTT off");
  }

  if (cmd == "getStatus") {
    return getSensorReadings();
  }

  return buildCmdResponse(cmd.c_str(), false, "unknown command");
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.print("WS recv: "); Serial.println((char*)data);

    // Legacy plaintext "getReadings" from the built-in dashboard
    if (strcmp((char*)data, "getReadings") == 0) {
      String sensorReadings = getSensorReadings();
      #ifdef DEBUG_STATE
      Serial.print(sensorReadings);
      #endif
      notifyClients(sensorReadings);
      return;
    }

    // JSON command processing
    JSONVar doc = JSON.parse((char*)data);
    if (JSON.typeof(doc) == "undefined" || doc.hasOwnProperty("cmd") == false) {
      Serial.println("WS: invalid JSON or missing 'cmd' field");
      return;
    }

    String response = processCommand(doc);
    ws.textAll(response);
  }
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
//-----------------------------------------------------------------------------------------
