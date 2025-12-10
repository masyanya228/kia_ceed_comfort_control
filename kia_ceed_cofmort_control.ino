bool isDebug = true;
#include <DHT.h>
#include <DHT_U.h>
#include <iarduino_VCC.h>
#include <EEPROM.h>
#define DHT22_PIN A5

#define SEAT_1_BTN A0;
#define SEAT_2_BTN A1;
#define FAN_1_CONTROL 9;
#define FAN_2_CONTROL 5;
#define IND_1_LOW 8;
#define IND_2_LOW 10;
#define IND_1_MID 7;
#define IND_2_MID 11;
#define IND_1_HIGH 6;
#define IND_2_HIGH 12;

#define WHEEL_BTN A2;
#define WHEEL_BTN_LED 3;
#define WHEEL_SIG 4;
#define WIND_SHEILD_SIG 2;
#define WHEEL_IND A3;
#define WIND_SHIELD_IND A4;


const int menuQuartzMax=20;//1sec
const int menu3QuartzMax=100;//5sec
const int maxMenuItem=12;

int menu=0;//0-def; 1-speedLow; 2-speedMid; 3-speedHigh; 4-ventilationMemory; 5-wheelMemory; 6-windshieldMemory; 7-wswTypeOfSwitch; 8-autoVent; 9-autoWheel; 10-autoWindShield; 11-wellcome; 12-diagnostic
bool isMenuEdit=false;
int editValue=0;
int menuQuartz=0;
int menu3Quartz=0;
bool autoOnExecuted = false;
DHT dht22(DHT22_PIN, DHT22);

int lastWheelInd = 0;

struct Memory{
  int lowSpeed = 10;
  int midSpeed = 8;
  int highSpeed = 6;

  int ventilationStateMemory = 1;
  int ventilationState1 = 1;
  int ventilationState2 = 2;

  int wsStateMemory = 0;
  int wsState = 0;

  int wStateMemory = 0;
  int wState = 0;

  int wswTypeOfSwitch=0;
  int autoVentilation = 0;
  int autoWheel = 0;
  int autoWindShield = 0;
  int wellcomeState = 1;
} memory;

struct Seat{
  int btnPin;
  int btnPressCounter=0;
  int mode=0;//0-idle; 1-low; 2-mid; 3-high
  int lowLed;
  int midLed;
  int highLed;
  int fanPwmPin;
};

struct FanSpeeds{
  int low=105;
  int mid=155;
  int high=195;
} fanSpeeds;

Seat seat1;
Seat seat2;

struct Wheel{
  int btn;
  int btnPressCounter=0;
  int ledMode=0;//0-idle; 1-w; 2-ws; 3-s
  int led;
  int wSignal;
  int wsSignal;
  int wIndicator;
  int wsIndicator;
};
Wheel wheel;

void setup()
{
  SetupPins();
  dht22.begin();
  Serial.begin(9600);

  ReadCheckMemory();

  log("FanLowSpeed", memory.lowSpeed, (fanSpeeds.low+5*memory.lowSpeed));
  log("FanMidSpeed", memory.midSpeed, (fanSpeeds.mid+5*memory.midSpeed));
  log("FanHighSpeed", memory.highSpeed, (fanSpeeds.high+5*memory.highSpeed));

  float i = analogRead_VCC();
  log("VCC", i);

  if(memory.wellcomeState==1)
    wellcome();
}

void loop()
{
  if (Serial.available()) {
    String com = Serial.readString();
    com.remove(com.length()-1);
    log("Input com", com.length());
    if (com.startsWith("set_wi"))
    {
      setWheelIndicator(com.substring(6).toInt());
    }
    else if (com.startsWith("temp"))
    {
      log("temp", GetTemp());
    }
    else if (com=="w")
    {
      wClickBtn();
      log("wheel switch");
    }
    else if (com=="ws")
    {
      wsClickBtn();
      log("wind shield switch");
    }
    else if (com=="s1")
    {
      nextMode(&seat1);
      log("seat1 click");
    }
    else if (com=="s2")
    {
      nextMode(&seat2);
      log("seat2 click");
    }
  }
  
  if(!autoOnExecuted && millis()>100)
  {
    autoOnExecuted=true;
    AutoOn();
  }

  int event1=getBtnEvent(&seat1);
  int event2=getBtnEvent(&seat2);
  int eventWheel=getBtnEvent(&wheel);
  if(event1!=0) log("Vent1 btn", event1);
  if(event2!=0) log("Vent2 btn", event2);
  if(eventWheel!=0) log("Wheel btn", eventWheel);
  if(menu==0)
  {
    if(event1==-1)
    {
      nextMode(&seat1);
    }
    if(event2==-1)
    {
      nextMode(&seat2);
    }
    if(event1==2)
    {
      BlinkBar(12, 1, 100);
      menu=1;
    }
  }
  else if(!isMenuEdit)
  {
    if(event1==-1)
    {
      menu--;
      if(menu<1) menu=maxMenuItem;
    }
    if(event2==-1)
    {
      menu++;
      if(menu>maxMenuItem) menu=1;
    }
    if(event1==2)
    {
      BlinkBar(12, 2, 100);
      isMenuEdit=true;
      editValue=getValue(menu);
    }
    setBar(menu);

    //exit to def
    if(event2==2)
    {
      isMenuEdit=false;
      menu=0;
      BlinkBar(12, 3, 100);
      setIndicator(seat1);
      setIndicator(seat2);
    }
  }
  else
  {
    if(event1==-1)
    {
      editValue=clamp(--editValue, 0, (menu<=3 | menu>=8&&menu<=10) ? 12 : 1);
    }
    if(event2==-1)
    {
      editValue=clamp(++editValue, 0, (menu<=3 | menu>=8&&menu<=10) ? 12 : 1);
    }
    if(menu<4)
    {
      setEditFan(menu, editValue);
    }
    setBar(editValue);

    if(event1==2)//save
    {
      isMenuEdit=false;
      saveValue(menu);
      BlinkBar(12, 2, 100);
    }
    if(event2==2)//dont save changes
    {
      if(menu==12)
      {
        ShowTemp();
      }
      isMenuEdit=false;
      BlinkBar(12, 3, 50);
      setVentilation(seat1);
      setVentilation(seat2);
    }
  }

  if(memory.wswTypeOfSwitch==0)
    wswSwitch0(eventWheel);
  else if(memory.wswTypeOfSwitch==1)
    wswSwitch1(eventWheel);
  else
  {
    err("Missed typeOfSwitch", memory.wswTypeOfSwitch);
  }
  setWheelIndicator();

  menuQuartz++;
  if(menuQuartz>menuQuartzMax) menuQuartz=0;
  menu3Quartz++;
  if(menu3Quartz>menu3QuartzMax) menu3Quartz=0;
  delay(50);
}

/*
Получить событие кнопки "Вентиляция"
0-idle; 1-down; 2-long; -1-up
*/
int getBtnEvent(Seat* seat)
{
  int pinValue=analogRead(seat->btnPin);

  if(pinValue<500)
  {
    seat->btnPressCounter++;
    if(seat->btnPressCounter==30){
      return 2;
    }
    else if(seat->btnPressCounter==1){
      return 1;
    }
    else{
      return 0;
    }
  }
  else if(seat->btnPressCounter>=30)
  {
    seat->btnPressCounter=0;
    return 0;
  }
  else if(seat->btnPressCounter>0)
  {
    seat->btnPressCounter=0;
    return -1;
  }
  else{
    return 0;
  }
}

/*
Получить событие кнопки "Обогрев руля"
0-idle; 1-down; 2-long; -1-up
*/
int getBtnEvent(Wheel* wheel)
{
  int pinValue=analogRead(wheel->btn);

  if(pinValue<500)
  {
    wheel->btnPressCounter++;
    if(wheel->btnPressCounter==14){
      return 2;
    }
    else if(wheel->btnPressCounter==1){
      return 1;
    }
    else{
      return 0;
    }
  }
  else if(wheel->btnPressCounter>=14)
  {
    wheel->btnPressCounter=0;
    return 0;
  }
  else if(wheel->btnPressCounter>0)
  {
    wheel->btnPressCounter=0;
    return -1;
  }
  else{
    return 0;
  }
}

//Расчет анимации индикатора на кнопке "Обогрев руля"
void setWheelIndicator()
{
  //0-idle; 1-w; 2-ws; 3-s
  getWWSI();

  int pwm=0;
  if(wheel.ledMode==0)
    pwm = 0;
  else if(wheel.ledMode==1)
    pwm = 100;// / (menu3QuartzMax/2) * min(menu3QuartzMax/2, 0+abs(menu3Quartz-(menu3QuartzMax/2)));
  else if(wheel.ledMode==2)
    pwm = menuQuartz > menuQuartzMax/2 ? 255 : 180;
  else if(wheel.ledMode==3)
    pwm = menuQuartz > menuQuartzMax/2 ? 255 : 0;

  if(wheel.ledMode==0 && lastWheelInd==0) return;
  
  lastWheelInd = pwm;
  setWheelIndicator(pwm);
}

//Установить индикатор на кнопке "Обогрев руля"
void setWheelIndicator(int pwm)
{
  analogWrite(wheel.led, pwm);
}

//Получить статус работы обогрева руля
bool getWWSI()
{
  int wiVal=0;
  int wsiVal=0;
  for(int i=0; i<10; i++)//Получить усредненное значение шима
  {
    delay(1);
    wiVal += analogRead(wheel.wIndicator);
    wsiVal += analogRead(wheel.wsIndicator);
  }
  wiVal = wiVal / 10;
  wsiVal = wsiVal / 10;

  bool wI=wiVal > 10;
  bool wsI=wsiVal > 10;

  if(wI == false)
  {
    wheel.ledMode = wsI==false
      ? 0
      : 3;
  }
  else
  {
    wheel.ledMode = wsI==false
      ? 1
      : 2;
  }
  return wheel.ledMode;
}

//Получить статус работы обогрева руля
bool getWI()
{
  int ledMode=getWWSI();
  return ledMode==1 || ledMode==2;
}

//Получить статус работы обогрева лобаша
bool getWSI()
{
  int ledMode=getWWSI();
  return ledMode==2 || ledMode==3;
}

bool modeToW()
{
  return wheel.ledMode==1 || wheel.ledMode==2;
}

bool modeToWS()
{
  return wheel.ledMode==2 || wheel.ledMode==3;
}

void wswSwitch0(int eventWheel)
{
  if(eventWheel==0 || eventWheel==1)
    return;

  getWWSI();
  int wI = modeToW();
  int wsI = modeToWS();
  log("wswi0", wI, wsI);

  if(eventWheel==-1)
  {
    //руль переключить
    wClickBtn();
  }
  else if(eventWheel==2)
  {
    if(wI==LOW && wsI==LOW)
    {
      //руль+стекло включить
      w_wsClickBtn();
    }
    else
    {
      //стекло переключить
      wsClickBtn();
    }
  }
  memoryMode(&wheel);
}

void wswSwitch1(int eventWheel)
{
  if(eventWheel==0 || eventWheel==1)
    return;

  getWWSI();
  int wI = modeToW();
  int wsI = modeToWS();
  log("wswi1", wI, wsI);

  if(eventWheel==-1)
  {
    if(wI==LOW && wsI==LOW)
    {
      //руль включить
      wClickBtn();
    }
    else if(wI==HIGH && wsI==LOW)
    {
      //стекло включить
      wsClickBtn();
    }
    else if(wI==HIGH && wsI==HIGH)
    {
      //руль выключить
      wClickBtn();
    }
    else if(wI==LOW && wsI==HIGH)
    {
      //стекло выключить
      wsClickBtn();
    }
  }
  else if(eventWheel==2)
  {
    if(wI==HIGH)
    {
      //руль выключить
      wClickBtn();
    }
    if(wsI==HIGH)
    {
      //стекло выключить
      wsClickBtn();
    }
  }
  memoryMode(&wheel);
}

void wClickBtn()
{
  digitalWrite(wheel.wSignal, HIGH);
  delay(150);
  digitalWrite(wheel.wSignal, LOW);
}

void wsClickBtn()
{
  digitalWrite(wheel.wsSignal, HIGH);
  delay(150);
  digitalWrite(wheel.wsSignal, LOW);
}

void w_wsClickBtn()
{
  digitalWrite(wheel.wSignal, HIGH);
  digitalWrite(wheel.wsSignal, HIGH);
  delay(150);
  digitalWrite(wheel.wSignal, LOW);
  digitalWrite(wheel.wsSignal, LOW);
}

void nextMode(Seat* seat)
{
  seat->mode--;
  if(seat->mode<0)
    seat->mode=3;
  setVentilation(*seat);
  memoryMode(seat);
}

void setVentilation(Seat seat)
{
  setIndicator(seat);
  setFan(seat);
}

void setIndicator(Seat seat)
{
  digitalWrite(seat.lowLed, (int)(seat.mode>0));
  digitalWrite(seat.midLed, (int)(seat.mode>1));
  digitalWrite(seat.highLed, (int)(seat.mode>2));
}

void setFan(Seat seat)
{
  int speed=0;
  if(seat.mode==0)
  {
    speed=0;
  }
  else if(seat.mode==1)
  {
    speed=fanSpeeds.low+5*memory.lowSpeed;
  }
  else if(seat.mode==2)
  {
    speed=fanSpeeds.mid+5*memory.midSpeed;
  }
  else if(seat.mode==3)
  {
    speed=fanSpeeds.high+5*memory.highSpeed;
  }
  analogWrite(seat.fanPwmPin, speed);
}

//0-12; Each odd value gets blinking
void setBar(int bar)
{
  digitalWrite(seat1.lowLed, ((bar==1 & menuQuartz<10==0)|(bar>1))?HIGH:LOW);
  digitalWrite(seat1.midLed, ((bar==3 & menuQuartz<10==0)|(bar>3))?HIGH:LOW);
  digitalWrite(seat1.highLed, ((bar==5 & menuQuartz<10==0)|(bar>5))?HIGH:LOW);
  digitalWrite(seat2.highLed, ((bar==7 & menuQuartz<10==0)|(bar>7))?HIGH:LOW);
  digitalWrite(seat2.midLed, ((bar==9 & menuQuartz<10==0)|(bar>9))?HIGH:LOW);
  digitalWrite(seat2.lowLed, ((bar==11 & menuQuartz<10==0)|(bar>11))?HIGH:LOW);
}

//Приветствие
void wellcome()
{
  digitalWrite(seat1.lowLed, LOW);
  digitalWrite(seat1.midLed, LOW);
  digitalWrite(seat1.highLed, LOW);
  digitalWrite(seat2.lowLed, LOW);
  digitalWrite(seat2.midLed, LOW);
  digitalWrite(seat2.highLed, LOW);

  digitalWrite(seat1.lowLed, HIGH);
  digitalWrite(seat2.lowLed, HIGH);
  delay(50);
  digitalWrite(seat1.midLed, HIGH);
  digitalWrite(seat2.midLed, HIGH);
  delay(150);
  digitalWrite(seat1.highLed, HIGH);
  digitalWrite(seat2.highLed, HIGH);
  delay(200);
  digitalWrite(seat1.highLed, LOW);
  digitalWrite(seat2.highLed, LOW);
  delay(150);
  digitalWrite(seat1.midLed, LOW);
  digitalWrite(seat2.midLed, LOW);
  delay(50);
  digitalWrite(seat1.lowLed, LOW);
  digitalWrite(seat2.lowLed, LOW);
  delay(200);
}

void BlinkBar(int val, int times, int pause)
{
  for (int i=0; i<times; i++)
  {
    delay(pause);
    setBar(val);
    delay(pause);
    setBar(0);
  }
}
