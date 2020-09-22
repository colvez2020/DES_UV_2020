#include "COMUNICA_CMD.h"
#include "SONIDO_UTIL.h"



#define COMU_TEST

#define WIFI_RESET           46
#define WIFI_RESET_DELAY     1000


//WIFI
char WIFI_COMUNU_ATEMP=0;
boolean WIFI_CONFIG_OK=true;
boolean WIFI_INICIO_RESET_FLAG=true;
unsigned long lastResetMillis;

//BLUETHOOT
boolean Mensaje_disc=false;
int     Mensaje_disc_contador=0;

HardwareSerial & BluethootSerial = Serial1;
HardwareSerial & TabletaSerial = Serial2;
HardwareSerial & WIFISerial =    Serial3;






/////////////////////////////////////////BLUETHOOT//////////////////////////////////////////
boolean CMD_bluethoot(char *CMD_bluethoot)
{
  if (BluethootSerial.available())
  {
    //leeemos la opcion
    *CMD_bluethoot= BluethootSerial.read();
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
   BluethootSerial.print(cadena);              
}

void Print_ON_BLUETHOOT_int(int cadena)
{
   BluethootSerial.print(cadena);              
}
void Print_ON_BLUETHOOT_char(char cadena)
{
   BluethootSerial.print(cadena);              
}

void AT_Bluethoot(void)
{
    
  do{
    if (BluethootSerial.available())    // read from HC-05 and send to Arduino Serial Monitor
      Serial.write(BluethootSerial.read());

    if (Serial.available())     // Keep reading from Arduino Serial Monitor and send to HC-05
      BluethootSerial.write(Serial.read());      
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


void Print_ON_Tableta(char* cadena)
{
   TabletaSerial.print(cadena);               
}


//////////////////////////////////WIFI///////////////////////////////////////////
//////////////////////////////////WIFI///////////////////////////////////////////
//////////////////////////////////WIFI///////////////////////////////////////////
//////////////////////////////////WIFI///////////////////////////////////////////

char Print_SENSORDATA_ON_WIFI(char op,int Data_int, float Data_float, char Data_type)
{
  long timeout=2000;
  String OutData;
  char ACK='F';

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
      if(WIFISerial.available())
        ACK=WIFISerial.read();
      delay(1);
      timeout++;
      if(timeout>3000) //3 segundos
      {
        Serial.println("WIFISEND_BAD");
        return WIFI_ACK_NO_RESPONCE;
      }
    }while(ACK!='K');
    Serial.println("WIFISEND_OK");
    return WIFI_ACK_OK;
  }
  return WIFI_NO_CONFIG;
}

/*
 *       WIFI_COMUNU_ATEMP=0;
    if(WIFI_COMUNU_ATEMP==5)
    {
      WIFI_8266_RESET();
      WIFI_COMUNU_ATEMP=0;
    }  
 */
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


boolean WIFI_8266_RESET(void)
{
  digitalWrite(WIFI_RESET,LOW);
  if(WIFI_INICIO_RESET_FLAG==true)
  {
    WIFI_INICIO_RESET_FLAG=false;
    lastResetMillis=millis();
  }

  if (millis() - lastResetMillis >= WIFI_RESET_DELAY)
  {
    WIFI_INICIO_RESET_FLAG=true;
    digitalWrite(WIFI_RESET,HIGH);
    Sonido_Beep_reset();
    Serial.println("RESET_WIFI");
    return true;
  }
  return false;
}


//////////////////////////////////GENERAL///////////////////////////////////////////
//////////////////////////////////GENERAL///////////////////////////////////////////
//////////////////////////////////GENERAL///////////////////////////////////////////
//////////////////////////////////GENERAL///////////////////////////////////////////


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


void COMUNI_ini(void)
{
  pinMode(WIFI_RESET, OUTPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  digitalWrite(WIFI_RESET,LOW);
  Serial.begin(9600);
  #ifndef AT_MODE
  BluethootSerial.begin(9600);
  #endif
  #ifdef AT_MODE
  BluethootSerial.begin(38400);
  #endif
  TabletaSerial.begin(9600);
  WIFISerial.begin(9600);
  
  while (!Serial) {
    ; // Wait for Serial
  }
  while (!BluethootSerial) {
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
