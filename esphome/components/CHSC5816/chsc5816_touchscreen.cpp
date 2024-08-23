
#include "chsc5816_touchscreen.h"
#include "esphome/core/log.h"

namespace esphome {
namespace chsc5816 {

static const char *const TAG = "chsc5816.touchscreen";

void CHSC5816Touchscreen::setup() {
  ESP_LOGCONFIG(TAG, "Setting up CHSC5816 Touchscreen...");

  if (this->interrupt_pin_ != nullptr) {
    this->interrupt_pin_->setup();
    this->interrupt_pin_->pin_mode(gpio::FLAG_INPUT | gpio::FLAG_PULLUP);
    this->attach_interrupt_(this->interrupt_pin_, gpio::INTERRUPT_FALLING_EDGE);
  }

  // Initialize the CHSC5816 sensor
  uint8_t data[4];
  this->set_mode_(CHSC5816_MODE_REG);
  
  // Read Vendor ID
  if (this->read_register(CHSC5816_VENDOR_ID_REG, data, 1)) {
    ESP_LOGD(TAG, "Vendor ID: 0x%02X", data[0]);
  }
}

void CHSC5816Touchscreen::update_touches() {
  uint8_t status;
  this->read_register(CHSC5816_TD_STATUS, &status, 1);

  // Check if any touches are detected
  if (status == 0) return;

  // Read touch points from CHSC5816
  uint8_t buffer[6 * MAX_TOUCHES];
  this->read_register(CHSC5816_TOUCH_DATA, buffer, sizeof(buffer));

  for (int i = 0; i < MAX_TOUCHES; i++) {
    int16_t x = (buffer[6 * i + 0] & 0x0F) << 8 | buffer[6 * i + 1];
    int16_t y = (buffer[6 * i + 2] & 0x0F) << 8 | buffer[6 * i + 3];
    ESP_LOGD(TAG, "Touch #%d: X=%d, Y=%d", i, x, y);
  }
}

void CHSC5816Touchscreen::dump_config() {
  ESP_LOGCONFIG(TAG, "CHSC5816 Touchscreen:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Max touches: %d", MAX_TOUCHES);
}

}  // namespace chsc5816
}  // namespace esphome
