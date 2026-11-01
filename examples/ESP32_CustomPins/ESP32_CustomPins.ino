// AlashBH1750 на ESP32 со своими пинами I2C: begin(SDA, SCL).
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
