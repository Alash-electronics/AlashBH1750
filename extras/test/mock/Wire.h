// Имитация I2C и BH1750: хранит режим и время измерения, считает код по "истинной" освещённости.
#pragma once
#include "Arduino.h"
struct FakeBh {
  bool present = true; uint8_t addr = 0x23;
  float trueLux = 100.0f;
  uint8_t mt = 69; bool poweredOn = false; uint8_t mode = 0; bool high2 = false;
  int measurements = 0; uint32_t lastCmdAt = 0; bool measurementDone = false;
  uint32_t minWaitMs() const { return (uint32_t)(((mode & 0x03) == 0x03 ? 16 : 120) * mt / 69.0f); }
};
inline FakeBh g_bh;
class TwoWire {
 public:
  void begin() {}
  void begin(int, int) {}
  void beginTransmission(uint8_t a) { _a = a; _n = 0; }
  size_t write(uint8_t v) { _cmd = v; _n++; return 1; }
  uint8_t endTransmission(bool = true) {
    if (!(g_bh.present && _a == g_bh.addr)) return 2;
    if (_n) apply(_cmd);
    return 0;
  }
  uint8_t requestFrom(uint8_t a, uint8_t len) {
    if (!(g_bh.present && a == g_bh.addr) || len != 2) return 0;
    // Результат верный только если после команды прошло достаточно времени (иначе в настоящем датчике там ноль)
    float lx = g_bh.trueLux;
    float raw = lx * 1.2f * g_bh.mt / 69.0f * (g_bh.high2 ? 2.0f : 1.0f);
    uint32_t r = raw > 65535.0f ? 65535 : (uint32_t)(raw + 0.5f);
    if (g_ms - g_bh.lastCmdAt < g_bh.minWaitMs()) r = 0;
    _buf[0] = r >> 8; _buf[1] = r & 0xFF; _pos = 0; _len = 2;
    return 2;
  }
  int read() { return _pos < _len ? _buf[_pos++] : -1; }
 private:
  uint8_t _a = 0, _cmd = 0, _n = 0, _buf[4], _len = 0, _pos = 0;
  void apply(uint8_t c) {
    if (c == 0x01) { g_bh.poweredOn = true; return; }
    if (c == 0x00) { g_bh.poweredOn = false; return; }
    if (c == 0x07) return;
    if ((c & 0xE0) == 0x40) { g_bh.mt = (g_bh.mt & 0x1F) | ((c & 0x07) << 5); return; }
    if ((c & 0xE0) == 0x60) { g_bh.mt = (g_bh.mt & 0xE0) | (c & 0x1F); return; }
    if (c == 0x10 || c == 0x11 || c == 0x13 || c == 0x20 || c == 0x21 || c == 0x23) {
      g_bh.mode = c; g_bh.high2 = (c & 0x0F) == 0x01; g_bh.lastCmdAt = g_ms; g_bh.measurements++; g_bh.poweredOn = true;
    }
  }
};
inline TwoWire Wire;
