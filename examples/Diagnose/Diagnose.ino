// [ru] AlashBH1750: датчик не работает? Скетч покажет устройства на шине I2C, значения и подскажет, что проверить.
// [kk] AlashBH1750: датчик жұмыс істемей ме? Скетч I2C шинасындағы құрылғыларды, мәндерді көрсетеді және нені тексеру керегін айтады.
// [en] AlashBH1750: the sensor does not work? The sketch shows the devices on the I2C bus, the values and tells what to check.
#include <AlashBH1750.h>

AlashBH1750 light;

void setup() {
  Serial.begin(115200);
  delay(500);
  light.setLanguage(AlashBH1750::LANG_RU);   // LANG_KK: қазақша, LANG_EN: English
  light.begin();
  light.diagnose(Serial);
}

void loop() {}
