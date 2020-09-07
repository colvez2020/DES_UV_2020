#include <Arduino.h>
#include "LAMP_UTIL.h"

//Led12V
#define LED12V_UC           22
#define LedDelay12V         500                          



//Lampara
#define Lamp_RELAY_02_UC    25 
#define Lamp_RELAY_01_UC    24
#define Lamp_RELAY_00_UC    23
#define LampDelay           1000                          


boolean Alterna_flag12V=true;
boolean Alterna_flagUVC=true;
unsigned long lastPingled12V;
unsigned long lastPingLampUVC;
int contador_prueba_actual;

void Lamp_ini(void)
{
  pinMode(LED12V_UC, OUTPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  pinMode(Lamp_RELAY_02_UC, OUTPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  pinMode(Lamp_RELAY_01_UC, OUTPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  pinMode(Lamp_RELAY_00_UC, OUTPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  digitalWrite(Lamp_RELAY_02_UC,LOW);
  digitalWrite(Lamp_RELAY_01_UC,LOW);
  digitalWrite(Lamp_RELAY_00_UC,LOW);
  lastPingled12V=millis();                             //Punto de inico para el tiempo   
  lastPingLampUVC=millis();
  contador_prueba_actual=0; 
}

void Control_Lamp12V(void)
{
  if (millis() - lastPingled12V >= LedDelay12V)
  {
    if(Alterna_flag12V==true)
    {
      digitalWrite(LED12V_UC, HIGH);    // HIGH pulse for at least 10µs
      Alterna_flag12V=false;
    }
    else
    {
     digitalWrite(LED12V_UC, LOW);    // HIGH pulse for at least 10µs
      Alterna_flag12V=true;
    }
    lastPingled12V = millis();
  }
}


void Control_UVC(char cmd)
{
    if(cmd==1)
    {      
      digitalWrite(Lamp_RELAY_00_UC,HIGH);
      delay(200);
      digitalWrite(Lamp_RELAY_01_UC,HIGH);
      delay(200);
      digitalWrite(Lamp_RELAY_02_UC,HIGH);
      delay(200);
    }
    else
    {
      digitalWrite(Lamp_RELAY_00_UC,LOW);
      digitalWrite(Lamp_RELAY_01_UC,LOW);
      digitalWrite(Lamp_RELAY_02_UC,LOW);
    }
}


void test_UVC(int Balastro_N, int ON_OFFdelay, int Numero_de_pruebas)
{
  //if(contador_prueba_actual<Numero_de_pruebas)
  {
    if (millis() - lastPingLampUVC >= ON_OFFdelay)
    {
      if(Alterna_flagUVC==true)
      {
        if(Balastro_N==1)
        {
          digitalWrite(Lamp_RELAY_00_UC,HIGH);
        }
        else if(Balastro_N==2)
        {
          digitalWrite(Lamp_RELAY_01_UC,HIGH);
        }
        else if(Balastro_N==3)
        {
          digitalWrite(Lamp_RELAY_02_UC,HIGH);
        }
        Alterna_flagUVC=false;
      }
      else
      {
        if(Balastro_N==1)
        {
          digitalWrite(Lamp_RELAY_00_UC,LOW);
        }
        else if(Balastro_N==2)
        {
          digitalWrite(Lamp_RELAY_01_UC,LOW);
        }
        else if(Balastro_N==3)
        {
          digitalWrite(Lamp_RELAY_02_UC,LOW);
        }
        Alterna_flagUVC=true;
      }
      lastPingLampUVC = millis();
    }
  }
 /* else
  {
    Control_UVC(0);
  }*/
  contador_prueba_actual++;
  Serial.println("actual");
  Serial.println(contador_prueba_actual);
  Serial.println("fin");
  Serial.println(Numero_de_pruebas);

}
