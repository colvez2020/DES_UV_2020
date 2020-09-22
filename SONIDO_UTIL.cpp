#include <Arduino.h>
#include "SONIDO_UTIL.h"


//Alarma
#define SPK_ALARMA_UC       26


void SPK_ini(void)
{
  pinMode(SPK_ALARMA_UC, OUTPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  digitalWrite(SPK_ALARMA_UC,LOW);
}

void Sonido_Beep_lamp(void)
{
   tone(SPK_ALARMA_UC,2000,100);
   delay(250);
   tone(SPK_ALARMA_UC,1000,100);
   delay(250);
   tone(SPK_ALARMA_UC,500,100);
   delay(250);
}


void Sonido_Beep_reset(void)
{
   tone(SPK_ALARMA_UC,3000,20);
   delay(250);
}

void Sonido_Beep_choque(void)
{
   tone(SPK_ALARMA_UC,2000,100);
   delay(250);
}

 
