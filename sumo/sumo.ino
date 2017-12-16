/*somo*/
#include "SR04.h"
/*
 * code record
 * delay 100 * 4 = 90度
 * 优先级：检线，检测碰撞，行为
 */

/* ---------------------global defined-------------------- */

const int INA = 4; // 电机A正反转控制端
const int PWMA = 5; //  电机A调速端
const int INB = 7; // 电机B正反控制端
const int PWMB = 6; //  电机B调速端

const int MLT = 8;  //  寻线传感器M
const int LLT = 9;  //  寻线传感器L
const int RLT = 10; //  寻线传感器R

const int TrigPin = 11; //  超声波传感器TrigPin
const int EchoPin = 12; //  超声波传感器EchoPin

const int IRM = 14;  //  红外传感器M
const int IRL = 15;  //  红外传感器L
const int IRR = 16; //  红外传感器R

const int defaultSpeedA = 90 * 1.5;
const int defaultSpeedB = 99 * 1.5;
int speedA = defaultSpeedA;
int speedB = defaultSpeedB;

const bool IS_LINE = 0;
const bool NOT_LINE = 1;

//  3个寻线传感器的值
bool value_MLT;
bool value_LLT;
bool value_RLT;

//  3个红外传感器的值
bool value_IRM;
bool value_IRL;
bool value_IRR;

//  超声波传感器的值
long dis;
SR04 sr04 = SR04(EchoPin, TrigPin);

//  控制动作执行次数
const int MAX_TRYTIME = 1000;
int tryTime = 0;



//  动作定义
typedef enum _Action {
  MOVEING_AWAY_FROM_EDGE,
  ATTACK,
  FINDING_TARGET
} Action;



//  记录当前动作以及前一次动作
Action currentAction = FINDING_TARGET;
Action lastAction = FINDING_TARGET;

//  记录寻找时旋转方向
int lastFindingAction = 0;

/* -----------------logical code place--------------------- */

void updateAction(Action a) {
  lastAction = currentAction;
  currentAction = a;
  if (lastAction != currentAction) tryTime = 0;
  if (currentAction != FINDING_TARGET) lastFindingAction = 0;
}

boolean isNearEdge() {
  return (value_MLT == IS_LINE) || (value_LLT == IS_LINE) || (value_RLT == IS_LINE);
}

void stayAwayFromEdge() {
  //巡线检测状态
  value_MLT = digitalRead(MLT);
  value_LLT = digitalRead(LLT);
  value_RLT = digitalRead(RLT);
  if (value_MLT == IS_LINE) {
    goBack(1, 1);
  } else if (value_LLT == IS_LINE) {
    turnRight(0.8);
  } else if (value_RLT == IS_LINE) {
    turnLeft(0.8);
  }
  //if (tryTime >= MAX_TRYTIME) updateAction(FINDING_TARGET);
}

void findTarget() {
  setSpeedValue(1.1, 1.1);
  if (dis <= 30) {
    stopCar(50);
    lastFindingAction++;
    if (lastFindingAction > 20) {
      updateAction(ATTACK);
    }
  } else {
    updateAction(FINDING_TARGET);
    if (lastFindingAction % 2 == 0) {
      turnRight(-1);
    } else {
      turnLeft(-1);
    }
  }
}

void attack() {
  if (tryTime < 500) goBack(1, 1);
  else goStraight(1);
}

void act() {
  switch (currentAction) {
    case MOVEING_AWAY_FROM_EDGE:
      stayAwayFromEdge();
    case ATTACK:
      attack();
      break;
    case FINDING_TARGET:
      findTarget();
      break;
    default:
      break;
  }
  tryTime++;
  Serial.println("t++");
}

void sumoLoop() {
  //  更新传感器值
  updateValue();
  //  判断是不是在边界附近，优先级最高
  if (isNearEdge()) {
    tryTime = 0;
    updateAction(MOVEING_AWAY_FROM_EDGE);
  }
  
  act();
}

void loop()
{
//  sumoLoop();
//  delay(1000);
  updateValue();
  stayAwayFromEdge();
}

/* -----------car running function code place-------------- */

void updateValue() {
  Serial.println("---------------------------------------------");
  //巡线检测状态
  value_MLT = digitalRead(MLT);
  value_LLT = digitalRead(LLT);
  value_RLT = digitalRead(RLT);
  Serial.print("MLT: ");
  Serial.println(value_MLT);
  Serial.print("LLT: ");
  Serial.println(value_LLT);
  Serial.print("RLT: ");
  Serial.println(value_RLT);
  
  //距离状态
  dis = sr04.Distance();
  Serial.print("Dis:");
  Serial.print(dis);
  Serial.println(" cm");
  
  //  红外状态
  value_IRM = digitalRead(IRM);
  value_IRL = digitalRead(IRL);
  value_IRR = digitalRead(IRR);
  Serial.print("IRM: ");
  Serial.println(value_IRM);
  Serial.print("IRL: ");
  Serial.println(value_IRL);
  Serial.print("IRR: ");
  Serial.println(value_IRR);

  Serial.print("Current Action: ");
  Serial.println(currentAction);
  Serial.print("tryTime: ");
  Serial.println(tryTime);
}

void setup()
{
  //  车轮
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);

  //  巡线传感器
  pinMode(MLT, INPUT);
  pinMode(LLT, INPUT);
  pinMode(RLT, INPUT);

  //  红外传感器
  pinMode(IRM, INPUT);
  pinMode(IRL, INPUT);
  pinMode(IRR, INPUT);

  //  超声波
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  //  初始方向
  setForward();
  
  //  串口输出
  Serial.begin(9600);

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

void stopCar(int _delay) {
  boolean da = digitalRead(INA);
  boolean db = digitalRead(INB);
  digitalWrite(INA, !da);
  digitalWrite(INB, !db);
  delay(_delay);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  setForward();
}

/***
 * @param rate, control the speed of the car
 */
void goStraight(float rate) {
  setForward();
  setSpeedValue(rate, rate);
}

void goBack(float rate1, float rate2) {
  setBack();
  setSpeedValue(rate1, rate2);
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

/***
 * @param rate, control the speed of the left wheel.
 * @param rate, control the speed of the right wheel.
 * @param _delay, give a delay if you need.
 */
void turn(float rate1, float rate2, int _delay) {
  setSpeedValue(rate1, rate2);
  delay(_delay);
}

void setSpeedValue(float rate1, float rate2) {
  speedA = defaultSpeedA * rate1;
  speedB = defaultSpeedB * rate2;
  speedA = speedA > 255 ? 255 : speedA;
  speedB = speedB > 255 ? 255 : speedB;
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, speedB);
}

