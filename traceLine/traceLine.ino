/*OPEN JUMPER L298 Shield*/
const int INA = 4; //电机A正反转控制端
const int PWMA = 5; //电机A调速端
const int INB = 7; //电机B正反控制端
const int PWMB = 6; //电机B调速端

const int MLT = 8;
const int LLT = 9;
const int RLT = 10;

const float avoidRate = 0.6;
const float nomalRate = 0.6;

double defaultRate = 1.5;
int speedA = 90 * defaultRate;
int speedB = 99 * defaultRate;

const bool IS_LINE = 0;
const bool NOT_LINE = 1;

int lastState = -1;

// use to avoid out of line;
int tryTime = 0;
const int MaxTryTime = 100;

void traceLineLoop() {
  bool M = digitalRead(MLT);
  bool L = digitalRead(LLT);
  bool R = digitalRead(RLT);
  

  if (M == IS_LINE && L != IS_LINE && R != IS_LINE) {
    goStraight(1);
    lastState = 0;
  } else if (M == IS_LINE && L == IS_LINE && R != IS_LINE) {
    turnLeft(0.5);
    lastState = 1;
  } else if (M == IS_LINE && R == IS_LINE && L != IS_LINE) {
    turnRight(0.5);
    lastState = 2;
  } else if (L == IS_LINE && R == IS_LINE) {
    goStraight(1);
    lastState = 0;
  } else if (L == IS_LINE) {
    turnLeft(0);
    lastState = 1;
  } else if (R == IS_LINE) {
    turnRight(0);
    lastState = 2;
  } else {
    if (lastState == 0) {
      tryTime++;
      if (MaxTryTime == tryTime) {
        setBack();
      }
    }
  }
}

void setup()
{
  //车轮
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);

  //初始方向
  setForward();
  //串口输出
  Serial.begin(9600);
}

void loop()
{
  traceLineLoop();
}

void checkCarDirection() {
  goStraight(2);
  delay(1000);
  turnLeft(nomalRate);
  delay(1000);
  turnRight(nomalRate);
  delay(1000);
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

/***
 * @param rate, control the speed of the left wheel.
 * @param rate, control the speed of the right wheel.
 * @param _delay, give a delay if you need.
 */
void turn(float rate1, float rate2, int _delay) {
  int sA = speedA * rate1;
  int sB = speedB * rate2;
  sA = sA > 255 ? 255 : sA;
  sB = sB > 255 ? 255 : sB;
  analogWrite(PWMA, sA);
  analogWrite(PWMB, sB);
  delay(_delay);
}

/***
 * @param rate, control the speed of the car
 */
void goStraight(float rate) {
  setForward();
  turn(rate, rate, 0);
}

void turnLeft(float rate) {
  if (rate < 0) {
    digitalWrite(INA, HIGH);
    digitalWrite(INB, HIGH);
    rate = -rate;
  } else {
    setForward();
  }
  turn(rate, 1, 0);
}

void turnRight(float rate) {
  if (rate < 0) {
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    rate = -rate;
  } else {
    setForward();
  }
  turn(1, rate, 0);
}

