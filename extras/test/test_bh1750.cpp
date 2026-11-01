// Тесты AlashBH1750 без железа (имитация датчика с учётом времени измерения):
//   g++ -std=c++17 -Wall -I mock -I ../../src test_bh1750.cpp ../../src/AlashBH1750.cpp && ./a.out
#include <stdio.h>
#include "Arduino.h"
#include "Wire.h"
#include "AlashBH1750.h"

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); fails++; } } while (0)
// допуск: относительный плюс шаг квантования датчика (целое raw), 0.5 лк в HIGH2 / 69 * MT
static bool within(float v, float truth, float rel, float quantum = 0.6f) { return fabsf(v - truth) <= truth * rel + quantum; }
static void reset() { g_bh = FakeBh(); g_ms += 5000; }

int main() {
  // 1) запуск: адрес 0x23, питание включено, MT по умолчанию 69
  reset(); AlashBH1750 s;
  CHECK(s.begin() && s.address() == 0x23 && g_bh.poweredOn && g_bh.mt == 69);
  reset(); g_bh.addr = 0x5C; AlashBH1750 s2;
  CHECK(s2.begin() && s2.address() == 0x5C);

  // 2) формула: raw/1.2 лк. 300 лк -> raw 360
  reset(); g_bh.trueLux = 300; AlashBH1750 a; a.begin(); a.setAutoRange(false);
  CHECK(within(a.lux(), 300, 0.01));
  CHECK(g_bh.mode == 0x21);                                  // разовое измерение, высокое разрешение H2 (по умолчанию)
  a.setResolution(AlashBH1750::RES_HIGH); g_ms += 300;
  CHECK(within(a.lux(), 300, 0.01) && g_bh.mode == 0x20);

  // 3) режим HIGH2: шаг 0.5 лк, формула делит на 2
  reset(); g_bh.trueLux = 5.5f; AlashBH1750 h; h.begin(); h.setAutoRange(false); h.setResolution(AlashBH1750::RES_HIGH2);
  CHECK(within(h.lux(), 5.5f, 0.05) && g_bh.mode == 0x21);

  // 4) режим LOW: быстро (16 мс), проверка ожидания
  reset(); g_bh.trueLux = 400; AlashBH1750 l; l.begin(); l.setAutoRange(false); l.setResolution(AlashBH1750::RES_LOW);
  uint32_t t0 = g_ms; CHECK(within(l.lux(), 400, 0.02));
  CHECK(g_ms - t0 < 40 && g_bh.mode == 0x23);

  // 5) автоподбор чувствительности: от 0.2 лк до 100 000 лк результат верный
  const float cases[] = {0.2f, 1, 5, 30, 300, 3000, 20000, 54000, 100000};
  for (float lx : cases) {
    reset(); g_bh.trueLux = lx; AlashBH1750 r; r.begin();
    float v = r.lux();
    float q = 0.5f / 1.2f * 69.0f / r.sensitivity() * 1.1f;      // шаг датчика на этой чувствительности
    if (!within(v, lx, 0.03, q)) printf("  lux %.1f -> %.2f (MT %d)\n", lx, v, r.sensitivity());
    CHECK(within(v, lx, 0.03, q));
  }

  // 6) свет сильнее предела: ERR_SATURATED и NAN
  reset(); g_bh.trueLux = 150000; AlashBH1750 sat; sat.begin();
  CHECK(isnan(sat.lux()) && sat.error() == AlashBH1750::ERR_SATURATED);

  // 7) ручная чувствительность и границы
  reset(); AlashBH1750 m; m.begin();
  CHECK(m.setSensitivity(254) && g_bh.mt == 254 && m.sensitivity() == 254);
  CHECK(m.setSensitivity(31) && g_bh.mt == 31);
  CHECK(!m.setSensitivity(30) && !m.setSensitivity(255));
  g_bh.trueLux = 1000; CHECK(within(m.lux(), 1000, 0.02));    // при MT=31 тоже верно (автоподбор выключен)

  // 8) кэш: повторные вызовы без измерения, потом обновление
  reset(); g_bh.trueLux = 200; AlashBH1750 c; c.begin(); c.lux();
  int n = g_bh.measurements; c.lux(); c.lux();
  CHECK(g_bh.measurements == n);
  g_ms += 300; g_bh.trueLux = 400; CHECK(within(c.lux(), 400, 0.03));
  CHECK(g_bh.measurements > n);

  // 9) непрерывный режим
  reset(); g_bh.trueLux = 250; AlashBH1750 k; k.begin(); k.setAutoRange(false); k.setContinuous(true); k.setResolution(AlashBH1750::RES_HIGH);
  CHECK(within(k.lux(), 250, 0.02) && g_bh.mode == 0x10);
  g_ms += 300; g_bh.trueLux = 500; CHECK(within(k.lux(), 500, 0.02));

  // 10) описание словами и isDark
  reset(); AlashBH1750 w; w.begin();
  g_bh.trueLux = 0.3f; g_ms += 300; CHECK(w.isDark(10) && strcmp((const char *)w.describe(), "темно") == 0);
  g_bh.trueLux = 300; g_ms += 300; CHECK(!w.isDark(10) && strcmp((const char *)w.describe(), "освещённая комната") == 0);
  w.setLanguage(AlashBH1750::LANG_EN); CHECK(strcmp((const char *)w.describe(), "lit room") == 0);
  g_bh.trueLux = 60000; g_ms += 300; CHECK(strcmp((const char *)w.describe(), "direct sunlight") == 0);

  // 11) ошибки: нет датчика, пропал на ходу, powerDown
  reset(); g_bh.present = false; AlashBH1750 n1;
  CHECK(!n1.begin() && n1.error() == AlashBH1750::ERR_NO_DEVICE && isnan(n1.lux()));
  reset(); AlashBH1750 p; p.begin(); g_bh.present = false; g_ms += 300;
  CHECK(isnan(p.lux()) && p.error() == AlashBH1750::ERR_I2C);
  reset(); AlashBH1750 d; d.begin(); d.powerDown(); CHECK(!g_bh.poweredOn);
  printf("--- diagnose ---\n"); reset(); g_bh.trueLux = 0.1f; AlashBH1750 dg; dg.begin(); dg.setLanguage(AlashBH1750::LANG_KK); dg.diagnose(*(new Print));

  if (fails) { printf("FAILED: %d\n", fails); return 1; }
  printf("ALL TESTS PASSED\n");
  return 0;
}
