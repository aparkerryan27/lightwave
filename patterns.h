#pragma once

#include <FastLED.h>

enum PatternType {
  None = 0,
  HueWash = 1,
  Fill = 2,
  Snake = 3
}

// abstract base class of lighting objects effects
class LightObjectPattern {
public:
    virtual void draw();
    
    void fill(CHSV new_hsv){
        fill_solid(m_leds, m_num_leds, new_hsv); 
    }
    
    virtual void set_hsv(uint8_t new_h, uint8_t new_s, uint8_t new_v){
        m_hue = new_h;
        m_sat = new_s;
        m_value = new_v;
    }

    //any broad variable change required by the controller regardless of pattern must be in this class so it is accessible
    //TODO: add a setUpdateRate function, to modify the rate of change of internal state values (position, hue delta)
    
    int m_id {0};
protected:
    uint8_t m_hue;
    uint8_t m_sat;
    uint8_t m_value;
    CRGB* m_leds;
    int m_num_leds;
};


//Mark: Rainbow Wash
// a better version of fill_rainbow with soft circular change in state
class HueWash : public LightObjectPattern{ 
public:
  HueWash(CRGB* leds, int num_leds) {
    m_hsv = CHSV(m_hue, 255, 50);
    m_leds = leds;
    m_num_leds = num_leds;
  }

  void draw() {
    for(int i = 0; i < m_num_leds; i++) {
        m_leds[i] = CHSV((uint8_t)(m_hue + (i * 5)), 255, 255);
    }
    m_hue++; 
  }

private:
  uint8_t m_hue {0};   
};

//MARK: Pulse
//fill_solid(m_leds, m_num_leds, new_hsv) is a general command to fill the array with a solid color

class Snake : public LightObjectPattern{
public:
  Snake(CRGB* leds, int num_leds, int tail_length){
      m_leds = leds;
      m_num_leds = num_leds;
      m_hsv = CHSV(random(0, 255), 255, 255);

      m_tail_length = tail_length;
  }

  void update(){
    m_head_idx = (m_head_idx + 1) >= (m_num_leds - 1) ? 0 : m_head_idx + 1;
  }
  
  void draw() override{
    m_leds[m_head_idx] = m_hsv;

    for (int i = 1; i < m_tail_length; i++) {
      int current_pos = m_head_idx - i;
      if (current_pos >= 0 && current_pos <= m_head_idx - 1)
        m_leds[current_pos] = m_hsv;
    }
    int tail_delete = (m_head_idx - m_tail_length) < 0 ? m_num_leds + (m_head_idx - m_tail_length) :  (m_head_idx - m_tail_length);
    m_leds[tail_delete] = CHSV(0,0,0);
    update();
  }

private:  
  int m_head_idx{0};
  int m_tail_length;
  int m_start_pos;
  uint8_t m_hue;
  uint8_t m_value {255};
};


//TODO: FastLED has a default color manipulations library for simpler stuff, should use as much of that as possible
//blend( const CRGB* src1, const CRGB* src2, CRGB* dest, uint16_t count, fract8 amountOfsrc2 ) probably useful for HueWash
