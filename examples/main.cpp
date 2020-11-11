#include <Arduino.h>

#include "controllers/ssd1306_display.h"
#include "sensesp_app.h"
#include "sensesp_app_builder.h"
#include "sensors/analog_input.h"
#include "signalk/signalk_output.h"
#include "system/led_controller.h"
#include "transforms/hysteresis.h"

#define SDA_PIN 21
#define SCL_PIN 22

ReactESP app([]() {
#ifndef SERIAL_DEBUG_DISABLED
  SetupSerialDebug(115200);
#endif

  // this creates an LED blinker controller
  LedController* led_controller = new LedController(LED_PIN);

  // this creates the SS1306 display controller
  SSD1306DisplayController* display_controller =
      new SSD1306DisplayController(SDA_PIN, SCL_PIN);

  SensESPAppBuilder builder;

  sensesp_app = builder.set_hostname("sensespstatusdisplay")
                    ->set_wifi("my-wifi-ssid", "my-wifi-password")
                    ->set_sk_server("skserver.lan", 80)
                    // these two lines connect the new controllers to 
                    // SensESPApp internals
                    ->add_visual_controller(led_controller)
                    ->add_visual_controller(display_controller)
                    ->get_app();

  // everything else is just to create some arbitrary data to be output

#ifdef ESP8266
  uint8_t analog_pin = A0;
#elif defined(ESP32)
  uint8_t analog_pin = 32;
#endif

  float output_scale = 3.3;
  uint read_delay = 191;

  const char* analog_in_config_path = "/indoor_illumination/analog_in";

  // Use AnalogInput as an example sensor. Connect it e.g. to a photoresistor
  // or a potentiometer with a voltage divider to get an illustrative test
  // input.

  auto* analog_input =
      new AnalogInput(analog_pin, read_delay, analog_in_config_path, output_scale);

  // Connect the analog input via a hysteresis transform
  // to an SKOutputBool object. The hysteresis function has arbitrary voltage
  // limits that trigger the function when moving one's hand over a
  // particular photoresistor test setup.

  analog_input->connect_to(new SKOutputNumber("indoor.illumination"));

  analog_input
      ->connect_to(new Hysteresis<float, bool>(0.3, 0.5, false, true,
                                               "/transforms/hysteresis"))
      ->connect_to(new SKOutputBool("indoor.illumination.hysteresis1"));

  analog_input
      ->connect_to(new Hysteresis<float, bool>(0.6, 1.0, false, true,
                                               "/transforms/hysteresis2"))
      ->connect_to(new SKOutputBool("indoor.illumination.hysteresis2"));

  sensesp_app->enable();
});
