
#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/touchscreen/touchscreen.h"
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace chsc5816 {

enum CHSC5816Cmd : uint8_t {
  CHSC5816_MODE_REG = 0x00,
  CHSC5816_ORIGIN_REG = 0x08,
  CHSC5816_RESOLUTION_REG = 0x0C,
  CHSC5816_VENDOR_ID_REG = 0xA8,
  CHSC5816_TD_STATUS = 0x02,
  CHSC5816_TOUCH_DATA = 0x03,
};

static const size_t MAX_TOUCHES = 5;  // max number of possible touches reported

class CHSC5816Touchscreen : public touchscreen::Touchscreen, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update_touches() override;

  void set_interrupt_pin(GPIOPin *pin) { this->interrupt_pin_ = pin; }

 protected:
  GPIOPin *interrupt_pin_{nullptr};
};

}  // namespace chsc5816
}  // namespace esphome
