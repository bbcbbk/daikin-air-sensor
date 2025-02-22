#include "pm2105.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pm2105 {

void PM2105Sensor::setup() {
  ESP_LOGCONFIG(TAGpm2105i, "Setting up PM2105 at address 0x%02X", this->address_);
  this->command();
}

void PM2105Sensor::dump_config() {
  ESP_LOGCONFIG(TAGpm2105i, "PM2105 Sensor:");
  ESP_LOGCONFIG(TAGpm2105i, "  Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAGpm2105i, "  Update Interval: %u ms", this->get_update_interval());
  if (this->pm2p5_sensor_ != nullptr) {
    ESP_LOGCONFIG(TAGpm2105i, "  PM2.5 Sensor: %s", this->pm2p5_sensor_->get_name().c_str());
  }
  if (this->pm10_sensor_ != nullptr) {
    ESP_LOGCONFIG(TAGpm2105i, "  PM10 Sensor: %s", this->pm10_sensor_->get_name().c_str());
  }
}

void PM2105Sensor::update() {
  uint8_t ret = this->read();
  if (ret == 0) {
    if (this->pm2p5_sensor_ != nullptr) {
      this->pm2p5_sensor_->publish_state(this->pm2p5_grimm_);
    }
    if (this->pm10_sensor_ != nullptr) {
      this->pm10_sensor_->publish_state(this->pm10_grimm_);
    }
  }
}

void PM2105Sensor::command() {
  uint16_t data;
  this->buffer_[0] = PM2105i_FRAME_HEADER;
  this->buffer_[1] = 0x7;  // frame length
  this->buffer_[2] = PM2105i_CONTROL_MODE;

  switch (PM2105i_CONTROL_MODE) {
    case PM2105i_CTRL_SET_UP_CONTINUOUSLY_MEASUREMENT:
      data = 0xFFFF;
      break;
    case PM2105i_CTRL_SET_UP_CALIBRATION_COEFFICIENT:
      data = PM2105i_CALIBRATION_COEFFICIENT;
      break;
    default:
      data = PM2105i_MEASURING_TIME;
      break;
  }

  this->buffer_[3] = data >> 8;
  this->buffer_[4] = data & 0xFF;
  this->buffer_[5] = 0;  // Reserved

  this->buffer_[6] = this->buffer_[0];
  for (uint8_t i = 1; i < 6; i++) {
    this->buffer_[6] ^= this->buffer_[i];
  }

  if (!this->write(this->buffer_, 7)) {
#ifdef PM2105i_DEBUG
    ESP_LOGD(TAGpm2105i, "Failed to send command to PM2105");
#endif
  }
}

uint8_t PM2105Sensor::read() {
  // Use 0x00 as the register address if no specific register is required
  if (!this->read_bytes(0x00, this->buffer_, 22)) {
#ifdef PM2105i_DEBUG
    ESP_LOGD(TAGpm2105i, "PM2105::read failed to read 22 bytes");
#endif
    return 1;
  }

  if (this->buffer_[0] != PM2105i_FRAME_HEADER) {
#ifdef PM2105i_DEBUG
    ESP_LOGD(TAGpm2105i, "PM2105::read : frame header is different 0x%02X", this->buffer_[0]);
#endif
    return 2;
  }

  if (this->buffer_[1] != 22) {
#ifdef PM2105i_DEBUG
    ESP_LOGD(TAGpm2105i, "PM2105::read : frame length is not 22 0x%02X", this->buffer_[1]);
#endif
    return 3;
  }

  uint8_t check_code = this->buffer_[0];
  for (uint8_t i = 1; i < 21; i++) {
    check_code ^= this->buffer_[i];
  }

  if (this->buffer_[21] != check_code) {
#ifdef PM2105i_DEBUG
    ESP_LOGD(TAGpm2105i, "PM2105::read failed : check code is different - buffer_[21] : 0x%02X, check_code : 0x%02X",
             this->buffer_[21], check_code);
#endif
    return 4;
  }

  this->status_ = this->buffer_[2];
  this->measuring_mode_ = (this->buffer_[9] << 8) + this->buffer_[10];
  this->calibration_coefficient_ = (this->buffer_[11] << 8) + this->buffer_[12];
  this->pm2p5_grimm_ = (this->buffer_[5] << 8) + this->buffer_[6];
  this->pm10_grimm_ = (this->buffer_[7] << 8) + this->buffer_[8];

#ifdef PM2105i_DEBUG
  ESP_LOGD(TAGpm2105i, "status:%u measuring_mode:%u calibration_coefficient:%u",
           this->status_, this->measuring_mode_, this->calibration_coefficient_);
  ESP_LOGD(TAGpm2105i, "PM2.5:%u PM10:%u", this->pm2p5_grimm_, this->pm10_grimm_);
#endif
  return 0;
}

}  // namespace pm2105
}  // namespace esphome