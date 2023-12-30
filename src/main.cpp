/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <stdlib.h>

WiFiClient espClient;
PubSubClient client(espClient);
TFT_eSPI tft = TFT_eSPI();

long lastMsg = 0;
char msg[50];
int value = 0;

#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <AsyncTCP.h>

AsyncWebServer server(80);
unsigned long ota_progress_millis = 0;

int fontSize = 2;

int host_start = 2;
int cpu_start = 50;
int net_start = 90;
int disk_start = 130;
int mem_start = 170;
int therm_start = 210;
int la_start = 250;
int availability_start = 290;

int cpu_green = 50;
int cpu_yellow = 80;

float net_green = 0.50;
float net_yellow = 0.80;

int disk_green = 70;
int disk_yellow = 80;

int mem_green = 70;
int mem_yellow = 80;

int therm_green = 60;
int therm_yellow = 80;

float la_green = 4.0;
float la_yellow = 6.0;

String hosts_array [] = {
    "k8s-1",
    "k8s-2",
    "k8s-3",
    "k8s-4", 
    "mfs-1",
    "mfs-2",
    "mfs-3",
    "mfs-4",   
    "pve",
    "router",
};

String net_array [] = {
    "bond0",
    "bond0",
    "bond0",
    "bond0",
    "enx001e06377171",
    "enx001e06377210",
    "enx001e06377166",
    "enx001e06377209",  
    "enp7s0",
    "igc0"
};

int tft_positioning[] = {
    30,
    51,
    72,
    93,
    114,
    135,
    156,
    177,
    198,
    219,
    240,
    261,
    282,
    303
};

#define TFT_NAVY 0x000F

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  for (byte idx = 0; idx < sizeof(hosts_array) / sizeof(hosts_array[0]); idx++) {

    String CPU_Topic = "homeassistant/sensor/" + hosts_array[idx] + "/cpu/state";
    String Net_Topic = "homeassistant/sensor/" + hosts_array[idx] + "/net_" + net_array[idx] + "/state";
    String Disk_Topic = "homeassistant/sensor/" + hosts_array[idx] + "/disk_usage__/state";
    String Mem_Topic = "homeassistant/sensor/" + hosts_array[idx] + "/virtual_memory/state";
    String Therm_Topic = "homeassistant/sensor/" + hosts_array[idx] + "/therm";
    String Availability_Topic = "homeassistant/sensor/" + hosts_array[idx] + "/cpu/availability";
    String LA_Topic = "homeassistant/sensor/" + hosts_array[idx] + "/la5";
   
    if (String(topic) == CPU_Topic) {
            if ((messageTemp.toFloat() < cpu_green) && (messageTemp.toFloat() > 0)) {
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString(messageTemp, cpu_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > cpu_green) && (messageTemp.toFloat() < cpu_yellow)) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
                tft.drawString(messageTemp, cpu_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > cpu_yellow) && (messageTemp.toFloat() < 100)) {
                tft.setTextColor(TFT_WHITE, TFT_RED);
                tft.drawString(messageTemp, cpu_start, tft_positioning[idx], fontSize);
            }
      }

    if (String(topic) == Net_Topic) {
        float adjustedNet = (messageTemp.toFloat() / 1024 / 1024);
        String adjustedNetString = String(adjustedNet, 2);
            if ((adjustedNetString.toFloat() < net_green)) {
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString(adjustedNetString, net_start, tft_positioning[idx], fontSize);
            }
            else if ((adjustedNetString.toFloat() > net_green) && (adjustedNetString.toFloat() < net_yellow)) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
                tft.drawString(adjustedNetString, net_start, tft_positioning[idx], fontSize);
            }
            else if ((adjustedNetString.toFloat() > net_yellow) && (adjustedNetString.toFloat() < 5.00)) {
                tft.setTextColor(TFT_WHITE, TFT_RED);
                tft.drawString(adjustedNetString, net_start, tft_positioning[idx], fontSize);
            }
      }

    if (String(topic) == Disk_Topic) {
            if ((messageTemp.toFloat() < disk_green) && (messageTemp.toFloat() > 0)) {
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString(messageTemp, disk_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > disk_green) && (messageTemp.toFloat() < disk_yellow)) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
                tft.drawString(messageTemp, disk_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > disk_yellow) && (messageTemp.toFloat() < 100)) {
                tft.setTextColor(TFT_WHITE, TFT_RED);
                tft.drawString(messageTemp, disk_start, tft_positioning[idx], fontSize);
            }
      }

    if (String(topic) == Mem_Topic) {
            if ((messageTemp.toFloat() < mem_green) && (messageTemp.toFloat() > 0)) {
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString(messageTemp, mem_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > mem_green) && (messageTemp.toFloat() < mem_yellow)) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
                tft.drawString(messageTemp, mem_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > mem_yellow) && (messageTemp.toFloat() <= 100)) {
                tft.setTextColor(TFT_WHITE, TFT_RED);
                tft.drawString(messageTemp, mem_start, tft_positioning[idx], fontSize);
            }
        }

      if (String(topic) == Therm_Topic) {
            if ((messageTemp.toFloat() < therm_green) && (messageTemp.toFloat() > 0)) {
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString(messageTemp, therm_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > therm_green) && (messageTemp.toFloat() < therm_yellow)) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
                tft.drawString(messageTemp, therm_start, tft_positioning[idx], fontSize);
            }
            else if ((messageTemp.toFloat() > therm_yellow) && (messageTemp.toFloat() < 140)) {
                tft.setTextColor(TFT_WHITE, TFT_RED);
                tft.drawString(messageTemp, therm_start, tft_positioning[idx], fontSize);
            }
        }
      if (String(topic) == Availability_Topic) {
            if (messageTemp == "online") {
              tft.setTextColor(TFT_GREEN, TFT_BLACK);
              tft.drawString("V", availability_start, tft_positioning[idx], fontSize);
            }
            else {
              tft.setTextColor(TFT_RED, TFT_BLACK);
              tft.drawString("X", availability_start, tft_positioning[idx], fontSize);              
            }
      }
            if (String(topic) == LA_Topic) {
              float adjustedLA = (messageTemp.toFloat());
              String adjustedLAString = String(adjustedLA, 1);
              if ((adjustedLAString.toFloat() < la_green)) {
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString(adjustedLAString, la_start, tft_positioning[idx], fontSize);
              }
            else if ((adjustedLAString.toFloat() > la_green) && (adjustedLAString.toFloat() < la_yellow)) {
                tft.setTextColor(TFT_RED, TFT_YELLOW);
                tft.drawString(adjustedLAString, la_start, tft_positioning[idx], fontSize);
            }
            else if ((adjustedLAString.toFloat() > net_yellow)) {
                tft.setTextColor(TFT_WHITE, TFT_RED);
                tft.drawString(adjustedLAString, la_start, tft_positioning[idx], fontSize);
            }     }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Cluster-Screen-1")) {
      Serial.println("connected");
      for (byte idx = 0; idx < sizeof(hosts_array) / sizeof(hosts_array[0]); idx++) {

        String availability = "homeassistant/sensor/" + hosts_array[idx] + "/cpu/availability";
        Serial.println(availability);
        client.subscribe(availability.c_str());

        String network = "homeassistant/sensor/" + hosts_array[idx] + "/net_" + net_array[idx] +"/state";
        Serial.println(network); 
        client.subscribe(network.c_str());

        String cpu = "homeassistant/sensor/" + hosts_array[idx] + "/cpu/state";
        client.subscribe(cpu.c_str());

        String disk = "homeassistant/sensor/" + hosts_array[idx] + "/disk_usage__/state";
        client.subscribe(disk.c_str());

        String memory = "homeassistant/sensor/" + hosts_array[idx] + "/virtual_memory/state";
        client.subscribe(memory.c_str());

        String temp = "homeassistant/sensor/" + hosts_array[idx] + "/therm";
        client.subscribe(temp.c_str());

        String la = "homeassistant/sensor/" + hosts_array[idx] + "/la5";
        client.subscribe(la.c_str());
        
      }

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Cluster's Screen 1 OTA Update Server.");
  });

  ElegantOTA.begin(&server);    
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
  Serial.println("HTTP server started");
  
  
  // seting up lcd
  Serial.print("Seting up lcd");
  tft.init();
  tft.setRotation(1);
  
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK); 
  tft.drawString("Host", host_start, 10, fontSize); 
  tft.drawString("CPU", cpu_start, 10, fontSize); 
  tft.drawString("Net", net_start, 10, fontSize); 
  tft.drawString("DSK", disk_start, 10, fontSize); 
  tft.drawString("Mem", mem_start, 10, fontSize);
  tft.drawString("TC", therm_start, 10, fontSize);
  tft.drawString("LA5", la_start, 10, fontSize);
  tft.drawString("A", availability_start, 10, fontSize);
  
  tft.drawFastVLine((cpu_start - 5), 0, 290, TFT_NAVY);
  tft.drawFastVLine((net_start - 5), 0, 290, TFT_NAVY);
  tft.drawFastVLine((disk_start - 5), 0, 290, TFT_NAVY);
  tft.drawFastVLine((mem_start - 5), 0, 290, TFT_NAVY);
  tft.drawFastVLine((therm_start - 5), 0, 290, TFT_NAVY);
  tft.drawFastVLine((availability_start - 5), 0, 290, TFT_NAVY);
  tft.drawFastVLine((la_start - 5), 0, 290, TFT_NAVY);

  // tft.drawFastHLine(0, 27, 290, TFT_NAVY);
  // tft.drawFastHLine(0, 48, 290, TFT_NAVY);
  // tft.drawFastHLine(0, 69, 290, TFT_NAVY);
  // tft.drawFastHLine(0, 90, 290, TFT_NAVY);
  // tft.drawFastHLine(0, 111, 290, TFT_NAVY);

  for (byte idx = 0; idx < sizeof(tft_positioning) / sizeof(tft_positioning[0]); idx++) {
    tft.drawFastHLine(0, (tft_positioning[idx] - 3), 300, TFT_NAVY);
  }

  for (byte idx = 0; idx < sizeof(hosts_array) / sizeof(hosts_array[0]); idx++) {
    tft.drawString(hosts_array[idx], host_start, tft_positioning[idx], fontSize);
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  ElegantOTA.loop();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  }
}