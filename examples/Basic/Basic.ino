// [ru] AlashBH1750: самый простой пример. Освещённость в люксах с датчика BH1750 (модуль GY-302).
//
//      Подключение: VCC к 3.3 В или 5 В, GND к GND, SDA к A4 (ESP32: GPIO21), SCL к A5 (ESP32: GPIO22).
//      Вывод ADDR оставьте свободным (адрес 0x23) или подключите к питанию (адрес 0x5C).
// [kk] AlashBH1750: ең қарапайым мысал. BH1750 датчигінен (GY-302 модулі) люкспен жарықтық.
//
//      Қосу: VCC 3.3 В немесе 5 В-қа, GND GND-ға, SDA A4-ке (ESP32: GPIO21), SCL A5-ке (ESP32: GPIO22).
//      ADDR шықпасын бос қалдырыңыз (мекенжай 0x23) немесе қуатқа қосыңыз (мекенжай 0x5C).
// [en] AlashBH1750: the simplest example. Illuminance in lux from a BH1750 sensor (GY-302 module).
//
//      Wiring: VCC to 3.3 V or 5 V, GND to GND, SDA to A4 (ESP32: GPIO21), SCL to A5 (ESP32: GPIO22).
//      Leave the ADDR pin free (address 0x23) or connect it to supply (address 0x5C).
#include <AlashBH1750.h>

AlashBH1750 light;

void setup() {
  Serial.begin(115200);
  if (!light.begin()) {                   // сам находит датчик
    Serial.println(light.errorText());    // объяснит, что проверить
    while (true) delay(1000);
  }
}

void loop() {
  Serial.print(light.lux());              // от 0.1 до 100 000 лк; чувствительность подбирается сама
  Serial.print(F(" лк  ("));
  Serial.print(light.describe());         // словами: "темно", "освещённая комната", "прямое солнце"
  Serial.println(')');
  delay(1000);
}
