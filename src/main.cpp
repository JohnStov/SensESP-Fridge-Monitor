#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "sensesp/signalk/signalk_output.h"
#include "sensesp/transforms/linear.h"
#include "sensesp_app_builder.h"
#include "sensesp_onewire/onewire_temperature.h"
#include "ssd1306_display.h"

using namespace sensesp;

ReactESP app;

char buff1[12];
char buff2[12];

auto temperature_to_string_fn = [](float input, const char* prefix) -> const char*
{
    auto degrees_c = input - 273.15; 

    strcpy(buff1, prefix);
    dtostrf(degrees_c, 3, 1, buff2);
    strcat(buff1, buff2); 
    return strcat(buff1, "C");
};

auto plate_temperature_to_string_fn = [](float input) -> const char*
{
  return temperature_to_string_fn(input, "Frz ");
};

auto fridge_temperature_to_string_fn = [](float input) -> const char*
{
  return temperature_to_string_fn(input, "Box ");
};

void setup() {
#ifndef SERIAL_DEBUG_DISABLED
  SetupSerialDebug(115200);
#endif


  // Create the global SensESPApp() object.
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
    ->set_hostname("SensESP Fridge Monitor")
    ->enable_uptime_sensor()
    ->enable_free_mem_sensor()
    ->get_app();

  /*
     Find all the sensors and their unique addresses. Then, each new instance
     of OneWireTemperature will use one of those addresses. You can't specify
     which address will initially be assigned to a particular sensor, so if you
     have more than one sensor, you may have to swap the addresses around on
     the configuration page for the device. (You get to the configuration page
     by entering the IP address of the device into a browser.)
  */

  /*
     Tell SensESP where the sensor is connected to the board
     ESP32 pins are specified as just the X in GPIOX
  */
  
  DallasTemperatureSensors* dts = new DallasTemperatureSensors(19);

  SSD1306DisplayController* display = new SSD1306DisplayController(21, 22);

  // Define how often SensESP should read the sensor(s) in milliseconds
  uint read_delay = 750;

  // Below are temperatures sampled and sent to Signal K server
  // To find valid Signal K Paths that fits your need you look at this link:
  // https://signalk.org/specification/1.4.0/doc/vesselsBranch.html

  // Measure fridge temperature
  auto* fridge_temp =
      new OneWireTemperature(dts, read_delay, "/fridgeCompartmentTemperature/oneWire");

  fridge_temp->connect_to(new Linear(1.0, 0.0, "/fridgeCompartmentTemperature/linear"))
      ->connect_to(new SKOutputFloat("environment.fridge.compartment.temperature",
                                     "/fridgeCompartmentTemperature/skPath",
                                     new SKMetadata("K", 
                                                    "Fridge Temperature", 
                                                    "Fridge Compartment Temperature",
                                                    "Fridge Temp")))
      ->connect_to(new LambdaTransform<float, const char*>(fridge_temperature_to_string_fn))
      ->connect_to(display, 0);

    // Measure fridge temperature
  auto* plate_temp =
      new OneWireTemperature(dts, read_delay, "/fridgePlateTemperature/oneWire");

  plate_temp->connect_to(new Linear(1.0, 0.0, "/fridgePlateTemperature/linear"))
      ->connect_to(new SKOutputFloat("environment.fridge.plate.temperature",
                                     "/fridgePlateTemperature/skPath",
                                     new SKMetadata("K", 
                                                    "Plate Temperature", 
                                                    "Fridge Cooling Plate Temperature",
                                                    "Plate Temp")))
      ->connect_to(new LambdaTransform<float, const char*>(plate_temperature_to_string_fn))
      ->connect_to(display, 1);

  // Configuration is done, lets start the readings of the sensors!
  sensesp_app->start();
}

// main program loop
void loop() { app.tick(); }