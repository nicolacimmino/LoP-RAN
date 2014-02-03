
#ifndef __LopDia_h__
#define __LopDia_h__

void dia_logTime();
void dia_logString(char* string);
void dia_logInt(int value);
void dia_closeLog();
void dia_simpleFormNumericLog(const char* event, int count, ...);
void dia_simpleFormTextLog(const char* event, const char* text);

#endif


