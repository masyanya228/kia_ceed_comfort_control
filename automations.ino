//Вентиляция водителя по темпратуре при запуске
//1: 14
//12: 36
bool IsNeedVentilationByTemp()
{
  log("autoVentilation", memory.autoVentilation);
  if(memory.autoVentilation == clamp(memory.autoVentilation, 1, 12))
  {
    log("temp target/real", (12 + memory.autoVentilation*2), getTemp());
    return (12 + memory.autoVentilation*2) < getTemp();
  }
  else
    return false;
}

//Обогрев руля по темпратуре при запуске
//1: 12
//12: -10
bool IsNeedWByTemp()
{
  log("autoWheel", memory.autoWheel);
  if(memory.autoWheel == clamp(memory.autoWheel, 1, 12))
  {
    log("temp target/real", (14 + memory.autoWheel*-2), getTemp());
    return (14 + memory.autoWheel*-2) > getTemp();
  }
  else
    return false;
}

//Обогрев лобового стекла по темпратуре при запуске
//1: -1
//12: -12
bool IsNeedWSByTemp()
{
  log("autoWindShield", memory.autoWindShield);
  if(memory.autoWindShield == clamp(memory.autoWindShield, 1, 12))
  {
    log("temp target/real", (0 + memory.autoWindShield*-1), getTemp());
    return (0 + memory.autoWindShield*-1) > getTemp();
  }
  else
    return false;
}

//Управляет автоматическим включением вентиляции, обогрева руля и обогрева лобового стекла пи запуске авто.
void AutoOn()
{
  getWWSI();
  log("Current vent1 mode", seat1.mode);
  if(seat1.mode==0 && IsNeedVentilationByTemp())
  {
    seat1.mode=1;
    log("vent1 auto ON: mode 1", getTemp());
    setVentilation(seat1);
  }
  else if(seat1.mode==0 && memory.ventilationStateMemory)
  {
    seat1.mode=memory.ventilationState1;
    log("vent1 memory ON", seat1.mode);
    setVentilation(seat1);
  }

  log("Current wheel mode", getWI());
  if(!getWI() && IsNeedWByTemp())
  {
    log("wheel auto ON", getTemp());
    wClickBtn();
  }
  else if(!getWI() && memory.wStateMemory && memory.wState==1)
  {
    log("wheel memory ON", 1);
    wClickBtn();
  }

  log("Current wind shield mode", getWSI());
  if(!getWSI() && IsNeedWSByTemp())
  {
    log("wind shield auto ON", getTemp());
    wsClickBtn();
  }
  else if(!getWSI() && memory.wsStateMemory && memory.wsState==1)
  {
    log("wind shield memory ON", 1);
    wsClickBtn();
  }
}
