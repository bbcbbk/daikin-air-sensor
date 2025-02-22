#include "cm1106.h"

namespace esphome {
namespace cm1106 {

CM1106::CM1106(uart::UARTComponent *parent) : uart::UARTDevice(parent) {}

void CM1106::set_co2_calib_value(uint16_t ppm) {
  uint8_t cmd[6];
  memcpy(cmd, CM1106_CMD_SET_CO2_CALIB, sizeof(cmd));
  cmd[3] = ppm >> 8;
  cmd[4] = ppm & 0xFF;
  uint8_t response[4] = {0};
  bool success = send_uart_command(cmd, sizeof(cmd), response, sizeof(response));

  if (!success) {
    ESP_LOGW(TAG, "Reading data from CM1106 failed!");
    return;
  }

  if (memcmp(response, CM1106_CMD_SET_CO2_CALIB_RESPONSE, sizeof(response)) != 0) {
    ESP_LOGW(TAG, "Got wrong UART response: %02X %02X %02X %02X", response[0], response[1], response[2], response[3]);
    return;
  }

  ESP_LOGD(TAG, "CM1106 Successfully calibrated sensor to %u ppm", ppm);
}

int16_t CM1106::get_co2_ppm() {
  uint8_t response[8] = {0};
  bool success = send_uart_command(CM1106_CMD_GET_CO2, sizeof(CM1106_CMD_GET_CO2), response, sizeof(response));

  if (!success) {
    ESP_LOGW(TAG, "Reading data from CM1106 failed!");
    return -1;
  }

  if (!(response[0] == 0x16 && response[1] == 0x05 && response[2] == 0x01)) {
    ESP_LOGW(TAG, "Got wrong UART response: %02X %02X %02X %02X...", response[0], response[1], response[2], response[3]);
    return -1;
  }

  uint8_t checksum = calc_crc(response, sizeof(response));
  if (response[7] != checksum) {
    ESP_LOGW(TAG, "Got wrong UART checksum: 0x%02X - Calculated: 0x%02X", response[7], checksum);
    return -1;
  }

  int16_t ppm = (response[3] << 8) | response[4];
  ESP_LOGD(TAG, "CM1106 Received CO₂=%u ppm DF3=%02X DF4=%02X", ppm, response[5], response[6]);
  return ppm;
}

uint8_t CM1106::calc_crc(uint8_t *response, size_t len) {
  uint8_t crc = 0;
  for (size_t i = 0; i < len - 1; i++) {
    crc -= response[i];
  }
  return crc;
}

bool CM1106::send_uart_command(uint8_t *command, size_t command_len, uint8_t *response, size_t response_len) {
  while (available()) {
    read();
  }
  command[command_len - 1] = calc_crc(command, command_len);
  write_array(command, command_len);
  flush();
  if (response == nullptr) {
    return true;
  }
  return read_array(response, response_len);
}

CM1106Sensor::CM1106Sensor(uart::UARTComponent *parent) : cm1106_(new CM1106(parent)) {}

float CM1106Sensor::get_setup_priority() const { return setup_priority::DATA; }

void CM1106Sensor::setup() {}

void CM1106Sensor::update() {
  int16_t ppm = cm1106_->get_co2_ppm();
  if (ppm > -1) {
    publish_state(ppm);
  }
}

CM1106CalibrateSwitch::CM1106CalibrateSwitch(uart::UARTComponent *parent) : cm1106_(new CM1106(parent)) {}

void CM1106CalibrateSwitch::write_state(bool state) {
  if (state) {
    publish_state(state);
    cm1106_->set_co2_calib_value();
    turn_off();
  } else {
    publish_state(state);
  }
}

}  // namespace cm1106
}  // namespace esphome