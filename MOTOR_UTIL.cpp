#include <LEANTEC_ControlMotor.h>
#include "SENSOR_UTIL.h"
#include "MOTOR_UTIL.h"
#include "SONIDO_UTIL.h"
#include "LAMP_UTIL.h"

//Motor
#define IN11_UC              11
#define IN10_UC              9
#define PWM12_UC             13
#define IN00_UC             8
#define IN01_UC             10
#define PWM02_UC            12


boolean  Sentido_Horario=true;
// Motores
ControlMotor control(IN00_UC,IN01_UC,IN10_UC,IN11_UC,PWM02_UC,PWM12_UC); // MotorDer1,MotorDer2,MotorIzq1,MotorIzq2,PWM_Derecho,PWM_Izquierdo
                                       // IN01     ,IN02     ,IN11     ,IN12     ,PWM00      ,PWM10


void Test_motores(char MOTOR, char sentido)
{
  if(MOTOR=='D')
    control.Test_motor(MOT_DER,sentido,255);
  if(MOTOR=='I')
    control.Test_motor(MOT_IZQ,sentido,255);
  
}

void Test_motores_fuerte(void)
{
  do{      
      if(Sentido_Horario==true)
      {
        control.Test_motor(MOT_DER,-1,220);
        control.Test_motor(MOT_IZQ,-1,220);
        Sentido_Horario=false;
      }
      else
      {
        control.Test_motor(MOT_DER,1,220);
        control.Test_motor(MOT_IZQ,1,220);
        Sentido_Horario=true;
      }
  delay(800);
  }while(1);

}


//#define ADELANTE           1
//#define IZQUIERA           2
//#define DERECHA            3

void Control_Maquina(char CMD,int *MOV_actual)
{
  switch(CMD) //donde opción es la variable a comparar
  {
    case 'I':    
      if(!Sonic_precacion_choque(IZQUIERA))
      {
        control.Motor(-255,0);
        *MOV_actual=IZQUIERA;            
      }  
      else
      {
        Sonido_Beep_choque();
        control.Motor(0,0);
      }  
    break;
    case 'A': 
     if(!Sonic_precacion_choque(ADELANTE))
     {
        control.Motor(255,100);;
        *MOV_actual=ADELANTE;
     }
     else
     {
        Sonido_Beep_choque();
        control.Motor(0,0);
     }
    break;
    case 'B': 
      control.Motor(255,-100);    
      *MOV_actual=ATRAS;
    break;
      case 'D': 
      if(!Sonic_precacion_choque(DERECHA))
      {
       control.Motor(255,0); //OK
       *MOV_actual=DERECHA;
      }
      else
      {
        Sonido_Beep_choque();   
        control.Motor(0,0);
      }
    break;
    case 'P': //Bloque de instrucciones 1;
      control.Motor(0,0);
      *MOV_actual=PARO;
    break;
    case 'O':
      Sonido_Beep_lamp();
      Control_UVC(1);
    break;
    case 'F':
      Control_UVC(0);
    break;  
    ///////////////////////////////////////////////////////
    case 0x01:
      Sonido_Beep_lamp();
      Control_UVC(1);
     break;
     case 0x03:
      Control_UVC(0);
     break;
  }
}
