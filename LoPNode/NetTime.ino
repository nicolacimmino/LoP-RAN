
NetTime getNetworkTime(void)
{
  NetTime time;
  unsigned long timemS = millis();
  
  time.off = timemS % 100;
  time.slot = (timemS / 100) % 10;
  time.frame = (timemS / 1000 ) % 10;
  time.block = (timemS / 10000) % 6;
  
  return time;
}
