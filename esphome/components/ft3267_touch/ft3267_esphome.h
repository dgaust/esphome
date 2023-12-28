#ifndef FT3267_ESPHOME_H
#define FT3267_ESPHOME_H

#include "esphome.h"
#include "esphome/components/i2c/i2c.h"

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
  void set_interrupt_pin(int pin) { interrupt_pin_ = pin; }
  void setup() override;
  void update() override;

  // Method to retrieve the number of touch points
  int get_touch_point_count() const { return touch_point_count_; }

 private:
  int interrupt_pin_ = -1;  // Interrupt pin
  int touch_point_count_ = 0;  // Store the number of touch points

  TouchData read_touch_data();
  bool read_i2c(uint8_t reg, uint8_t *data, int len);
};

}  // namespace ft3267
}  // namespace esphome

#endif  // FT3267_ESPHOME_H
