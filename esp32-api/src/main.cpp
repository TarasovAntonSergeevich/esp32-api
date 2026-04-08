#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const int GPIO_PINS[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
const int NUM_PINS = sizeof(GPIO_PINS) / sizeof(GPIO_PINS[0]);

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 GPIO Control</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; padding: 20px; background: #f0f0f0; }
    h1 { color: #333; }
    .gpio-group { margin: 20px 0; }
    .gpio-item { 
      display: inline-block; 
      margin: 5px; 
      padding: 15px;
      background: white;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    .gpio-item h3 { margin: 0 0 10px 0; }
    button {
      padding: 10px 20px;
      margin: 2px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
      font-size: 14px;
    }
    .on { background: #4CAF50; color: white; }
    .off { background: #f44336; color: white; }
    .status { font-size: 12px; margin-top: 5px; }
    .on-status { color: #4CAF50; }
    .off-status { color: #f44336; }
  </style>
</head>
<body>
  <h1>ESP32 GPIO Control API</h1>
  <p>IP: <span id="ip"></span></p>
  
  <div class="gpio-group" id="gpio-container"></div>

  <script>
    function updateStatus(pin, state) {
      document.getElementById('status-' + pin).textContent = state ? 'ON' : 'OFF';
      document.getElementById('status-' + pin).className = 'status ' + (state ? 'on-status' : 'off-status');
    }

    async function setPin(pin, state) {
      try {
        const response = await fetch('/gpio/' + pin + '/' + (state ? 'on' : 'off'));
        const data = await response.json();
        updateStatus(pin, data.state);
      } catch (e) {
        console.error(e);
      }
    }

    function createGpioControls() {
      const container = document.getElementById('gpio-container');
      [0,1,2,3,4,5,6,7,8].forEach(pin => {
        const div = document.createElement('div');
        div.className = 'gpio-item';
        div.innerHTML = `
          <h3>GPIO ${pin}</h3>
          <button class="on" onclick="setPin(${pin}, true)">ON</button>
          <button class="off" onclick="setPin(${pin}, false)">OFF</button>
          <div class="status off-status" id="status-${pin}">OFF</div>
        `;
        container.appendChild(div);
      });
    }

    createGpioControls();
    document.getElementById('ip').textContent = window.location.hostname;
  </script>
</body>
</html>
)";

String getStateJson() {
  String json = "{";
  for (int i = 0; i < NUM_PINS; i++) {
    if (i > 0) json += ",";
    json += "\"gpio" + String(GPIO_PINS[i]) + "\":" + String(digitalRead(GPIO_PINS[i]));
  }
  json += "}";
  return json;
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(GPIO_PINS[i], OUTPUT);
    digitalWrite(GPIO_PINS[i], LOW);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getStateJson());
  });

  for (int i = 0; i < NUM_PINS; i++) {
    int pin = GPIO_PINS[i];
    
    server.on("/gpio/" + String(pin) + "/on", HTTP_POST, [pin](AsyncWebServerRequest *request) {
      digitalWrite(pin, HIGH);
      request->send(200, "application/json", "{\"gpio\":" + String(pin) + ",\"state\":1}");
    });

    server.on("/gpio/" + String(pin) + "/off", HTTP_POST, [pin](AsyncWebServerRequest *request) {
      digitalWrite(pin, LOW);
      request->send(200, "application/json", "{\"gpio\":" + String(pin) + ",\"state\":0}");
    });

    server.on("/gpio/" + String(pin) + "/toggle", HTTP_POST, [pin](AsyncWebServerRequest *request) {
      int current = digitalRead(pin);
      digitalWrite(pin, !current);
      request->send(200, "application/json", "{\"gpio\":" + String(pin) + ",\"state\":" + String(!current) + "}");
    });

    server.on("/gpio/" + String(pin) + "/state", HTTP_GET, [pin](AsyncWebServerRequest *request) {
      request->send(200, "application/json", "{\"gpio\":" + String(pin) + ",\"state\":" + String(digitalRead(pin)) + "}");
    });
  }

  server.begin();
  Serial.println("Server started");
}

void loop() {
}