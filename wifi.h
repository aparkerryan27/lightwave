#pragma once
#include <WiFi.h>

#define WI_FI(NETWORK, PASSWORD, IP1, IP2, IP3, IP4, PORT) \
const char* wifi_ssid = #NETWORK; \
const char* wifi_password = #PASSWORD; \
IPAddress wifi_local_IP(IP1, IP2, IP3, IP4); \ 
IPAddress wifi_gateway(IP1, IP2, IP3, 1); \
IPAddress wifi_subnet(255, 255, 255, 0); \
WiFiServer wifi_Server(PORT);\
WiFiClient wifi_client;

#define WIFI_INIT(...) \
  if (!WiFi.config(wifi_local_IP, wifi_gateway, wifi_subnet)) { \
    Serial.println("STA Failed to configure"); \
  } \
  delay(100); \
  WiFi.begin(wifi_ssid, wifi_password); \
  while (WiFi.status() != WL_CONNECTED) { \
    __VA_ARGS__;\
  }\
  wifi_Server.begin(); 


#define WIFI_ATTEND_CLIENT() \
  if (!wifi_client) { \
    wifi_client = wifi_Server.available(); \
  }


bool wifi_read_data(WiFiClient &, uint8_t *, size_t, bool);

template<typename T>
bool wifi_read_data(WiFiClient &client, T &v, bool wait){
  return wifi_read_data(client, (uint8_t *) &v, sizeof(T), wait);
}


#define WIFI_DATA_AVAILABLE(DATA) \
  wifi_read_data (wifi_client, DATA, false)


bool wifi_write_data(WiFiClient &, uint8_t *, size_t);

template<typename T>
bool wifi_write_data(WiFiClient &client, T &v){
    return wifi_write_data(client, (uint8_t *) &v, sizeof(v));
}

#define WIFI_SEND_DATA(DATA) \
  wifi_write_data(wifi_client, DATA)
  
