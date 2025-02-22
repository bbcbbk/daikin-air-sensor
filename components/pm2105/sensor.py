import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, i2c
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
    CONF_UPDATE_INTERVAL,
    UNIT_MICROGRAMS_PER_CUBIC_METER,
    ICON_CHEMICAL_WEAPON,
    DEVICE_CLASS_PM25,
    DEVICE_CLASS_PM10,
)

DEPENDENCIES = ["i2c"]

pm2105_ns = cg.esphome_ns.namespace("pm2105")
PM2105Sensor = pm2105_ns.class_("PM2105Sensor", cg.PollingComponent, i2c.I2CDevice)

CONF_PM2_5 = "pm2_5"
CONF_PM10 = "pm10"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PM2105Sensor),
        cv.Optional(CONF_ADDRESS, default=0x28): cv.i2c_address,
        cv.Optional(CONF_UPDATE_INTERVAL, default="15s"): cv.update_interval,
        cv.Required(CONF_PM2_5): sensor.sensor_schema(
            unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
            icon=ICON_CHEMICAL_WEAPON,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_PM25,
        ),
        cv.Required(CONF_PM10): sensor.sensor_schema(
            unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
            icon=ICON_CHEMICAL_WEAPON,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_PM10,
        ),
    }
).extend(cv.polling_component_schema("15s")).extend(i2c.i2c_device_schema(0x28))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    # Set PM2.5 sensor
    pm2_5 = await sensor.new_sensor(config[CONF_PM2_5])
    cg.add(var.set_pm2p5_sensor(pm2_5))

    # Set PM10 sensor
    pm10 = await sensor.new_sensor(config[CONF_PM10])
    cg.add(var.set_pm10_sensor(pm10))