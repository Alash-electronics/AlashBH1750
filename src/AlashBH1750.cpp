// MIT License. Alash Electronics.
#include "AlashBH1750.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

static const uint8_t CMD_POWER_DOWN = 0x00;
static const uint8_t CMD_POWER_ON = 0x01;
static const uint8_t CMD_RESET = 0x07;
static const uint8_t CMD_CONT_H = 0x10, CMD_CONT_H2 = 0x11, CMD_CONT_L = 0x13;
static const uint8_t CMD_ONE_H = 0x20, CMD_ONE_H2 = 0x21, CMD_ONE_L = 0x23;
static const uint8_t DEFAULT_MT = 69;

bool AlashBH1750::begin(TwoWire &wire, bool startWire) {
  _wire = &wire;
  if (startWire) _wire->begin();
  _err = ERR_NO_DEVICE;
  if (probe(0x23)) return true;
  if (probe(0x5C)) return true;
  _err = ERR_NO_DEVICE;
  return false;
}

bool AlashBH1750::begin(uint8_t address, TwoWire &wire, bool startWire) {
  _wire = &wire;
  if (startWire) _wire->begin();
  return probe(address);
}

#if defined(ESP32) || defined(ESP8266)
bool AlashBH1750::begin(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  return begin(Wire, false);
}
#endif

bool AlashBH1750::probe(uint8_t address) {
  _addr = address;
  _wire->beginTransmission(address);
  if (_wire->endTransmission() != 0) {
    _err = ERR_NO_DEVICE;
    return false;
  }
  if (!command(CMD_POWER_ON) || !command(CMD_RESET) || !setMeasurementTime(DEFAULT_MT)) {
    _err = ERR_I2C;
    return false;
  }
  _started = true;
  _hasData = false;
  _err = ERR_OK;
  return true;
}

bool AlashBH1750::isConnected() {
  _wire->beginTransmission(_addr);
  return _wire->endTransmission() == 0;
}

bool AlashBH1750::command(uint8_t c) {
  _wire->beginTransmission(_addr);
  _wire->write(c);
  return _wire->endTransmission() == 0;
}

// Время измерения MT (31..254) задаётся двумя командами: старшие 3 бита и младшие 5
bool AlashBH1750::setMeasurementTime(uint8_t mt) {
  if (mt < 31) mt = 31;
  if (mt > 254) mt = 254;
  if (!command(0x40 | (mt >> 5)) || !command(0x60 | (mt & 0x1F))) return false;
  _mt = mt;
  return true;
}

bool AlashBH1750::setSensitivity(uint8_t mt) {
  if (mt < 31 || mt > 254) return false;
  _auto = false;
  return _started ? setMeasurementTime(mt) : false;
}

void AlashBH1750::powerDown() {
  if (_started) command(CMD_POWER_DOWN);
  _hasData = false;
}

// Сколько ждать результата: 120 мс (запас до 180) при MT=69, пропорционально MT; в LOW 16 мс (запас 24)
uint16_t AlashBH1750::waitMs() const {
  uint32_t base = _effRes == RES_LOW ? 24 : 180;
  return (uint16_t)(base * _mt / DEFAULT_MT + 2);
}

bool AlashBH1750::measureRaw(uint16_t &raw) {
  if (!_continuous || !_hasData) {
    uint8_t cmd = _continuous ? (_effRes == RES_HIGH ? CMD_CONT_H : _effRes == RES_HIGH2 ? CMD_CONT_H2 : CMD_CONT_L)
                              : (_effRes == RES_HIGH ? CMD_ONE_H : _effRes == RES_HIGH2 ? CMD_ONE_H2 : CMD_ONE_L);
    if (!command(cmd)) return false;
    delay(waitMs());
  }
  if (_wire->requestFrom(_addr, (uint8_t)2) != 2) return false;
  raw = (uint16_t)(_wire->read() << 8);
  raw |= (uint8_t)_wire->read();
  return true;
}

// raw / 1.2 лк при MT=69; при другом MT чувствительность пропорциональна; в режиме HIGH2 шаг вдвое меньше
float AlashBH1750::toLux(uint16_t raw) const {
  float lx = raw / 1.2f * ((float)DEFAULT_MT / _mt);
  return _effRes == RES_HIGH2 ? lx / 2.0f : lx;
}

bool AlashBH1750::update() {
  if (!_started) {
    _err = ERR_NO_DEVICE;
    return false;
  }
  uint16_t raw = 0;
  _effRes = _res;
  for (uint8_t pass = 0; pass < 4; pass++) {
    if (!measureRaw(raw)) {
      _err = ERR_I2C;
      return false;
    }
    if (!_auto) break;
    if (raw >= 65535 && _mt <= 31 && _effRes == RES_HIGH2) {   // в режиме HIGH2 шкала вдвое короче: на ярком свете переходим на HIGH
      _effRes = RES_HIGH;
      _hasData = false;
      continue;
    }
    // Автоподбор: хорошее значение лежит в середине шкалы (10 000..50 000 из 65 535). Иначе меняем MT и меряем снова.
    if (raw >= 10000 && raw <= 60000) break;
    uint32_t want = (uint32_t)_mt * 30000UL / (raw ? raw : 1);
    if (want < 31) want = 31;
    if (want > 254) want = 254;
    if (want == _mt) break;                    // уже на пределе чувствительности
    if (!setMeasurementTime((uint8_t)want)) {
      _err = ERR_I2C;
      return false;
    }
    _hasData = false;                          // для непрерывного режима: после смены MT результат старый
  }
  _hasData = true;
  _readAt = millis();
  if (raw >= 65535 && _mt <= 31) {
    _lux = NAN;
    _err = ERR_SATURATED;
    return false;
  }
  _lux = toLux(raw);
  _err = ERR_OK;
  return true;
}

void AlashBH1750::ensureFresh() {
  if (_hasData && (uint32_t)(millis() - _readAt) < _cacheMs) return;
  update();
}

float AlashBH1750::lux() {
  ensureFresh();
  return _lux;
}

bool AlashBH1750::isDark(float thresholdLux) {
  float l = lux();
  return !isnan(l) && l < thresholdLux;
}

const __FlashStringHelper *AlashBH1750::describe() {
  float l = lux();
  if (isnan(l)) return F("?");
  if (l < 1) return T("темно", "қараңғы", "dark");
  if (l < 10) return T("сумерки", "ымырт", "twilight");
  if (l < 100) return T("слабый свет", "әлсіз жарық", "dim light");
  if (l < 500) return T("освещённая комната", "жарық бөлме", "lit room");
  if (l < 5000) return T("яркое помещение или пасмурно", "жарық бөлме не бұлтты", "bright room or overcast");
  if (l < 30000) return T("яркий день", "жарық күн", "bright day");
  return T("прямое солнце", "тікелей күн сәулесі", "direct sunlight");
}

const __FlashStringHelper *AlashBH1750::errorText() {
  switch (_err) {
    case ERR_OK: return F("OK");
    case ERR_NO_DEVICE:
      return T("Датчик не найден: проверьте SDA / SCL, питание (3.3-5 В) и GND. Адрес 0x23 (ADDR на землю) или 0x5C (ADDR на питание)",
               "Сенсор табылмады: SDA / SCL, қуат (3.3-5 В) және GND тексеріңіз. Мекенжай 0x23 (ADDR жерге) не 0x5C (ADDR қуатқа)",
               "Sensor not found: check SDA / SCL, power (3.3-5 V) and GND. Address 0x23 (ADDR to ground) or 0x5C (ADDR to power)");
    case ERR_I2C:
      return T("Ошибка I2C: проверьте контакты и подтяжку линий", "I2C қатесі: байланысты және тартқыш резисторларды тексеріңіз",
               "I2C error: check connections and pull-up resistors");
    case ERR_SATURATED:
      return T("Света больше предела датчика (около 100 000 лк): закройте датчик рассеивателем", "Жарық сенсор шегінен көп (шамамен 100 000 лк): сенсорды шашыратқышпен жабыңыз",
               "More light than the sensor can measure (about 100,000 lx): cover it with a diffuser");
  }
  return F("");
}

void AlashBH1750::diagnose(Print &out) {
  out.println(T("AlashBH1750: диагностика", "AlashBH1750: диагностика", "AlashBH1750: diagnostics"));
  out.println(T("Найденные устройства I2C:", "Табылған I2C құрылғылары:", "I2C devices found:"));
  for (uint8_t a = 1; a < 127; a++) {
    _wire->beginTransmission(a);
    if (_wire->endTransmission() == 0) {
      out.print(F("  0x"));
      if (a < 16) out.print('0');
      out.print(a, HEX);
      if (a == 0x23 || a == 0x5C) out.print(F("  BH1750"));
      else if (a == 0x76 || a == 0x77) out.print(F("  BME280 / BMP280"));
      else if (a == 0x3C || a == 0x3D) out.print(F("  OLED SSD1306"));
      else if (a == 0x68) out.print(F("  DS3231 / MPU6050"));
      out.println();
    }
  }
  if (!_started) {
    out.println(errorText());
    return;
  }
  out.print(T("Адрес датчика: 0x", "Сенсор мекенжайы: 0x", "Sensor address: 0x"));
  out.println(_addr, HEX);
  if (update()) {
    out.print(T("Освещённость: ", "Жарықтылық: ", "Illuminance: "));
    out.print(_lux, 1);
    out.print(F(" lx  ("));
    out.print(describe());
    out.println(F(")"));
    out.print(T("Чувствительность (MT): ", "Сезімталдық (MT): ", "Sensitivity (MT): "));
    out.println((int)_mt);
    if (_lux < 0.5f) out.println(T("Показывает почти ноль: датчик закрыт или в темноте. Посветите на него телефоном.", "Нөлге жақын: сенсор жабық немесе қараңғы. Телефонмен жарықтандырыңыз.", "Almost zero: the sensor is covered or in the dark. Shine a phone light on it."));
  } else {
    out.println(errorText());
  }
}
