void setup() {
  // put your setup code here, to run once:
  for (int i = 4; i <= 11; i++) pinMode(i, OUTPUT);

  for (int i = 4; i <= 11; i++) {
    digitalWrite(i, HIGH);
    delay(200);
  }
  for (int i = 4; i <= 11; i++) {
    digitalWrite(i, LOW);
    delay(200);
  }
}

void loop() {
  // put your main code here, to run repeatedly:


}
