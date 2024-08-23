
#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/touchscreen/touchscreen.h"
#include "esphome/core/component.h"
#include "CHSC5816Constants.h"

namespace esphome {
namespace chsc5816 {

class CHSC5816Touchscreen : public touchscreen::Touchscreen, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void loop() override;

  void set_interrupt_pin(GPIOPin *pin) { this->interrupt_pin_ = pin; }

 protected:
  GPIOPin *interrupt_pin_{nullptr};

  void read_touch_data();
};

}  // namespace chsc5816
}  // namespace esphome
