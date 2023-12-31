/**************************************************************************/
/*!
  Author: Gustavo Ambrozio
  Based on work by: Atsushi Sasaki (https://github.com/aselectroworks/Arduino-FT6336U)
*/
/**************************************************************************/

#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/components/touchscreen/touchscreen.h"

namespace esphome {
namespace ft3267 {

using namespace touchscreen;

enum ft3267_gesture {
  ft3267_gesture_none = 0x00,
  ft3267_gesture_move_up = 0x10,
  ft3267_gesture_move_left = 0x14,
  ft3267_gesture_move_down = 0x18,
  ft3267_gesture_move_right = 0x1c,
  ft3267_gesture_zoom_in = 0x48,
  ft3267_gesture_zoom_out = 0x49,
};

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
  uint16_t read_touch_coordinate_(uint8_t coordinate);
  uint8_t read_touch_id_(uint8_t id_address);
  uint8_t get_position_(uint8_t *touch_points_num, uint8_t *x, uint8_t *y);
  uint8_t read_gesture(ft3267_gesture *gesture); 
  uint8_t ft3267_get_touch_points_num(uint8_t *touch_points_num);
  u_int8_t ft3267_read_pos(uint8_t *touch_points_num, uint8_t *x, uint8_t *y);
};



}  // namespace ft3267
}  // namespace esphome
