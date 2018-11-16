void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(A13, INPUT);
  Serial.println("trying");
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // i was getting 1640
  float voltage = analogRead(A13);
  //voltage *= 2;
//  voltage *= 3.3;
//  voltage /= 1100;
  Serial.println(voltage);
  delay(1000*2);
}
