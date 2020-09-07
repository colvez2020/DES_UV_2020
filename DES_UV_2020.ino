#include "COMUNICA_CMD.h"
#include "LAMP_UTIL.h"
#include "MOTOR_UTIL.h"
#include "SENSOR_UTIL.h"
#include "SONIDO_UTIL.h"



//#define TEST_SONIC
//#define AT_MODE
//#define TEST_MOTOR
//#define TEST_LUZ

char option_CONTROL;
char option_BLUETHOOT;
char option_TABLETA;
int Movimiento_ACTUAL_RObot=0;
float Temp_BATA, Temp_BATB, votaje_bat, corriente_bat;

//RF
long C_precionado=0;
long   C_precionado_ANT=0;


/****************************************************************
      SETUP
****************************************************************/
void setup()
{
  COMUNI_ini();
  #ifndef AT_MODE
  //RF24_Ini(); 
  Lamp_ini();
  Analog_ini();
  Sonic_ini();
  SPK_ini();
  #endif
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
  //Manejo de luces
  Control_Lamp12V();  
  test_UVC(3,6000,4);
  //Control Motor bluethoot, RF, Tableta
  /*if(Sonic_precacion_choque(Movimiento_ACTUAL_RObot))
  {
    Sonido_Beep_choque();
    Control_Maquina('P',&Movimiento_ACTUAL_RObot); 
  }*/
  if(CMD_bluethoot(&option_BLUETHOOT))
  {
    Control_Maquina(option_BLUETHOOT,&Movimiento_ACTUAL_RObot);
    Serial.print("MOV a=");
    Serial.println(Movimiento_ACTUAL_RObot);
  }
  /*
  //No hay botton de parada, se mueve solo si preciona continuamente.
  else if(CMD_RF24(&option_CONTROL))
  {
    Serial.print("Lego dato RF=");
    Serial.println(option_CONTROL);
    Control_Maquina(option_CONTROL,&Movimiento_ACTUAL_RObot);
    C_precionado++;
  }*/
  else if(CMD_tableta(&option_TABLETA))
  {
    Control_Maquina(option_TABLETA,&Movimiento_ACTUAL_RObot);
  }
  /*
  //Sigue precionado
  if(C_precionado!=C_precionado_ANT)
  {
    C_precionado_ANT=C_precionado;
  }
  else
  {
    C_precionado_ANT=0;
    C_precionado=0;
    Control_Maquina('P',&Movimiento_ACTUAL_RObot);
  }
  */
  if(Read_sensor_analog(&votaje_bat,&corriente_bat,&Temp_BATA,&Temp_BATB))
  {
    int Porcentaje_uso;
    
    Estimar_capacidad_4LED(votaje_bat);
    Porcentaje_uso=Estimar_capacidad_MAP(votaje_bat);
    //Serial.print("Voltaje Baterias MAP=  ");
    //Serial.println(Porcentaje_uso);
    //Print_ON_BLUETHOOT_float(votaje_bat);
    //Print_ON_BLUETHOOT_char(' ');
    //Print_ON_BLUETHOOT_char('(');
    Print_ON_BLUETHOOT_int(Porcentaje_uso);
    //Print_ON_BLUETHOOT_char(')');
    Print_ON_BLUETHOOT_char(',');
  }
    
} 
/****************************************************************
      Retrieve measurement and set next trigger
****************************************************************/

/*
//Un motor gira en sentido horario y el otro antihorario
//Si estuvierán montados en un robot el robot avanzaria. 
// Provocamos aceleración
// La velocidad solo toma valores entre -254 y 254
// El giro solo toma valores entre -100 y 100
for (velocidad=150;velocidad<254;++velocidad) {
     control.Motor(velocidad,1);
     
 //Un motor gira en sentido horario y el otro antihorario 
 //Si estuvierán montados en un robot el robot retrocedería.

control.Motor(-180,1);
delay(3000);

//Los dos motores giran en sentido horario.
//Si estuvieran montados en un robot este giraria a la derecha.

control.Motor(200,100);
delay(3000);

//Los dos motores giran en sentido antihorario.
//Si estuvieran montados en un robot este giraria a la derecha.

control.Motor(200,-100);
delay(3000);

//Los motores se paran.

control.Motor(0,1);
delay(3000);
*/
//END
