from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import i2c, touchscreen
from esphome.const import CONF_ID, CONF_INTERRUPT_PIN, CONF_RESET_PIN, CONF_THRESHOLD

CODEOWNERS = ["@dgaust"]
DEPENDENCIES = ["i2c"]

chsc5816_ns = cg.esphome_ns.namespace("chsc5816")
CHSC5816Touchscreen = chsc5816_ns.class_(
    "CHSC5816Touchscreen",
    touchscreen.Touchscreen,
    i2c.I2CDevice,
)

CONFIG_SCHEMA = touchscreen.TOUCHSCREEN_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(CHSC5816Touchscreen),
        cv.Optional(CONF_INTERRUPT_PIN): pins.internal_gpio_input_pin_schema,
        cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_THRESHOLD): cv.uint8_t,
    }
).extend(i2c.i2c_device_schema(0x48))  # Default I2C address for CHSC5816

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await i2c.register_i2c_device(var, config)
    await touchscreen.register_touchscreen(var, config)

    if interrupt_pin := config.get(CONF_INTERRUPT_PIN):
        pin = await cg.gpio_pin_expression(interrupt_pin)
        cg.add(var.set_interrupt_pin(pin))

    if reset_pin := config.get(CONF_RESET_PIN):
        pin = await cg.gpio_pin_expression(reset_pin)
        cg.add(var.set_reset_pin(pin))

    if CONF_THRESHOLD in config:
        cg.add(var.set_threshold(config[CONF_THRESHOLD]))