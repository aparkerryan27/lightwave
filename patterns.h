#pragma once
#include <FastLED.h>

//TODO: remove non crucial HSV resets from pattern changes?

/* 
  LightObjectPattern: the abstract base class of lighting objects effects 
  -- any broad variable change required by the controller regardless of pattern must be in this class so it is accessible
*/
class LightObjectPattern {
public:
  virtual void draw() = 0;  //forces pure virtual

  virtual void set_hsv(uint8_t new_h, uint8_t new_s, uint8_t new_v) {
    m_hue = new_h;
    m_sat = new_s;
    m_val = new_v;
  }

  //TODO: add a virtual setUpdateRate function, to modify the rate of change of internal state values (position, hue delta)

protected:
  uint8_t m_hue = 0;
  uint8_t m_sat = 0;
  uint8_t m_val = 0;
  int m_num_leds;
  CRGB* m_leds;
};


class HueWash : public LightObjectPattern {
//a better version of fill_rainbow with soft circular change in state
public:
  HueWash(CRGB* leds, int num_leds) {
    m_hue = 100;
    m_sat = 255;
    m_val = 50;
    m_leds = leds;
    m_num_leds = num_leds;
  }

  void draw() {
    for (int i = 0; i < m_num_leds; i++) {
      m_leds[i] = CHSV((uint8_t)(m_hue + (i * 5)), 255, m_val);
    }
    m_hue++;
  }
};

class Fill : public LightObjectPattern {
public:
  Fill(CRGB* leds, int num_leds) {
    m_leds = leds;
    m_num_leds = num_leds;
  }

  void draw() {
    fill_solid(m_leds, m_num_leds, CHSV(m_hue, m_sat, m_val));
  }
};


class Snake : public LightObjectPattern {
//a constantly forward moving set of active LEDs of a defined length
public:
  Snake(CRGB* leds, int num_leds, int tail_length) {
    m_leds = leds;
    m_num_leds = num_leds;
    m_hue = random(0, 255);
    m_sat = 255;
    m_val = 255;
    m_tail_length = tail_length;

    fill_solid(m_leds, m_num_leds, CHSV(0, 0, 0));
  }

  void update() {
    m_head_idx = (m_head_idx + 1) >= (m_num_leds - 1) ? 0 : m_head_idx + 1;
  }

  void draw() override {
    m_leds[m_head_idx] = CHSV(m_hue, m_sat, m_val);

    for (int i = 1; i < m_tail_length; i++) {
      int current_pos = m_head_idx - i;
      if (current_pos >= 0 && current_pos <= m_head_idx - 1)
        m_leds[current_pos] = CHSV(m_hue, m_sat, m_val);
    }
    int tail_delete = (m_head_idx - m_tail_length) < 0 ? m_num_leds + (m_head_idx - m_tail_length) : (m_head_idx - m_tail_length);
    m_leds[tail_delete] = CHSV(0, 0, 0);
    update();
  }

private:
  int m_head_idx{ 0 };
  int m_tail_length;
  int m_start_pos;
};


class Sparkle : public LightObjectPattern {
//a randomly generated
public:
  Sparkle(CRGB* leds, int num_leds) {
    m_leds = leds;
    m_num_leds = num_leds;
    m_hue = random(0, 255);
    m_sat = 255;
    m_val = 255;

    fill_solid(m_leds, m_num_leds, CHSV(0, 0, 0));
  }

  void draw() override {
    for (int i = 1; i < m_num_leds; i++) {
      m_leds[i] = (random(2) == 0) ? CHSV(m_hue, m_sat, m_val) : CHSV(0, 0, 0);
    }
  }
};

//NOTE: FastLED has a default color manipulations library for simpler stuff, should use as much of that as possible
//blend( const CRGB* src1, const CRGB* src2, CRGB* dest, uint16_t count, fract8 amountOfsrc2 ) probably useful for HueWash
