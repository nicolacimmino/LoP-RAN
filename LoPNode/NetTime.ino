

long off_off = 0;

NetTime getNetworkTime(void)
{
  NetTime time;
  unsigned long timemS = millis() - off_off;
  
  time.off = timemS % 100;
  time.slot = (timemS / 100) % 10;
  time.frame = (timemS / 1000 )  % 10;
  time.block = (timemS / 10000)  % 6;
  
  return time;
}

void setNetworkTime(byte block, byte frame, byte slot)
{
  unsigned long wantedMillis = block * 10000l + frame * 1000l + slot * 100l; 
  off_off = (millis() % 60000) - wantedMillis;
}
