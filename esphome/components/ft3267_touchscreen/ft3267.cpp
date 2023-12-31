/**************************************************************************/
/*!
  Author: Gustavo Ambrozio
  Based on work by: Atsushi Sasaki (https://github.com/aselectroworks/Arduino-FT6336U)
*/
/**************************************************************************/

#include "ft3267.h"
#include "esphome/core/log.h"
#include "Wire.h"

// Registers
// Reference: https://focuslcds.com/content/FT6236.pdf
namespace esphome {
namespace ft3267 {

enum ft3267_gesture {
  ft3267_gesture_none = 0x00,
  ft3267_gesture_move_up = 0x10,
  ft3267_gesture_move_left = 0x14,
  ft3267_gesture_move_down = 0x18,
  ft3267_gesture_move_right = 0x1c,
  ft3267_gesture_zoom_in = 0x48,
  ft3267_gesture_zoom_out = 0x49,
};

static const char *const TAG = "ft3267Touchscreen";

#define FT3267_ADDR                    (0x51)

#define FT3267_DEVICE_MODE             (0x00)
#define FT3267_GESTURE_ID              (0x01)
#define FT3267_TOUCH_POINTS            (0x02)

#define FT3267_TOUCH1_EV_FLAG          (0x03)
#define FT3267_TOUCH1_XH               (0x03)
#define FT3267_TOUCH1_XL               (0x04)
#define FT3267_TOUCH1_YH               (0x05)
#define FT3267_TOUCH1_YL               (0x06)

#define FT3267_TOUCH2_EV_FLAG          (0x09)
#define FT3267_TOUCH2_XH               (0x09)
#define FT3267_TOUCH2_XL               (0x0A)
#define FT3267_TOUCH2_YH               (0x0B)
#define FT3267_TOUCH2_YL               (0x0C)

#define FT3267_TOUCH3_EV_FLAG          (0x0F)
#define FT3267_TOUCH3_XH               (0x0F)
#define FT3267_TOUCH3_XL               (0x10)
#define FT3267_TOUCH3_YH               (0x11)
#define FT3267_TOUCH3_YL               (0x12)

#define FT3267_TOUCH4_EV_FLAG          (0x15)
#define FT3267_TOUCH4_XH               (0x15)
#define FT3267_TOUCH4_XL               (0x16)
#define FT3267_TOUCH4_YH               (0x17)
#define FT3267_TOUCH4_YL               (0x18)

#define FT3267_TOUCH5_EV_FLAG          (0x1B)
#define FT3267_TOUCH5_XH               (0x1B)
#define FT3267_TOUCH5_XL               (0x1C)
#define FT3267_TOUCH5_YH               (0x1D)
#define FT3267_TOUCH5_YL               (0x1E)

#define FT3267_ID_G_THGROUP            (0x80)
#define FT3267_ID_G_THPEAK             (0x81)
#define FT3267_ID_G_THCAL              (0x82)
#define FT3267_ID_G_THWATER            (0x83)
#define FT3267_ID_G_THTEMP             (0x84)
#define FT3267_ID_G_THDIFF             (0x85)
#define FT3267_ID_G_CTRL               (0x86)
#define FT3267_ID_G_TIME_ENTER_MONITOR (0x87)
#define FT3267_ID_G_PERIODACTIVE       (0x88)
#define FT3267_ID_G_PERIODMONITOR      (0x89)
#define FT3267_ID_G_AUTO_CLB_MODE      (0xA0)
#define FT3267_ID_G_LIB_VERSION_H      (0xA1)
#define FT3267_ID_G_LIB_VERSION_L      (0xA2)
#define FT3267_ID_G_CIPHER             (0xA3)
#define FT3267_ID_G_MODE               (0xA4)
#define FT3267_ID_G_PMODE              (0xA5)
#define FT3267_ID_G_FIRMID             (0xA6)
#define FT3267_ID_G_STATE              (0xA7)
#define FT3267_ID_G_FT5201ID           (0xA8)
#define FT3267_ID_G_ERR                (0xA9)

TwoWire *_Wrie = NULL;

void ft3267Touchscreen::setup() {
  ESP_LOGCONFIG(TAG, "Setting up FT3267Touchscreen...");
  
  if (this->interrupt_pin_ != nullptr) {
    this->interrupt_pin_->pin_mode(gpio::FLAG_INPUT | gpio::FLAG_PULLUP);
    this->interrupt_pin_->setup();
    this->attach_interrupt_(this->interrupt_pin_, gpio::INTERRUPT_FALLING_EDGE);
  }

  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
  }

  this->hard_reset_();

  // Setup device based on ft3267 driver
  this->write_byte(FT3267_ID_G_THGROUP, 70);
  this->write_byte(FT3267_ID_G_THPEAK, 60);
  this->write_byte(FT3267_ID_G_THCAL, 16);
  this->write_byte(FT3267_ID_G_THWATER, 60);
  this->write_byte(FT3267_ID_G_THTEMP, 10);
  this->write_byte(FT3267_ID_G_THDIFF, 20);
  this->write_byte(FT3267_ID_G_TIME_ENTER_MONITOR, 2);
  this->write_byte(FT3267_ID_G_PERIODACTIVE, 12);
  this->write_byte(FT3267_ID_G_PERIODMONITOR, 40);
  this->write_byte(FT3267_ID_G_MODE, 0);

  // Set the touch resolution
  this->x_raw_max_ = this->get_width_();
  this->y_raw_max_ = this->get_height_();
  intiatewire(*_Wrie);
}

void intiatewire(TwoWire &Wrie) {
  _Wrie = &Wrie;
}

void ft3267Touchscreen::update_touches() {
  uint8_t touch_id = this->read_touch_id_(FT3267_TOUCH1_EV_FLAG);  // id1 = 0 or 1
  int touch_count = ft3267_get_touch_points_num(&touch_id);
  ESP_LOGD("FT3267", "Touch ID: %d", touch_id);
  ESP_LOGD("FT3267", "Touch Count: %d", touch_count);
}

static uint8_t ft3267_get_touch_points_num(uint8_t *touch_points_num)
{
    return ft3267_read_byte(FT3267_TOUCH_POINTS, touch_points_num);
}

static inline uint8_t ft3267_read_byte(uint8_t reg_addr, uint8_t *data)
{
    // return i2c_bus_read_byte(ft3267_handle, reg_addr, data);
    _Wrie->beginTransmission(FT3267_ADDR);
    _Wrie->write(reg_addr);
    _Wrie->endTransmission();
    uint8_t bytesReceived = _Wrie->requestFrom(FT3267_ADDR, 1);
    if (bytesReceived)
        _Wrie->readBytes(data, bytesReceived);
    return 0;
}

static inline uint8_t ft3267_read_bytes(uint8_t reg_addr, size_t data_len, uint8_t *data)
{
    // return i2c_bus_read_bytes(ft3267_handle, reg_addr, data_len, data);
    _Wrie->beginTransmission(FT3267_ADDR);
    _Wrie->write(reg_addr);
    _Wrie->endTransmission();
    uint8_t bytesReceived = _Wrie->requestFrom(FT3267_ADDR, data_len);
    uint8_t index = 0;
    while (_Wrie->available())
        data[index++] = _Wrie->read();
    return 0;
}

uint8_t ft3267_read_pos(uint8_t *touch_points_num, uint16_t *x, uint16_t *y)
{
    uint8_t ret_val = 0;
    static uint8_t data[4];

    ret_val |= ft3267_get_touch_points_num(touch_points_num);
    *touch_points_num = (*touch_points_num) & 0x0f;
    if (0 == *touch_points_num) {
    } else {
        ret_val |= ft3267_read_bytes(FT3267_TOUCH1_XH, 4, data);

        *x = ((data[0] & 0x0f) << 8) + data[1];
        *y = ((data[2] & 0x0f) << 8) + data[3];
    }

    return ret_val;
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

}  // namespace ft3267
}  // namespace esphome