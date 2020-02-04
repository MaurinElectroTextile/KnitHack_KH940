// Use this sketch to set up the THRESHOLD value in the main Arduino code
// #define THRESHOLD 400 // end lines sensors threshold

int endLineLeftAPin = 1;
int endLineRightAPin = 0;

void setup(){
  Serial.begin(38400);
}

void loop(){
  int valueEndLineLeft = analogRead(endLineLeftAPin);
  int valueEndLineRight = analogRead(endLineRightAPin);
  Serial.print(valueEndLineLeft);
  Serial.print("\t");
  Serial.println(valueEndLineRight);
}
