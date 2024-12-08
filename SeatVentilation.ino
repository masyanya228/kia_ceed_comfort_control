#include <EEPROM.h>

int menu=0;//0-def; 1-speedLow; 2-speedMid; 3-speedHigh; 4-ventilationMemory; 5-wheelMemory; 6-windshieldMemory; 7-wswTypeOfSwitch; 8-autoVent; 9-autoWheel; 10-autoWindShield; 11-wellcome; 12-diagnostic
bool isMenuEdit=false;
int editValue=0;
int menuQuarz=0;
int menu3Quarz=0;
const int menuQuartzMax=20;//1sec
const int menu3QuartzMax=100;//5sec
const int maxMenuItem=12;

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
  int btn=A2;
  int btnPressCounter=0;
  int led = 3;
  int ledMode=0;//0-idle; 1-w; 2-ws; 3-s
  int wSignal = 4;
  int wsSignal = 2;
  int wIndicator = A3;
  int wsIndicator = A4;
} wheel;

void setup()
{
  delay(2000);
  Serial.begin(9600);

  seat1.btnPin=A0;
  seat1.fanPwmPin=5;
  seat1.lowLed=8;
  seat1.midLed=7;
  seat1.highLed=6;

  seat2.btnPin=A1;
  seat2.fanPwmPin=9;
  seat2.lowLed=10;
  seat2.midLed=11;
  seat2.highLed=12;

  pinMode(seat1.btnPin, INPUT);
  pinMode(seat1.fanPwmPin, OUTPUT);
  pinMode(seat1.lowLed, OUTPUT);
  pinMode(seat1.midLed, OUTPUT);
  pinMode(seat1.highLed, OUTPUT);
  
  pinMode(seat2.btnPin, INPUT);
  pinMode(seat2.fanPwmPin, OUTPUT);
  pinMode(seat2.lowLed, OUTPUT);
  pinMode(seat2.midLed, OUTPUT);
  pinMode(seat2.highLed, OUTPUT);

  pinMode(wheel.btn, INPUT);
  pinMode(wheel.led, OUTPUT);
  pinMode(wheel.wIndicator, INPUT);
  pinMode(wheel.wsIndicator, INPUT);
  pinMode(wheel.wSignal, OUTPUT);
  pinMode(wheel.wSignal, OUTPUT);

  EEPROM.get(0, memory);
  if(memory.lowSpeed==255 || memory.lowSpeed==-1)
  {
    Memory newMemory;
    EEPROM.put(0, newMemory);
    log("New memory", 0);
  }
  else
  {
    if(clamp(memory.lowSpeed, 0, 12) != memory.lowSpeed)
      err("Out of range lowSpeed", memory.lowSpeed);
    if(clamp(memory.midSpeed, 0, 12) != memory.midSpeed)
      err("Out of range midSpeed", memory.midSpeed);
    if(clamp(memory.highSpeed, 0, 12) != memory.highSpeed)
      err("Out of range highSpeed", memory.highSpeed);
    if(clamp(memory.ventilationStateMemory, 0, 1) != memory.ventilationStateMemory)
      err("Out of range ventilationStateMemory", memory.ventilationStateMemory);
    if(clamp(memory.ventilationState1, 0, 3) != memory.ventilationState1)
      err("Out of range ventilationState1", memory.ventilationState1);
    if(clamp(memory.ventilationState2, 0, 3) != memory.ventilationState2)
      err("Out of range ventilationState2", memory.ventilationState2);
    if(clamp(memory.wsStateMemory, 0, 1) != memory.wsStateMemory)
      err("Out of range wsStateMemory", memory.wsStateMemory);
    if(clamp(memory.wsState, 0, 1) != memory.wsState)
      err("Out of range wsState", memory.wsState);
    if(clamp(memory.wStateMemory, 0, 1) != memory.wStateMemory)
      err("Out of range wStateMemory", memory.wStateMemory);
    if(clamp(memory.wState, 0, 1) != memory.wState)
      err("Out of range wState", memory.wState);
    if(clamp(memory.wswTypeOfSwitch, 0, 1) != memory.wswTypeOfSwitch)
      err("Out of range wswTypeOfSwitch", memory.wswTypeOfSwitch);
    if(clamp(memory.autoVentilation, 0, 12) != memory.autoVentilation)
      err("Out of range autoVentilation", memory.autoVentilation);
    if(clamp(memory.autoWheel, 0, 12) != memory.autoWheel)
      err("Out of range autoWheel", memory.autoWheel);
    if(clamp(memory.autoWindShield, 0, 12) != memory.autoWindShield)
      err("Out of range autoWindShield", memory.autoWindShield);
    if(clamp(memory.wellcomeState, 0, 1) != memory.wellcomeState)
      err("Out of range wellcomeState", memory.wellcomeState);
  }

  log("FanLowSpeed", memory.lowSpeed, (fanSpeeds.low+5*memory.lowSpeed));
  log("FanMidSpeed", memory.midSpeed, (fanSpeeds.mid+5*memory.midSpeed));
  log("FanHighSpeed", memory.highSpeed, (fanSpeeds.high+5*memory.highSpeed));

  if(memory.wellcomeState==1)
    wellcome();
    
  if(memory.ventilationStateMemory)
  {
    seat1.mode=memory.ventilationState1;
    seat2.mode=memory.ventilationState2;
    setVentilation(seat1);
    setVentilation(seat2);
  }

  if(memory.wsStateMemory && memory.wsState==1)
  {
    wsClickBtn();
  }

  if(memory.wStateMemory && memory.wState==1)
  {
    wClickBtn();
  }
}

void loop()
{
  int event1=getBtnEvent(&seat1);
  int event2=getBtnEvent(&seat2);
  int eventWheel=getBtnEvent(&wheel);
  if(event1!=0) log("Vent1", event1);
  if(event2!=0) log("Vent2", event2);
  if(eventWheel!=0) log("Wheel", eventWheel);
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
      isMenuEdit=true;
      editValue=getValue(menu);
    }
    setBar(menu);

    //exit to def
    if(event2==2)
    {
      isMenuEdit=false;
      menu=0;
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

    if(event1==2)
    {
      isMenuEdit=false;
      saveValue(menu);
    }
    if(event2==2)
    {
      isMenuEdit=false;
      setVentilation(seat1);
      setVentilation(seat2);
    }
    setBar(editValue);
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

  menuQuarz++;
  if(menuQuarz>menuQuartzMax) menuQuarz=0;
  menu3Quarz++;
  if(menu3Quarz>menu3QuartzMax) menu3Quarz=0;
  delay(50);
}

//0-idle; 1-down; 2-long; -1-up
int getBtnEvent(Seat* seat)
{
  int pinValue=analogRead(seat->btnPin);

  if(pinValue<900)
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

//0-idle; 1-down; 2-long; -1-up
int getBtnEvent(Wheel* wheel)
{
  int pinValue=analogRead(wheel->btn);

  if(pinValue<900)
  {
    wheel->btnPressCounter++;
    if(wheel->btnPressCounter==30){
      return 2;
    }
    else if(wheel->btnPressCounter==1){
      return 1;
    }
    else{
      return 0;
    }
  }
  else if(wheel->btnPressCounter>=30)
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

void setWheelIndicator()
{
  //0-idle; 1-w; 2-ws; 3-s
  int wI=getWI();
  int wsI=getWSI();
  if(wI == LOW)
  {
    wheel.ledMode = wsI==LOW
      ? 0
      : 3;
  }
  else
  {
    wheel.ledMode = wsI==LOW
      ? 1
      : 2;
  }

  int pwm=0;
  if(wheel.ledMode==0)
    pwm=0;
  else if(wheel.ledMode==1)
    pwm=255.0f / (menu3QuartzMax/2) * min(menu3QuartzMax/2, 0+abs(menu3Quarz-(menu3QuartzMax/2)));
  else if(wheel.ledMode==2)
    pwm=255;
  else if(wheel.ledMode==3)
    pwm=255.0f / (menuQuartzMax/2) * min(menuQuartzMax/2, 0+abs(menuQuarz-(menuQuartzMax/2)));
  
  setWheelIndicator(pwm);
}

void setWheelIndicator(int pwm)
{
  //log("Led", wheel.ledMode, pwm);
  analogWrite(wheel.led, pwm);
}

int getWI()
{
  return analogRead(wheel.wIndicator) > 500
    ? HIGH
    : LOW;
}

int getWSI()
{
  return analogRead(wheel.wsIndicator) > 500
    ? HIGH
    : LOW;
}

void wswSwitch0(int eventWheel)
{
  if(eventWheel==0 || eventWheel==1)
    return;

  int wI=getWI();
  int wsI=getWSI();
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
      wsClickBtn();
      wClickBtn();
    }
    else
    {
      //стекло переключить
      wsClickBtn();
    }
  }

  setWheelIndicator(0);
  delay(50);
  setWheelIndicator(255);

  memoryMode(&wheel);
}

void wswSwitch1(int eventWheel)
{
  if(eventWheel==0 || eventWheel==1)
    return;

  int wI=getWI();
  int wsI=getWSI();
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

  setWheelIndicator(0);
  delay(100);
  setWheelIndicator(255);
  delay(100);

  memoryMode(&wheel);
}

void wClickBtn()
{
  digitalWrite(wheel.wSignal, HIGH);
  delay(50);
  digitalWrite(wheel.wSignal, LOW);
}

void wsClickBtn()
{
  digitalWrite(wheel.wsSignal, HIGH);
  delay(50);
  digitalWrite(wheel.wsSignal, LOW);
}

void nextMode(Seat* seat)
{
  seat->mode++;
  if(seat->mode>3)
    seat->mode=0;
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

void setEditFan(int mode, int editValue)
{
  int speed=0;
  if(mode==1)
  {
    speed=fanSpeeds.low+5*editValue;
  }
  else if(mode==2)
  {
    speed=fanSpeeds.mid+5*editValue;
  }
  else if(mode==3)
  {
    speed=fanSpeeds.high+5*editValue;
  }
  log("setEditFan", mode, speed);
  analogWrite(seat1.fanPwmPin, speed);
  analogWrite(seat2.fanPwmPin, speed);
}

//0-12; Each odd value gets blinking
void setBar(int bar)
{
  digitalWrite(seat1.lowLed, ((bar==1 & menuQuarz<10==0)|(bar>1))?HIGH:LOW);
  digitalWrite(seat1.midLed, ((bar==3 & menuQuarz<10==0)|(bar>3))?HIGH:LOW);
  digitalWrite(seat1.highLed, ((bar==5 & menuQuarz<10==0)|(bar>5))?HIGH:LOW);
  digitalWrite(seat2.highLed, ((bar==7 & menuQuarz<10==0)|(bar>7))?HIGH:LOW);
  digitalWrite(seat2.midLed, ((bar==9 & menuQuarz<10==0)|(bar>9))?HIGH:LOW);
  digitalWrite(seat2.lowLed, ((bar==11 & menuQuarz<10==0)|(bar>11))?HIGH:LOW);
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

int clamp(int val, int min, int max)
{
  return val<min?min:(val>max?max:val);
}

//Получить настройку из памяти
int getValue(int menu)
{
  if(menu==1)
    return memory.lowSpeed;
  if(menu==2)
    return memory.midSpeed;
  if(menu==3)
    return memory.highSpeed;
  if(menu==4)
    return memory.ventilationStateMemory;
  if(menu==5)
    return memory.wStateMemory;
  if(menu==6)
    return memory.wsStateMemory;
  if(menu==7)
    return memory.wswTypeOfSwitch;
  if(menu==8)
    return memory.autoVentilation;
  if(menu==9)
    return memory.autoWheel;
  if(menu==10)
    return memory.autoWindShield;
  if(menu==11)
    return memory.wellcomeState;
  if(menu==12)
    return 1;

  err("GetValue imposible", menu);
  return 0; //imposible
}

//Сохранить настройку в памяти
void saveValue(int menu)
{
  if(menu==1)
    memory.lowSpeed=editValue;
  if(menu==2)
    memory.midSpeed=editValue;
  if(menu==3)
    memory.highSpeed=editValue;
  if(menu==4)
    memory.ventilationStateMemory=editValue;
  if(menu==5)
    memory.wStateMemory=editValue;
  if(menu==6)
    memory.wsStateMemory=editValue;
  if(menu==7)
    memory.wswTypeOfSwitch=editValue;
  if(menu==8)
    memory.autoVentilation=editValue;
  if(menu==9)
    memory.autoWheel=editValue;
  if(menu==10)
    memory.autoWindShield=editValue;
  if(menu==11)
    memory.wellcomeState=editValue;
  if(menu==12)
  {
    selfTest();
    return;
  }

  EEPROM.put(0, memory);

  setVentilation(seat1);
  setVentilation(seat2);
}

//Сохранить состояние вентиляций
void memoryMode(Seat* seat)
{
  if(memory.ventilationStateMemory!=1)
    return;
  if(seat==&seat1)
  {
    if(memory.ventilationState1 != seat->mode)
    {
      memory.ventilationState1=seat->mode;
      EEPROM.put(0, memory);
    }
  }
  else if(seat==&seat2)
  {
    if(memory.ventilationState2 != seat->mode)
    {
      memory.ventilationState2=seat->mode;
      EEPROM.put(0, memory);
    }
  }
}

//Сохранить состояние руля и лобаша
void memoryMode(Wheel* wheel)
{
  bool hadChanged = memory.wsState != getWSI() || memory.wState != getWI();
  memory.wsState = getWSI();
  memory.wState = getWI();
  if(memory.wsStateMemory==1 && hadChanged)
    EEPROM.put(0, memory);
}

void selfTest()
{

}

void log(String msg, int val)
{
  Serial.print(msg+": ");
  Serial.println(val);
}

void log(String msg, int val, int val2)
{
  Serial.print(msg+": ");
  Serial.print(val);
  Serial.print(", ");
  Serial.println(val2);
}

void err(String msg, int val)
{
  Serial.print("ERR: " + msg + ": ");
  Serial.println(val);
}