import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL, CONF_TOUCHSCREEN

DEPENDENCIES = ['i2c']

ft3267_ns = cg.esphome_ns.namespace('ft3267')
FT3267Component = ft3267_ns.class_('FT3267Component', cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(FT3267Component),
    cv.Optional(CONF_UPDATE_INTERVAL): cv.update_interval,
    cv.Optional(CONF_TOUCHSCREEN, default=False): cv.boolean,
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x38))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

    if CONF_UPDATE_INTERVAL in config:
        cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))

    if config[CONF_TOUCHSCREEN]:
        # Additional configuration for touchscreen if needed
        pass
