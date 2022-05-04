# GyverPower
GyverPower - library for power management of MK AVR
- System clock management
- Enable/disable peripherals:
    -BOD
    - Timers
    - I2C/UART/SPI
    - USB
    -ADC
- Sleep in different modes (list below)
- Sleep for any period
    - Timer calibration for accurate sleep time
    - millis() adjustment

### Compatibility
- Atmega2560/32u4/328
- Attiny85/84/167

### Documentation
The library has [extended documentation](https://alexgyver.ru/GyverPower/)

## Content
- [Install](#install)
- [Initialization](#init)
- [Usage](#usage)
- [Example](#example)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="install"></a>
## Installation
- The library can be found by the name **GyverPower** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download Library](https://github.com/GyverLibs/GyverPower/archive/refs/heads/main.zip) .zip archive for manual installation:
    - Unzip and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unzip and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/*
    - (Arduino IDE) automatic installation from .zip: *Sketch/Include library/Add .ZIP library…* and specify the downloaded archive
- Read more detailed instructions for installing librariesek [here] BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Initialization
Not

<a id="usage"></a>
## Usage
```cpp
void hardwareEnable(uint16_t data); // enable the specified peripheral (see "Peripheral Constants" below)
void hardwareDisable(uint16_tdata); // turn off the specified peripheral (see "Peripheral Constants" below)
void setSystemPrescaler(prescalers_t prescaler);// set the system frequency divider (see "Divisor Constants" below)
void adjustInternalClock(int8_tadj); // adjusting the frequency of the internal generator (number -120...+120)

void bodInSleep(bool en); // Brown-out detector in sleep mode (true on - false off) by default disabled!
void setSleepMode(sleepmodes_t mode); // set the current sleep mode (see "Sleep Modes" below)
void setSleepResolution(uint8_tperiod); // set sleep permission (see "Sleep periods" below)

void autoCalibrate(void); // automatic calibration of the sleep timer, runs 16ms
void sleep(uint8_t period); // sleep for a fixed period (see "Sleep periods" below)
uint8_t sleepDelay(uint32_t ms); // sleep for an arbitrary period in milliseconds (up to 52 days), returns the rest of the time to adjust the timers
void correctMillis(bool state); // adjust millis for sleep time sleepDelay() (enabled by default)
void wakeUp(void); // helps to exit sleepDelay with an interrupt (call in a wakeup interrupt)
bool inSleep(void); // will return true if the MCU is sleeping (for checking in an interrupt)
```

```cpp
===== SLEEP MODES for setSleepMode() =====
IDLE_SLEEP - Light sleep, turns off only the CPU and Flash clock, wakes up instantly from any interrupts
ADC_SLEEP - Light sleep, turns off CPU and system clock, ADCstarts transformation when going to sleep (see ADCinSleep example)
EXTSTANDBY_SLEEP - Deep sleep, identical to POWERSAVE_SLEEP + system clock active
STANDBY_SLEEP - Deep sleep, identical to POWERDOWN_SLEEP + system clock active
POWERSAVE_SLEEP - Deep sleep, identical to POWERDOWN_SLEEP + timer 2 active (+ can be woken up by its interrupts), can be used to count time (see powersaveMillis example)
POWERDOWN_SLEEP - The deepest sleep, everything is turned off except WDT and external interrupts, wakes up from hardware (regular + PCINT) or WDT

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
SLEEP_FOREVER - eternal sleep

===== DIVISION CONSTANTS for setSystemPrescaler() =====
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
PWR_ALL - all hardware
PWR_ADC - ADC and comparator
PWR_TIMER1 - Timer 0
PWR_TIMER0 - Timer 1
PWR_TIMER2 - Timer 2
PWR_TIMER3 - Timer 3
PWR_TIMER4 - Timer 4
PWR_TIMER5 - Timer 5
PWR_UART0 - Serial 0
PWR_UART1 - Serial 1
PWR_UART2 - Serial 2
PWR_UART3 - Serial 3
PWR_I2C - Wire
PWR_SPI - SPI
PWR_USB - USB
PWR_USI - Wire + Spi (ATtinyXX)
PWR_LIN - USART LIN (ATtinyXX)
```

### A simple dream
- The sleep mode is configured in `power.setSleepMode()`, `POWERDOWN_SLEEP` is active by default (see the rest above).
- To sleep - call `power.sleep()` specifying one of the standard periods (see above).
- The actual sleep time will be slightly different as the "sleep timer" is not very accurate.

### Sleep for any period
- The sleep mode is configured in `power.setSleepMode()`, `POWERDOWN_SLEEP` is active by default (see the rest above).
- To sleep - call `power.sleepDelay()` specifying the period in milliseconds (`uint32_t`, up to ~50 days).
*How it works? Just loop withabout standard sleep periods within this feature.*
- By default, this function "sleeps" in periods of 128 milliseconds. Wake time between sleep periods is about 2.2 µs (at 16 MHz),
which is 0.0017% of sleep time. Accordingly, the accuracy of sleep time is a multiple of one sleep period. This period can be set to
`power.setSleepResolution()` which takes the same constants as `sleep()`. If you need more accurate sleep - you can set 16 ms (`SLEEP_16MS`),
if the maximum power saving is 8 seconds (`SLEEP_8192MS`).
- It is mandatory to call `power.wakeUp()` inside the interrupt handler to wake up prematurely on an interrupt.
- Sleep `sleepDelay()` has two very useful features:
  - Sleep for a very precise period with a calibrated timer (see below)
  - Save time count `millis()` for sleep time (see sleeptime example)

### Timer calibration
In version 2.0 of the library, calibration has been simplified: just call `power.autoCalibrate()` when starting the microcontroller. The function is executed ~16 ms.
**Attention! power.setSleepResolution() must be called after the timer is calibrated.**

<a id="example"></a>
## Example
See **examples** for other examples!
```cpp
// demo of library features
#include <GyverPower.h>

void setup() {
  pinMode(13, OUTPUT); // set pin with LED to output
  Serial.begin(9600);

  power.autoCalibrate(); // automatic calibration

  // disable unnecessary peripherals
  power.hardwareDisable(PWR_ADC | PWR_TIMER1); // see constant section in GyverPower.h delimited by " | "

  // system frequency control
  power.setSystemPrescaler(PRESCALER_2); // see constants in GyverPower.h
  
  // setting sleep parameters
  power.setSleepMode(STANDBY_SLEEP); // if you need a different sleep mode, see constants in GyverPower.h (default POWERDOWN_SLEEP)
  //power.bodInSleep(false); // it is recommended to turn off bod in sleep to save power (default is false - already off!!)

  // single y examplesleep cranberry
  Serial.println("go to sleep");
  delay(100); // give time to send
  
  power.sleep(SLEEP_2048MS); // sleep ~ 2 seconds
  
  Serial.println("wake up!");
  delay(100); // give time to send
}

void loop() {
  // example of a cyclic sleep
  sleepDelay(1500); // sleep 1.5 seconds
  digitalWrite(13, !digitalRead(13)); // invert the state on the pin
}
```

<a id="versions"></a>
## Versions
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
When you find bugs, create an **Issue**, or better, immediately write to the mail [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your **Pull Request**'s!