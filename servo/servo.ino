#include <Servo.h>
Servo servo;
int pos1 = 0;

void setup() {
  servo.attach(8);
  servo.write(360);

}

void loop() {
  servo.write(90);
  delay(1000);
  servo.write(180);
  delay(1000);
  servo.write(0);
  delay(5000);
}
