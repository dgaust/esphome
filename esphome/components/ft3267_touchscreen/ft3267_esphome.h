#ifndef FT3267_TOUCHSCREEN_H
#define FT3267_TOUCHSCREEN_H

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ft3267 {

struct TouchPoint {
  uint16_t x;
  uint16_t y;
};

class FT3267TouchscreenComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_address(uint8_t address) { address_ = address; }
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  std::vector<TouchPoint> get_touch_points() const { return touch_points_; }

 protected:
  uint8_t address_;
  std::vector<TouchPoint> touch_points_;
};

}  // namespace ft3267
}  // namespace esphome

#endif  // FT3267_TOUCHSCREEN_H
