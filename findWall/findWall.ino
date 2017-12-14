/*OPEN JUMPER L298 Shield*/
const int INA = 4; //电机A正反转控制端
const int PWMA = 5; //电机A调速端
const int INB = 7; //电机B正反控制端
const int PWMB = 6; //电机B调速端

const int TrigPin = 11;
const int EchoPin = 12;

const int IRM = 8;
const int IRL = 9;
const int IRR = 10;

const float wheelWidth = 3.0;
const float diff = 4.0;
const float DEFINE_DIS = 16.0;

const float avoidRate = 0.6;
const float nomalRate = 0.8;

int speedA = 90;
int speedB = 99;

const int minSpeedA = 50;
const int minSpeedB = 50;

void setup()
{
  //车轮
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
  //红外传感器
  pinMode(IRM, INPUT);
  pinMode(IRL, INPUT);
  pinMode(IRR, INPUT);
  //初始方向
  setForward();
  //超声波
  Serial.begin(9600);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
}

void loop()
{
  //checkStop();
  //checkCarDirection();
  findWallLoop();
  //goStraight(1);
}

void checkCarDirection() {
  goStraight(2);
  delay(1000);
  turnLeft(nomalRate);
  delay(1000);
  turnRight(nomalRate);
  delay(1000);
}

void findWallLoop() {
  if (avoidObstacle()) {
    return;
  } else {
    float dis =  measureDistance();
    if (DEFINE_DIS + diff < dis) {
      if (dis > 60) {
        turnLeft(0.6);
      } else {
        turnLeft(nomalRate);
      }
    } else if (DEFINE_DIS - diff > dis) {
      turnRight(nomalRate);
    } else {
      goStraight(1);
    }
  }
}

bool avoidObstacle() {
  if (!digitalRead(IRM))  {
    Serial.println("avoidObstacle M");
    turnRight(0);
    return true;
  } else if (!digitalRead(IRL)) {
    Serial.println("avoidObstacle L");
    turnRight(0);
    return true;
  } else if (!digitalRead(IRR)) {
    Serial.println("avoidObstacle R");
    turnLeft(avoidRate);
    return true;
  } else {
    return false;
  }
}

void stopSetting() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

void setBack() {
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
}

void setForward() {
  digitalWrite(INA, LOW);
  digitalWrite(INB, HIGH);
}

void stopCar() {
  setBack();
  delay(50);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  setForward();
}

void turn(float rate1, float rate2, int _delay) {
  analogWrite(PWMA, speedA * rate1);
  analogWrite(PWMB, speedB * rate2);
  delay(10);
}

void goStraight(float rate) {
  turn(rate, rate, 50);
}

void turnLeft(float rate) {
  turn(rate, 1, 10);
}

void turnRight(float rate) {
 turn(1, rate, 10);
}

float measureDistance() {
  float cm;
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  cm = pulseIn(EchoPin, HIGH) / 58.0;;
  cm = (int(cm * 100.0)) / 100.0;  //保留两位小数 
  Serial.print(cm);
  Serial.println("cm");
  return cm;
}

