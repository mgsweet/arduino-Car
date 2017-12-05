int n = 2, m = 0;
int a[] = {3,4,5,6,7,8,9};
int f[] = {0,0};
int p[7][10] = {
//{0,1,2,3,4,5,6,7,8,9}
  {1,0,1,1,0,1,1,1,1,1},
  {1,1,1,1,1,0,0,1,1,1},
  {0,0,1,1,1,1,1,0,1,1},
  {1,1,0,1,1,1,1,1,1,1},
  {1,0,1,1,0,1,1,0,1,1},
  {1,0,0,0,1,1,1,0,1,1},
  {1,0,1,0,0,0,1,0,1,0}
  };

void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  m = 9;
}

void loop() {
  // put your main code here, to run repeatedly:
  /*m = (m+1)%4;
  int x = m;
  for (int i = 0; i < n; i++) {
    f[i] = x%2;
    if (f[i]) digitalWrite(a[i], HIGH);
    else digitalWrite(a[i], LOW);
    x = x/2;
  }
  delay(500);
  */
  m = (m+1)%10;
  for (int i = 0; i < 7; i++) {
    if (p[i][m] == 1) digitalWrite(a[i], HIGH);
    else digitalWrite(a[i], LOW);    
  }
  delay(500);              
  /*
  digitalWrite(7, HIGH);
  delay(1000);
  digitalWrite(7, LOW);
  delay(1000);
  */
}
