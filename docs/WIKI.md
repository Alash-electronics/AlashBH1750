# AlashBH1750: руководство / нұсқаулық / guide

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для датчика освещённости **BH1750** (модуль **GY-302**): люксы одной строкой, чувствительность подбирается сама, поэтому результат верный и в тёмной комнате, и на солнце. Освещённость можно получить и словами («темно», «освещённая комната»). Только встроенная `Wire`. Сделана для уроков и наборов Alash Electronics.

### Установка
Arduino IDE: Скетч, Подключить библиотеку, Управлять библиотеками, найти **AlashBH1750** и нажать Установить. Или скачайте ZIP с GitHub: Скетч, Подключить библиотеку, Добавить .ZIP библиотеку.

### Платы
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 и другие, где работает Arduino-ядро.

### Подключение
| Модуль GY-302 | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | свободен (0x23) или 3.3 В (0x5C) | то же |

Не закрывайте окошко датчика пальцем и не светите на него ближе 5 см для проверки: яркие светодиоды дадут огромные значения.

### Быстрый старт
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

### Справочник по методам
| Метод | Что делает |
|---|---|
| `begin()` | Найти датчик и включить |
| `lux()` | Освещённость, лк |
| `describe()` | Освещённость словами |
| `isDark(threshold)` | Темнее порога? |
| `setResolution(r)` | RES_HIGH2 (0.5 лк, по умолчанию), RES_HIGH, RES_LOW |
| `setAutoRange(on)` | Автоподбор чувствительности (включён) |
| `setSensitivity(mt)` | Вручную 31-254 (по умолчанию 69) |
| `setContinuous(on)` | Непрерывные измерения |
| `powerDown()` | Спать |
| `diagnose(Serial)` | Самопроверка |

### Если что-то не работает
**Датчик не найден**

Проверьте SDA/SCL, питание и GND. Запустите пример Diagnose. Адрес 0x23 при свободном ADDR и 0x5C при ADDR на питание.

**Всегда около нуля**

Датчик закрыт корпусом или заклеен. Окошко датчика должно смотреть на свет.

**Значения прыгают при лампах**

Лампы с ШИМ-диммером и светодиодные лампы мерцают на высокой частоте; датчик усредняет, но показания слегка колеблются. Усредните несколько измерений.

### Самопроверка
В библиотеке есть `diagnose(Serial)`: она печатает, что видит плата, и подсказывает, что проверить. Язык подсказок: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Примеры
`Basic`, `NightLight`, `Diagnose`, `ESP32_CustomPins`

---

## Қазақша

**BH1750** жарықтылық сенсорына (**GY-302** модулі) арналған қарапайым кітапхана: люкс бір жолмен, сезімталдық өзі таңдалады, сондықтан нәтиже қараңғы бөлмеде де, күнде де дұрыс. Жарықтылықты сөзбен де алуға болады. Тек кірістірілген `Wire`.

### Орнату
Arduino IDE: Sketch, Include Library, Manage Libraries, **AlashBH1750** табып, Install басыңыз. Немесе GitHub-тан ZIP жүктеп, Sketch, Include Library, Add .ZIP Library.

### Тақталар
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 және Arduino ядросы бар басқалар.

### Қосу
| GY-302 модулі | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | свободен (0x23) или 3.3 В (0x5C) | то же |

Сенсор терезесін саусақпен жаппаңыз.

### Жылдам бастау
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

### Әдістер анықтамасы
| Әдіс | Не істейді |
|---|---|
| `begin()` | Сенсорды табу және қосу |
| `lux()` | Жарықтылық, лк |
| `describe()` | Жарықтылық сөзбен |
| `isDark(threshold)` | Шектен қараңғы ма? |
| `setResolution(r)` | Ажыратымдылық |
| `setAutoRange(on)` | Авто-ауқым |
| `setSensitivity(mt)` | Қолмен |
| `setContinuous(on)` | Үздіксіз өлшеу |
| `powerDown()` | Ұйқы |
| `diagnose(Serial)` | Өзін тексеру |

### Бірдеңе жұмыс істемесе
**Сенсор табылмады**

SDA/SCL, қуат және GND тексеріңіз. Diagnose мысалын іске қосыңыз.

**Әрқашан нөлге жақын**

Сенсор жабық. Терезе жарыққа қарауы керек.

**Мәндер секіреді**

ШИМ-диммері бар шамдар жыпылықтайды. Бірнеше өлшемді орташалаңыз.

### Өзін-өзі тексеру
Кітапханада `diagnose(Serial)` бар: тақта не көргенін басып шығарады және нені тексеру керегін айтады. Кеңес тілі: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Мысалдар
`Basic`, `NightLight`, `Diagnose`, `ESP32_CustomPins`

---

## English

A simple library for the **BH1750** light sensor (**GY-302** module): lux in one line, the sensitivity is picked by itself so the result is right in a dark room and in the sun. The light level can also be returned in words ("dark", "lit room"). Only the built-in `Wire`. Made for classes and Alash Electronics kits.

### Installation
Arduino IDE: Sketch, Include Library, Manage Libraries, search **AlashBH1750** and click Install. Or download the ZIP from GitHub: Sketch, Include Library, Add .ZIP Library.

### Boards
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 and others with an Arduino core.

### Wiring
| GY-302 module | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | свободен (0x23) или 3.3 В (0x5C) | то же |

Keep the sensor window clear; very bright LEDs held close give huge readings.

### Quick start
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

### Method reference
| Method | What it does |
|---|---|
| `begin()` | Find the sensor and power it on |
| `lux()` | Illuminance, lx |
| `describe()` | Light level in words |
| `isDark(threshold)` | Darker than the threshold? |
| `setResolution(r)` | RES_HIGH2 (0.5 lx, default), RES_HIGH, RES_LOW |
| `setAutoRange(on)` | Automatic sensitivity (on) |
| `setSensitivity(mt)` | Manual 31-254 (default 69) |
| `setContinuous(on)` | Continuous measurement |
| `powerDown()` | Power down |
| `diagnose(Serial)` | Self-check |

### If something does not work
**The sensor is not found**

Check SDA/SCL, power and GND. Run the Diagnose example. The address is 0x23 with ADDR free and 0x5C with ADDR to power.

**Always near zero**

The sensor is covered. Its window must face the light.

**Values jump under lamps**

Lamps with PWM dimming flicker at high frequency; average several readings.

### Self-check
The library has `diagnose(Serial)`: it prints what the board sees and tells what to check. Tips language: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Examples
`Basic`, `NightLight`, `Diagnose`, `ESP32_CustomPins`

---

MIT License, Alash Electronics. https://github.com/Alash-electronics/AlashBH1750
