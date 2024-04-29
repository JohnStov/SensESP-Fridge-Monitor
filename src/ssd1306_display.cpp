#include "sensesp_app.h"
#include "ssd1306_display.h"

using namespace sensesp;

#define ROW0 0
#define ROW1 18

SSD1306DisplayController::SSD1306DisplayController(int sda_pin, int scl_pin, int screen_width, int screen_height)
{
#ifdef ESP8266
    i2c = new TwoWire();
#elif defined(ESP32)
    i2c = new TwoWire(0);
#endif

    i2c->begin(sda_pin, scl_pin);
    display = new Adafruit_SSD1306(screen_width, screen_height, i2c, -1);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        debugW("Display allocation failed");
    } else {
        display->clearDisplay();
        display->display();
        display->setTextSize(2);
    }
}


void SSD1306DisplayController::set_input(char* new_value, uint8_t input_channel)
{
    switch (input_channel)
    {
        case 0:
            show_text(ROW0, new_value);
            break;
        case 1:
            show_text(ROW1, new_value);
            break;
        default:
            return;
    }
}

void SSD1306DisplayController::show_text(int baseline, const char* text) 
{
  // erase the old text
  display->fillRect(0, baseline, 132, 16, SSD1306_BLACK);
  display->setCursor(0, baseline);
  display->setTextColor(SSD1306_WHITE);
  display->print(text);
  display->display();
}
