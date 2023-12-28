/**************************************************************************/
/*!
  Author: Gustavo Ambrozio
  Based on work by: Atsushi Sasaki (https://github.com/aselectroworks/Arduino-FT6336U)
*/
/**************************************************************************/

#include "ft3267.h"
#include "esphome/core/log.h"

// Registers
// Reference: https://focuslcds.com/content/FT6236.pdf
namespace esphome {
namespace ft3267 {

static const uint8_t FT3267_ADDR_TOUCH_COUNT = 0x02;

static const uint8_t FT3267_ADDR_TOUCH1_ID = 0x05;
static const uint8_t FT3267_ADDR_TOUCH1_X = 0x03;
static const uint8_t FT3267_ADDR_TOUCH1_Y = 0x05;

static const uint8_t FT3267_ADDR_TOUCH2_ID = 0x0B;
static const uint8_t FT3267_ADDR_TOUCH2_X = 0x09;
static const uint8_t FT3267_ADDR_TOUCH2_Y = 0x0B;

static const uint8_t FT3267_GESTUREID = 0x01;

static const char *const TAG = "ft3267Touchscreen";

void ft3267Touchscreen::setup() {
  ESP_LOGCONFIG(TAG, "Setting up FT3267Touchscreen Touchscreen...");
  if (this->interrupt_pin_ != nullptr) {
    this->interrupt_pin_->pin_mode(gpio::FLAG_INPUT | gpio::FLAG_PULLUP);
    this->interrupt_pin_->setup();
    this->attach_interrupt_(this->interrupt_pin_, gpio::INTERRUPT_FALLING_EDGE);
  }

  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
  }

  this->hard_reset_();

  // Get touch resolution
  this->x_raw_max_ = 240;
  this->y_raw_max_ = 240;
}

void ft3267Touchscreen::update_touches() {
  
  int touch_count = this->read_touch_count_();
  if (touch_count == 0) {
    return;
  }

  uint8_t touch_id = this->read_touch_id_(FT3267_ADDR_TOUCH1_ID);  // id1 = 0 or 1
  uint8_t gesture_id= this->test_gesture_(FT3267_GESTUREID);
  int16_t x = this->read_touch_coordinate_(FT3267_ADDR_TOUCH1_X);
  int16_t y = this->read_touch_coordinate_(FT3267_ADDR_TOUCH1_Y);
  ESP_LOGD("FT3267", "Gesture ID: %d", gesture_id);
  this->add_raw_touch_position_(touch_id, x, y);
  ESP_LOGD("FT3267", "Touch %d detected at x: %d, y and gesture id %d: %d", touch_id, x, y, gesture_id);
  if (touch_count >= 2) {
    touch_id = this->read_touch_id_(FT3267_ADDR_TOUCH2_ID);  // id2 = 0 or 1(~id1 & 0x01)
    x = this->read_touch_coordinate_(FT3267_ADDR_TOUCH2_X);
    y = this->read_touch_coordinate_(FT3267_ADDR_TOUCH2_Y);
    this->add_raw_touch_position_(touch_id, x, y);
    ESP_LOGD("FT3267", "Touch %d detected at x: %d, y: %d", touch_id, x, y);
  }
}

void ft3267Touchscreen::hard_reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
  }
}

void ft3267Touchscreen::dump_config() {
  ESP_LOGCONFIG(TAG, "ft3267 Touchscreen:");
  LOG_I2C_DEVICE(this);
  LOG_PIN("  Interrupt Pin: ", this->interrupt_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
}

uint8_t ft3267Touchscreen::read_touch_count_() { return this->read_byte_(FT3267_ADDR_TOUCH_COUNT); }

uint8_t ft3267Touchscreen::test_gesture_() { return this->read_byte_(FT3267_GESTUREID); }
// Touch functions
uint16_t ft3267Touchscreen::read_touch_coordinate_(uint8_t coordinate) {
  uint8_t read_buf[2];
  read_buf[0] = this->read_byte_(coordinate);
  read_buf[1] = this->read_byte_(coordinate + 1);
  return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}

uint16_t ft3267Touchscreen::read_touch_gesture_(uint8_t coordinate) {
  uint8_t read_buf[2];
  read_buf[0] = this->read_byte_(coordinate);
  read_buf[1] = this->read_byte_(coordinate + 1);
  ESP_LOGD("FT3267","Gesture: %d", coordinate);
  return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}

uint8_t ft3267Touchscreen::read_touch_id_(uint8_t id_address) { return this->read_byte_(id_address) >> 4; }

uint8_t ft3267Touchscreen::read_byte_(uint8_t addr) {
  uint8_t byte = 0;
  this->read_byte(addr, &byte);
  return byte;
}


}  // namespace ft3267
}  // namespace esphome