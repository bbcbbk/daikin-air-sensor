import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, uart
from esphome.const import CONF_ID, CONF_UART_ID

DEPENDENCIES = ['uart']

cm1106_ns = cg.esphome_ns.namespace('cm1106')
CM1106CalibrateSwitch = cm1106_ns.class_('CM1106CalibrateSwitch', switch.Switch, cg.Component)

CONFIG_SCHEMA = switch.switch_schema(CM1106CalibrateSwitch).extend({
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    uart_component = await cg.get_variable(config[CONF_UART_ID])
    var = cg.new_Pvariable(config[CONF_ID], uart_component)
    await cg.register_component(var, config)
    await switch.register_switch(var, config)