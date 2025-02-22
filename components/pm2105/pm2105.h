#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace pm2105 {

static const char *TAGpm2105i = "pm2105";

#define PM2105i_ADDRESS                    0x28
#define PM2105i_FRAME_HEADER               0x16

// Control modes
#define PM2105i_CTRL_CLOSE_MEASUREMENT                 0x1
#define PM2105i_CTRL_OPEN_SINGLE_MEASUREMENT           0x2
#define PM2105i_CTRL_SET_UP_CONTINUOUSLY_MEASUREMENT   0x3
#define PM2105i_CTRL_SET_UP_TIMING_MEASUREMENT         0x4
#define PM2105i_CTRL_SET_UP_DYNAMIC_MEASUREMENT        0x5
#define PM2105i_CTRL_SET_UP_CALIBRATION_COEFFICIENT    0x6
#define PM2105i_CTRL_SET_UP_WARM_MODE                  0x7

#define PM2105i_CONTROL_MODE               PM2105i_CTRL_SET_UP_CONTINUOUSLY_MEASUREMENT
#define PM2105i_MEASURING_TIME             180
#define PM2105i_CALIBRATION_COEFFICIENT    70

// Status
#define PM2105i_STATUS_CLOSE               0x1
#define PM2105i_STATUS_UNDER_MEASURING     0x2
#define PM2105i_STATUS_FAILED              0x7
#define PM2105i_STATUS_DATA_STABLE         0x80

class PM2105Sensor : public PollingComponent, public esphome::i2c::I2CDevice {
 public:
  PM2105Sensor() : PollingComponent(15000) {}

  void set_pm2p5_sensor(sensor::Sensor *sens) { pm2p5_sensor_ = sens; }
  void set_pm10_sensor(sensor::Sensor *sens) { pm10_sensor_ = sens; }

  void setup() override;
  void dump_config() override;
  void update() override;

  void command();
  uint8_t read();

 protected:
  sensor::Sensor *pm2p5_sensor_{nullptr};
  sensor::Sensor *pm10_sensor_{nullptr};

  uint8_t buffer_[32];
  uint8_t status_;
  uint16_t measuring_mode_;
  uint16_t calibration_coefficient_;
  uint16_t pm2p5_grimm_;
  uint16_t pm10_grimm_;
};

}  // namespace pm2105
}  // namespace esphome