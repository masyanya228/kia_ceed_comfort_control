
void log(int val)
{
  if(!isDebug) return;
  Serial.println(val);
}

void log(String msg, float val)
{
  if(!isDebug) return;
  Serial.print(msg+": ");
  Serial.println(val);
}

void log(String msg, int val)
{
  if(!isDebug) return;
  Serial.print(msg+": ");
  Serial.println(val);
}

void log(String msg, uint16_t val)
{
  if(!isDebug) return;
  Serial.print(msg+": ");
  Serial.println(val);
}

void log(String msg, int val, int val2)
{
  if(!isDebug) return;
  Serial.print(msg+": ");
  Serial.print(val);
  Serial.print(", ");
  Serial.println(val2);
}

void err(String msg, int val)
{
  if(!isDebug) return;
  Serial.print("ERR: " + msg + ": ");
  Serial.println(val);
}
