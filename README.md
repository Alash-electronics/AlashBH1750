# AlashBH1750

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для датчика освещённости **BH1750** (модуль **GY-302**): люксы одной строкой, чувствительность подбирается сама, поэтому результат верный и в тёмной комнате, и на солнце. Освещённость можно получить и словами («темно», «освещённая комната»). Только встроенная `Wire`. Сделана для уроков и наборов Alash Electronics.

```cpp
#include <AlashBH1750.h>
AlashBH1750 light;

void setup() { Serial.begin(115200); light.begin(); }
void loop() {
  Serial.println(light.lux());      // освещённость, лк / lux
  delay(1000);
}
```

### Что умеет
- **Сам находит датчик:** адрес 0x23 или 0x5C (вывод ADDR).
- **Автодиапазон:** чувствительность и разрешение меняются сами, от 0.1 лк (ночь) до 100 000 лк (прямое солнце). Не нужно знать про регистр MTreg.
- **Словами:** `describe()` («темно», «сумерки», «освещённая комната», «прямое солнце») на русском, казахском и английском; `isDark(порог)`.
- **Режимы:** разовые измерения (датчик экономит ток) или непрерывные `setContinuous(true)`; быстрый режим `RES_LOW` (16 мс).
- **Если света слишком много:** `NAN` и понятное сообщение, а не неверное число.
- **Кэш:** повторные вызовы подряд не заставляют ждать 120 мс каждый раз.
- **Понятные подсказки** на трёх языках: `errorText()`, `diagnose(Serial)`.
- Работает на **Arduino AVR, ESP32, ESP8266**.

### Подключение
| Модуль GY-302 | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | свободен (0x23) или 3.3 В (0x5C) | то же |

Не закрывайте окошко датчика пальцем и не светите на него ближе 5 см для проверки: яркие светодиоды дадут огромные значения.

### Чем отличается от других библиотек
Мы посмотрели BH1750 (Christopher Laws), hp_BH1750 (Stefan Armborst) и BH1750FVI:
- В большинстве нужно выбирать режим (`CONTINUOUS_HIGH_RES_MODE_2`...) и самому настраивать `setMTreg`, чтобы датчик не упирался в 54 612 лк на солнце или не терял точность ночью. У нас `lux()` и автодиапазон.
- Нет перевода люксов в слова.
- Нет подсказок при ошибке; у нас `diagnose()`.

### Примеры
`Basic`, `NightLight`, `Diagnose`, `ESP32_CustomPins`

### Тесты
Формула лк, режимы HIGH / HIGH2 / LOW, время ожидания измерения (имитация датчика отдаёт нули, если читать рано), автоподбор на светах от 0.2 до 100 000 лк, переполнение, кэш, непрерывный режим проверены без железа: `extras/test`.

Подробное руководство: [docs/WIKI.md](docs/WIKI.md).

---

## Қазақша

**BH1750** жарықтылық сенсорына (**GY-302** модулі) арналған қарапайым кітапхана: люкс бір жолмен, сезімталдық өзі таңдалады, сондықтан нәтиже қараңғы бөлмеде де, күнде де дұрыс. Жарықтылықты сөзбен де алуға болады. Тек кірістірілген `Wire`.

```cpp
#include <AlashBH1750.h>
AlashBH1750 light;

void setup() { Serial.begin(115200); light.begin(); }
void loop() {
  Serial.println(light.lux());      // освещённость, лк / lux
  delay(1000);
}
```

### Мүмкіндіктері
- **Сенсорды өзі табады:** 0x23 не 0x5C мекенжайы (ADDR шығысы).
- **Авто-ауқым:** сезімталдық пен ажыратымдылық өзі өзгереді, 0.1 лк-тан 100 000 лк-қа дейін.
- **Сөзбен:** `describe()` («қараңғы», «ымырт», «жарық бөлме», «тікелей күн») үш тілде; `isDark(шек)`.
- **Режимдер:** бір реттік не үздіксіз `setContinuous(true)`; жылдам `RES_LOW`.
- **Жарық тым көп болса:** `NAN` және түсінікті хабар.
- **Кэш:** қайта шақыру 120 мс күттірмейді.
- **Түсінікті кеңестер** үш тілде: `errorText()`, `diagnose(Serial)`.
- **Arduino AVR, ESP32, ESP8266**.

### Қосу
| GY-302 модулі | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | свободен (0x23) или 3.3 В (0x5C) | то же |

Сенсор терезесін саусақпен жаппаңыз.

### Басқа кітапханалардан айырмашылығы
BH1750 (Christopher Laws), hp_BH1750 және BH1750FVI қарадық:
- Көбінде режимді таңдап, `setMTreg` өзің баптайсың. Бізде `lux()` және авто-ауқым.
- Люксті сөзге аудару жоқ.
- Қате кеңестері жоқ; бізде `diagnose()`.

### Мысалдар
`Basic`, `NightLight`, `Diagnose`, `ESP32_CustomPins`

### Тесттер
Формула, режимдер, күту уақыты, авто-ауқым 0.2-100 000 лк, толу, кэш, үздіксіз режим тексерілген: `extras/test`.

Толық нұсқаулық: [docs/WIKI.md](docs/WIKI.md).

---

## English

A simple library for the **BH1750** light sensor (**GY-302** module): lux in one line, the sensitivity is picked by itself so the result is right in a dark room and in the sun. The light level can also be returned in words ("dark", "lit room"). Only the built-in `Wire`. Made for classes and Alash Electronics kits.

```cpp
#include <AlashBH1750.h>
AlashBH1750 light;

void setup() { Serial.begin(115200); light.begin(); }
void loop() {
  Serial.println(light.lux());      // освещённость, лк / lux
  delay(1000);
}
```

### Features
- **Finds the sensor itself:** address 0x23 or 0x5C (the ADDR pin).
- **Auto range:** sensitivity and resolution change by themselves, from 0.1 lx (night) to 100,000 lx (direct sun). No need to know the MTreg register.
- **In words:** `describe()` ("dark", "twilight", "lit room", "direct sunlight") in Russian, Kazakh and English; `isDark(threshold)`.
- **Modes:** one-shot (the sensor saves power) or continuous `setContinuous(true)`; fast `RES_LOW` (16 ms).
- **Too much light:** `NAN` and a clear message, not a wrong number.
- **Cache:** repeated calls do not wait 120 ms each time.
- **Clear tips** in three languages: `errorText()`, `diagnose(Serial)`.
- Works on **Arduino AVR, ESP32, ESP8266**.

### Wiring
| GY-302 module | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | свободен (0x23) или 3.3 В (0x5C) | то же |

Keep the sensor window clear; very bright LEDs held close give huge readings.

### How it differs from other libraries
We looked at BH1750 (Christopher Laws), hp_BH1750 (Stefan Armborst) and BH1750FVI:
- Most make you pick a mode (`CONTINUOUS_HIGH_RES_MODE_2`...) and tune `setMTreg` yourself so the sensor does not clip at 54,612 lx in the sun or lose precision at night. We have `lux()` and an auto range.
- No lux-to-words.
- No tips on errors; we have `diagnose()`.

### Examples
`Basic`, `NightLight`, `Diagnose`, `ESP32_CustomPins`

### Tests
The lux formula, HIGH / HIGH2 / LOW modes, measurement wait (the simulated sensor returns zeros if read too early), auto range from 0.2 to 100,000 lx, saturation, cache and continuous mode are checked without hardware: `extras/test`.

Full guide: [docs/WIKI.md](docs/WIKI.md).

---

MIT License, Alash Electronics.
