#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>
#include <SoftwareSerial.h>
MPU6050 mpu6050;
Servo myServo;
SoftwareSerial bluetoothSerial(2, 3); // RX, TX
const int LOOP_DELAY = 10;
const int PITCH_FALL_ANGLE_TRIGGER = 15;
const int ROLL_FALL_ANGLE_TRIGGER = 15;
const int PITCH_FALLEN_ANGLE = 60;
const int ROLL_FALLEN_ANGLE = 60;
const int MAX_FALL_COUNT = 5;
const int MAX_LONG_FALL_DURATION = 3000;
const int TIMER_INTERVAL = 100;
const int SERVO_PIN = 9;
int timer = 0;
int pitch = 0;
int roll = 0;
int prevPitch = 0;
int prevRoll = 0;
int fallCount = 0;
int longFallCount = 0;
bool fallReported = false;
unsigned long fallStartTime = 0;
void getPitchAndRoll() {
int16_t accX, accY, accZ, gyroX, gyroY, gyroZ;
mpu6050.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);
pitch = -(atan2(accX, sqrt(accY * accY + accZ * accZ)) * 180.0) / M_PI;
roll = -(atan2(-accY, accZ) * 180.0) / M_PI;
}
void setup() {
Serial.begin(9600);
bluetoothSerial.begin(9600); // Bluetooth serial başlatılıyor
Wire.begin();
mpu6050.initialize();
myServo.attach(SERVO_PIN);
fallCount = 0;
longFallCount = 0;
fallReported = false;
if (!mpu6050.testConnection()) {
Serial.println("Could not connect to MPU6050, check wiring!");
while (1);
}
}
void loop() {
unsigned long currentTime = millis();
getPitchAndRoll();
String output=("Pitch = "+ String(pitch)+ " Roll = " + String(roll));
Serial.println(output);
sendBluetoothData(output);
int pitchDelta = abs(pitch - prevPitch);
int rollDelta = abs(roll - prevRoll);
if (pitchDelta > PITCH_FALL_ANGLE_TRIGGER || rollDelta >
ROLL_FALL_ANGLE_TRIGGER) {
fallCount++;
if (fallCount >= MAX_FALL_COUNT && pitch<0) {
if (!fallReported) {
fallReported = true;
fallStartTime = currentTime;
}
}
} else {
fallCount = 0;
fallReported = false;
}
if (fallReported) {
if (currentTime - fallStartTime >= MAX_LONG_FALL_DURATION) {
sendBluetoothData("Uzun Süreli Düşme Algılandı!");
Serial.println("Uzun Süreli Düşme Algılandı!");
rotateServoForDuration(180, 1000);
exit(0);
}
}
if (currentTime % TIMER_INTERVAL == 0) {
prevPitch = pitch;
prevRoll = roll;
}
delay(LOOP_DELAY);
}
void rotateServoForDuration(int angle, unsigned long duration) {
unsigned long startTime = millis();
while (millis() - startTime <= duration) {
myServo.write(angle);
delay(20);
}
}
void sendBluetoothData(String data) {
bluetoothSerial.println(data);
}
