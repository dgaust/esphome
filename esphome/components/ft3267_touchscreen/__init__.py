import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

DEPENDENCIES = ['i2c']

ft3267_ns = cg.esphome_ns.namespace('ft3267')
FT3267Touchscreen = ft3267_ns.class_('FT3267Touchscreen', cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(FT3267Touchscreen),
    # Here you can add configuration options, like I2C address, update interval, etc.
}).extend(cv.polling_component_schema('60s')).extend(i2c.i2c_device_schema(0x38))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)
