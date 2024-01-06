import esphome.codegen as cg
import esphome.config_validation as cv

from esphome import pins
from esphome.components import i2c, touchscreen
from esphome.const import CONF_ID, CONF_INTERRUPT_PIN, CONF_RESET_PIN

CODEOWNERS = ["@dgaust"]
DEPENDENCIES = ["i2c"]

ft3267_ns = cg.esphome_ns.namespace("ft3267")
ft3267Touchscreen = ft3267_ns.class_(
    "ft3267Touchscreen",
    touchscreen.Touchscreen,
    i2c.I2CDevice,
)

CONF_FT3267_ID = "ft3267_id"
CONF_ADDRESS = "address"

CONFIG_SCHEMA = touchscreen.TOUCHSCREEN_SCHEMA.extend(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ft3267Touchscreen),
            cv.Optional(CONF_INTERRUPT_PIN): cv.All(
                pins.internal_gpio_input_pin_schema
            ),
            cv.Optional(CONF_ADDRESS, default=0x38): cv.i2c_address,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        }
    ).extend(i2c.i2c_device_schema(CONF_ADDRESS))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await touchscreen.register_touchscreen(var, config)
    await i2c.register_i2c_device(var, config)

    if interrupt_pin_config := config.get(CONF_INTERRUPT_PIN):
        interrupt_pin = await cg.gpio_pin_expression(interrupt_pin_config)
        cg.add(var.set_interrupt_pin(interrupt_pin))
    if reset_pin_config := config.get(CONF_RESET_PIN):
        reset_pin = await cg.gpio_pin_expression(reset_pin_config)
        cg.add(var.set_reset_pin(reset_pin))
    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))