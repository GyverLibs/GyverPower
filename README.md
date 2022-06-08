[![Foo](https://img.shields.io/badge/Version-2.1-brightgreen.svg?style=flat-square)](#versions)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)

[![Foo](https://img.shields.io/badge/README-ENGLISH-brightgreen.svg?style=for-the-badge)](https://github-com.translate.goog/GyverLibs/GyverPower?_x_tr_sl=ru&_x_tr_tl=en)

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


# GyverPower --中文版readme
GyverPower - 电源管理库（arduino） МК AVR
- 系统时钟管理			
- 启用/停用 外围设备:
    - BOD
    - 定时器（Timers）
    - I2C/UART/SPI
    - USB
    - ADC		
- 不同的睡眠模式 (见下表)		
- 自定义睡眠时长
    - 通过计数器
    - 调节 millis()		

### 硬件兼容性
- Atmega2560/32u4/328
- Attiny85/84/167

### 参考文档
参考文档见 [文档链接](https://alexgyver.ru/GyverPower/)

## 目录
- [安装](#install)
- [初始化](#init)
- [使用方式](#usage)
- [范例](#example)
- [版本历史](#versions)
- [BUG和问题反馈](#feedback)

<a id="install"></a>
## 安装方法
- 通过下列软件内的库管理器内搜索关键则 “GyverPower” 来完成安装:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [下载库文件](https://github.com/GyverLibs/GyverPower/archive/refs/heads/main.zip) .zip 手动安装:
    - 解压并且放在这个路径下 “C:\Program Files (x86)\Arduino\libraries” (Windows x64)
    - 解压并且放在这个路径下 “C:\Program Files\Arduino\libraries” (Windows x32)
    - 解压并且放在这个路径下 “Документы/Arduino/libraries/”
    - (Arduino IDE) 会自动安装 .zip 压缩包文件 （操作路径“Sketch/Include library/Add .ZIP library…“ ）
- 更多详细信息可以看以下链接（俄语） [这里](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## 初始化设置
无

<a id="usage"></a>
## 使用方式
```cpp
void hardwareEnable(uint16_t data);             // enable the specified peripheral (see "Peripheral Constants" below) 
void hardwareDisable(uint16_t data);            // turn off the specified peripheral (see "Peripheral Constants" below)
void setSystemPrescaler(prescalers_t prescaler);// set the system frequency divider (see "Divisor Constants" below)
void adjustInternalClock(int8_t adj);           // adjusting the frequency of the internal generator (-120...+120)

void bodInSleep(bool en);                       // Brown-out detector in sleep mode (true on - false off) by default disabled!
void setSleepMode(sleepmodes_t mode);           // set the current sleep mode (see "Sleep Modes" below)
void setSleepResolution(uint8_t period);        // et sleep permission (see "Sleep periods" below)

void autoCalibrate(void);                       // automatic calibration of the sleep timer, runs 16ms
void sleep(uint8_t period);                     // sleep for a fixed period (see "Sleep periods" below)
uint8_t sleepDelay(uint32_t ms);                // sleep for an arbitrary period in milliseconds (up to 52 days), returns the rest of the time to adjust the timers
void correctMillis(bool state);                 // adjust millis for sleep time sleepDelay() (enabled by default)
void wakeUp(void);                              // helps to exit sleepDelay with an interrupt (call in a wakeup interrupt)
bool inSleep(void);                             // will return true if the MCU is sleeping (for checking in an interrupt)
```

```cpp
===== SLEEP MODES for  setSleepMode() =====
IDLE_SLEEP          - Light sleep, turns off only the CPU and Flash clock, wakes up instantly from any interrupts
ADC_SLEEP           - Light sleep, turns off CPU and system clock, ADC starts conversion when going to sleep (see ADCinSleep example)
EXTSTANDBY_SLEEP    - Deep sleep, identical to POWERSAVE_SLEEP + system clock active
STANDBY_SLEEP       - Deep sleep, identical to POWERDOWN_SLEEP + system clock active
POWERSAVE_SLEEP     - Deep sleep, identical to POWERDOWN_SLEEP + timer 2 active (+ can be woken up by its interrupts), can be used to count time (see powersaveMillis example)
POWERDOWN_SLEEP     - The deepest sleep, everything is turned off except WDT and external interrupts, wakes up from hardware (regular + PCINT) or WDT（默认值）

===== SLEEP PERIODS for sleep() and setSleepResolution() =====
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
SLEEP_FOREVER	- eternal sleep （需要外部唤醒）

===== DIVISOR CONSTANTS for  setSystemPrescaler() =====
PRESCALER_1
PRESCALER_2
PRESCALER_4
PRESCALER_8
PRESCALER_16
PRESCALER_32
PRESCALER_64
PRESCALER_128
PRESCALER_256

===== PERIPHERY CONSTANTS for hardwareDisable() and hardwareEnable() =====
PWR_ALL		- all hardware
PWR_ADC		- ADC and comparator
PWR_TIMER1	- Timer  0
PWR_TIMER0	- Timer 1
PWR_TIMER2	- Timer 2
PWR_TIMER3	- Timer 3
PWR_TIMER4	- Timer 4
PWR_TIMER5	- Timer 5	
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

### 简单总结
- 可以通过函数 'power.setSleepMode()'来设置SleepMode, 默认是设置为 'POWERDOWN_SLEEP' (详情见上表). 
- 如要设置sleep模式 调用函数 'power.sleep()'并且设置睡眠时间长度(参数见上面). 
- 实际的睡眠时间会与’sleep timer‘的标值有些差异。

### Sleep for any period
- 可以通过函数 'power.setSleepMode()'来设置SleepMode, 默认是设置为 'POWERDOWN_SLEEP' (详情见上表). 
- 可以通过函数 'power.sleepDelay()'；来设置延迟睡眠时间 (’uint32_t‘, 按毫秒时间计算，最长可以达 52天). 
*工作原理是什么？ 就是在大loop中有若干个标准时间长的sleep动作* 
- 默认情况下，函数按按照128 毫秒为一个周期进行休眠“休眠”。睡眠唤醒周期为2.2 μs (CPU频率为 16Mhz), 
约为睡眠时长的0.0017%. 因此，睡眠时间的准确度来说是一个睡眠周期的若干倍，通过设置倍数来调节睡眠时长。
如果你要更准确的睡眠时长，建议 'power.setSleepResolution()'和'sleep()'函数采用相同的值. 如果要准确的时间长度建议使用 16 ms ('SLEEP_16MS'), 
如果睡眠时间设置为8s(`SLEEP_8192MS`).
- 必须在中断处理程序中调用 'power.wakeUp()' ,以便提前唤醒CPU。
- 函数 'sleepDelay()' 有两个非常有用的功能:
  - 用一个经过校准的Timer来实现准确的睡眠时间 (见下面)
  - 在睡眠时间内减少 'millis()'时间计数量  (见下面sleeptime)

### Timer 校准
2.0版本以上，简化了校准方式。只要Arduino上电启动时调用函数 'power.autoCalibrate()'，函数大概在启动后~16 ms执行. 
**注意! 调用Timer校准之后必须调用power.setSleepResolution() **

<a id="example"></a>
## Example
See **examples** for other examples!
```cpp
// demo of library features
#include <GyverPower.h>

void setup() {
  pinMode(13, OUTPUT); // set pin with LED to output
  Serial.begin(9600);

  power.autoCalibrate(); //automatic calibration

  // disable unnecessary peripherals
  power.hardwareDisable(PWR_ADC | PWR_TIMER1); // see constant section in GyverPower.h delimited by " | "

  // system frequency control
  power.setSystemPrescaler(PRESCALER_2); // see constants in GyverPower.h
  
  // setting sleep parameters
  power.setSleepMode(STANDBY_SLEEP); // if you need a different sleep mode, see constants in GyverPower.h (default POWERDOWN_SLEEP)
  //power.bodInSleep(false); //  it is recommended to turn off bod in sleep to save power (default is false - already off!!)

  //example of going to sleep once
  Serial.println("go to sleep");
  delay(100); // give time to send
  
  power.sleep(SLEEP_2048MS); //sleep ~ 2 seconds
  
  Serial.println("wake up!");
  delay(100); //  give time to send
}

void loop() {
  // пример циклического сна
  power.sleepDelay(1500);               // sleep 1.5 seconds
  digitalWrite(13, !digitalRead(13));   //  invert the state on the pin
}
```

<a id="versions"></a>
## Версии
- v1.2 - calibration fix
- v1.3 - fix for 32U4
- v1.4 - added adjustInternalClock
- v1.5 - compatible with attini
- v1.6 - more ATTINI compatibility
- v1.7 - optimization, compatibility with ATtiny13
- v1.8 - compatible with ATmega32U4
- v2.0 - memory optimization, sleepDelay redone, you can accurately know the actual sleep time
- v2.0.1 - fix compiler warnings
- v2.0.2 - fixed ATtiny85 compilation error
- v2.1 - added bool inSleep() to check if MK is sleeping

<a id="feedback"></a>
## Bugs and feedback
When you find bugs, create an **Issue**, or better, immediately write to the mail  [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your  **Pull Request**'ов!
