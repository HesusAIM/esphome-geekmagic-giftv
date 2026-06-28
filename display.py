import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID, CONF_LAMBDA

smalltv_ns = cg.esphome_ns.namespace("smalltv")
SmallTVDisplay = smalltv_ns.class_("SmallTVDisplay", display.DisplayBuffer)

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(SmallTVDisplay),
}).extend(cv.polling_component_schema("5s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA],
            [(display.DisplayBuffer.operator("ref"), "it")],
            return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))