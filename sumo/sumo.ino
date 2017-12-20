/*somo*/
#include "SR04.h"
/*
 * code record
 * delay 100 * 4 = 90度
 * 优先级：检线，检测碰撞，行为
 */

/* ---------------------global defined-------------------- */

const int INA = 7; // 电机A正反转控制端
const int PWMA = 6; //  电机A调速端
const int INB = 4; // 电机B正反控制端
const int PWMB = 5; //  电机B调速端

const int MLT = 8;  //  寻线传感器M
const int LLT = 9;  //  寻线传感器L
const int RLT = 10; //  寻线传感器R

const int TrigPin = 11; //  超声波传感器TrigPin
const int EchoPin = 12; //  超声波传感器EchoPin

const int IRM = 14;  //  红外传感器M
const int IRL = 15;  //  红外传感器L
const int IRR = 16; //  红外传感器R

const int defaultSpeedA = 99 * 1.5;
const int defaultSpeedB = 90 * 1.5;
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
const int MAX_TRYTIME = 100;
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
  tryTime = 0;
}

bool stayAwayFromEdge() {
  //巡线检测状态
  value_MLT = digitalRead(MLT);
  value_LLT = digitalRead(LLT);
  value_RLT = digitalRead(RLT);
  if (value_MLT == IS_LINE) {
    if (value_LLT == IS_LINE) {
      turnRight(0);
    } else if (value_LLT == IS_LINE) {
      turnLeft(0);
    } else {
      goStraightFast();
    }
    return true;
  } else if (value_LLT == IS_LINE || value_RLT == IS_LINE) {
    //goBack(1, 1);
    setBack();
    return true;
  } else {
    return false;
  }
}

void findTarget() {
  if (dis <= 30) {
    stopCar(50);
    lastFindingAction++;
    if (lastFindingAction > 10) {
      lastFindingAction = 0;
      stopCar(0);
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
  if (tryTime < MAX_TRYTIME / 5) {
//    if (!value_IRL) turnLeft(0.6);
//    else if (!value_IRR) turnRight(0.6); 
    
//    else goStraightFast();
  }
//  else goBack(1,0.6);
  else {
    goStraight(1);
  }
}

void act() {
  switch (currentAction) {
    case MOVEING_AWAY_FROM_EDGE:
      if (tryTime >= MAX_TRYTIME / 5) updateAction(FINDING_TARGET);
      break;
    case ATTACK:
      attack();
      if (tryTime >= MAX_TRYTIME) updateAction(FINDING_TARGET);
      break;
    case FINDING_TARGET:
      findTarget();
      break;
    default:
      break;
  }
  tryTime++;
  //Serial.println("t++");
}

void sumoLoop() {
  if (stayAwayFromEdge()) {
    updateAction(MOVEING_AWAY_FROM_EDGE);
  } else {
    updateValues();
    act();
  }
}

void loop()
{
//  updateValues();
  stayAwayFromEdge();
//  act();
//  sumoLoop();
  
}

/* -----------car running function code place-------------- */

//  for debug
//void showValues() {
//  Serial.println("---------------------------------------------");
//  Serial.print("MLT: ");
//  Serial.println(value_MLT);
//  Serial.print("LLT: ");
//  Serial.println(value_LLT);
//  Serial.print("RLT: ");
//  Serial.println(value_RLT);
//  
//  Serial.print("Dis:");
//  Serial.print(dis);
//  Serial.println(" cm");
//  
//  Serial.print("IRM: ");
//  Serial.println(value_IRM);
//  Serial.print("IRL: ");
//  Serial.println(value_IRL);
//  Serial.print("IRR: ");
//  Serial.println(value_IRR);
//
//  Serial.print("IRM: ");
//  Serial.println(value_IRM);
//  Serial.print("IRL: ");
//  Serial.println(value_IRL);
//  Serial.print("IRR: ");
//  Serial.println(value_IRR); 
//}

void updateValues() {
  // 由于巡线检测状态需要及时反馈，所以单独出来处理；
//  value_MLT = digitalRead(MLT);
//  value_LLT = digitalRead(LLT);
//  value_RLT = digitalRead(RLT)
  
  //距离状态
  dis = sr04.Distance();
  
  //  红外状态
  value_IRM = digitalRead(IRM);
  value_IRL = digitalRead(IRL);
  value_IRR = digitalRead(IRR);
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

void setStop() {
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
  if (_delay != 0) delay(_delay);
  setStop();
  setForward();
}

/***
 * @param rate, control the speed of the car
 */
void goStraight(float rate) {
  setForward();
  setSpeedValue(rate, rate);
}



void goStraightFast() {
  setForward();
  analogWrite(PWMA, 230);
  analogWrite(PWMB, 255);
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
  setSpeedValue(rate, 1);
}

void turnRight(float rate) {
  if (rate < 0) {
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    rate = -rate;
  } else {
    setForward();
  }
  setSpeedValue(1, rate);
}

void setSpeedValue(float rate1, float rate2) {
  speedA = defaultSpeedA * rate1;
  speedB = defaultSpeedB * rate2;
  speedA = speedA > 255 ? 255 : speedA;
  speedB = speedB > 255 ? 255 : speedB;
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, speedB);
}

