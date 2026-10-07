#include <IRremote.h>
#include <Servo.h>

int trigPin = 2;
int echoPin = 7;

IRrecv irrecv(10);
decode_results results;

Servo neck;

int ir1 = 3;
int ir3 = A4;
int ir2 = 4;

int leftEN = 5;
int leftIN1 = 8;
int leftIN2 = 9;

int rightEN = 6;
int rightIN3 = 13;
int rightIN4 = 12;

// setup robot
float maxSpeed = 150;   // top speed (0 - 255)
float leftOffset = 15;  // slow the left wheel by this much
float rightOffset = 0;  // slow the right wheel by this much
float slowSpeed = 20;   // how much to slow down when turning

// tune here
float softTurn = 1.5;   // small turn = slowSpeed x this
float hardTurn = 2.5;   // big turn = slowSpeed x this
int searchSpeed = -35;  // spin speed when the line is lost

int lastSide = 0;

void setup() {
  Serial.begin(9600);

  irrecv.enableIRIn();
  neck.attach(11);

  pinMode(ir1, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir2, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(leftEN, OUTPUT);
  pinMode(leftIN1, OUTPUT);
  pinMode(leftIN2, OUTPUT);

  pinMode(rightEN, OUTPUT);
  pinMode(rightIN3, OUTPUT);
  pinMode(rightIN4, OUTPUT);
}

void loop() {
  int left = digitalRead(ir1);
  int center = digitalRead(ir2);
  int right = digitalRead(ir3);

  float turn = slowSpeed * (center ? softTurn : hardTurn);

  if (left && right) {
    motorLeft(maxSpeed - leftOffset);
    motorRight(maxSpeed - rightOffset);
  } else if (right) {
    motorLeft(maxSpeed - leftOffset);
    motorRight(maxSpeed - turn - rightOffset);
    lastSide = 1;
  } else if (left) {
    motorLeft(maxSpeed - turn - leftOffset);
    motorRight(maxSpeed - rightOffset);
    lastSide = -1;
  } else if (center) {
    motorLeft(maxSpeed - leftOffset);
    motorRight(maxSpeed - rightOffset);
  } else if (lastSide == -1) {
    Serial.println("turn left");
    motorLeft(searchSpeed);
    motorRight(0);
  } else if (lastSide == 1) {
    Serial.println("turn right");
    motorLeft(0);
    motorRight(searchSpeed);
  } else {
    Serial.println("off");
    motorLeft(0);
    motorRight(0);
  }
}

void motor(int en, int in1, int in2, int speed) {
  digitalWrite(in1, speed > 0);
  digitalWrite(in2, speed < 0);
  analogWrite(en, abs(speed));
}

void motorLeft(int speed) {
  motor(leftEN, leftIN1, leftIN2, speed);
}

void motorRight(int speed) {
  motor(rightEN, rightIN3, rightIN4, speed);
}

int readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH, 30000) / 58;
}

unsigned long readRemote() {
  unsigned long code = 0;
  if (irrecv.decode(&results)) {
    code = results.value;
    irrecv.resume();
  }
  return code;
}

void printAll() {
  Serial.print("  ir1: ");
  Serial.print(digitalRead(ir1));
  Serial.print("  ir2: ");
  Serial.print(digitalRead(ir2));
  Serial.print("  ir3: ");
  Serial.print(digitalRead(ir3));
  Serial.print("  Distance: ");
  Serial.print(readDistance());
  Serial.print(" cm  Remote: ");
  Serial.println(readRemote(), HEX);
}
