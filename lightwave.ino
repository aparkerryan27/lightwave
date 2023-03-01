#include "wifi.h"
#include "ota.h"

WI_FI(BatLAN, thedarknet, 10, 0, 0, 154, 4500);
OTA(5000);

void(* resetFunc) (void) = 0; //declare power reset function

int led = 23;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is

unsigned long sync_time = 0;

void setup() {

  pinMode(led, OUTPUT);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("connecting to wifi");
  
  sync_time = millis();
  
  //TODO: check to see if this actually restarts the device after 30 seonds of waiting
  WIFI_INIT(Serial.print("."); delay(500); if (millis() - sync_time > 30000) {resetFunc(); });
  OTA_INIT();
  Serial.println("Done");
}

//MARK: WIFI Recieve and store data 
//what the ESP recieves from the lightwave client master
struct MyData {
  int16_t r,g,b;
} data;

//what the ESP reports back to the lightwave client
struct reportData {
  int16_t r,g,b;
} report;

void loop() {
  // put your main code here, to run repeatedly:

  OTA_CHECK_UPDATES();
  WIFI_ATTEND_CLIENT();
  //recieve data into object format
  if (WIFI_DATA_AVAILABLE(data)) {
    Serial.println("new values!");
    report.r = data.r;
    report.g = data.g;
    report.b = data.b;
    sync_time = millis();
    
    //send a report back
    if (WIFI_SEND_DATA(report)) {
      Serial.println("DATA SENT SUCCESSFULLY");
    }
    
  }

  //testing fadeout
  analogWrite(led, brightness);
  Serial.println(brightness);
  brightness = ((int)((millis()-sync_time)/5) %255); 

  delay(2); //can be helpful if there are conflicts
  
}
