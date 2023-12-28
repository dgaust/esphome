import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_ADDRESS, CONF_UPDATE_INTERVAL
from . import FT3267_TOUCHSCREEN_COMPONENT_SCHEMA

ft3267_ns = cg.esphome_ns.namespace('ft3267')
FT3267TouchscreenComponent = ft3267_ns.class_('FT3267TouchscreenComponent', cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(FT3267TouchscreenComponent),
    cv.Required(CONF_ADDRESS): cv.i2c_address,
    cv.Optional(CONF_UPDATE_INTERVAL, default='60s'): cv.update_interval,
}).extend(i2c.i2c_device_schema(0x38)).extend(FT3267_TOUCHSCREEN_COMPONENT_SCHEMA)
