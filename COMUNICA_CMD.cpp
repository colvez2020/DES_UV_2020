#include "COMUNICA_CMD.h"
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

//RF24
#define CNS_RF24             49
#define IRQ_RF24             3
#define CE_RF24              48

#define COMU_TEST

const int pinCE = CE_RF24;
const int pinCSN = CNS_RF24;
RF24 radio(pinCE, pinCSN);
const uint64_t pipe = 0xE8E8F0F0E1LL;
char data_RF;

boolean Mensaje_disc=false;
int     Mensaje_disc_contador=0;
HardwareSerial & ComandoSerial = Serial1;
HardwareSerial & TabletaSerial = Serial2;

////////////////////////GENERAL///////////////////////////////////////////
void COMUNI_ini(void)
{
  Serial.begin(9600);
  #ifndef AT_MODE
  ComandoSerial.begin(9600);
  #endif
  #ifdef AT_MODE
  ComandoSerial.begin(38400);
  #endif

  TabletaSerial.begin(9600);
  
  while (!Serial) {
    ; // Wait for Serial
  }
  while (!ComandoSerial) {
    ; // Wait for Serial
  }
  while (!TabletaSerial) {
    ; // Wait for Serial
  }
  #ifdef COMU_TEST
    Serial.println("--- Serial monitor started ---");
  #endif
}

boolean CMD_valid(char CMD)
{
  switch(CMD) //donde opción es la variable a comparar
  {
    case 'B': //Bloque de instrucciones 1;
    case 'D': //Bloque de instrucciones 1;
    case 'I': //Bloque de instrucciones 1;
    case 'A': //Bloque de instrucciones 1;
    case 'P': //Bloque de instrucciones 1;
    case 'O':
    case 'F':
    case 0x01:
    case 0x03:
       return true;
    break;
  }
  return false;
}

///////////////////////////RF//////////////////////////////////////////
boolean RF24_Ini(void)
{
  if(!Test_RF())
    return false;
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  return true;
}

boolean CMD_RF24(char *CMD_RF24)
{
  if(radio.isChipConnected())
  {
    if (radio.available())
    {
        radio.read(CMD_RF24,1);
        if(CMD_valid(*CMD_RF24))
        { 
           #ifdef COMU_TEST
            Serial.print("RF "); 
            Serial.println(*CMD_RF24);
           #endif
           return true;
        }
    }    
  }
  return false;
}

boolean Test_RF(void)
{
  radio.begin();
  if(radio.isChipConnected())
  {
      Serial.println("Spi OK");
      if(radio.isPVariant())
      Serial.println("Chip Original");
      return true;
  }
  else
  {
    Serial.println("Spi ERROR");
    return false;
  }  
}

/////////////////////////////////////////BLUETHOOT//////////////////////////////////////////
boolean CMD_bluethoot(char *CMD_bluethoot)
{
  if (ComandoSerial.available())
  {
    //leeemos la opcion
    *CMD_bluethoot= ComandoSerial.read();
    #ifdef COMU_TEST
      Serial.print("Blue=");
      Serial.println(*CMD_bluethoot);
    #endif
    if(*CMD_bluethoot=='+')
      Mensaje_disc=true;
      
    if(Mensaje_disc==true)
    {
      Mensaje_disc_contador++;
      if(Mensaje_disc_contador==16)
      {
        Mensaje_disc=false;
        Mensaje_disc_contador=0;
      }    
    }
    else
    {
      #ifdef COMU_TEST
        //Serial.print("modo_normal");
      #endif
      if(CMD_valid(*CMD_bluethoot))
        return true;
    }
  }
  return false;  
}

void Print_ON_BLUETHOOT_float(float cadena)
{
   ComandoSerial.print(cadena);              
}

void Print_ON_BLUETHOOT_int(int cadena)
{
   ComandoSerial.print(cadena);              
}
void Print_ON_BLUETHOOT_char(char cadena)
{
   ComandoSerial.print(cadena);              
}

void AT_Bluethoot(void)
{
    
  do{
    if (ComandoSerial.available())    // read from HC-05 and send to Arduino Serial Monitor
      Serial.write(ComandoSerial.read());

    if (Serial.available())     // Keep reading from Arduino Serial Monitor and send to HC-05
      ComandoSerial.write(Serial.read());      
  }while(1);

}

///////////////////////////////////////////TABLETA//////////////////////////////////////////
boolean CMD_tableta(char *CMD_tableta)
{
   if (TabletaSerial.available()>0)
  {
    *CMD_tableta = TabletaSerial.read();
    #ifdef COMU_TEST
      Serial.print("Tableta=");
      Serial.print(*CMD_tableta);
      Serial.print(" HEX=");
      Serial.print(*CMD_tableta,HEX);
    #endif
    if(CMD_valid(*CMD_tableta))
        return true;
  }  
  return false;
}


void Print_ON_Tableta(char* cadena)
{
   TabletaSerial.print(cadena);               
}
