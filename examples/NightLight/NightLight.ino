// [ru] AlashBH1750: ночник. Светодиод включается, когда стемнело, и выключается, когда светло.
//      Два порога (гистерезис) не дают светодиоду мигать на границе. Светодиод на пин 13 (Uno) / встроенный.
// [kk] AlashBH1750: түнгі шам. Қараңғы түскенде жарықдиод қосылады, жарық болғанда өшеді.
//      Екі шек (гистерезис) жарықдиодтың шекарада жыпылықтауына жол бермейді. Жарықдиод 13 пинде (Uno) / кіріктірілген.
// [en] AlashBH1750: a night light. The LED switches on when it gets dark and off when it is light.
//      Two thresholds (hysteresis) keep the LED from blinking at the boundary. The LED on pin 13 (Uno) / the built-in one.
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
