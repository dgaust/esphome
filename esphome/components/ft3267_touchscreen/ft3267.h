/**************************************************************************/
/*!
  Author: Gustavo Ambrozio
  Based on work by: Atsushi Sasaki (https://github.com/aselectroworks/Arduino-FT6336U)
*/
/**************************************************************************/

#pragma once

#include "esphome/components/i2c/i2c.h"
#include "touchscreen.h"
#include "esphome/core/component.h"

namespace esphome {
namespace ft3267 {

typedef enum {
  ft3267_gesture_none = 0x00,
  ft3267_gesture_move_up = 0x10,
  ft3267_gesture_move_left = 0x14,
  ft3267_gesture_move_down = 0x18,
  ft3267_gesture_move_right = 0x1c,
  ft3267_gesture_zoom_in = 0x48,
  ft3267_gesture_zoom_out = 0x49,
} ft3267_gesture_t;

using namespace touchscreen;

class ft3267Touchscreen : public Touchscreen, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;

  void set_interrupt_pin(InternalGPIOPin *pin) { this->interrupt_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }

 protected:
  void hard_reset_();
  uint8_t read_byte_(uint8_t addr);
  void update_touches() override;

  InternalGPIOPin *interrupt_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};

  uint8_t read_touch_count_();
  uint8_t test_gesture_();
  uint16_t read_touch_coordinate_(uint8_t coordinate);
  uint8_t read_touch_id_(uint8_t id_address);
  uint16_t read_touch_gesture_(uint8_t coordinate);
  uint16_t read_touch_gesture_(uint8_t coordinate);
  uint8_t gesture_read_byte_(uint8_t reg_addr);
};

}  // namespace ft3267
}  // namespace esphome
