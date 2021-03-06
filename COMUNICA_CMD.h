#ifndef COMUNICA_CMD_H
#define COMUNICA_CMD_H

#include <Arduino.h>

#define WIFI_ACK_NO_RESPONCE 1
#define WIFI_ACK_OK          2
#define WIFI_NO_CONFIG       3

void COMUNI_ini(void);
boolean CMD_valid(char CMD);

boolean RF24_Ini(void);
boolean CMD_RF24(char *CMD_RF24);
boolean Test_RF(void);

boolean CMD_bluethoot(char *CMD_bluethoot);
void Print_ON_BLUETHOOT_float(float cadena);
void Print_ON_BLUETHOOT_char(char cadena);
void Print_ON_BLUETHOOT_int(int cadena);

char Print_SENSORDATA_ON_WIFI(char op,int Data_int, float Data_float, char Data_type);
void Reconfig_WIFI(void);
void WIFI_Send_START(void);
boolean WIFI_8266_RESET(void);

void AT_Bluethoot(void);

boolean CMD_tableta(char *CMD_tableta);
void Print_ON_BLUETHOOT(float cadena);




#endif
