#ifndef FT3267_ESPHOME_H
#define FT3267_ESPHOME_H

#include "esphome.h"
#include "esphome/components/i2c/i2c.h"  // Include I2C component

namespace esphome {
namespace ft3267 {

struct TouchPoint {
  uint16_t x; // X coordinate
  uint16_t y; // Y coordinate
  uint8_t weight; // Touch weight (pressure)
  uint8_t event; // Touch event type (e.g., down, up, contact)
};

struct TouchData {
  uint8_t touch_count; // Number of touch points
  TouchPoint points[10]; // Array of touch points
};

class FT3267Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_interrupt_pin(int pin);
  void setup() override;
  void update() override;
  int get_touch_point_count() const;

 private:
  int interrupt_pin_ = -1; // Default to -1 to indicate no pin assigned
  void on_interrupt();
  TouchData read_touch_data();
  bool read_i2c(uint8_t reg, uint8_t *data, int len);
};

}  // namespace ft3267
}  // namespace esphome

#endif  // FT3267_ESPHOME_H