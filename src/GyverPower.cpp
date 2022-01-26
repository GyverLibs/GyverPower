#include <GyverPower.h>

#ifdef MILLIS_CORRECT_IS_SUPPURT
extern volatile unsigned long timer0_millis;
#endif
static volatile bool _wdtFlag = false;

// ===================== ЖЕЛЕЗО =====================
void GyverPower::hardwareEnable(uint16_t data) {
#if defined(PRR1)
    PRR1 &= ~ highByte(data); 		// загрузили данные в регистр
    PRR0 &= ~ lowByte(data);
#else
    PRR &= ~ lowByte(data);
#endif
    if (data & PWR_ADC) { 			// если что то сказали про ацп
        ADCSRA |= (1 << ADEN); 		// вкл ацп
        ACSR &= ~ (1 << ACD); 		// вкл компаратор
    }
}

void GyverPower::hardwareDisable(uint16_t data) {
    if (data & PWR_ADC) {           // если что то сказали про ацп
        ADCSRA &= ~ (1 << ADEN);    // выкл ацп
        ACSR |= (1 << ACD);         // выкл компаратор
    }
#if defined(PRR1)
    PRR1 |= highByte(data);         // загрузили данные в регистр
    PRR0 |= lowByte(data);
#else
    PRR |= lowByte(data);
#endif
}

// ===================== КЛОК =====================
void GyverPower::setSystemPrescaler(prescalers_t prescaler) {
    CLKPR = (1 << CLKPCE); 			// разрешили менять делитель
    CLKPR = prescaler; 				// загрузили новый
}

void adjustInternalClock(int8_t adj) {
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
    bodEnable = state;
}

void GyverPower::setSleepMode(sleepmodes_t mode) {
    sleepMode = mode;
}

void GyverPower::correctMillis(bool state) {
    correct = state;
}

// ===================== КАЛИБРОВКА =====================
void GyverPower::autoCalibrate() {
    _wdtFlag = false;
    uint32_t us = micros();
    _wdt_start(WDTO_15MS);
    while (!_wdtFlag);
    wdt_disable();
    us16 = micros() - us;
}

uint16_t GyverPower::getMaxTimeout(void) {return 8000;}
void GyverPower::calibrate(uint16_t ms) { autoCalibrate(); }

// ===================== СОН =====================
void GyverPower::sleep(uint8_t prd) {
    prepareSleep();
    goSleep(prd);
    finishSleep();
}

void GyverPower::setSleepResolution(uint8_t prd) {
    uint32_t stepUs = (uint32_t)us16 * (1 << prd);
    step = stepUs / 1000ul;
    fstep = (stepUs % 1000ul) >> 3;
    slpPrd = prd;
}

uint8_t GyverPower::sleepDelay(uint32_t ms) {
#ifdef MILLIS_CORRECT_IS_SUPPURT
    uint32_t saveMs = ms;
#endif
    wakeFlag = false;
    fcount = 0;
    prepareSleep();
    while (ms > step) {
        goSleep(slpPrd);
        ms -= step;
        fcount += fstep;
        if (fcount >= (1000 >> 3)) {
            ms--;
            fcount -= (1000 >> 3);
        }
        if (wakeFlag) {
#ifdef MILLIS_CORRECT_IS_SUPPURT
            if (correct) timer0_millis += saveMs - ms;
#endif	
            finishSleep();
            return ms;
        }
    }
#ifdef MILLIS_CORRECT_IS_SUPPURT
    if (correct) timer0_millis += saveMs - ms;
#endif
    finishSleep();
    return ms;      // вернуть остаток времени
}

void GyverPower::wakeUp(void) {
    wakeFlag = true;
}

bool GyverPower::inSleep(void) {
    return sleepF;
}

// ===================== PRIVATE =====================
void GyverPower::prepareSleep() {
    sleepF = 1;
    // принудительно выкл АЦП и компаратор
    if (sleepMode != IDLE_SLEEP && sleepMode != ADC_SLEEP) {
        ADCSRA &= ~ (1 << ADEN);    // Выкл ацп
        ACSR |= (1 << ACD);         // Выкл аналог компаратор
    }

    // Принудительное отключение PLL
#if defined(__AVR_ATtiny85__)
    pllCopy = PLLCSR;       // Запомнили настройки
    PLLCSR &= ~ (1 << PLLE);        // Выключили
#endif
}

void GyverPower::goSleep(uint8_t period) {
    if (period < 10) _wdt_start(period);    // Если не sleep forever
    set_sleep_mode(sleepMode);      // Настраиваем нужный режим сна
    sleep_enable(); 				// Разрешаем сон
    if (bodEnable) interrupts();    // для БОД, если он включен
    else {
#if defined(sleep_bod_disable)
        noInterrupts();				// Запрет прерываний
        sleep_bod_disable();		// Выключаем BOD
        interrupts();				// Разрешаем прерывания
#endif
    }
    sleep_cpu(); 				    //  <<< точка ухода в сон
    wdt_disable();                  // выкл пса
    wdt_reset();                    // сброс пса
    sleep_disable();				// Запрещаем сон
}

void GyverPower::finishSleep() {
    // Восстановление настроек АЦП
#if defined(PRR0)
    if (!(PRR0 & (1 << PRADC))) {   // если ацп не выключен принудительно
        ADCSRA |= (1 << ADEN);      // вкл после сна
        ACSR &= ~ (1 << ACD);
    }
#else
    if (!(PRR & (1 << PRADC))) {    // если ацп не выключен принудительно
        ADCSRA |= (1 << ADEN);      // вкл после сна
        ACSR &= ~ (1 << ACD);
    }
#endif

    // Восстановление настроек PLL (для тини85)
#if defined(__AVR_ATtiny85__)
    PLLCSR = pllCopy;
#endif
    sleepF = 0;
}

void GyverPower::_wdt_start(uint8_t timeout) {
    wdt_enable(timeout);            // Таймаут ~ 8 c
    WDTCSR |= 1 << WDIE;            // Режим ISR+RST	 		
    wdt_reset();                    // Сброс WDT
}

ISR(WDT_vect) {                     // просыпаемся тут
    _wdtFlag = true;                // для калибровки
}

GyverPower power = GyverPower();