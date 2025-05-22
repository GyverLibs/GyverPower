#ifndef _GyverPower_h
#define _GyverPower_h
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "powerConstants.h"

// =============== ФУНКЦИИ ===============
class GyverPower {
   public:
    void hardwareEnable(uint16_t data);               // включение указанной периферии (см. ниже "Константы периферии")
    void hardwareDisable(uint16_t data);              // выключение указанной периферии (см. ниже "Константы периферии")
    void setSystemPrescaler(prescalers_t prescaler);  // установка делителя системной частоты
    void adjustInternalClock(int8_t adj);             // подстройка частоты внутреннего генератора (число -120...+120)
    void bodInSleep(bool en);                         // Brown-out detector в режиме сна (true вкл - false выкл) [умолч. false]
    void setSleepMode(sleepmodes_t mode);             // установка текущего режима сна [умолч. POWERDOWN_SLEEP]
    void sleep(sleepprds_t period);                   // сон на стандартный период
    bool inSleep();                                   // вернёт true, если МК спит для проверки в прерывании
    
    uint16_t sleepDelay(uint32_t ms);                 // сон на произвольный период в миллисекундах, возвращает остаток времени для коррекции таймеров
    uint16_t sleepDelay(uint32_t ms, uint32_t sec, uint16_t min = 0, uint16_t hour = 0, uint16_t day = 0);
    void setSleepResolution(sleepprds_t period);      // установить разрешение сна sleepDelay() [умолч. SLEEP_128MS]
    void correctMillis(bool state);                   // корректировать миллис на время сна sleepDelay() [умолч. true]
    void calibrate();                                 // автоматическая калибровка таймера сна sleepDelay(), выполняется 16 мс
    void wakeUp();                                    // помогает выйти из sleepDelay() прерыванием (вызывать в будящем прерывании)

    // устарело
    void autoCalibrate();         // автоматическая калибровка таймера сна, выполняется 16 мс
    void calibrate(uint16_t ms);  // ручная калибровка тайм-аутов watchdog для sleepDelay (ввести макс период из getMaxTimeout)
    uint16_t getMaxTimeout();     // возвращает реальный период "8 секунд", выполняется ~8 секунд

   private:
    void _prepare();
    void _sleep(sleepprds_t period);
    void _finish();
    void _wdt_start(uint8_t timeout);

    uint16_t _step = 1 << (SLEEP_128MS + 4);
    uint16_t _us16 = 16000;
    uint8_t _fstep = 0;
    sleepprds_t _delayPrd = SLEEP_128MS;
    sleepmodes_t _sleepMode = POWERDOWN_SLEEP;

    volatile bool _wakeF = false;
    volatile bool _sleepF = 0;
    bool _correctF = true;
    bool _bodEnable = false;

#if defined(__AVR_ATtiny85__)
    uint8_t _pllCopy = 0;
#endif
};

extern GyverPower power;

// =============== КОНСТАНТЫ ===============
/*
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
*/
#endif