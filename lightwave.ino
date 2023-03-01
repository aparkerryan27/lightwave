#include "wifi.h"
#include "ota.h"
#include "adsr.h"                             

#define DACSIZE 8192                                // vertical resolution

// variables
unsigned long   adsr_attack = 500000;               // time in µs - time to reach full
unsigned long   adsr_decay = 0;                // time in µs - time to reach sustain level
int             adsr_sustain = DACSIZE;                //default 1/2 sustain level -> from 0 to DACSIZE-1 //level to hold at between decay and release
unsigned long   adsr_release = 500000;              // time in µs (0.5s) //time it takes to fade back to zero
unsigned long   trigger_duration = 100000;          // time in µs
unsigned long   space_between_triggers = 300000;    // time in µs

// internal variables
bool trigger_on = false;                            // simple bool to switch trigger on and off
unsigned long   t = 0;                              // timestamp: current time
unsigned long   t_0 = 0;                            // timestamp: last trigger on/off event

// internal classes
adsr            adsr_class(DACSIZE);                // ADSR class initialization


WI_FI(BatLAN, thedarknet, 10, 0, 0, 154, 4500);
OTA(5000);

void setup() {

  delay(100);
  
  //MARK: initial ADSR settings
  adsr_class.setAttack(adsr_attack);                // initialize attack
  adsr_class.setDecay(adsr_decay);                  // initialize decay
  adsr_class.setSustain(adsr_sustain);              // initialize sustain
  adsr_class.setRelease(adsr_release);              // initialize release
  
  //MARK: wireless tools setup
  Serial.begin(115200);
  /*
  Serial.print("connecting to wifi");
  WIFI_INIT(Serial.print("."); delay(500););
  OTA_INIT();
  Serial.println("Done");
  */
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
  
  /*
   
  OTA_CHECK_UPDATES();
  WIFI_ATTEND_CLIENT();

  //recieve data into object format
  if (WIFI_DATA_AVAILABLE(data)) { //TODO: does this hang / will interrupt phasor pacing
    //Serial.println("new values!");
    report.r = data.r;
    report.g = data.g;
    report.b = data.b;
    
    //send a report back
    if (WIFI_SEND_DATA(report)) {
      //Serial.println("DATA SENT SUCCESSFULLY");
    }
    
  }
  */
  t = micros();                                     // take timestamp

  Serial.println(adsr_class.getWave(t));        

  if (trigger_on) {                                 
    if (t > t_0 + trigger_duration) {               // check if trigger can be switched off
      adsr_class.noteOff(t);                        // inform ADSR class that note/trigger is switched offs
      t_0 = t;                                      // reset timestamp
      trigger_on = false;                           // set trigger_on to false
    }
  }
  else {
    if (t > t_0 + space_between_triggers) {            // check if trigger can be switched on
      adsr_class.noteOn(t);                         // inform ADSR class that note/trigger is switched on
      t_0 = t;                                      // reset timestamp
      trigger_on = true;                            // set trigger_on to false
    }
  }
 
  //delay(1); //can be helpful if there are timing conflicts from the loop running too fast
  
}
