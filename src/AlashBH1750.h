// AlashBH1750: датчик освещённости BH1750 (GY-302): люксы, автоподбор чувствительности от 0.1 до 100 000 лк.
// Без зависимостей, только встроенная Wire. MIT License. Alash Electronics, https://github.com/Alash-electronics/AlashBH1750
#ifndef ALASH_BH1750_H
#define ALASH_BH1750_H

#include <Arduino.h>
#include <Wire.h>

class AlashBH1750 {
 public:
  enum Error : uint8_t {
    ERR_OK = 0,
    ERR_NO_DEVICE,    // на шине I2C нет ответа по адресам 0x23 и 0x5C
    ERR_I2C,          // ошибка чтения или записи по I2C
    ERR_SATURATED     // света больше, чем может измерить датчик (более 100 000 лк)
  };
  // Разрешение: HIGH2 шаг 0.5 лк (по умолчанию, лучше при слабом свете), HIGH шаг 1 лк, LOW шаг 4 лк (быстро, 16 мс). Шаг ещё делится на чувствительность MT/69
  enum Resolution : uint8_t { RES_HIGH = 0, RES_HIGH2 = 1, RES_LOW = 2 };
  enum Language : uint8_t { LANG_RU = 0, LANG_KK = 1, LANG_EN = 2 };

  // ---- Запуск --------------------------------------------------------------
  // begin() находит датчик сам: адрес 0x23 (вывод ADDR на землю или не подключён) или 0x5C (ADDR на питание).
  bool begin(TwoWire &wire = Wire, bool startWire = true);
  bool begin(uint8_t address, TwoWire &wire = Wire, bool startWire = true);
#if defined(ESP32) || defined(ESP8266)
  bool begin(int sdaPin, int sclPin);
#endif

  // ---- Измерения -------------------------------------------------------------
  float lux();                              // освещённость, лк (при первом вызове за интервал датчик измеряет сам)
  const __FlashStringHelper *describe();    // словами: "темно", "сумерки", "освещённая комната", "яркий свет", "прямое солнце"...
  bool isDark(float thresholdLux = 10.0f);
  bool update();                            // принудительно измерить

  // ---- Настройки -----------------------------------------------------------------
  void setResolution(Resolution r) { _res = r; _hasData = false; }
  void setAutoRange(bool on) { _auto = on; }          // подбирать чувствительность под свет (по умолчанию включено)
  bool setSensitivity(uint8_t mt);                    // 31..254, по умолчанию 69; автоподбор выключится
  uint8_t sensitivity() const { return _mt; }
  void setContinuous(bool on) { _continuous = on; _hasData = false; }   // непрерывные измерения вместо разовых
  void setCacheMs(uint16_t ms) { _cacheMs = ms; }
  void powerDown();                         // спать
  void setLanguage(Language lang) { _lang = lang; }

  // ---- Состояние и диагностика --------------------------------------------------------
  uint8_t address() const { return _addr; }
  bool isConnected();
  Error error() const { return _err; }
  const __FlashStringHelper *errorText();
  void diagnose(Print &out);

 private:
  TwoWire *_wire = &Wire;
  uint8_t _addr = 0;
  Error _err = ERR_OK;
  Language _lang = LANG_RU;
  Resolution _res = RES_HIGH2;
  Resolution _effRes = RES_HIGH2;            // реально использованное разрешение (при очень ярком свете автоматически HIGH)
  bool _auto = true, _continuous = false, _hasData = false, _started = false;
  uint8_t _mt = 69;
  uint16_t _cacheMs = 250;
  uint32_t _readAt = 0;
  float _lux = NAN;

  bool probe(uint8_t address);
  bool command(uint8_t c);
  bool setMeasurementTime(uint8_t mt);
  bool measureRaw(uint16_t &raw);
  uint16_t waitMs() const;
  float toLux(uint16_t raw) const;
  void ensureFresh();
};

#endif  // ALASH_BH1750_H
