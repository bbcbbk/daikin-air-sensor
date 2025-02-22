#ifndef CM1106_H
#define CM1106_H

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace cm1106 {

class CM1106 : public uart::UARTDevice {
 public:
  CM1106(uart::UARTComponent *parent);
  void set_co2_calib_value(uint16_t ppm = 400);
  int16_t get_co2_ppm();

 private:
  const char *TAG = "cm1106";
  uint8_t CM1106_CMD_GET_CO2[4] = {0x11, 0x01, 0x01, 0xED};
  uint8_t CM1106_CMD_SET_CO2_CALIB[6] = {0x11, 0x03, 0x03, 0x00, 0x00, 0x00};
  uint8_t CM1106_CMD_SET_CO2_CALIB_RESPONSE[4] = {0x16, 0x01, 0x03, 0xE6};
  uint8_t calc_crc(uint8_t *response, size_t len);
  bool send_uart_command(uint8_t *command, size_t command_len, uint8_t *response = nullptr, size_t response_len = 0);
};

class CM1106Sensor : public PollingComponent, public sensor::Sensor {
 public:
  CM1106Sensor(uart::UARTComponent *parent);
  float get_setup_priority() const override;
  void setup() override;
  void update() override;

 private:
  CM1106 *cm1106_;
};

class CM1106CalibrateSwitch : public Component, public switch_::Switch {
 public:
  CM1106CalibrateSwitch(uart::UARTComponent *parent);
  void write_state(bool state) override;

 private:
  CM1106 *cm1106_;
};

}  // namespace cm1106
}  // namespace esphome

#endif  // CM1106_H