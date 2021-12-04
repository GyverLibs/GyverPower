// измеряем время, проведённое во сне
// кнопка на D3 и GND
#include <GyverPower.h>

// здесь просыпаемся
void wake() {
  power.wakeUp();
}

void setup() {
  Serial.begin(9600);
  power.autoCalibrate();  // калибруем таймер сна
  // можно установить разрешение
  //power.setSleepResolution(SLEEP_1024MS);

  pinMode(3, INPUT_PULLUP);
  // будильник по кнопке  
  attachInterrupt(1, wake, FALLING);
}

uint32_t tmr;
void loop() {
  tmr = millis();
  power.sleepDelay(50000);

  // выводим время сна
  Serial.println(millis() - tmr);
  delay(300);
}
