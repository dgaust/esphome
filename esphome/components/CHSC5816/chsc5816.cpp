
#include "chsc5816.h"
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

  // Initialize the CHSC5816 sensor (e.g., reset, configure I2C)
}

void CHSC5816Touchscreen::dump_config() {
  ESP_LOGCONFIG(TAG, "CHSC5816 Touchscreen:");
  LOG_I2C_DEVICE(this);
}

void CHSC5816Touchscreen::loop() {
  this->read_touch_data();
}

void CHSC5816Touchscreen::read_touch_data() {
  uint8_t status;
  this->read_register(CHSC5816_TD_STATUS, &status, 1);

  if (status == 0) return;  // No touch detected

  uint8_t buffer[6 * MAX_TOUCHES];
  this->read_register(CHSC5816_TOUCH_DATA, buffer, sizeof(buffer));

  for (int i = 0; i < MAX_TOUCHES; i++) {
    int16_t x = (buffer[6 * i] & 0x0F) << 8 | buffer[6 * i + 1];
    int16_t y = (buffer[6 * i + 2] & 0x0F) << 8 | buffer[6 * i + 3];
    ESP_LOGD(TAG, "Touch #%d: X=%d, Y=%d", i, x, y);
  }
}

}  // namespace chsc5816
}  // namespace esphome
