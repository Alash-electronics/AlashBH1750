// AlashBH1750: самый простой пример. Освещённость в люксах с датчика BH1750 (модуль GY-302).
//
// Подключение: VCC к 3.3 В или 5 В, GND к GND, SDA к A4 (ESP32: GPIO21), SCL к A5 (ESP32: GPIO22).
// Вывод ADDR оставьте свободным (адрес 0x23) или подключите к питанию (адрес 0x5C).
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
