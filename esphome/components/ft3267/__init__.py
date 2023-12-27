import esphome.codegen as cg
from esphome import core
from . import touchscreen

# Register the component with ESPHome
cg.register_component("ft3267", touchscreen.FT3267Component)

# Link to the FT3267 touchscreen C++ code
FT3267_TOUCH_COMPONENT_SCHEMA = touchscreen.CONFIG_SCHEMA.extend({
    # Add any additional configuration options here if needed
})

# Setup function for FT3267
def setup_ft3267_touch(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield touchscreen.register_touchscreen(var, config)

# Registering the component with ESPHome core
core.register_platform("i2c", "ft3267_touch", FT3267_TOUCH_COMPONENT_SCHEMA, setup_ft3267_touch)
