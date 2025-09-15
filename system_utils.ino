
int clamp(int val, int min, int max)
{
  return val<min?min:(val>max?max:val);
}

//Провести самодиагностику
void selfTest()
{
  int prevVent1=seat1.mode;
  int prevVent2=seat2.mode;
  int prevW=getWI();
  int prevWS=getWSI();

  seat1.mode=0;
  setVentilation(seat1);
  seat2.mode=0;
  setVentilation(seat2);
  if(prevW)
    wClickBtn();
  if(prevWS)
    wsClickBtn();
  
  BlinkBar(12, 3, 1000);
  
  //check wheel
  wClickBtn();
  delay(1000);
  if(getWI())
  {
    setWheelIndicator(255);
    delay(2000);
    setWheelIndicator(0);
    wClickBtn();
  }
  else
  {
    //3 short, 1 long
    setWheelIndicator(255);
    delay(200);
    setWheelIndicator(0);
    delay(200);
    setWheelIndicator(255);
    delay(200);
    setWheelIndicator(0);
    delay(200);
    setWheelIndicator(255);
    delay(200);
    setWheelIndicator(0);
    delay(200);

    setWheelIndicator(255);
    delay(2000);
    setWheelIndicator(0);
  }

  //check wind shield
  wsClickBtn();
  delay(1000);
  if(getWSI())
  {
    setWheelIndicator(255);
    delay(2000);
    setWheelIndicator(0);
    wsClickBtn();
  }
  else
  {
    //3 short, 2 long
    setWheelIndicator(255);
    delay(200);
    setWheelIndicator(0);
    delay(200);
    setWheelIndicator(255);
    delay(200);
    setWheelIndicator(0);
    delay(200);
    setWheelIndicator(255);
    delay(200);
    setWheelIndicator(0);
    delay(200);

    setWheelIndicator(255);
    delay(2000);
    setWheelIndicator(0);
    delay(1000);
    setWheelIndicator(255);
    delay(2000);
    setWheelIndicator(0);
  }

  //vent 1 LOW
  seat1.mode=1;
  setVentilation(seat1);

  delay(3000);

  //vent 1 MID
  seat1.mode=2;
  setVentilation(seat1);

  delay(3000);

  //vent 1 HIGH
  seat1.mode=3;
  setVentilation(seat1);

  delay(3000);

  //vent 1 OFF
  seat1.mode=0;
  setVentilation(seat1);

  delay(1000);

  //vent 2 LOW
  seat2.mode=1;
  setVentilation(seat2);

  delay(3000);

  //vent 2 MID
  seat2.mode=2;
  setVentilation(seat2);

  delay(3000);

  //vent 2 HIGH
  seat2.mode=3;
  setVentilation(seat2);

  delay(3000);

  //vent 2 OFF
  seat2.mode=0;
  setVentilation(seat2);

  delay(1000);

  //vent 1 + 2 HIGH
  seat1.mode=3;
  setVentilation(seat1);
  seat2.mode=3;
  setVentilation(seat2);
  delay(6000);
  
  ShowTemp();

  //Set as before
  seat1.mode=prevVent1;
  setVentilation(seat1);
  seat2.mode=prevVent2;
  setVentilation(seat2);
  if(prevW)
    wClickBtn();
  if(prevWS)
    wsClickBtn();
}

//Показывает текущую температуру в полу салона
void ShowTemp()
{
  int curTemp = GetTemp();
  if(curTemp < 0)
  {
    //left LED
    BlinkBar(2, 1, 1000);
  }
  else
  {
    //right LED
    BlinkBar(12, 1, 1000);
  }

  delay(1000);
  int firstDigit=curTemp/10;
  BlinkBar(firstDigit*2, 1, 1000);

  delay(1000);
  int lastDigit=curTemp-firstDigit*10;
  BlinkBar(lastDigit, 1, 1000);
}

//Возвращает текущую температура с встроенного датчика
int GetTemp()
{
  return dht22.readTemperature();
}
