// AlashBH1750: ночник. Светодиод включается, когда стемнело, и выключается, когда светло.
// Два порога (гистерезис) не дают светодиоду мигать на границе. Светодиод на пин 13 (Uno) / встроенный.
#include <AlashBH1750.h>

AlashBH1750 light;
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
const float DARK_BELOW = 20.0;           // включить ниже этой освещённости, лк
const float LIGHT_ABOVE = 50.0;          // выключить выше этой освещённости, лк
bool ledOn = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  if (!light.begin()) {
    Serial.println(light.errorText());
    while (true) delay(1000);
  }
}

void loop() {
  float lx = light.lux();
  if (!isnan(lx)) {
    if (!ledOn && lx < DARK_BELOW) ledOn = true;
    if (ledOn && lx > LIGHT_ABOVE) ledOn = false;
    digitalWrite(LED_BUILTIN, ledOn);
    Serial.print(lx, 1);
    Serial.println(ledOn ? F(" лк  ночник включён") : F(" лк  ночник выключен"));
  }
  delay(500);
}
