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
