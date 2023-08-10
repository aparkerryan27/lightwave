#include "ota.h"
#include <FastLED.h>
//#include "patterns.h
//#include "sml_wifi.h"
#include <Wifi.h>
#include <ESP32Time.h>
#include <esp_now.h>

//MARK: Sizes
#define LRG_FIXTURE 106
#define MED_FIXTURE 96
#define SML_FIXTURE 60
#define FIXTURE_SIZE LRG_FIXTURE

#define LED_PIN 5
#define LED_BUILTIN 2

//RTC
ESP32Time rtc(0);

//Structure example to receive data
//Must match the sender structure
typedef struct color_struct {
  int h;
  int s;
  int v;
};
color_struct clr;

esp_now_peer_info_t peerInfo;
      
int h = 0;
int s = 0;
int v = 0;


//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&clr, incomingData, sizeof(clr));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("h: ");
  Serial.println(clr.h);
  Serial.print("s: ");
  Serial.println(clr.s);
  Serial.print("v: ");
  Serial.println(clr.v);
  Serial.println();
  h = clr.h;
  s = clr.s;
  v = clr.v;
 
}


//Colors
CHSV CHSV_YELLOW = CHSV(64, 255, 255);
CHSV CHSV_GREEN = CHSV(96, 255, 255);
CHSV CHSV_BLUE = CHSV(160, 255, 255);
CHSV CHSV_PURPLE = CHSV(192, 255, 255);

//WI_FI(BatLAN, thedarknet, 10, 0, 0, 154, 4500);
//OTA(5000);

CRGB leds[FIXTURE_SIZE];

//LightObjectPattern *pattern;
//enum PatternType patternType;
//bool patternChanged = true;

void(* resetFunc) (void) = 0; //declare power reset function


float arValue(float pt, float offset, int phrase_step, float atk, float atk_alpha, float rls, float rls_alpha){
  //AR
  //atk+rls are some values that adds up to 1
  //pt is an x value somewhere along the sum of atk+rls
  //alpha values are 0.1 (arcing down) to 10 (arcing up)

  //if (pt within active step) {
    //check for active steps behind
    //check for active steps in front
    //proportionally increase atk+rls to fill in the empty phrases
      //i.e. 3 empty steps, 1 behind, atk*3 and rls*3, and pt-1
  //}
  
  if (pt <= 0) {
    return 0;
  } else if (pt <= atk) {
    return pow(((1/atk)*pt),atk_alpha);
  } else if (pt <= atk + rls) {
    return pow(((-1/rls)*(pt-atk)+1),rls_alpha);
  } else { return 0; } //pt outside of phrase
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, FIXTURE_SIZE);

  rtc.setTime(0); //i believe this sets the device to epoch time
  
  //TODO: remove all serial instructions for production run
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register callback recv
  esp_now_register_recv_cb(OnDataRecv);
  
  /*
   * Serial.print("connecting to wifi");
   * unsigned long ms_time = millis();
  WIFI_INIT(Serial.print("."); delay(500); if (millis() - ms_time > 30000) {resetFunc(); });
  //OTA_INIT(); //TODO: will OTA cause interrupt problems or take too many cycles to check in regular operation, should i only have it run for the first minute?
  Serial.println("Done");
  analogWrite(LED_BUILTIN, HIGH); //turn the onboard LED to HIGH //TODO: blink or something, 
  delay(1000);
  analogWrite(LED_BUILTIN, 0);
  delay(1000);
  analogWrite(LED_BUILTIN, HIGH);
  delay(1000);
  analogWrite(LED_BUILTIN, 0);
  */
  
  Serial.print("MAC Address is.. ");
  Serial.println(WiFi.macAddress());
 
}

//MARK: WIFI Recieve and store data 
//what the ESP recieves from the lightwave client master
struct fromClientData {
  bool syncPulse = 0;
  int16_t r,g,b;
} data;

//what the ESP reports back to the lightwave client
struct toClientData {
  bool syncPulse = 0;
  int16_t r,g,b;
} report;

void loop() {
  //OTA_CHECK_UPDATES();
  //WIFI_ATTEND_CLIENT();
  //Serial.println(rtc.getMillis() + (rtc.getSecond() * 1000) + rtc.getMinute() * 60000);
  
  
 
  
/*
  //TODO: i'm not sure if this multi-class abstraction vs. one big class with all the
    //possibly unused state variables for different patterns and a milti-function
    //setup would be less cluttered, and then this logic would just always check each loop what the patternType is and run that function from all the same class
  if (patternChanged){
    patternChanged = false;
    switch (patternType) {
    case 1:
      pattern = new HueWash(leds, FIXTURE_SIZE);
    default:
      pattern = new Fill(leds, FiXTURE_SIZE);
    }
  }
 */

  EVERY_N_MILLISECONDS(20){  //TODO: alternative to delay and .show() might be FastLED.delay(1000/FRAMES_PER_SECOND);
   
    //int current_val = compute_envelope(); 
    
    //pattern.set_hsv(150, 150, 150); //this should be unchanging for patterns that internally manage all color state, and will change via command for others (most will have value set here by looping brightness function) 
    //pattern.draw();
    //fill_rainbow(leds, FIXTURE_SIZE, 150);
    fill_solid(leds, FIXTURE_SIZE, CHSV(h, s, v)); //TODO: not actually HSV at the moment, Max MSP problem, deal with laterr
    FastLED.show();
    
  }
  
}
