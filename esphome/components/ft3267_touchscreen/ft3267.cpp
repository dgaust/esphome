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
static const uint8_t FT3267_ADDR_TOUCH2_ID = 0x09;
static const uint8_t FT3267_ADDR_TOUCH2_X = 0x09;
static const uint8_t FT3267_ADDR_TOUCH2_Y = 0x0B;
static const uint8_t FT3267_GESTUREID = 0x01;
static const uint8_t FT3267_ADDR = 0x51;
static const char *const TAG = "ft3267Touchscreen";

#define FT5x06_ADDR                    (0x38)

#define FT5x06_DEVICE_MODE             (0x00)
#define FT5x06_GESTURE_ID              (0x01)
#define FT5x06_TOUCH_POINTS            (0x02)

#define FT5x06_TOUCH1_EV_FLAG          (0x03)
#define FT5x06_TOUCH1_XH               (0x03)
#define FT5x06_TOUCH1_XL               (0x04)
#define FT5x06_TOUCH1_YH               (0x05)
#define FT5x06_TOUCH1_YL               (0x06)

#define FT5x06_TOUCH2_EV_FLAG          (0x09)
#define FT5x06_TOUCH2_XH               (0x09)
#define FT5x06_TOUCH2_XL               (0x0A)
#define FT5x06_TOUCH2_YH               (0x0B)
#define FT5x06_TOUCH2_YL               (0x0C)

#define FT5x06_TOUCH3_EV_FLAG          (0x0F)
#define FT5x06_TOUCH3_XH               (0x0F)
#define FT5x06_TOUCH3_XL               (0x10)
#define FT5x06_TOUCH3_YH               (0x11)
#define FT5x06_TOUCH3_YL               (0x12)

#define FT5x06_TOUCH4_EV_FLAG          (0x15)
#define FT5x06_TOUCH4_XH               (0x15)
#define FT5x06_TOUCH4_XL               (0x16)
#define FT5x06_TOUCH4_YH               (0x17)
#define FT5x06_TOUCH4_YL               (0x18)

#define FT5x06_TOUCH5_EV_FLAG          (0x1B)
#define FT5x06_TOUCH5_XH               (0x1B)
#define FT5x06_TOUCH5_XL               (0x1C)
#define FT5x06_TOUCH5_YH               (0x1D)
#define FT5x06_TOUCH5_YL               (0x1E)

#define FT5x06_ID_G_THGROUP            (0x80)
#define FT5x06_ID_G_THPEAK             (0x81)
#define FT5x06_ID_G_THCAL              (0x82)
#define FT5x06_ID_G_THWATER            (0x83)
#define FT5x06_ID_G_THTEMP             (0x84)
#define FT5x06_ID_G_THDIFF             (0x85)
#define FT5x06_ID_G_CTRL               (0x86)
#define FT5x06_ID_G_TIME_ENTER_MONITOR (0x87)
#define FT5x06_ID_G_PERIODACTIVE       (0x88)
#define FT5x06_ID_G_PERIODMONITOR      (0x89)
#define FT5x06_ID_G_AUTO_CLB_MODE      (0xA0)
#define FT5x06_ID_G_LIB_VERSION_H      (0xA1)
#define FT5x06_ID_G_LIB_VERSION_L      (0xA2)
#define FT5x06_ID_G_CIPHER             (0xA3)
#define FT5x06_ID_G_MODE               (0xA4)
#define FT5x06_ID_G_PMODE              (0xA5)
#define FT5x06_ID_G_FIRMID             (0xA6)
#define FT5x06_ID_G_STATE              (0xA7)
#define FT5x06_ID_G_FT5201ID           (0xA8)
#define FT5x06_ID_G_ERR                (0xA9)

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
  uint8_t gesture_id= this->read_touch_gesture_();
  uint8_t x = this->read_touch_coordinate_(FT3267_ADDR_TOUCH1_X);
  uint8_t y = this->read_touch_coordinate_(FT3267_ADDR_TOUCH1_Y);
 
  ESP_LOGD("FT3267", "Gesture ID: %d", gesture_id);
  this->add_raw_touch_position_(touch_id, x, y);
  ESP_LOGD("FT3267", "Touches %d Touch %d detected at x: %d, y and gesture id %d: %d", touch_count, touch_id, x, y, gesture_id);
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
uint8_t ft3267Touchscreen::read_touch_coordinate_(uint8_t coordinate) {
  uint8_t read_buf[2];
  read_buf[0] = this->read_byte_(coordinate);
  read_buf[1] = this->read_byte_(coordinate + 1);
  return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}

uint8_t ft3267Touchscreen::read_touch_gesture_() {
  return gesture_read_byte_(FT5x06_GESTURE_ID);
}

uint8_t ft3267Touchscreen::read_touch_id_(uint8_t id_address) { return this->read_byte_(id_address) >> 4; }

uint8_t ft3267Touchscreen::read_byte_(uint8_t addr) {
  uint8_t byte = 0;
  this->read_byte(addr, &byte);
  return byte;
}

uint8_t ft3267Touchscreen::gesture_read_byte_(uint8_t reg_addr) {
  uint8_t byte = 0;
  // this->read_byte(reg_addr);
  byte = this->read_byte(reg_addr);
  return byte;
}


}  // namespace ft3267
}  // namespace esphome