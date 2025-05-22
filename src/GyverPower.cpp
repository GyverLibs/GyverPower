#include <GyverPower.h>

#ifdef MILLIS_CORRECT_IS_SUPPURT
extern volatile unsigned long timer0_millis;
#endif
static volatile bool _wdtFlag = false;

// ===================== ЖЕЛЕЗО =====================
void GyverPower::hardwareEnable(uint16_t data) {
#if defined(PRR1)
    PRR1 &= ~highByte(data);  // загрузили данные в регистр
    PRR0 &= ~lowByte(data);
#else
    PRR &= ~lowByte(data);
#endif
    if (data & PWR_ADC) {       // если что то сказали про ацп
        ADCSRA |= (1 << ADEN);  // вкл ацп
        ACSR &= ~(1 << ACD);    // вкл компаратор
    }
}

void GyverPower::hardwareDisable(uint16_t data) {
    if (data & PWR_ADC) {        // если что то сказали про ацп
        ADCSRA &= ~(1 << ADEN);  // выкл ацп
        ACSR |= (1 << ACD);      // выкл компаратор
    }
#if defined(PRR1)
    PRR1 |= highByte(data);  // загрузили данные в регистр
    PRR0 |= lowByte(data);
#else
    PRR |= lowByte(data);
#endif
}

// ===================== КЛОК =====================
void GyverPower::setSystemPrescaler(prescalers_t prescaler) {
    CLKPR = (1 << CLKPCE);  // разрешили менять делитель
    CLKPR = prescaler;      // загрузили новый
}

void GyverPower::adjustInternalClock(int8_t adj) {
    static uint8_t currentCal;
    static bool startup = false;
    if (!startup) {
        startup = true;
        currentCal = OSCCAL;
    }
    uint8_t oldSreg = SREG;
    currentCal = constrain(currentCal + adj, 0, 255);
    cli();
    OSCCAL = currentCal;
    SREG = oldSreg;
}

// ===================== НАСТРОЙКИ СНА =====================
void GyverPower::bodInSleep(bool state) {
    _bodEnable = state;
}

void GyverPower::setSleepMode(sleepmodes_t mode) {
    _sleepMode = mode;
}

void GyverPower::correctMillis(bool state) {
    _correctF = state;
}

// ===================== КАЛИБРОВКА =====================
void GyverPower::calibrate() {
    _wdtFlag = false;
    uint32_t us = micros();
    _wdt_start(SLEEP_16MS);
    while (!_wdtFlag);
    wdt_disable();
    _us16 = micros() - us;
    setSleepResolution(_delayPrd);
}

// ===================== СОН =====================
void GyverPower::sleep(sleepprds_t prd) {
    _prepare();
    _sleep(prd);
    _finish();
}

void GyverPower::setSleepResolution(sleepprds_t prd) {
    uint32_t stepUs = (uint32_t)_us16 * (1 << prd);
    _step = stepUs / 1000ul;
    _fstep = (stepUs % 1000ul) >> 3;
    _delayPrd = prd;
}

uint16_t GyverPower::sleepDelay(uint32_t ms, uint32_t sec, uint16_t min, uint16_t hour, uint16_t day) {
    if (sec) ms += sec * 1000ul;
    if (min) ms += min * 60 * 1000ul;
    if (hour) ms += hour * 60 * 60 * 1000ul;
    if (day) ms += day * 24 * 60 * 60 * 1000ul;
    return sleepDelay(ms);
}

uint16_t GyverPower::sleepDelay(uint32_t ms) {
#ifdef MILLIS_CORRECT_IS_SUPPURT
    uint32_t saveMs = ms;
#endif
    uint8_t fcount = 0;
    _wakeF = false;
    _prepare();
    while (ms > _step) {
        _sleep(_delayPrd);
        ms -= _step;
        fcount += _fstep;
        if (fcount >= (1000 >> 3)) {
            --ms;
            fcount -= (1000 >> 3);
        }
        if (_wakeF) {
#ifdef MILLIS_CORRECT_IS_SUPPURT
            if (_correctF) {
                uint8_t oldSREG = SREG;
                cli();
                timer0_millis += saveMs - ms;
                SREG = oldSREG;
            }
#endif
            _finish();
            return ms;
        }
    }
#ifdef MILLIS_CORRECT_IS_SUPPURT
    if (_correctF) {
        uint8_t oldSREG = SREG;
        cli();
        timer0_millis += saveMs - ms;
        SREG = oldSREG;
    }
#endif
    _finish();
    return ms;  // вернуть остаток времени
}

void GyverPower::wakeUp() {
    _wakeF = true;
}

bool GyverPower::inSleep() {
    return _sleepF;
}

// ===================== PRIVATE =====================
void GyverPower::_prepare() {
    _sleepF = 1;
    // принудительно выкл АЦП и компаратор
    switch (_sleepMode) {
        case IDLE_SLEEP:
        case ADC_SLEEP:
            break;
        default:
            ADCSRA &= ~(1 << ADEN);  // Выкл ацп
            ACSR |= (1 << ACD);      // Выкл аналог компаратор
    }

#if defined(__AVR_ATtiny85__)
    // Принудительное отключение PLL
    _pllCopy = PLLCSR;       // Запомнили настройки
    PLLCSR &= ~(1 << PLLE);  // Выключили
#endif
}

void GyverPower::_sleep(sleepprds_t period) {
    if (period != SLEEP_FOREVER) _wdt_start(period);
    set_sleep_mode(_sleepMode);    // Настраиваем нужный режим сна
    sleep_enable();                // Разрешаем сон
    if (_bodEnable) interrupts();  // для БОД, если он включен
#if defined(sleep_bod_disable)
    else {
        noInterrupts();       // Запрет прерываний
        sleep_bod_disable();  // Выключаем BOD
        interrupts();         // Разрешаем прерывания
    }
#endif
    sleep_cpu();      //  <<< точка ухода в сон
    wdt_disable();    // выкл пса
    wdt_reset();      // сброс пса
    sleep_disable();  // Запрещаем сон
}

void GyverPower::_finish() {
    // Восстановление настроек АЦП
#if defined(PRR0)
    if (!(PRR0 & (1 << PRADC))) {  // если ацп не выключен принудительно
        ADCSRA |= (1 << ADEN);     // вкл после сна
        ACSR &= ~(1 << ACD);
    }
#else
    if (!(PRR & (1 << PRADC))) {  // если ацп не выключен принудительно
        ADCSRA |= (1 << ADEN);    // вкл после сна
        ACSR &= ~(1 << ACD);
    }
#endif

    // Восстановление настроек PLL (для тини85)
#if defined(__AVR_ATtiny85__)
    PLLCSR = _pllCopy;
#endif
    _sleepF = 0;
}

void GyverPower::_wdt_start(uint8_t timeout) {
    wdt_enable(timeout);
    WDTCSR |= 1 << WDIE;  // Режим ISR+RST
    wdt_reset();          // Сброс WDT
}

ISR(WDT_vect) {       // просыпаемся тут
    _wdtFlag = true;  // для калибровки
}

GyverPower power = GyverPower();

//////////////////////
uint16_t GyverPower::getMaxTimeout() { return 8000; }
void GyverPower::calibrate(uint16_t ms) { calibrate(); }
void GyverPower::autoCalibrate() { calibrate(); }