#include "toyotomi.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace toyotomi { // namespace toyotomi

static const char *const TAG = "toyotomi.climate";

// setters
uint8_t ToyotomiClimate::set_temp_() {
  return (uint8_t) roundf(clamp<float>(this->target_temperature, TOYOTOMI_TEMP_MIN, TOYOTOMI_TEMP_MAX) - TOYOTOMI_TEMP_MIN);
}

uint8_t ToyotomiClimate::set_mode_() {
  switch (this->mode) {
    case climate::CLIMATE_MODE_COOL:
      return TOYOTOMI_MODE_COOL;
    case climate::CLIMATE_MODE_DRY:
      return TOYOTOMI_MODE_DRY;
    case climate::CLIMATE_MODE_HEAT:
      return TOYOTOMI_MODE_HEAT;
    case climate::CLIMATE_MODE_FAN_ONLY:
      return TOYOTOMI_MODE_FAN;
    case climate::CLIMATE_MODE_HEAT_COOL:
    default:
      return TOYOTOMI_MODE_HEAT_COOL;
  }
}

uint8_t ToyotomiClimate::set_fan_speed_() {
  switch (this->fan_mode.value()) {
    case climate::CLIMATE_FAN_LOW:
      return TOYOTOMI_FAN_1;
    case climate::CLIMATE_FAN_MEDIUM:
      return TOYOTOMI_FAN_2;
    case climate::CLIMATE_FAN_HIGH:
      return TOYOTOMI_FAN_3;
    case climate::CLIMATE_FAN_AUTO:
    default:
      return TOYOTOMI_FAN_AUTO;
  }
}

////////////////////////////////////////////////////////
// add to on-off display
uint8_t ToyotomiClimate::set_preset_() {  
  switch (this->preset.value()) {
    case climate::CLIMATE_PRESET_SLEEP:
      return TOYOTOMI_LIGHT_OFF;
    case climate::CLIMATE_PRESET_NONE:
    default:
      return TOYOTOMI_LIGHT_ON;
  }
}
////////////////////////////////////////////////////////

uint8_t ToyotomiClimate::set_blades_() {
  if (this->swing_mode == climate::CLIMATE_SWING_VERTICAL) {
    switch (this->blades_) {
      case TOYOTOMI_BLADES_1:
      case TOYOTOMI_BLADES_2:
      case TOYOTOMI_BLADES_HIGH:
        this->blades_ = TOYOTOMI_BLADES_HIGH;
        break;
      case TOYOTOMI_BLADES_3:
      case TOYOTOMI_BLADES_MID:
        this->blades_ = TOYOTOMI_BLADES_MID;
        break;
      case TOYOTOMI_BLADES_4:
      case TOYOTOMI_BLADES_5:
      case TOYOTOMI_BLADES_LOW:
        this->blades_ = TOYOTOMI_BLADES_LOW;
        break;
      default:
        this->blades_ = TOYOTOMI_BLADES_FULL;
        break;
    }
  } else {
    switch (this->blades_) {
      case TOYOTOMI_BLADES_1:
      case TOYOTOMI_BLADES_2:
      case TOYOTOMI_BLADES_HIGH:
        this->blades_ = TOYOTOMI_BLADES_1;
        break;
      case TOYOTOMI_BLADES_3:
      case TOYOTOMI_BLADES_MID:
        this->blades_ = TOYOTOMI_BLADES_3;
        break;
      case TOYOTOMI_BLADES_4:
      case TOYOTOMI_BLADES_5:
      case TOYOTOMI_BLADES_LOW:
        this->blades_ = TOYOTOMI_BLADES_5;
        break;
      default:
        this->blades_ = TOYOTOMI_BLADES_STOP;
        break;
    }
  }
  return this->blades_;
}

uint8_t ToyotomiClimate::gen_checksum_() { return (this->set_temp_() + this->set_mode_() + 2) % 16; }

// getters
float ToyotomiClimate::get_temp_(uint8_t temp) { return (float) (temp + TOYOTOMI_TEMP_MIN); }

climate::ClimateMode ToyotomiClimate::get_mode_(uint8_t mode) {
  switch (mode) {
    case TOYOTOMI_MODE_COOL:
      return climate::CLIMATE_MODE_COOL;
    case TOYOTOMI_MODE_DRY:
      return climate::CLIMATE_MODE_DRY;
    case TOYOTOMI_MODE_HEAT:
      return climate::CLIMATE_MODE_HEAT;
    case TOYOTOMI_MODE_HEAT_COOL:
      return climate::CLIMATE_MODE_HEAT_COOL;
    case TOYOTOMI_MODE_FAN:
      return climate::CLIMATE_MODE_FAN_ONLY;
    default:
      return climate::CLIMATE_MODE_HEAT_COOL;
  }
}

climate::ClimateFanMode ToyotomiClimate::get_fan_speed_(uint8_t fan_speed) {
  switch (fan_speed) {
    case TOYOTOMI_FAN_1:
      return climate::CLIMATE_FAN_LOW;
    case TOYOTOMI_FAN_2:
      return climate::CLIMATE_FAN_MEDIUM;
    case TOYOTOMI_FAN_3:
      return climate::CLIMATE_FAN_HIGH;
    case TOYOTOMI_FAN_AUTO:
    default:
      return climate::CLIMATE_FAN_AUTO;
  }
}

////////////////////////////////////////////////////////
// add to on-off display
climate::ClimatePreset ToyotomiClimate::get_preset_(uint8_t preset) {
  switch (preset) {
    case TOYOTOMI_LIGHT_OFF:
      return climate::CLIMATE_PRESET_SLEEP;
    case TOYOTOMI_LIGHT_ON:
    default:
      return climate::CLIMATE_PRESET_NONE;
  }
}
////////////////////////////////////////////////////////


climate::ClimateSwingMode ToyotomiClimate::get_swing_(uint8_t bitmap) {
  return (bitmap >> 1) & 0x01 ? climate::CLIMATE_SWING_VERTICAL : climate::CLIMATE_SWING_OFF;
}

template<typename T> T ToyotomiClimate::reverse_(T val, size_t len) {
  T result = 0;
  for (size_t i = 0; i < len; i++) {
    result |= ((val & 1 << i) != 0) << (len - 1 - i);
  }
  return result;
}

template<typename T> void ToyotomiClimate::add_(T val, size_t len, esphome::remote_base::RemoteTransmitData *data) {
  for (size_t i = len; i > 0; i--) {
    data->mark(TOYOTOMI_BIT_MARK);
    data->space((val & (1 << (i - 1))) ? TOYOTOMI_ONE_SPACE : TOYOTOMI_ZERO_SPACE);
  }
}

template<typename T> void ToyotomiClimate::add_(T val, esphome::remote_base::RemoteTransmitData *data) {
  data->mark(TOYOTOMI_BIT_MARK);
  data->space((val & 1) ? TOYOTOMI_ONE_SPACE : TOYOTOMI_ZERO_SPACE);
}

template<typename T>
void ToyotomiClimate::reverse_add_(T val, size_t len, esphome::remote_base::RemoteTransmitData *data) {
  this->add_(this->reverse_(val, len), len, data);
}

bool ToyotomiClimate::check_checksum_(uint8_t checksum) {
  uint8_t expected = this->gen_checksum_();
  ESP_LOGV(TAG, "Expected checksum: %X", expected);
  ESP_LOGV(TAG, "Checksum received: %X", checksum);

  return checksum == expected;
}

void ToyotomiClimate::transmit_state() {
  auto transmit = this->transmitter_->transmit();
  auto *data = transmit.get_data();
  data->set_carrier_frequency(TOYOTOMI_IR_FREQUENCY);

  data->mark(TOYOTOMI_HEADER_MARK);
  data->space(TOYOTOMI_HEADER_SPACE);

  if (this->mode != climate::CLIMATE_MODE_OFF) {
    this->reverse_add_(this->set_mode_(), 3, data);
    this->add_(1, data);
    this->reverse_add_(this->set_fan_speed_(), 2, data);
    this->add_(this->swing_mode != climate::CLIMATE_SWING_OFF, data);
    this->add_(0, data);  // sleep mode
    this->reverse_add_(this->set_temp_(), 4, data);
    this->add_(0, 8, data);      // zeros
    this->add_(0, data);         // turbo mode
//    this->add_(1, data);         // light on
    this->add_(this->preset.value() != climate::CLIMATE_PRESET_SLEEP, data); // light mode off preset sleep // add to on-off display	
    this->add_(1, data);         // tree icon thingy
    this->add_(0, data);         // blow mode
    this->add_(0x52, 11, data);  // idk

    data->mark(TOYOTOMI_BIT_MARK);
    data->space(TOYOTOMI_MESSAGE_SPACE);

    this->reverse_add_(this->set_blades_(), 4, data);
    this->add_(0, 4, data);          // zeros
    this->reverse_add_(2, 2, data);  // thermometer
    this->add_(0, 18, data);         // zeros
    this->reverse_add_(this->gen_checksum_(), 4, data);
  } else {
    this->add_(9, 12, data);
    this->add_(0, 8, data);
    this->add_(0x2052, 15, data);
    data->mark(TOYOTOMI_BIT_MARK);
    data->space(TOYOTOMI_MESSAGE_SPACE);
    this->add_(0, 8, data);
    this->add_(1, 2, data);
    this->add_(0, 18, data);
    this->add_(0x0C, 4, data);
  }
  data->mark(TOYOTOMI_BIT_MARK);
  data->space(0);

  transmit.perform();
}

bool ToyotomiClimate::parse_state_frame_(ToyotomiState curr_state) {
  this->mode = this->get_mode_(curr_state.mode);
  this->fan_mode = this->get_fan_speed_(curr_state.fan_speed);
  this->target_temperature = this->get_temp_(curr_state.temp);
  this->swing_mode = this->get_swing_(curr_state.bitmap);
  // this->blades_ = curr_state.fan_pos;
  if (!(curr_state.bitmap & 0x01)) {
    this->mode = climate::CLIMATE_MODE_OFF;
  }

  this->publish_state();
  return true;
}

bool ToyotomiClimate::on_receive(remote_base::RemoteReceiveData data) {
  if (!data.expect_item(TOYOTOMI_HEADER_MARK, TOYOTOMI_HEADER_SPACE)) {
    return false;
  }
  ESP_LOGD(TAG, "Received toyotomi frame");

  ToyotomiState curr_state;

  for (size_t pos = 0; pos < 3; pos++) {
    if (data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ONE_SPACE)) {
      curr_state.mode |= 1 << pos;
    } else if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
      return false;
    }
  }

  ESP_LOGD(TAG, "Mode: %d", curr_state.mode);

  if (data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ONE_SPACE)) {
    curr_state.bitmap |= 1 << 0;
  } else if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
    return false;
  }

  ESP_LOGD(TAG, "On: %d", curr_state.bitmap & 0x01);

  for (size_t pos = 0; pos < 2; pos++) {
    if (data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ONE_SPACE)) {
      curr_state.fan_speed |= 1 << pos;
    } else if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
      return false;
    }
  }

  ESP_LOGD(TAG, "Fan speed: %d", curr_state.fan_speed);

  for (size_t pos = 0; pos < 2; pos++) {
    if (data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ONE_SPACE)) {
      curr_state.bitmap |= 1 << (pos + 1);
    } else if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
      return false;
    }
  }

  ESP_LOGD(TAG, "Swing: %d", (curr_state.bitmap >> 1) & 0x01);
  ESP_LOGD(TAG, "Sleep: %d", (curr_state.bitmap >> 2) & 0x01);

  for (size_t pos = 0; pos < 4; pos++) {
    if (data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ONE_SPACE)) {
      curr_state.temp |= 1 << pos;
    } else if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
      return false;
    }
  }

  ESP_LOGD(TAG, "Temp: %d", curr_state.temp);

  for (size_t pos = 0; pos < 8; pos++) {
    if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
      return false;
    }
  }

  for (size_t pos = 0; pos < 4; pos++) {
    if (data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ONE_SPACE)) {
      curr_state.bitmap |= 1 << (pos + 3);
    } else if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
      return false;
    }
  }

  ESP_LOGD(TAG, "Turbo: %d", (curr_state.bitmap >> 3) & 0x01);
  ESP_LOGD(TAG, "Light: %d", (curr_state.bitmap >> 4) & 0x01);
  ESP_LOGD(TAG, "Tree: %d", (curr_state.bitmap >> 5) & 0x01);
  ESP_LOGD(TAG, "Blow: %d", (curr_state.bitmap >> 6) & 0x01);

  uint16_t control_data = 0;
  for (size_t pos = 0; pos < 11; pos++) {
    if (data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ONE_SPACE)) {
      control_data |= 1 << pos;
    } else if (!data.expect_item(TOYOTOMI_BIT_MARK, TOYOTOMI_ZERO_SPACE)) {
      return false;
    }
  }

  if (control_data != 0x250) {
    return false;
  }

  return this->parse_state_frame_(curr_state);
}

}  // namespace toyotomi
}  // namespace esphome
