#define BLYNK_TEMPLATE_ID "TMPL6Wwfy0GG0"
#define BLYNK_TEMPLATE_NAME "Shelby Car"
#define BLYNK_AUTH_TOKEN "VdFMbNn4pP0Wsr_t1uEQtosjd4MfvGbg"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Servo.h>

//Motor PINs
#define ENA D0
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENB D5

long duration;
float distance;
#define echoPin D7
#define trigPin D6

#define DHTPIN D8
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

Servo myservo;
int pos = 90;
int obstacleAvoidance = 0;

#define OBSTACLE_PIN V8
#define SERVO_PIN D9 //RX pin

int sensorValue;
#define BUZZER_PIN D10

bool left = 0;
bool right = 0;
bool forward = 0;
bool backward = 0;
int Speed;
char auth[] = BLYNK_AUTH_TOKEN; //Enter your Blynk application auth token
// char ssid[] = "ASUS_X01AD"; //Enter your WIFI name
// char pass[] = "87912852"; //Enter your WIFI passowrd

char ssid[] = "Tmas";
char pass[] = "masmas12";

void setup() {
  Serial.begin(9600);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(BUZZER_PIN, OUTPUT);

  myservo.attach(SERVO_PIN);
  myservo.write(pos);

  dht.begin();
  timer.setInterval(1000L, sendSensor);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

BLYNK_WRITE(V0) {
  left = param.asInt();
}

BLYNK_WRITE(V1) {
  right = param.asInt();
}

BLYNK_WRITE(V2) {
  forward = param.asInt();
}

BLYNK_WRITE(V3) {
  backward = param.asInt();
}

BLYNK_WRITE(V4) {
  Speed = param.asInt();
}
BLYNK_WRITE(OBSTACLE_PIN) {
  obstacleAvoidance = param.asInt();
}


void smartcar() {
  if (left == 1) {
    carturnleft();
    Serial.println("carturnleft");
  } else if (right == 1) {
    carturnright();
    Serial.println("carturnright");
  } else if (forward == 1) {
    carforward();
    Serial.println("carforward");
  } else if (backward == 1) {
    carbackward();
    Serial.println("carbackward");
  } else if (left == 0 && right == 0 && forward == 0 && backward == 0) {
    carStop();
    Serial.println("carstop");
  }
}

void loop() {
  Blynk.run();
  timer.run();
  smartcar();
  obstacleAvoid();
  ultrasonic();
  LDR();
}

void carturnleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carturnright() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carforward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carbackward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void obstacleAvoid() {
  if (obstacleAvoidance == 1) {
    myservo.write(180);
    delay(500);
    int distanceRight = ultrasonicMeasure(100);
    delay(300);
    int distanceLeft = ultrasonicMeasure(270);
    delay(300);
    if (distanceLeft > distanceRight) {
      carturnleft();
      delay(500);
      carforward();
      delay(500);
      carStop();
    } else if (distanceLeft < distanceRight) {
      carturnright();
      delay(500);
      carforward();
      delay(500);
      carStop();
    } else if (distanceLeft < 20 && distanceRight < 20 ){
      carbackward();
      delay(500);
      carStop();
    } else if (distanceLeft > 150 && distanceRight > 150 ){
      carforward();
      delay(500);
      carStop();
    }
    
    myservo.write(0);
    delay(500);
    carStop();
    myservo.write(90);
    delay(500);
    carStop();
  }
}

int ultrasonicMeasure(int angle) {
  int servoPos = map(angle, 0, 180, 0, 180);
  myservo.write(servoPos);
  delay(500);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

void ultrasonic()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2; //formula to calculate the distance for ultrasonic sensor
    if (distance < 15) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
  }
    else if (distance <10){
      carStop();
      delay(500);
    }
    Serial.print("Distance: ");
    Serial.println(distance);
    Blynk.virtualWrite(V5, distance);
    delay(500);
}

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V6, h);
  Blynk.virtualWrite(V7, t);
}

void LDR(){
  sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  Blynk.virtualWrite(V9, sensorValue);
  delay(1000);
}