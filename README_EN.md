This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![Foo](https://img.shields.io/badge/Version-2.2-brightgreen.svg?style=flat-square)](#versions)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)

[![Foo](https://img.shields.io/badge/README-ENGLISH-brightgreen.svg?style=for-the-badge)](https://github-com.translate.goog/GyverLibs/GyverPower?_x_tr_sl=ru&_x_tr_tl=en)

# GyverPower
GyverPower – Energy Management Library for MK AVR
- System clogging management
- Peripheral on/off:
    - BOD
    - Timers.
    - I2C/UART/SPI
    - USB
    - ADC		
- Sleep in different modes (list below)
- Sleep for any period
    - Calibration of the timer for the exact time of sleep
    - Adjustment millis()

### Compatibility
- Atmega2560/32u4/328
- Attiny85/84/167

### Documentation.
There's a library[extended documentation](https://alexgyver.ru/GyverPower/)

## Contents
- [Use of use](#usage)
- [Example](#example)
- [Installation](#install)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="usage"></a>

## Use of use
```cpp
void hardwareEnable(uint16_t data);               // inclusion of said periphery (see below "Peripheral constants")
void hardwareDisable(uint16_t data);              // turning off the specified periphery (see below "Peripheral constants")
void setSystemPrescaler(prescalers_t prescaler);  // splitter
void adjustInternalClock(int8_t adj);             // adjusting the frequency of the internal generator (number -120). +120)
void bodInSleep(bool en);                         // Brown-out detector in sleep mode (true on - false off)

void setSleepMode(sleepmodes_t mode);             // Setting the current sleep pattern [silence]. POWERDOWN SLEEP
void sleep(sleepprds_t period);                   // sleep
bool inSleep();                                   // will return true if the MK is asleep for an interrupt check

uint32_t sleepDelay(uint32_t ms);                 // Sleeping for an arbitrary period in milliseconds returns the rest of the time to adjust timers
uint32_t sleepDelay(uint32_t ms, uint32_t sec, uint16_t min = 0, uint16_t hour = 0, uint16_t day = 0);
void setSleepResolution(sleepprds_t period);      // Set the sleepdelay() resolution. SLEEP 128MS
void correctMillis(bool state);                   // Adjust Millis for SleepDelay() [Silent True]
void calibrate();                                 // automatic calibration of sleepDelay(), performed 16 ms
void wakeUp();                                    // Helps to exit sleepDelay() by interruption (call in a future interruption)
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

### Simple sleep.
- Sleep mode is adjusted in`power.setSleepMode()`by default active`POWERDOWN_SLEEP`(For the rest, see above).
- We call to sleep.`power.sleep()`with an indication of one of the standard periods (see above).
- The actual sleep time will be slightly different, as the "sleep timer" is not very accurate.

### Sleep for any period
- Sleep mode is adjusted in`power.setSleepMode()`by default active`POWERDOWN_SLEEP`(For the rest, see above).
- We call to sleep.`power.sleepDelay()`period in milliseconds (`uint32_t`, up to ~50 days.
How does it work? Just a cycle with standard sleep periods within that function. *
- By default, this function sleeps in periods of 128 milliseconds. The waking time between periods of sleep is about 2.2 μs (at 16 MHz).
This is 0.0017% of sleep time. Accordingly, the accuracy of sleep time is a multiple of one period of sleep. This period can be adjusted to
`power.setSleepResolution()`which assumes the same constants as`sleep()`. If you need a more accurate sleep, you can put 16 ms.`SLEEP_16MS`), 
the maximum energy saving is 8 seconds (`SLEEP_8192MS`).
- For premature awakening by interruption, it is necessary to call`power.wakeUp()`inside the interrupt handler.
- Son`sleepDelay()`It has two very useful possibilities:
  - Sleep for a very precise period with a calibrated timer (see below)
  - Saving time`millis()`during sleep (see example of sleeptime)

### Timer calibration
In version 2.0 of the library, calibration was simplified: just call`power.autoCalibrate()`When you start the microcontroller. The function is performed ~16 ms.
**Warning! power.setSleepResolution() must be called after the timer is calibrated.**

<a id="example"></a>

## Example
For more examples see **examples**!
```cpp
// Demo library capabilities
#include <GyverPower.h>

void setup() {
  pinMode(13, OUTPUT); // Configure the output with LED to the exit
  Serial.begin(9600);

  power.autoCalibrate(); // calibration

  // shutdown
  power.hardwareDisable(PWR_ADC | PWR_TIMER1); // see the constant section in GyverPower.h separating the sign "|"

  // frequency control
  power.setSystemPrescaler(PRESCALER_2); // See constants in GyverPower. h h
  
  // sleep setting
  power.setSleepMode(STANDBY_SLEEP); // If you need a different sleep mode, see constants in GyverPower.h (POWERDOWN SLEEP by default)
  //power.bodInSleep(false) It is recommended to turn off the bod in your sleep to save energy (by default false - already off!!)

  // single-sleeping
  Serial.println("go to sleep");
  delay(100); // give time to ship
  
  power.sleep(SLEEP_2048MS); // sleep ~ 2 seconds
  
  Serial.println("wake up!");
  delay(100); // give time to ship
}

void loop() {
  // cyclic sleep
  power.sleepDelay(1500);               // sleep 1.5 seconds
  digitalWrite(13, !digitalRead(13));   // invert the state on the pin
}
```

<a id="install"></a>

## Installation
- The library can be found under the name **GyverPower** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/GyverPower/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="versions"></a>

## Versions
- v1.2 - calibration fix
- v1.3 - fix for 32U4
- v1.4 Adds adjustInternalClock
- v1.5 - compatibility with attini
- v1.6 - still compatible with Attini
- v1.7 - Optimization, compatibility with ATtiny13
- v1.8 - Compatibility with ATmega32U4
- v2.0 - Memory optimization, redesigned sleepDelay, you can accurately know the actual sleep time
- v2.0.1 - fix compiler warnings
- v2.0.2 - ATtiny85 compilation error fixed
- v2.1 - added bool inSleep(), to check if the MK sleeps
- v2.2 - improved stability

<a id="feedback"></a>
## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!
