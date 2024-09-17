#include <EEPROM.h>

int menu=0;//0-def; 1-speedLow; 2-speedMid; 3-speedHigh; 4-wellcome; 5-ventilationMemory; 6-windshieldMemory
bool isMenuEdit=false;
int editValue=0;
int menuQuarz=0;

struct Memory{
  int lowSpeed=10;
  int midSpeed=8;
  int highSpeed=6;
  int wellcomeState=1;
  int ventilationStateMemory=1;
  int ventilationState1=1;
  int ventilationState2=2;
  int windshieldStateMemory=0;
  int windshieldState=0;
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
  int led=A3;
  int ledMode=0;//0-idle; 1-light; 2-50-100 blind; 3-0-50 blind
  int wSignal;
  int wsSignal;
  int wIndicator;
  int wsIndicator;
} wheel;

void setup()
{
  delay(2000);
  Serial.begin(9600);

  seat1.btnPin=A0;
  seat1.fanPwmPin=5;
  seat1.lowLed=6;
  seat1.midLed=7;
  seat1.highLed=8;

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

  EEPROM.get(0, memory);
  if(memory.lowSpeed==255)
  {
    Memory newMemory;
    EEPROM.put(0, newMemory);
    Serial.println("new memory");
  }

  Serial.println(memory.lowSpeed);
  Serial.println(memory.midSpeed);
  Serial.println(memory.highSpeed);

  if(memory.wellcomeState==1)
    wellcome();
    
  if(memory.ventilationStateMemory)
  {
    seat1.mode=memory.ventilationState1;
    seat2.mode=memory.ventilationState2;
    setVentilation(seat1);
    setVentilation(seat2);
  }
}

void loop()
{
  int event1=getBtnEvent(&seat1);
  int event2=getBtnEvent(&seat2);
  int eventWheel=getBtnEvent(&wheel);//micro data
  if(event1!=0) Serial.print(event1);
  if(event2!=0) Serial.print(event2);
  if(eventWheel!=0) Serial.print(eventWheel);
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
      if(menu<1) menu=6;
    }
    if(event2==-1)
    {
      menu++;
      if(menu>6) menu=1;
    }
    if(event1==2)
    {
      isMenuEdit=true;
      editValue=getValue(menu);
    }
    setBar(menu*2-1);

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
      editValue=clamp(--editValue, 0, menu<4?12:1);
    }
    if(event2==-1)
    {
      editValue=clamp(++editValue, 0, menu<4?12:1);
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

  if(eventWheel==-1)
  {
    int wI=digitalRead(wheel.wIndicator);
    int wsI=digitalRead(wheel.wsIndicator);
    if(wI==LOW && wsI==LOW)
    {
      //руль
      wheel.ledMode=1;
    }
    else if(wI==HIGH && wsI==LOW)
    {
      //выкл
      wheel.ledMode=0;
    }
    else if(wI==LOW && wsI==HIGH)
    {
      //руль+стекло
      wheel.ledMode=2;
    }
    else if(wI==HIGH && wsI==HIGH)
    {
      //стекло
      wheel.ledMode=3;
    }
  }
  else if(eventWheel==2)
  {
    int wI=digitalRead(wheel.wIndicator);
    int wsI=digitalRead(wheel.wsIndicator);
    if(wI==LOW && wsI==LOW)
    {
      //руль+стекло
      wheel.ledMode=2;
    }
    else if(wI==HIGH && wsI==LOW)
    {
      //руль+стекло
      wheel.ledMode=2;
    }
    else if(wI==LOW && wsI==HIGH)
    {
      //выкл
      wheel.ledMode=0;
    }
    else if(wI==HIGH && wsI==HIGH)
    {
      //руль
      wheel.ledMode=1;
    }
  }
  if(wheel.ledMode==0)
  {
    analogWrite(wheel.led, 0);
  }
  else if(wheel.ledMode==1)
  {
    analogWrite(wheel.led, 255);
  }
  else if(wheel.ledMode==2)
  {
    analogWrite(wheel.led, 128+127/20*menuQuarz);
  }
  else if(wheel.ledMode==3)
  {
    analogWrite(wheel.led, 127/20*menuQuarz);
  }

  //check menu explorer
  //check edit value
  menuQuarz++;
  if(menuQuarz>20) menuQuarz=0;
  delay(50);
}

//0-idle; 1-down; 2-long; -1-up
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

//0-idle; 1-down; 2-long; -1-up
int getBtnEvent(Wheel* wheel)
{
  int pinValue=analogRead(wheel->btn);
  if(pinValue<500)
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
  Serial.println(speed);
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
}

int clamp(int val, int min, int max)
{
  return val<min?min:(val>max?max:val);
}

int getValue(int menu)
{
  if(menu==1)
    return memory.lowSpeed;
  if(menu==2)
    return memory.midSpeed;
  if(menu==3)
    return memory.highSpeed;
  if(menu==4)
    return memory.wellcomeState;
  if(menu==5)
    return memory.ventilationStateMemory;
  if(menu==6)
    return memory.windshieldStateMemory;

  return 0; //imposible
}

void saveValue(int menu)
{
  if(menu==1)
    memory.lowSpeed=editValue;
  if(menu==2)
    memory.midSpeed=editValue;
  if(menu==3)
    memory.highSpeed=editValue;
  if(menu==4)
    memory.wellcomeState=editValue;
  if(menu==5)
    memory.ventilationStateMemory=editValue;
  if(menu==6)
    memory.windshieldStateMemory=editValue;

  EEPROM.put(0, memory);

  setVentilation(seat1);
  setVentilation(seat2);
}

void memoryMode(Seat* seat)
{
  if(seat==&seat1)
    memory.ventilationState1=seat->mode;
  else if(seat==&seat2)
    memory.ventilationState2=seat->mode;

  if(memory.ventilationStateMemory==1)
    EEPROM.put(0, memory);
}