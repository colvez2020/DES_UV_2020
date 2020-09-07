#ifndef SENSOR_UTIL_H
#define SENSOR_UTIL_H

#include <Arduino.h>

#define ADELANTE           2
#define IZQUIERA           3
#define DERECHA            1
#define ATRAS              4
#define PARO               5


void Analog_ini(void);

void Sonic_ini(void);
float Sensor_Sonic_medir(int* Sensor_Posicion);
boolean Sonic_precacion_choque(char Direccion_mov);
void Test_SensoresSonic(void);

float Sensor_CorrienteBAT(void);
float Sensor_VoltajeBAT(void);
void  Sensor_TempBAT(float*BATA,float *BATB);
boolean Read_sensor_analog(float *VoltajeBAT,float *CorrienteBAT,float *TempA,float *TemB);
int Estimar_capacidad_4LED(float votaje_bat);
int Estimar_capacidad_MAP(float votaje_bat);



#endif
