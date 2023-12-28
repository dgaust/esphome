import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c, gpio, touchscreen
from esphome.const import CONF_ID, CONF_ADDRESS, CONF_INTERRUPT_PIN

DEPENDENCIES = ['i2c']
CODEOWNERS = ["@dgaust"]

ft3267_ns = cg.esphome_ns.namespace('ft3267')
FT3267Touchscreen = ft3267_ns.class_('FT3267Touchscreen', cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = touchscreen.TOUCHSCREEN_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(FT3267Touchscreen),
    cv.Optional(CONF_ADDRESS, default=0x38): cv.i2c_address,
    cv.Optional(CONF_INTERRUPT_PIN): pins.internal_gpio_input_pin_schema,
    # Add other configuration options here if needed
}).extend(cv.polling_component_schema('60s')).extend(i2c.i2c_device_schema(0x38))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

    # Set the I2C address if it's provided
    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))

    # Configure the interrupt pin
    if interrupt_pin := config.get(CONF_INTERRUPT_PIN):
        cg.add(var.set_interrupt_pin(await cg.gpio_pin_expression(interrupt_pin)))
    

