#ifndef _SSD1306_DISPLAY_H_
#define _SSD1306_DISPLAY_H_

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "system/enable.h"
#include "system/visual_output_controller.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

class SSD1306DisplayController : public VisualOutputController, public Enable {
 protected:
  TwoWire* i2c;
  Adafruit_SSD1306* display;
  Networking* networking;
  WSClient* ws_client;

  int num_deltas = 0;
  int graph_ptr = 0;
  
  virtual void set_wifi_no_ap();
  virtual void set_wifi_disconnected();
  virtual void set_wifi_connected();
  virtual void set_wifimanager();

  virtual void set_ws_disconnected();
  virtual void set_ws_authorizing();
  virtual void set_ws_connecting();
  virtual void set_ws_connected();

  void show_text(int baseline, const char* text);

  void draw_circle(bool color);

  void update_delta_graph();

 public:
  SSD1306DisplayController(int sda_pin, int scl_pin);

  virtual void enable() override;

  // ValueConsumer interface for ValueConsumer<WifiState> (Networking object
  // state updates)
  virtual void set_input(WifiState new_value,
                         uint8_t input_channel = 0) override;
  // ValueConsumer interface for ValueConsumer<WSConnectionState>
  // (WSClient object state updates)
  virtual void set_input(WSConnectionState new_value,
                         uint8_t input_channel = 0) override;
  // ValueConsumer interface for ValueConsumer<int> (delta count producer
  // updates)
  virtual void set_input(int new_value, uint8_t input_channel = 0) override;

  void blip(int duration);
};


#endif
