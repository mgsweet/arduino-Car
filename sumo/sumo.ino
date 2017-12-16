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

const int defaultSpeedA = 90;
const int defaultSpeedB = 99;
double speedRate = 1.5;
int speedA = defaultSpeedA * speedRate;
int speedB = defaultSpeedB * speedRate;

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

const int MAX_TRYTIME = 1000;
int tryTime = 0;

typedef enum _Action {
  MOVEING_AWAY_FROM_LINE,
  ATTACK,
  DEFENSE,
  FINDING_TARGET
} Action;

Action currentAction = FINDING_TARGET;
Action lastAction = FINDING_TARGET;

void updateAction(Action a) {
  lastAction = currentAction;
  currentAction = a;
  if (lastAction != currentAction) tryTime = 0;
}

bool lastFindingAction = 1; //0: turn R, 1: turn L

/* -----------------logical code place--------------------- */
bool isNearEdge() {
  return value_MLT == IS_LINE || value_LLT == IS_LINE ||  value_RLT == IS_LINE;
}

void stayAwayFromEdge() {
  if (value_MLT == IS_LINE) {
    goBack(1, 1);
  } else if (value_LLT == IS_LINE) {
    turnRight(0.8);
  } else if (value_RLT == IS_LINE) {
    turnLeft(0.8);
  }
  if (tryTime == MAX_TRYTIME) updateAction(FINDING_TARGET);
}

void findTarget() {
  //若在边界附近优先摆脱边界不找目标
  if (currentAction = MOVEING_AWAY_FROM_LINE) return;
  
  if (dis <= 30) {
    if (currentAction == FINDING_TARGET) stopCar(50);
    lastFindingAction = !lastFindingAction;
    updateAction(DEFENSE);
  } else {
    updateAction(FINDING_TARGET);
    if (lastFindingAction) {
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

void defense() {
  stopCar(0);
  if (tryTime == MAX_TRYTIME) updateAction(ATTACK);
}

void act() {
  switch (currentAction) {
    case DEFENSE:
      defense();
      break;
    case ATTACK:
      attack();
      break;
    case FINDING_TARGET:
      break;
    default:
      break;
  }
  tryTime++;
}

void sumoLoop() {
  //  更新传感器值
  updateValue();
  //  判断是不是在边界附近，优先级最高
  if (isNearEdge()) {
    tryTime = 0;
    currentAction = MOVEING_AWAY_FROM_LINE;
  }
  
  findTarget();
  act();
}

/* -----------car running function code place-------------- */

void updateValue() {
  Serial.println("---------------------------------------------");
  //巡线检测状态
  value_MLT = digitalRead(MLT);
  value_LLT = digitalRead(LLT);
  value_IRR = digitalRead(RLT);
  Serial.print("MLT: ");
  Serial.println(value_MLT);
  Serial.print("LLT: ");
  Serial.println(value_LLT);
  Serial.print("RLT: ");
  Serial.println(value_IRR);
  
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
  goStraight(1);
  
  //  串口输出
  Serial.begin(9600);

}

void loop()
{
  updateValue();
  findTarget();
//  sumoLoop();
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
  turn(rate, rate, 0);
}

void goBack(float rate1, float rate2) {
  setBack();
  turn(rate1, rate2, 0);
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
  int sA = speedA * rate1;
  int sB = speedB * rate2;
  sA = sA > 255 ? 255 : sA;
  sB = sB > 255 ? 255 : sB;
  analogWrite(PWMA, sA);
  analogWrite(PWMB, sB);
  delay(_delay);
}
