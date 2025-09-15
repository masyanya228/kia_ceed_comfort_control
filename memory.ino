//Считывает сохраннные настройки и проверяет данные на валидность
void ReadCheckMemory()
{
  EEPROM.get(0, memory);
  if(memory.lowSpeed==255 || memory.lowSpeed==-1)
  {
    log("New memory", 0);
    Memory newMemory;
    EEPROM.put(0, newMemory);
    memory=newMemory;
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
