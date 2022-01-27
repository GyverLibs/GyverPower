[![Foo](https://img.shields.io/badge/Version-2.1-brightgreen.svg?style=flat-square)](#versions)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)

# GyverPower
GyverPower - библиотека для управления энергопотреблением МК AVR
- Управление системным клоком		
- Включение/выключение периферии:
    - BOD
    - Таймеры
    - I2C/UART/SPI
    - USB
    - ADC		
- Сон в разных режимах (список ниже)		
- Сон на любой период
    - Калибровка таймера для точного времени сна
    - Корректировка millis()		

### Совместимость
- Atmega2560/32u4/328
- Attiny85/84/167

### Документация
К библиотеке есть [расширенная документация](https://alexgyver.ru/GyverPower/)

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **GyverPower** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverPower/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Инициализация
Нет

<a id="usage"></a>
## Использование
```cpp
void hardwareEnable(uint16_t data);             // включение указанной периферии (см. ниже "Константы периферии")
void hardwareDisable(uint16_t data);            // выключение указанной периферии (см. ниже "Константы периферии")
void setSystemPrescaler(prescalers_t prescaler);// установка делителя системной частоты (см. ниже "Константы делителя")
void adjustInternalClock(int8_t adj);           // подстройка частоты внутреннего генератора (число -120...+120)

void bodInSleep(bool en);                       // Brown-out detector в режиме сна (true вкл - false выкл) по умолч. отключен!
void setSleepMode(sleepmodes_t mode);           // установка текущего режима сна (см. ниже "Режимы сна")
void setSleepResolution(uint8_t period);        // установить разрешение сна (см. ниже "Периоды сна")

void autoCalibrate(void);                       // автоматическая калибровка таймера сна, выполняется 16 мс
void sleep(uint8_t period);                     // сон на фиксированный период (см. ниже "Периоды сна")
uint8_t sleepDelay(uint32_t ms);                // сон на произвольный период в миллисекундах (до 52 суток), возвращает остаток времени для коррекции таймеров
void correctMillis(bool state);                 // корректировать миллис на время сна sleepDelay() (по умолчанию включено)
void wakeUp(void);                              // помогает выйти из sleepDelay прерыванием (вызывать в будящем прерывании)	
bool inSleep(void);                             // вернёт true, если МК спит (для проверки в прерывании)
```

```cpp
===== РЕЖИМЫ СНА для setSleepMode() =====
IDLE_SLEEP          - Легкий сон, отключается только клок CPU и Flash, просыпается мгновенно от любых прерываний
ADC_SLEEP           - Легкий сон, отключается CPU и system clock, АЦП начинает преобразование при уходе в сон (см. пример ADCinSleep)
EXTSTANDBY_SLEEP    - Глубокий сон, идентичен POWERSAVE_SLEEP + system clock активен
STANDBY_SLEEP       - Глубокий сон, идентичен POWERDOWN_SLEEP + system clock активен
POWERSAVE_SLEEP     - Глубокий сон, идентичен POWERDOWN_SLEEP + timer 2 активен (+ можно проснуться от его прерываний), можно использовать для счета времени (см. пример powersaveMillis)
POWERDOWN_SLEEP     - Наиболее глубокий сон, отключается всё кроме WDT и внешних прерываний, просыпается от аппаратных (обычных + PCINT) или WDT

===== ПЕРИОДЫ СНА для sleep() и setSleepResolution() =====
SLEEP_16MS
SLEEP_32MS
SLEEP_64MS
SLEEP_128MS
SLEEP_256MS
SLEEP_512MS
SLEEP_1024MS
SLEEP_2048MS
SLEEP_4096MS
SLEEP_8192MS
SLEEP_FOREVER	- вечный сон

===== КОНСТАНТЫ ДЕЛИТЕЛЯ для setSystemPrescaler() =====
PRESCALER_1
PRESCALER_2
PRESCALER_4
PRESCALER_8
PRESCALER_16
PRESCALER_32
PRESCALER_64
PRESCALER_128
PRESCALER_256

===== КОНСТАНТЫ ПЕРИФЕРИИ для hardwareDisable() и hardwareEnable() =====
PWR_ALL		- всё железо
PWR_ADC		- АЦП и компаратор
PWR_TIMER1	- Таймер 0
PWR_TIMER0	- Таймер 1
PWR_TIMER2	- Таймер 2
PWR_TIMER3	- Таймер 3
PWR_TIMER4	- Таймер 4
PWR_TIMER5	- Таймер 5	
PWR_UART0	- Serial 0
PWR_UART1	- Serial 1
PWR_UART2	- Serial 2
PWR_UART3	- Serial 3
PWR_I2C		- Wire
PWR_SPI		- SPI
PWR_USB		- USB	
PWR_USI		- Wire + Spi (ATtinyXX)
PWR_LIN		- USART LIN (ATtinyXX)
```

### Простой сон
- Режим сна настраивается в `power.setSleepMode()`, по умолчанию активен `POWERDOWN_SLEEP` (остальные см. выше). 
- Чтобы уснуть - вызываем `power.sleep()` с указанием одного из стандартных периодов (см. выше). 
- Реальное время сна будет слегка отличаться, так как "таймер сна" не очень точный. 

### Сон на любой период
- Режим сна настраивается в `power.setSleepMode()`, по умолчанию активен `POWERDOWN_SLEEP` (остальные см. выше). 
- Чтобы уснуть - вызываем `power.sleepDelay()` с указанием периода в миллисекундах (`uint32_t`, до ~50 суток). 
*Как это работает? Просто цикл со стандартными периодами сна внутри этой функции.* 
- По умолчанию данная функция "спит" периодами по 128 миллисекунд. Время бодрствования между периодами сна составляет около 2.2 мкс (при 16 Мгц), 
что составляет 0.0017% от времени сна. Соответственно точность времени сна кратна одному периоду сна. Этот период можно настроить в 
`power.setSleepResolution()`, которая принимает те же константы, что и `sleep()`. Если нужен более точный сон - можно поставить 16 мс (`SLEEP_16MS`), 
если максимальное энергосбережение - 8 секунд (`SLEEP_8192MS`).
- Для преждевременного пробуждения по прерыванию нужно обязательно вызвать `power.wakeUp()` внутри обработчика прерывания. 
- Сон `sleepDelay()` имеет две очень полезные возможности:
  - Сон на очень точный период при откалиброванном таймере (см. ниже)
  - Сохранение счёта времени `millis()` на время сна (см. пример sleeptime)

### Калибровка таймера
В версии 2.0 библиотеки калибровка была упрощена: достаточно вызвать `power.autoCalibrate()` при запуске микроконтроллера. Функция выполняется ~16 мс. 
**Внимание! power.setSleepResolution() нужно вызывать после калибровки таймера.**

<a id="example"></a>
## Пример
Остальные примеры смотри в **examples**!
```cpp
// демо возможностей библиотеки
#include <GyverPower.h>

void setup() {
  pinMode(13, OUTPUT); // настраиваем вывод со светодиодом на выход
  Serial.begin(9600);

  power.autoCalibrate(); // автоматическая калибровка

  // отключение ненужной периферии
  power.hardwareDisable(PWR_ADC | PWR_TIMER1); // см раздел константы в GyverPower.h, разделяющий знак " | "

  // управление системной частотой
  power.setSystemPrescaler(PRESCALER_2); // см константы в GyverPower.h
  
  // настройка параметров сна
  power.setSleepMode(STANDBY_SLEEP); // если нужен другой режим сна, см константы в GyverPower.h (по умолчанию POWERDOWN_SLEEP)
  //power.bodInSleep(false); // рекомендуется выключить bod во сне для сохранения энергии (по умолчанию false - уже выключен!!)

  // пример однократного ухода в сон
  Serial.println("go to sleep");
  delay(100); // даем время на отправку
  
  power.sleep(SLEEP_2048MS); // спим ~ 2 секунды
  
  Serial.println("wake up!");
  delay(100); // даем время на отправку
}

void loop() {
  // пример циклического сна
  power.sleepDelay(1500);               // спим 1.5 секунды
  digitalWrite(13, !digitalRead(13));   // инвертируем состояние на пине
}
```

<a id="versions"></a>
## Версии
- v1.2 - фикс калибровки
- v1.3 - фикс для 32U4
- v1.4 - добавлен adjustInternalClock
- v1.5 - совместимость с аттини
- v1.6 - ещё совместимость с аттини
- v1.7 - оптимизация, совместимость с ATtiny13
- v1.8 - совместимость с ATmega32U4
- v2.0 - оптимизация памяти, переделан sleepDelay, можно точно узнать фактическое время сна
- v2.0.1 - fix compiler warnings
- v2.0.2 - исправлена ошибка компиляции ATtiny85
- v2.1 - добавлена bool inSleep(), для проверки спит ли МК

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!
