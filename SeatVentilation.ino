
int menu=0;//0-def; 1-speedLow; 2-speedMid; 3-speedHigh; 4-wellcome; 5-ventilationMemory; 6-windshieldMemory
bool isMenuEdit=false;
int editValue=0;
int menuQuarz=0;

struct Memory{
  int lowSpeed=10;
  int midSpeed=8;
  int highSpeed=6;
  int wellcomeState;
  int ventilationStateMemory;
  int ventilationState1;
  int ventilationState2;
  int windshieldStateMemory;
  int windshieldState;
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
  int low=155;
  int mid=175;
  int high=195;
} fanSpeeds;

Seat seat1;
Seat seat2;

struct WheelHeat{
  int heatBtn;
  int led;
  int wSignal;
  int wsSignal;
  int wIndicator;
  int wsIndicator;
} wheelHeat;

void setup()
{
  Serial.begin(9600);

  seat1.btnPin=A0;
  seat1.fanPwmPin=4;
  seat1.lowLed=6;
  seat1.midLed=7;
  seat1.highLed=8;

  seat2.btnPin=A1;
  seat2.fanPwmPin=5;
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

  //read memory
  wellcome();
}

void loop()
{
  int event1=getBtnEvent(&seat1);
  if(event1!=0)
  {
    Serial.println(event1);
  }
  int event2=getBtnEvent(&seat2);
  if(menu==0)
  {
    if(event1==-1)
    {
      nextMode(&seat1);
      Serial.println(seat1.mode);
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
    setBar(menu*2);

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
    if(event1==2)
    {
      isMenuEdit=false;
      saveValue(menu);
    }
    setBar(editValue);
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

void nextMode(Seat* seat)
{
  seat->mode++;
  if(seat->mode>3)
    seat->mode=0;
  setVentilation(*seat);
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
    speed=fanSpeeds.low+50*memory.lowSpeed;
  }
  else if(seat.mode==2)
  {
    speed=fanSpeeds.mid+50*memory.midSpeed;
  }
  else if(seat.mode==3)
  {
    speed=fanSpeeds.high+50*memory.highSpeed;
  }
  analogWrite(seat.fanPwmPin, speed);
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
  delay(100);
  digitalWrite(seat1.midLed, HIGH);
  digitalWrite(seat2.midLed, HIGH);
  delay(70);
  digitalWrite(seat1.highLed, HIGH);
  digitalWrite(seat2.highLed, HIGH);
  delay(50);
  digitalWrite(seat1.highLed, LOW);
  digitalWrite(seat2.highLed, LOW);
  delay(70);
  digitalWrite(seat1.midLed, LOW);
  digitalWrite(seat2.midLed, LOW);
  delay(100);
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

  //saveToEEPROM

  setVentilation(seat1);
  setVentilation(seat2);
}