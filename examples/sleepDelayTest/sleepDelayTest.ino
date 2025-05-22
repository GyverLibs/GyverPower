#include <GyverPower.h>

void setup() {
    Serial.begin(115200);
    Serial.println("hello");
    delay(10);

    power.calibrate();
}

void loop() {
    power.sleepDelay(300000);

    delay(10);
    Serial.begin(115200);
    Serial.println("hello");
    delay(10);
}