#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/touchscreen/touchscreen.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ft3267 {

static const char *const TAG = "ft3267.touchscreen";

enum VendorId {
  FT3267_ID_UNKNOWN = 0,
  FT3267_ID_1 = 0x51,
  FT3267_ID_2 = 0x11,
  FT3267_ID_3 = 0xCD,
};

enum FTCmd : uint8_t {
  FT3267_MODE_REG = 0x00,
  FT3267_ORIGIN_REG = 0x08,
  FT3267_RESOLUTION_REG = 0x0C,
  FT3267_VENDOR_ID_REG = 0xA8,
  FT3267_TD_STATUS = 0x02,
  FT3267_TOUCH_DATA = 0x03,
  FT3267_I_MODE = 0xA4,
  FT3267_TOUCH_MAX = 0x4C,
};

enum FTMode : uint8_t {
  FT3267_OP_MODE = 0,
  FT3267_SYSINFO_MODE = 0x10,
  FT3267_TEST_MODE = 0x40,
};

static const size_t MAX_TOUCHES = 5;  // max number of possible touches reported

class FT3267Touchscreen : public touchscreen::Touchscreen, public i2c::I2CDevice {
 public:
  void setup() override {
    esph_log_config(TAG, "Setting up FT3267 Touchscreen...");
    // wait 200ms after reset.
    this->set_timeout(200, [this] { this->continue_setup_(); });
  }

  void continue_setup_(void) {
    uint8_t data[4];
    if (!this->set_mode_(FT3267_OP_MODE))
      return;

    if (!this->err_check_(this->read_register(FT3267_VENDOR_ID_REG, data, 1), "Read Vendor ID"))
      return;
    switch (data[0]) {
      case FT3267_ID_1:
      case FT3267_ID_2:
      case FT3267_ID_3:
        this->vendor_id_ = (VendorId) data[0];
        esph_log_d(TAG, "Read vendor ID 0x%X", data[0]);
        break;

      default:
        esph_log_e(TAG, "Unknown vendor ID 0x%X", data[0]);
        this->mark_failed();
        return;
    }
    // reading the chip registers to get max x/y does not seem to work.
    this->x_raw_max_ = this->display_->get_width();
    this->y_raw_max_ = this->display_->get_height();
    esph_log_config(TAG, "FT3267 Touchscreen setup complete");
  }

  void update_touches() override {
    uint8_t touch_cnt;
    uint8_t data[MAX_TOUCHES][6];

    if (!this->read_byte(FT3267_TD_STATUS, &touch_cnt) || touch_cnt > MAX_TOUCHES) {
      esph_log_w(TAG, "Failed to read status");
      return;
    }
    if (touch_cnt == 0)
      return;

    if (!this->read_bytes(FT3267_TOUCH_DATA, (uint8_t *) data, touch_cnt * 6)) {
      esph_log_w(TAG, "Failed to read touch data");
      return;
    }
    for (uint8_t i = 0; i != touch_cnt; i++) {
      uint8_t status = data[i][0] >> 6;
      uint8_t id = data[i][2] >> 3;
      uint16_t x = encode_uint16(data[i][0] & 0x0F, data[i][1]);
      uint16_t y = encode_uint16(data[i][2] & 0xF, data[i][3]);

      esph_log_d(TAG, "Read %X status, id: %d, pos %d/%d", status, id, x, y);
      if (status == 0 || status == 2) {
        this->add_raw_touch_position_(id, x, y);
      }
    }
  }

  void dump_config() override {
    esph_log_config(TAG, "FT3267 Touchscreen:");
    esph_log_config(TAG, "  Address: 0x%02X", this->address_);
    esph_log_config(TAG, "  Vendor ID: 0x%X", (int) this->vendor_id_);
  }

 protected:
  bool err_check_(i2c::ErrorCode err, const char *msg) {
    if (err != i2c::ERROR_OK) {
      this->mark_failed();
      esph_log_e(TAG, "%s failed - err 0x%X", msg, err);
      return false;
    }
    return true;
  }
  bool set_mode_(FTMode mode) {
    return this->err_check_(this->write_register(FT3267_MODE_REG, (uint8_t *) &mode, 1), "Set mode");
  }
  VendorId vendor_id_{FT3267_ID_UNKNOWN};
};

}  // namespace FT3267
}  // namespace esphome
