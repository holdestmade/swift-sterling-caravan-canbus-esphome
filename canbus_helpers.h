#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

// Helpers shared by the CAN-frame lambdas in esphome.yaml.
namespace caravan {

// 0x00B command frame layout:
//   {cmd, 150, dimmer1 %, dimmer2 %, 4, set temp °C, hot water mode, override hours}
// Bytes 1 and 4 are constant in every frame observed from the control panel;
// their meaning is unknown.
constexpr uint8_t B00B_CONST1 = 150;
constexpr uint8_t B00B_CONST4 = 4;

// 0x00B byte 0 values for heating-related commands.
constexpr uint8_t B00B_CMD_TIMER = 108;     // 0x6C
constexpr uint8_t B00B_CMD_OVERRIDE = 110;  // 0x6E

// Byte 0 for a heating command in the currently selected control mode
// (0 = Override, 1 = Timer).
inline uint8_t control_mode_byte(uint8_t control_mode_idx) {
  return control_mode_idx == 1 ? B00B_CMD_TIMER : B00B_CMD_OVERRIDE;
}

// Select option -> wire value mappings, shared between each select's
// canbus.send data lambda and its state-update lambda so the frame and the
// cached index can't diverge.
inline uint8_t electric_level_index(const std::string &s) {
  if (s == "Electric 1kW") return 1;
  if (s == "Electric 2kW") return 2;
  if (s == "Electric 3kW") return 3;
  return 0;
}

inline uint8_t hot_water_index(const std::string &s) {
  if (s == "Normal") return 1;
  if (s == "Boost") return 2;
  return 0;
}

inline uint8_t override_hours(const std::string &s) {
  if (s == "2h") return 2;
  if (s == "4h") return 4;
  if (s == "8h") return 8;
  if (s == "12h") return 12;
  return 1;
}

// Convert a 0.0–1.0 light level to a clamped 0–100 percentage byte.
inline uint8_t pct(float level) {
  return (uint8_t) roundf(fmaxf(0.0f, fminf(100.0f, level * 100.0f)));
}

inline std::vector<uint8_t> make_00b(uint8_t cmd, float d1_level, float d2_level,
                                     uint8_t set_temp, uint8_t hot_water_mode,
                                     uint8_t override_hours) {
  return {cmd, B00B_CONST1, pct(d1_level), pct(d2_level), B00B_CONST4,
          set_temp, hot_water_mode, override_hours};
}

// 0x0AD heating command frame: {electric level 0..3, gas on/off, fixed tail}.
// The trailing bytes (1,0,1,1,0,0) match the observed static payload.
inline std::vector<uint8_t> make_0ad(uint8_t electric_level, bool gas_on) {
  if (electric_level > 3) electric_level = 0;
  return {electric_level, (uint8_t) (gas_on ? 1 : 0), 1, 0, 1, 1, 0, 0};
}

// publish_state wrappers that skip redundant publishes.
template<typename S>
inline void pub_num(S *s, float v, float eps = 0.01f) {
  if (!s->has_state() || std::isnan(s->state) || fabsf(s->state - v) >= eps)
    s->publish_state(v);
}

template<typename S>
inline void pub_bin(S *s, bool v) {
  if (!s->has_state() || s->state != v) s->publish_state(v);
}

template<typename S>
inline void pub_txt(S *s, const std::string &v) {
  if (!s->has_state() || s->state != v) s->publish_state(v);
}

}  // namespace caravan
