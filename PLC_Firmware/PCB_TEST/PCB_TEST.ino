void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(12,LOW);
  digitalWrite(7,LOW);
  digitalWrite(1,HIGH);
  digitalWrite(0,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  digitalWrite(10,!(digitalRead(10)));
  digitalWrite(1, !(digitalRead(1)));
  digitalWrite(0, !(digitalRead(0)));
  delay(1000);
  digitalWrite(11,!(digitalRead(11)));
  digitalWrite(1, !(digitalRead(1)));
  digitalWrite(0, !(digitalRead(0)));
  delay(1000);
  digitalWrite(4,!(digitalRead(4)));
  digitalWrite(1, !(digitalRead(1)));
  digitalWrite(0, !(digitalRead(0)));
  delay(1000);
  digitalWrite(5,!(digitalRead(5)));
  digitalWrite(1, !(digitalRead(1)));
  digitalWrite(0, !(digitalRead(0)));
  delay(1000);
  digitalWrite(6,!(digitalRead(6)));
  digitalWrite(1, !(digitalRead(1)));
  digitalWrite(0, !(digitalRead(0)));
  delay(1000);
  digitalWrite(12,!(digitalRead(12)));
  digitalWrite(1, !(digitalRead(1)));
  digitalWrite(0, !(digitalRead(0)));
  delay(1000);
  digitalWrite(7,!(digitalRead(7)));
  digitalWrite(1, !(digitalRead(1)));
  digitalWrite(0, !(digitalRead(0)));
}
