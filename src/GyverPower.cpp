#include <GyverPower.h>

#ifdef MILLIS_CORRECT_IS_SUPPURT
extern volatile unsigned long timer0_millis;
#endif
static volatile bool _wdtFlag = false;

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
    if (data & PWR_ADC) { 				// если что то сказали про ацп
        ADCSRA &= ~ (1 << ADEN); 		// выкл ацп
        ACSR |= (1 << ACD); 			// выкл компаратор
    }
#if defined(PRR1)
    PRR1 |= highByte(data); 			// загрузили данные в регистр
    PRR0 |= lowByte(data);
#else
    PRR |= lowByte(data);
#endif
}

void GyverPower::setSystemPrescaler(prescalers_t prescaler) {
    CLKPR = (1 << CLKPCE); 			// разрешили менять делитель
    CLKPR = prescaler; 				// загрузили новый
}

void GyverPower::bodInSleep(bool state) {
    bodEnable = state;
}

void GyverPower::setSleepMode(sleepmodes_t mode) {
    sleepMode = mode;
}

uint16_t GyverPower::getMaxTimeout(void){
	_wdt_start(WDTO_8S);					// таймаут ~ 8 c
    _wdtFlag = false;
    uint16_t startTime = millis(); 			// засекли время	
    while (!_wdtFlag); 						// ждем таймаута	
    uint16_t ms = millis() - startTime;
    wdt_disable(); 							// выкл wdt 
    return ms;
}

void GyverPower::autoCalibrate(void) {
	_wdt_start(WDTO_2S);					// Таймаут ~ 2 c
    _wdtFlag = false;
    uint16_t startTime = millis(); 			// Засекли время
    while (!_wdtFlag); 						// Ждем таймаута
    uint16_t ms = millis() - startTime;
    wdt_disable();							// Выкл wdt 
    for (uint8_t i = 0; i < 9 ; i++) { 		// Пересчитываем массив
        timeOuts[9 - i] = ((ms * 4) >> i);  
    }
}

void GyverPower::calibrate(uint16_t ms) { 	// Пересчет массива таймаутов
    for (uint8_t i = 0; i < 9 ; i++) { 		// Пересчитываем массив
        timeOuts[9 - i] = (ms >> i);
    }
}

void GyverPower::sleep(uint8_t period) {

    /* принудительно выкл АЦП и компаратор */
    if (sleepMode != IDLE_SLEEP && sleepMode != ADC_SLEEP) {
        ADCSRA &= ~ (1 << ADEN); 			// Выкл ацп
        ACSR |= (1 << ACD); 				// Выкл аналог компаратор
    }

    /* Принудительное отключение PLL */
#if defined(__AVR_ATtiny85__)
    uint8_t pllCopy = PLLCSR; 			// Запомнили настройки
    PLLCSR &= ~ (1 << PLLE); 			// Выключили
#endif

    /* Если спим с WDT - настраиваем его */
    if (period < 10) {					// Если не бесконечный таймаут
       _wdt_start(period);				// Настраиваем WDT
    }

    /* Настраиваем нужный режим сна */
    set_sleep_mode(sleepMode);

    /* Процедура ухода в сон */
	sleep_enable(); 				// Разрешаем сон
    if (bodEnable){
		interrupts();				// Разрешаем прерывания
		sleep_cpu (); 	            //  <<< точка ухода в сон (с БОД)
    } else {
#if defined(sleep_bod_disable)
		noInterrupts();				// Запрет прерываний
        sleep_bod_disable();		// Выключаем BOD
		interrupts();				// Разрешаем прерывания
#endif
        sleep_cpu (); 				//  <<< точка ухода в сон (без БОД)
    }

    /* Процедура выхода из сна*/
    sleep_disable();				// Запрещаем сон

    /* Восстановление настроек АЦП */
#if defined(PRR0)
    if (!(PRR0 & (1 << PRADC))) { 		// если ацп не выключен принудительно
        ADCSRA |= (1 << ADEN); 			// вкл после сна
        ACSR &= ~ (1 << ACD);
    }
#else
    if (!(PRR & (1 << PRADC))) {		// если ацп не выключен принудительно
        ADCSRA |= (1 << ADEN);			// вкл после сна
        ACSR &= ~ (1 << ACD);
    }
#endif

    /* Восстановление настроек PLL (для тини85) */
#if defined(__AVR_ATtiny85__)
    PLLCSR = pllCopy;
#endif

}

uint8_t GyverPower::sleepDelay(uint32_t ms) {
    uint32_t saveMs = ms;
    wakeFlag = false;
    for (uint8_t i = 9; ms > timeOuts[0]; i--) { 	// начиная с длинного периода , пока мс > ~16
        while (ms > timeOuts[i]) {					// пока мс > текущего выбранного периода
            sleep(i);								// уйти в сон
            ms -= timeOuts[i];						// отнять время сна
            if (wakeFlag) {
#ifdef MILLIS_CORRECT_IS_SUPPURT
                if (correct) timer0_millis += saveMs - ms;
#endif	
                return ms;
            }
        }
    }
#ifdef MILLIS_CORRECT_IS_SUPPURT
    if (correct) timer0_millis += saveMs - ms;
#endif	
    return ms; 										// вернуть остаток времени
}

void GyverPower::correctMillis(bool state) {
    correct = state;
}

void GyverPower::wakeUp(void) {
    wakeFlag = true;
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

void _wdt_start(uint8_t timeout){
	wdt_enable(timeout);					// Таймаут ~ 8 c
	WDTCSR |= 1 << WDIE;					// Режим ISR+RST	 		
    wdt_reset();							// Сброс WDT
}

ISR(WDT_vect) {							// просыпаемся тут
    _wdtFlag = true;					// для калибровки
    wdt_disable();						// Выкл пса
	wdt_reset();
}

GyverPower power = GyverPower();