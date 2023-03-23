#include "sml_wifi.h"
#include "ota.h"
#include <FastLED.h>
//#include "patterns.h"

//MARK: Sizes
#define LRG_FIXTURE 106
#define MED_FIXTURE 96
#define SML_FIXTURE 60
#define FIXTURE_SIZE LRG_FIXTURE

#define LED_PIN 5
#define LED_BUILTIN 2

//Colors
CHSV CHSV_YELLOW = CHSV(64, 255, 255);
CHSV CHSV_GREEN = CHSV(96, 255, 255);
CHSV CHSV_BLUE = CHSV(160, 255, 255);
CHSV CHSV_PURPLE = CHSV(192, 255, 255);

WI_FI(BatLAN, thedarknet, 10, 0, 0, 154, 4500);
OTA(5000);

CRGB leds[FIXTURE_SIZE];

//LightObjectPattern *pattern;
//enum PatternType patternType;
//bool patternChanged = true;

void(* resetFunc) (void) = 0; //declare power reset function
      
int brightness = 0;

unsigned long sync_time = 0;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, FIXTURE_SIZE);
  
  Serial.begin(115200); //TODO: remove all serial instructions for production run
  Serial.print("connecting to wifi");
  sync_time = millis();
  WIFI_INIT(Serial.print("."); delay(500); if (millis() - sync_time > 30000) {resetFunc(); });
  OTA_INIT(); //TODO: will OTA cause interrupt problems or take too many cycles to check in regular operation, should i only have it run for the first minute?
  Serial.println("Done");
  analogWrite(LED_BUILTIN, HIGH); //turn the onboard LED to HIGH //TODO: blink or something, 
  delay(1000);
  analogWrite(LED_BUILTIN, 0);
  delay(1000);
  analogWrite(LED_BUILTIN, HIGH);
  delay(1000);
  analogWrite(LED_BUILTIN, 0);
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


  brightness = ((int)((millis()-sync_time)/5) %255); 
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
 
  EVERY_N_MILLISECONDS(15){  //TODO: alternative to delay and .show() might be FastLED.delay(1000/FRAMES_PER_SECOND);
   
    //if (envelopeChange) { compute_envelope(); }
    //current_val = compute_envelope(); 
    
    //pattern.set_hsv(150, 150, 150); //this should be unchanging for patterns that internally manage all color state, and will change via command for others (most will have value set here by looping brightness function) 
    //pattern.draw();
    fill_rainbow(leds, FIXTURE_SIZE, 150);
    FastLED.show();
    
  }

  

  
  delay(2); //can be helpful if there are conflicts
  
}
