#include <Adafruit_GFX.h>  // Core graphics library
#include <Arduino.h>

#include "sensesp_app.h"
#include "ssd1306_display.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define ROW0 0
#define ROW1 10
#define ROW2 20
#define DELTA_GRAPH_HEIGHT 4

SSD1306DisplayController::SSD1306DisplayController(int sda_pin, int scl_pin)
    : networking{networking}, ws_client{ws_client} {
#ifdef ESP8266
  i2c = new TwoWire();
#elif defined(ESP32)
  i2c = new TwoWire(0);
#endif
  i2c->begin(sda_pin, scl_pin);
  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, i2c, -1);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    debugW("Display allocation failed");
  } else {
    display->clearDisplay();
    display->display();
  }
}

void SSD1306DisplayController::enable() {
  app.onRepeat(50, [this]() { this->update_delta_graph(); });
}

void SSD1306DisplayController::show_text(int baseline, const char* text) {
  // erase the old text
  display->fillRect(0, baseline, 132, 10, SSD1306_BLACK);
  display->setCursor(0, baseline);
  display->setTextColor(SSD1306_WHITE);
  display->print(text);
  display->display();
}

void SSD1306DisplayController::set_wifi_no_ap() {
  this->show_text(ROW0, "No AP");
  this->show_text(ROW2, "");
}

void SSD1306DisplayController::set_wifi_disconnected() {
  this->show_text(ROW0, "WiFi disc.");
  this->show_text(ROW2, "");
}

void SSD1306DisplayController::set_wifi_connected() {
  this->show_text(ROW0, WiFi.SSID().c_str());
  this->show_text(ROW2, WiFi.localIP().toString().c_str());
}

void SSD1306DisplayController::set_wifimanager() {
  this->show_text(ROW0, "WifiManager");
  this->show_text(ROW2, WiFi.localIP().toString().c_str());
}

void SSD1306DisplayController::set_ws_disconnected() {
  this->show_text(ROW1, "WS disc.");
}

void SSD1306DisplayController::set_ws_authorizing() {
  this->show_text(ROW1, "Authorizing");
}

void SSD1306DisplayController::set_ws_connecting() {
  this->show_text(ROW1, "WS connecting");
}

void SSD1306DisplayController::set_ws_connected() {
  this->show_text(ROW1,
                  sensesp_app->get_ws_client()->get_server_address().c_str());
}

void SSD1306DisplayController::set_input(SystemStatus new_value,
                                         uint8_t input_channel) {
  // FIXME: If pointers to member functions would be held in an array,
  // this would be a simple array dereferencing
  switch (new_value) {
    case SystemStatus::kWifiNoAP:
      this->set_wifi_no_ap();
      break;
    case SystemStatus::kWifiDisconnected:
      this->set_wifi_disconnected();
      break;
    case SystemStatus::kWifiManagerActivated:
      this->set_wifimanager();
      break;
    case SystemStatus::kWSDisconnected:
      this->set_wifi_connected();
      this->set_ws_disconnected();
      break;
    case SystemStatus::kWSAuthorizing:
      this->set_wifi_connected();
      this->set_ws_authorizing();
      break;
    case SystemStatus::kWSConnecting:
      this->set_wifi_connected();
      this->set_ws_connecting();
      break;
    case SystemStatus::kWSConnected:
      this->set_wifi_connected();
      this->set_ws_connected();
      break;
  }
}

void SSD1306DisplayController::set_input(int new_value, uint8_t input_channel) {
  num_deltas++;
}

void SSD1306DisplayController::update_delta_graph() {
  display->drawLine(graph_ptr, SCREEN_HEIGHT - DELTA_GRAPH_HEIGHT, graph_ptr,
                    SCREEN_HEIGHT - 1, false);
  int next_ptr = (graph_ptr + 1) % SCREEN_WIDTH;
  display->drawLine(next_ptr, SCREEN_HEIGHT - DELTA_GRAPH_HEIGHT, next_ptr,
                    SCREEN_HEIGHT - 1, false);
  int y = (SCREEN_HEIGHT - 1) -
          (num_deltas > DELTA_GRAPH_HEIGHT ? DELTA_GRAPH_HEIGHT : num_deltas);
  display->drawLine(graph_ptr, y, graph_ptr, SCREEN_HEIGHT - 1, true);
  display->display();
  num_deltas = 0;
  graph_ptr = next_ptr;
}
