#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <MS5611.h>
#include <Servo.h>
MPU6050 mpu;
MS5611 barometer;
Servo servoMotor;
SoftwareSerial bluetoothSerial(2, 3); // RX, TX pins for SoftwareSerial
const int threshold = 18000; // Threshold for detecting rise or fall
const int stableThreshold = 14000; // Threshold for detecting stability
const int servoPin = 9; // Pin connected to servo motor
int fallCounter = 0; // Number of fall detection counter. It can be configured so that the parachute can be opened when a certain number of fall detection signals are obtained.
const int fallCounterThreshold = 1; // Minimum number of fall signal detection
void setup() {
Wire.begin();
Serial.begin(9600);
bluetoothSerial.begin(9600); // Initiate Bluetooth communication
servoMotor.attach(servoPin); // Attach servo motor
mpu.initialize();
barometer.begin(); // Initialize the MS5611 sensor
if (!barometer.begin()) {
Serial.println("Could not find a valid MS5611 sensor, check wiring!");
while (1); // Infinite loop
}
if (!mpu.testConnection()) {
Serial.println("Could not connect to MPU6050, check wiring!");
while(1); // Infinite loop
}
servoMotor.write(0); // Initialize the position of the servo as 0
}
void loop() {
barometer.read(); // Read sensor data (temperature and pressure)
float pressure = barometer.getPressure(); // Read Pressure
float temperature = barometer.getTemperature(); // Read Temperature
int16_t ax, ay, az;
int16_t gx, gy, gz;
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
// Calculate total acceleration
int16_t totalAcceleration = sqrt(pow(ax,2) + pow(ay,2) + pow(az,2));
// Convert raw acceleration data into g
float converted_ax = (float) ax / 16384.0;
float converted_ay = (float) ay / 16384.0;
float converted_az = (float) az / 16384.0;
// Convert raw gyroscope data into degrees per second
float converted_gx = (float) gx / 131.0;
float converted_gy = (float) gy / 131.0;
float converted_gz = (float) gz / 131.0;
if(totalAcceleration > threshold) {
String output = "Rise detected -> Total Acc:" + String(totalAcceleration) + "-[Accel:" +
String(converted_ax) + "g|" + String(converted_ay) + "g|" + String(converted_az) +"g]-[Gyro: " +
String(converted_gx) + "deg/s|" + String(converted_gy) + "deg/s|" + String(converted_gz)
+"deg/s]-[Pressure: " + String(pressure) + " hPa]-[Temperature: " + String(temperature) +
"Â°C]-[Parachute:Close]";
sendBluetoothData(output);
Serial.println(output);
delay(10); // Delay to avoid multiple consecutive readings
} else if (threshold > totalAcceleration and totalAcceleration > stableThreshold) {
String output = "Stable detected -> Total Acc:" + String(totalAcceleration) + "-[Accel:" +
String(converted_ax) + "g|" + String(converted_ay) + "g|" + String(converted_az) +"g]-[Gyro: " +
String(converted_gx) + "deg/s|" + String(converted_gy) + "deg/s|" + String(converted_gz)
+"deg/s]-[Pressure: " + String(pressure) + " hPa]-[Temperature: " + String(temperature) +
"Â°C]-[Parachute:Close]";
sendBluetoothData(output);
Serial.println(output);
delay(10); // Delay to avoid multiple consecutive readings
} else {
String output = "Fall detected -> Total Acc:" + String(totalAcceleration) + "-[Accel:" +
String(converted_ax) + "g|" + String(converted_ay) + "g|" + String(converted_az) +"g]-[Gyro: " +
String(converted_gx) + "deg/s|" + String(converted_gy) + "deg/s|" + String(converted_gz)
+"deg/s]-[Pressure: " + String(pressure) + " hPa]-[Temperature: " + String(temperature) +
"Â°C]";
sendBluetoothData(output);
Serial.print(output);
fallCounter += 1;
Serial.print("Fall Counter: " + String(fallCounter));
//operateServo();
delay(10); // Delay to avoid multiple consecutive readings
}
if (fallCounter >= fallCounterThreshold) {
operateServo();
delay(10);
fallCounter = 0; // Reset the fallCounter to prevent opening the servo continuously
}
delay(100); // Adjust delay to observe the outputs clearly
}
void operateServo() {
// Move servo motor to a specific angle
servoMotor.write(90); // Adjust the angle as per your requirement
String servoStatus = "Parachute is opened";
Serial.println(servoStatus);
sendBluetoothData(servoStatus);
//servoMotor.write(0);
delay(1500);
}
void sendBluetoothData(String data) {
bluetoothSerial.println(data);
}