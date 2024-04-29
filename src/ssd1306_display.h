#ifndef _SSD1306_DISPLAY_H_
#define _SSD1306_DISPLAY_H_

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "sensesp/system/startable.h"

using namespace sensesp;

class SSD1306DisplayController : public ValueConsumer<char*>
{
protected:
    TwoWire* i2c;
    Adafruit_SSD1306* display;

    void show_text(int baseline, const char* text);

public:
    SSD1306DisplayController(int sda_pin, int scl_pin, int screen_width=128, int screen_height=32);
    
    virtual void set_input(char* new_value, uint8_t input_channel=0) override;

};


#endif