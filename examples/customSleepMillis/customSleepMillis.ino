// Делаем свой счётчик времени сна
// допустим таймер 0 у нас используется для других целей, но нам
// нужно хотя бы приблизительно считать время и выполнять по нему действия

#include <GyverPower.h>

uint32_t myMillis = 0;  // наш миллис  //our millis //设置myMillis初始化值

void setup() {
  power.autoCalibrate(); // автоматическая калибровка  //automatic calibration  //自动校准timer值
}

void loop() {
  static uint32_t tmr1, tmr2;

  // "таймер" на 2 секунды  //"timer" for 2 seconds  //设置“timer” 的值为2s
  if (myMillis - tmr1 > 2000) {
    tmr1 += 2000;
    // действие 1 action 1 动作1
  }

  // "таймер" на 10 секунд //"timer" for 10 seconds  //设置“timer” 的值为10s
  if (myMillis - tmr2 > 10000) {
    tmr2 += 10000;
    // действие 2  //action 2 //动作2
  }

  // спим 1 секунду (1000 мс) 
  //sleep 1 second (1000ms) 
  //睡眠1s（1000ms）
  // sleepDelay возвращает остаток сна в мс, если он есть 
  //sleepDelay returns the rest of the sleep in ms, if any 
  //sleepDelay 返回剩余的睡眠时间（ms），如果有的话
  byte left = power.sleepDelay(1000);

  // прибавляем и корректируем наш миллис//adding and adjusting our millis
  // adding and adjusting our millis
  //增加和调整 myMillis
  myMillis += 1000 - left;
}
