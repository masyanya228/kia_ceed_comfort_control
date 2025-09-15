//Настраивает пины вводы и вывода информации
void SetupPins()
{
  seat1.btnPin=SEAT_1_BTN;
  seat1.fanPwmPin=FAN_1_CONTROL;
  seat1.lowLed=IND_1_LOW;
  seat1.midLed=IND_1_MID;
  seat1.highLed=IND_1_HIGH;

  seat2.btnPin=SEAT_2_BTN;
  seat2.fanPwmPin=FAN_2_CONTROL;
  seat2.lowLed=IND_2_LOW;
  seat2.midLed=IND_2_MID;
  seat2.highLed=IND_2_HIGH;

  wheel.btn=WHEEL_BTN;
  wheel.led = WHEEL_BTN_LED;
  wheel.wSignal = WHEEL_SIG;
  wheel.wsSignal = WIND_SHEILD_SIG;
  wheel.wIndicator = WHEEL_IND;
  wheel.wsIndicator = WIND_SHIELD_IND;

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
  pinMode(wheel.wsSignal, OUTPUT);
  
  pinMode(DHT22_PIN, INPUT);
  
  digitalWrite(wheel.wSignal, LOW);
  digitalWrite(wheel.wsSignal, LOW);
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

//Подстройка скорости работы вентиляции
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
