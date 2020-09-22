#include <EEPROM.h>
#include "COMUNICA_CMD.h"
#include "LAMP_UTIL.h"
#include "MOTOR_UTIL.h"
#include "SENSOR_UTIL.h"
#include "SONIDO_UTIL.h"



//#define TEST_SONIC
//#define AT_MODE
//#define TEST_MOTOR
//#define TEST_LUZ
//#define TEST_VOLTAJE



//COMUNICACION
char option_BLUETHOOT;
char option_TABLETA;

//CONTROL
int Movimiento_ACTUAL_RObot=0;

//SENSORES
float Temp_BATA, Temp_BATB, votaje_bat, corriente_bat;
int Porcentaje_uso,Ciclos_carga,Uso_lampara;
float Voltaje_test,Corriente_test;

//WIFI
boolean Reset_flag_WIFI=false;

/****************************************************************
      SETUP
****************************************************************/
void setup()
{
  COMUNI_ini();
  MEN_ini();
  #ifndef AT_MODE 
  Lamp_ini();
  Analog_ini();
  Sonic_ini();
  SPK_ini();
  #endif
  for(int test_tex=0;test_tex<5;test_tex++)
  {
    Serial.print("INICIO ");
    Serial.println(test_tex);
    delay(100);
  }

}
/****************************************************************
      LOOP
****************************************************************/
void loop()
{
  ////////////////////////////
  
  #ifdef TEST_SONIC
    Test_SensoresSonic();
  #endif  
  #ifdef TEST_MOTOR
    Test_motores("D",1);
  #endif
  #ifdef AT_MODE
  AT_Bluethoot();
  #endif
  #ifdef TEST_VOLTAJE
  do{
    Voltaje_test=Sensor_VoltajeBAT();
    Serial.print("Voltaje Baterias=  ");
    Serial.println(Voltaje_test);
   
  }while(1);
  #endif
  //Manejo de luces
  Control_Lamp12V();  
  //test_UVC(3,6000,4);
  //Control Motor bluethoot, RF, Tableta
  if(Sonic_precacion_choque(Movimiento_ACTUAL_RObot))
  {
    Sonido_Beep_choque();
    Control_Maquina('P',&Movimiento_ACTUAL_RObot); 
  }
  if(CMD_bluethoot(&option_BLUETHOOT))
  {
    Control_Maquina(option_BLUETHOOT,&Movimiento_ACTUAL_RObot);
    Serial.print("MOV a=");
    Serial.println(Movimiento_ACTUAL_RObot);
  }
  else if(CMD_tableta(&option_TABLETA))
  {
    Control_Maquina(option_TABLETA,&Movimiento_ACTUAL_RObot);
  }

  if(Read_sensor_analog(&votaje_bat,&corriente_bat,&Temp_BATA,&Temp_BATB))
  {
    Estimar_capacidad_4LED(votaje_bat);
    Porcentaje_uso=Estimar_capacidad_MAP(votaje_bat);
    Ciclos_carga=Actualizar_ciclos_carga(votaje_bat);
    Return_USO_lampara(&Uso_lampara);
    Print_ON_BLUETHOOT_int(Porcentaje_uso);
    Print_ON_BLUETHOOT_char(',');
    Reset_flag_WIFI=Send_to_Blink_WIFI();
    Serial.print("Voltaje Baterias=  ");
    Serial.println(votaje_bat);
    Serial.print("Corriente Baterias=  ");
    Serial.println(corriente_bat);
    Serial.print("Ciclos_carga Baterias=  ");
    Serial.println(Ciclos_carga);
    Serial.print("Uso_lampara=  ");
    Serial.println(Uso_lampara);    
  }

  if(Reset_flag_WIFI==true)
  {
    if(WIFI_8266_RESET()==true) 
      Reset_flag_WIFI=false; 
  }
    
} 

////////////////////////////////////////////GENERALES//////////////////////////////////////
////////////////////////////////////////////GENERALES//////////////////////////////////////
////////////////////////////////////////////GENERALES//////////////////////////////////////

void MEN_ini(void)
{
  char Men_INI_OK_flag;
  char TOKEN=0x43;
  Men_INI_OK_flag=EEPROM.read(50);
  if(Men_INI_OK_flag!=TOKEN)
  {
    for(int g=0;g<256;g++)
      EEPROM.write(g,0x00);
    EEPROM.write(50,TOKEN);
    EEPROM.write(10,false);
    EEPROM.write(11,false);  
  }
}

boolean Send_to_Blink_WIFI(void)
{
  if(Print_SENSORDATA_ON_WIFI(2,0,votaje_bat,'V')==WIFI_ACK_NO_RESPONCE)
    return false;
  if(Print_SENSORDATA_ON_WIFI(2,0,corriente_bat,'C')==WIFI_ACK_NO_RESPONCE)
    return false;
  if(Print_SENSORDATA_ON_WIFI(1,Ciclos_carga,0,'B')==WIFI_ACK_NO_RESPONCE)
    return false;
  if(Print_SENSORDATA_ON_WIFI(1,Uso_lampara,0,'L')==WIFI_ACK_NO_RESPONCE)
    return false;
  return true;
}


//END
