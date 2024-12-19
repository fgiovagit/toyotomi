#pragma once

#include "esphome/components/climate_ir/climate_ir.h"
// #include "climate_ir.h"

namespace esphome {
namespace toyotomi { // namespace toyotomi

const uint8_t TOYOTOMI_TEMP_MIN = 16;  // Celsius - limit 16 non modificabile sotto soglia
const uint8_t TOYOTOMI_TEMP_MAX = 31;  // Celsius - limit 30 used 34 for range display

// Modes

enum ToyotomiMode : uint8_t {
  TOYOTOMI_MODE_HEAT_COOL = 0x00,
  TOYOTOMI_MODE_COOL = 0x01,
  TOYOTOMI_MODE_DRY = 0x02,
  TOYOTOMI_MODE_FAN = 0x03,
  TOYOTOMI_MODE_HEAT = 0x04,
};

// Fan Speed

enum ToyotomiFanMode : uint8_t {
  TOYOTOMI_FAN_AUTO = 0x00,
  TOYOTOMI_FAN_1 = 0x01,
  TOYOTOMI_FAN_2 = 0x02,
  TOYOTOMI_FAN_3 = 0x03,
};

////////////////////////////////////////////////////////
// Light  // add to on-off display

enum ToyotomiPreset : uint8_t {
  TOYOTOMI_LIGHT_OFF = 0x00,
  TOYOTOMI_LIGHT_ON = 0x01,
};
////////////////////////////////////////////////////////

// Fan Position

enum ToyotomiBlades : uint8_t {
  TOYOTOMI_BLADES_STOP = 0x00,
  TOYOTOMI_BLADES_FULL = 0x01,
  TOYOTOMI_BLADES_1 = 0x02,
  TOYOTOMI_BLADES_2 = 0x03,
  TOYOTOMI_BLADES_3 = 0x04,
  TOYOTOMI_BLADES_4 = 0x05,
  TOYOTOMI_BLADES_5 = 0x06,
  TOYOTOMI_BLADES_LOW = 0x07,
  TOYOTOMI_BLADES_MID = 0x09,
  TOYOTOMI_BLADES_HIGH = 0x11,
};

// IR Transmission
const uint32_t TOYOTOMI_IR_FREQUENCY = 38000;
const uint32_t TOYOTOMI_HEADER_MARK = 9076;
const uint32_t TOYOTOMI_HEADER_SPACE = 4408;
const uint32_t TOYOTOMI_BIT_MARK = 660;
const uint32_t TOYOTOMI_ONE_SPACE = 1630;
const uint32_t TOYOTOMI_ZERO_SPACE = 530;
const uint32_t TOYOTOMI_MESSAGE_SPACE = 20000;

struct ToyotomiState {
  uint8_t mode = 0;
  uint8_t bitmap = 0;
  uint8_t fan_speed = 0;
  uint8_t temp = 0;
  uint8_t fan_pos = 0;
  uint8_t th = 0;
  uint8_t checksum = 0;
};

class ToyotomiClimate : public climate_ir::ClimateIR {
 public:
  ToyotomiClimate()
      : climate_ir::ClimateIR(TOYOTOMI_TEMP_MIN, TOYOTOMI_TEMP_MAX, 1.0f, true, true,
                              {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW,  
                               climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
							  {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL},
							  {climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_SLEEP }	 // add to on-off display						  
							  ) {}
 
 protected:
  // Transmit via IR the state of this climate controller
  void transmit_state() override;
  // Handle received IR Buffer
  bool on_receive(remote_base::RemoteReceiveData data) override;
  bool parse_state_frame_(ToyotomiState curr_state);

  // setters
  uint8_t set_mode_();
  uint8_t set_temp_();
  uint8_t set_fan_speed_();
  uint8_t gen_checksum_();
  uint8_t set_blades_();

  uint8_t set_preset_(); // add to on-off display

  // getters
  climate::ClimateMode get_mode_(uint8_t mode);
  climate::ClimateFanMode get_fan_speed_(uint8_t fan);
  void get_blades_(uint8_t fanpos);
  // get swing
  climate::ClimateSwingMode get_swing_(uint8_t bitmap);
  float get_temp_(uint8_t temp);

  climate::ClimatePreset get_preset_(uint8_t preset); // add to on-off display

  // check if the received frame is valid
  bool check_checksum_(uint8_t checksum);

  template<typename T> T reverse_(T val, size_t len);

  template<typename T> void add_(T val, size_t len, esphome::remote_base::RemoteTransmitData *ata);

  template<typename T> void add_(T val, esphome::remote_base::RemoteTransmitData *data);

  template<typename T> void reverse_add_(T val, size_t len, esphome::remote_base::RemoteTransmitData *data);

  uint8_t blades_ = TOYOTOMI_BLADES_STOP;
};

}  // namespace toyotomi
}  // namespace esphome
