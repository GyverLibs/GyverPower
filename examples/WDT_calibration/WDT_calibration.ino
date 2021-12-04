#include <GyverPower.h>

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);    // шоб мигать
  
  // WDT будит МК ото сна, его таймер не очень точный (с завода) и нуждается в калибровке
  power.autoCalibrate(); // автоматическая калибровка, выполняется ~16 миллисекунд
}

void loop() {
  // пример циклического сна
  power.sleepDelay(1500); // спим ровно 1.5 секунды (мы откалиброваны!)
  digitalWrite(13, !digitalRead(13)); // инвертируем состояние на пине
}