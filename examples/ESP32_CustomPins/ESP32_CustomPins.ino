// [ru] AlashBH1750 на ESP32 со своими пинами I2C: begin(SDA, SCL).
// [kk] AlashBH1750 ESP32-де өз I2C пиндерімен: begin(SDA, SCL).
// [en] AlashBH1750 on ESP32 with its own I2C pins: begin(SDA, SCL).
#include <AlashBH1750.h>

AlashBH1750 light;

void setup() {
  Serial.begin(115200);
  if (!light.begin(21, 22)) {             // SDA = GPIO21, SCL = GPIO22
    Serial.println(light.errorText());
    while (true) delay(1000);
  }
}

void loop() {
  Serial.println(light.lux());
  delay(1000);
}
