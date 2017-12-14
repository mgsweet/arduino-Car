/*somo*/

/*
 * code record
 * delay 100 * 4 = 90度
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

const int IRM = 1;  //  红外传感器M
const int IRL = 2;  //  红外传感器L
const int IRR = 3; //  红外传感器R

double speedRate = 1.5;
int speedA = 90 * speedRate;
int speedB = 99 * speedRate;

const bool IS_LINE = 0;
const bool NOT_LINE = 1;

String lastState = "111";

/* -----------------logical code place--------------------- */

void sumoLoop() {
  bool M = digitalRead(MLT);
  bool L = digitalRead(LLT);
  bool R = digitalRead(RLT);

  Serial.println(lastState);
}

void changeLastState(bool m, bool l, bool r) {
  lastState = "";
  lastState += m ? '1' : '0';
  lastState += l ? '1' : '0';
  lastState += r ? '1' : '0';
}

/* -----------car running function code place-------------- */

void setup()
{
  //车轮
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);

  //初始方向
  goStraight(1);
  //串口输出
  Serial.begin(9600);
}

void loop()
{
  sumoLoop();
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

