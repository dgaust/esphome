import esphome.codegen as cg
from esphome import core
from . import touchscreen

ft3267_ns = cg.esphome_ns.namespace('ft3267')
FT3267Component = ft3267_ns.class_('FT3267Component', cg.PollingComponent)

CONFIG_SCHEMA = touchscreen.CONFIG_SCHEMA.extend({
    # Additional configuration options here
})

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield touchscreen.register_touchscreen(var, config)
