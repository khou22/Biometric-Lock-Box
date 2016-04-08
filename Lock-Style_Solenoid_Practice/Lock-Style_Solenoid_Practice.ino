// References:
// https://www.adafruit.com/product/1512
// http://sensitiveresearch.com/elec/DoNotTIP/TIP120-motor.png
// https://learn.adafruit.com/secret-knock-activated-drawer-lock/wiring
// https://makenoisemakemusic.wordpress.com/tag/diy/

const int solenoidPin = 13;
int delayTime = 5000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(solenoidPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  unlockSolenoid();
  delay(delayTime);
}

void unlockSolenoid() {
  digitalWrite(solenoidPin, HIGH);
  Serial.println("Unlocking");
  delay(delayTime);
  digitalWrite(solenoidPin, LOW);
  Serial.println("Locking");
}

