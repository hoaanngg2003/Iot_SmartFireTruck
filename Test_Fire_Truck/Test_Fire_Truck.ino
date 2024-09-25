// Define the Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6JLjsiAN0"
#define BLYNK_TEMPLATE_NAME "Control Car"
#define BLYNK_AUTH_TOKEN "-pjPf77WMso9IjOgUlOl9HcfkZKf82UZ"
#define BLYNK_PRINT Serial

// Include the libraries
#include <ESP8266WiFi.h>
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h> 

// Configure Blynk wifi connection
char auth[] = "-pjPf77WMso9IjOgUlOl9HcfkZKf82UZ"; 
char ssid[] = "Hoang"; 
char pass[] = "123456789"; 

Servo myservo;
int pos = 0;
int fireOffComplete = false; 

//Motor PINs
const int ENABLE = D0; // Control Speed
const int IN1 = D1;   // Motor 1
const int IN2 = D2;   // Motor 2
const int IN3 = D3;   // Motor 3 
const int IN4 = D4;   // Motor 4
const int FlamerSensor = A0;  // Flamer sensor
const int IRSensorRight = D6; // Right IR Sensor
const int IRSensorLeft = D7;  // Left IR Sensor
const int RelayPump = D8;   // Relay on / off pump

// Set the value to receive signals from Blynk
const int STOP = 0;
const int FORWARD = 1; // Car forward
const int BACKWARD = 2;// Car backwward
const int LEFT = 3;    // Car turn left
const int RIGHT = 4;   // Car turnright
const int AUTOMODE = 5;// Car auto
int direction = STOP; // Changed from a character array to an integer
int Speed = 0; // Added this line

// Define the setup function setup Pin
void setup() {
  Serial.begin(9600); // Start the serial communication
  // set the motor pins as output
  pinMode(ENABLE, OUTPUT); 
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Set the sensor pins as input 
  pinMode(FlamerSensor, INPUT);
  pinMode(IRSensorRight, INPUT); 
  pinMode(IRSensorLeft, INPUT);

  // Set pin relay
  pinMode(RelayPump, OUTPUT);
  digitalWrite(RelayPump, HIGH);
  myservo.attach(D5, 500, 2500); // Gắn Servo vào chân D5, sử dụng giá trị PWM từ 500 đến 2500
  myservo.write(90); 
  // Start the Blynk connection
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80); 
  Blynk.syncVirtual(V0, V5);
}
// Get value from Blynk
BLYNK_WRITE(V0) {
  direction = param.asInt(); // get the value as an integer
}
BLYNK_WRITE(V5) {
  Speed = param.asInt();
}
// Check connection to wifi 
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0, V5);
}
BLYNK_DISCONNECTED() {
  carStop();
}

void loop() {
  Blynk.run(); // run the Blynk loop
  smartcar(direction, Speed ); // control the car based on the direction and speed
}

void smartcar(int direction, int speed) {
  switch (direction) {
    case FORWARD:
      carforward(speed);
      Serial.println("carforward");
      break;
    case BACKWARD:
      carbackward(speed);
      Serial.println("carbackward");
      break;
    case LEFT:
      carturnleft(speed);
      Serial.println("carfleft");
      break;
    case RIGHT:
      carturnright(speed);
      Serial.println("carright");
      break;
    case AUTOMODE: 
      carAuto(speed); 
      Serial.println("carauto");
      break;
    default: // STOP
      carStop();
      Serial.println("carstop");
      break;
  }
}
void setSpeed(int speed) {
  analogWrite(ENABLE, speed);
}
void carforward(int speed) {
  setSpeed(speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carbackward(int speed) {
  setSpeed(speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carturnleft(int speed) {
  setSpeed(speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carturnright(int speed) {
  setSpeed(speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carAuto(int speed) {
  if(analogRead(FlamerSensor) < 500 ){
    Serial.print(" Car Stop put off fire ");
    putOffFire();
    return;
  }
  if(analogRead(FlamerSensor) >= 500 ){
    Serial.print("Line Follower Mode ");
    if(digitalRead(IRSensorRight)==LOW  && digitalRead(IRSensorLeft)==LOW) // If both sensors are on the road
    {
      Serial.print(" Car forward ");
      carforward(speed); // Go forward
      return;
    }
    if(digitalRead(IRSensorRight)==HIGH  && digitalRead(IRSensorLeft)==LOW) // If right sensor is on the line
    {
      Serial.print(" Car carturnright ");
      carturnright(speed); // Turn right
      return;
    }
    if(digitalRead(IRSensorRight)==LOW  && digitalRead(IRSensorLeft)==HIGH) // If left sensor is on the line
    {
      Serial.print(" Car carturnleft ");
      carturnleft(speed); // Turn left
      return;
    }
    if(digitalRead(IRSensorRight)==HIGH  && digitalRead(IRSensorLeft)==HIGH) // If both sensors are on the line
    {
      Serial.print(" Car Stop ");
      carStop(); // Stop
      return;
    }
  }
}
// Define a function to make the car stop
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void putOffFire(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(RelayPump, LOW);
  delay(200);
  
  for (pos = 135; pos >= 45; pos -= 15) { 
    myservo.write(pos); 
    delay(100);
  }
  for (pos = 45; pos <= 135; pos += 15) { 
    myservo.write(pos); 
    delay(100);  
  }
  for (pos = 135; pos >= 45; pos -= 15) { 
    myservo.write(pos); 
    delay(100);
  }
  for (pos = 45; pos <= 135; pos += 15) { 
    myservo.write(pos); 
    delay(100);  
  }
  for (pos = 135; pos >= 45; pos -= 15) { 
    myservo.write(pos); 
    delay(100);
  }
  for (pos = 45; pos <= 135; pos += 15) { 
    myservo.write(pos); 
    delay(100);  
  }
  digitalWrite(RelayPump, HIGH);
  myservo.write(90); // Đặt Servo ở góc 0 độ
  fireOffComplete = true;
}