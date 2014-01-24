typedef struct
{
   byte off;
   byte frame;
   byte block;
   byte slot; 
} NetTime, *pNetTime;

NetTime getNetworkTime(void);


