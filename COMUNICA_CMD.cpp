#include "COMUNICA_CMD.h"
#include "SONIDO_UTIL.h"
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

//WIFI
char WIFI_COMUNU_ATEMP=0;
boolean WIFI_CONFIG_OK=true;

//RF24
#define CNS_RF24             49
#define IRQ_RF24             3
#define CE_RF24              48
#define WIFI_RESET           46
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
HardwareSerial & WIFISerial =    Serial3;
////////////////////////GENERAL///////////////////////////////////////////
void COMUNI_ini(void)
{
  pinMode(WIFI_RESET, OUTPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  digitalWrite(WIFI_RESET,LOW);
  Serial.begin(9600);
  #ifndef AT_MODE
  ComandoSerial.begin(9600);
  #endif
  #ifdef AT_MODE
  ComandoSerial.begin(38400);
  #endif
  TabletaSerial.begin(9600);
  WIFISerial.begin(9600);
  
  while (!Serial) {
    ; // Wait for Serial
  }
  while (!ComandoSerial) {
    ; // Wait for Serial
  }
  while (!TabletaSerial) {
    ; // Wait for Serial
  }

  while (!WIFISerial) {
    ; // Wait for Serial
  }
  #ifdef COMU_TEST
    Serial.println("--- Serial monitor started ---");
  #endif
  delay(500);
  delay(500);
  delay(500);
  digitalWrite(WIFI_RESET,HIGH);
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
    case 'W':
    case 'S':
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
      Serial.println(*CMD_tableta,HEX);
    #endif
    if(*CMD_tableta<0x01 && *CMD_tableta>0x03)
      return false;
    if(CMD_valid(*CMD_tableta))
        return true;
  }  
  return false;
}

void Print_SENSORDATA_ON_WIFI(char op,int Data_int, float Data_float, char Data_type)
{
  long timeout=2000;
  String OutData;
  char ACK;

  if(WIFI_CONFIG_OK==true)
  {
    if(op==1)
      OutData=String(Data_type)+String(Data_int)+String('!');
    else
      OutData=String(Data_type)+String(Data_float)+String('!');
    Serial.print("dato a mandar =");
    Serial.println(OutData);
    WIFISerial.println(OutData);
    do{
      ACK=WIFISerial.read();
      delay(1);
      timeout++;
      if(timeout>3000)
        break;
    }while(ACK!='K');
    if(ACK=='K')
    {
      Serial.println("WIFISEND_OK");
      WIFI_COMUNU_ATEMP=0;
    }
    else
    {
      Serial.println("WIFISEND_BAD");
      WIFI_COMUNU_ATEMP++;
    }
  
    if(WIFI_COMUNU_ATEMP==5)
    {
      WIFI_8266_RESET();
      WIFI_COMUNU_ATEMP=0;
    }  
  }
    
}

void Reconfig_WIFI(void)
{
  WIFISerial.println('W');
  WIFISerial.println('W');
  WIFI_CONFIG_OK=false;
  Sonido_Beep_choque();
}

void WIFI_Send_START(void)
{
  WIFI_CONFIG_OK=true;
}

void WIFI_8266_RESET(void)
{
  digitalWrite(WIFI_RESET,LOW);
  Sonido_Beep_reset();
  Serial.println("RESET_WIFI");
  delay(500);
  digitalWrite(WIFI_RESET,HIGH);
}


void Print_ON_Tableta(char* cadena)
{
   TabletaSerial.print(cadena);               
}
