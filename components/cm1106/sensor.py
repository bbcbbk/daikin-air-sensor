import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_ID, CONF_UART_ID, UNIT_PARTS_PER_MILLION, ICON_MOLECULE_CO2

DEPENDENCIES = ['uart']

cm1106_ns = cg.esphome_ns.namespace('cm1106')
CM1106Sensor = cm1106_ns.class_('CM1106Sensor', sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = sensor.sensor_schema(
    CM1106Sensor,
    unit_of_measurement=UNIT_PARTS_PER_MILLION,
    icon=ICON_MOLECULE_CO2,
    accuracy_decimals=0,
).extend({
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
}).extend(cv.polling_component_schema('60s'))

async def to_code(config):
    uart_component = await cg.get_variable(config[CONF_UART_ID])
    var = cg.new_Pvariable(config[CONF_ID], uart_component)
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)