#include "Arduino.h"
#include "MPU6050.h"
#include "Wire.h"
#include "I2Cdev.h"
#include <Servo.h>
int16_t mpu6050Ax, mpu6050Ay, mpu6050Az;
int16_t mpu6050Gx, mpu6050Gy, mpu6050Gz;
MPU6050 mpu6050;
Servo myservo;
const int servoPin = 9;
const int16_t fallThreshold = -8000;
const int16_t riseThreshold = 8000;
bool isFalling = false;
bool isRising = false;
void setup() {
Serial.begin(9600);
while (!Serial) ; // Wait for serial port to connect
Serial.println("Start");
Wire.begin();
mpu6050.initialize();
myservo.attach(servoPin);
myservo.write(90);
delay(1500); // Wait for servo movement
}
void loop() {
readMPU6050Data();
detectFallAndRise();
delay(100); // Adjust delay as needed
void readMPU6050Data() {
mpu6050.getMotion6(&mpu6050Ax, &mpu6050Ay, &mpu6050Az, &mpu6050Gx,
&mpu6050Gy, &mpu6050Gz);
double mpu6050Temp = ((double)mpu6050.getTemperature() + 12412.0) / 340.0;
Serial.print("a/g-\t");
Serial.print(mpu6050Ax); Serial.print("\t");
Serial.print(mpu6050Ay); Serial.print("\t");
Serial.print(mpu6050Az); Serial.print("\t");
Serial.print(mpu6050Gx); Serial.print("\t");
Serial.print(mpu6050Gy); Serial.print("\t");
Serial.print(mpu6050Gz); Serial.print("\t");
Serial.print(F("Temp- "));
Serial.println(mpu6050Temp);
}
void detectFallAndRise() {
if (mpu6050Ay < fallThreshold && !isFalling) {
Serial.println("rise detected!");
isFalling = true;
isRising = false; // Reset rise detection
}
myservo.write(0);
delay(1500); // Wait for servo movement
}
if (mpu6050Ay > riseThreshold && !isRising) {
Serial.println("Fall detected!");
isRising = true;
isFalling = false;
myservo.write(90);
delay(1500); // Wait for servo movement
}
}