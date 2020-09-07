#include <Arduino.h>
#include "SENSOR_UTIL.h"
#include "COMUNICA_CMD.h"

//Entradas Analogas
#define ANALG_SN01_T1         A10
#define ANALG_SN02_T2         A11
#define ANALG_SN03_VB         A8
#define ANALG_SN04_CB         A9
#define LEDST_00              42
#define LEDST_01              43
#define LEDST_02              44
#define LEDST_03              45

#define AnaDelay              4000  // tiempo de medida de sensores


///////////////////////////////////// VARIABLES GLOBALES //////////////////////////////////////////////////////
//Variables Analogas
float voltaje_baterias;
float voltaje_Sensor_corriente;
float Sensibilidad=0.66; //sensibilidad en Voltios/Amperio para sensor de 5A
float Corriente_baterias;
float voltaje_Sensot_temp1;
float voltaje_Sensot_temp2;
float celsius;
unsigned long lastPingAna;


//Ultrasonic
#define soundSpeed       343.0                      // Speed of sound in m/s (343m/s at 20°C with dry air)
#define tSENS                3                               // Total number of sensors
#define echoPin              2                             // Pin to collect echo signal --> This must be any interrupt pin INT0, pin tarjeta #2
#define TRIGER00            4
#define TRIGER01            5
#define TRIGER02            6
#define pingDelay          200                          // How many milliseconds between each measurement (ping) ; best to keep > 5ms



//                           1    ,   2    ,   3
int triggerPins[tSENS] = {TRIGER00,TRIGER01,TRIGER02};  // Trigger pin for each sensor
volatile unsigned long startEcho;             // Place to store start time (interrupt)
volatile unsigned long stopEcho;              // Place to store stop time (interrupt)
float distancesCm[tSENS];                     // Distances in cm
float distancia_result;
volatile int ultra_RSP;
unsigned long lastPingMillis;


/****************************************************************
      INTERRUPT handling (keep as fast(=short) as possible)
****************************************************************/

void ISR_ECHO()
{
  // For ATmega328(P) chips, INT0 as at pin PD2
  //byte pinRead = PIND >> 2 & B0001;  // Read state of interrupt pin
  // For ATmega2560(P) chips, INT0 as at pin PB0
//  byte pinRead = PINB & B0001;  // Read state of interrupt pin
  ultra_RSP=digitalRead(2);
  if (ultra_RSP)  
  {
    // State = HIGH -> Remember start time
    startEcho = micros();
  }
  else 
  {
    // State is LOW -> Remember stop time
    stopEcho = micros();
  }
}

void Analog_ini(void)
{
  //analogReference(EXTERNAL);
  lastPingAna = millis();              //Tiempo de referencia para lectura de sonic
  pinMode(LEDST_00, OUTPUT);
  pinMode(LEDST_01, OUTPUT);
  pinMode(LEDST_02, OUTPUT);
  pinMode(LEDST_03, OUTPUT);

}

void Sonic_ini(void)
{
// Set all trigger pins as OUTPUTS
  for (int i = 0; i < tSENS; i++) {
    pinMode(triggerPins[i], OUTPUT);   // Set trigger pin x as OUTPUT
  }
  // Set echo pin
  pinMode(echoPin, INPUT);                  // Set echo pin as INPUT (do not use pullup when using diodes !)
  attachInterrupt(digitalPinToInterrupt(2), ISR_ECHO, CHANGE );    // Set interrupt call for echo pin
  
  lastPingMillis = millis();              //Tiempo de referencia para lectura de sonic
}


//Scanea todos y devuelve el Id del mas cerca y su distancia
float Sensor_Sonic_medir(int* Sensor_Posicion)
{
  float temp=100000.0;
  // First read will be bad (or 0)
  unsigned long startWait;
  unsigned long timeout = 50; // 50ms is the max amount of time we'll wait (23ms = 4m = out of range)
  // For each sensor
  for (int s = 0; s < tSENS; s++)
  {
    startEcho = 0;  // Reset start
    stopEcho = 0;   // Reset stop time
    digitalWrite(triggerPins[s], HIGH);    // HIGH pulse for at least 10µs
    delayMicroseconds(10);
    digitalWrite(triggerPins[s], LOW);    // Set LOW again
    startWait = millis();   // remember time we started to wait for the echo signal
    while (startEcho == 0 or stopEcho == 0) {
      // Keep looping unill start and stoptime is not 0 OR a timeout occurs
      if (startWait + timeout < millis())
      {
        // This will result in a distance of 0
        startEcho = 0;
        stopEcho = 0;
        break;  // exit the while loop
      }
    }
    noInterrupts();   // cli()
    distancesCm[s] = (stopEcho - startEcho) * (float)soundSpeed / 1000.0 / 10.0 / 2.0;   // in cm
    if((temp>distancesCm[s]) && (distancesCm[s]!=0))
    {
      //Serial.print( "Minima PRE A:");
      //Serial.println(temp);
      
      temp=distancesCm[s];
   //   Serial.print( "pre_pos:");
    //  Serial.println(s);
      *Sensor_Posicion=s+1;
    }
    interrupts();   // sei();
  }
 // delay(500);
  return temp;
}
//
boolean Sonic_precacion_choque(char Direccion_mov)
{
  int Direccion_waring;
  if (millis() - lastPingMillis >= pingDelay)
  {
    distancia_result=Sensor_Sonic_medir(&Direccion_waring);
     // Serial.println(   String(distancesCm[0]) + " - " 
       //               + String(distancesCm[1]) + " - " 
         //             + String(distancesCm[2]));
    //Serial.print("direccion=");
    //Serial.println(Direccion_waring);
    if((distancia_result<20) && (Direccion_waring == Direccion_mov))
    {
   
        //Serial.print("CHOQUE a !");
        //Serial.print(distancia_result);
        return false;
    }         
    //ComandoSerial.print(distancia_result);              
    //ComandoSerial.println(",");              
    lastPingMillis = millis();
  }
  return false;
}

void Test_SensoresSonic(void)
{
  int Pos_Sensor;
  do{
    if (millis() - lastPingMillis >= pingDelay)
    {
      distancia_result=Sensor_Sonic_medir(&Pos_Sensor);
      Serial.println(   String(distancesCm[0]) + " - " 
                      + String(distancesCm[1]) + " - " 
                      + String(distancesCm[2]));
      Serial.print( "Minima a:");
      Serial.println(distancia_result);
      Serial.print( "En Cara:");
      Serial.println(Pos_Sensor);
      lastPingMillis = millis();
    }
  }while(1);
}

float Sensor_CorrienteBAT(void)
{
    voltaje_Sensor_corriente=analogRead(ANALG_SN04_CB)*(5.0 / 1023.0); //lectura del sensor    
    Corriente_baterias=(voltaje_Sensor_corriente-2.5)/Sensibilidad; //Ecuación  para obtener la corriente
    Serial.print("Corriente_Baterias: ");
    Serial.println(Corriente_baterias,3);
    return  Corriente_baterias;
}

float Sensor_VoltajeBAT(void)
{
  float result=0;
  
  for(int i=0;i<100;i++)
  {
    voltaje_baterias =(float)12.5*(float)5*analogRead(ANALG_SN03_VB)/1023.0;
    result=result+voltaje_baterias;
  }
    result=result/100;
    Serial.print("Voltaje Baterias =  ");
    Serial.println(result);
    return result;
}

void  Sensor_TempBAT(float *BATA, float *BATB)
{
    voltaje_Sensot_temp1 =(float)5000.0* analogRead(ANALG_SN01_T1)/1023.0;
    *BATA = voltaje_Sensot_temp1/ 10; 
    Serial.print(*BATA);
    Serial.println(" C BAT1");
    delay(50);
    voltaje_Sensot_temp2 =(float)5000.0* analogRead(ANALG_SN02_T2)/1023.0;
    *BATB = voltaje_Sensot_temp2 / 10; 
    Serial.print(*BATB);
    Serial.println(" C BAT2");
  
}

boolean Read_sensor_analog(float *VoltajeBAT,float *CorrienteBAT,float *TempA,float *TemB)
{
  if (millis() - lastPingAna >= AnaDelay)
  {
    //*CorrienteBAT=Sensor_CorrienteBAT();
    *VoltajeBAT=Sensor_VoltajeBAT();
    //Sensor_TempBAT(TempA,TemB);
    
    lastPingAna = millis();
    return true;
  }
  return false;
}
//100% 4.1  *7=28.7   (28)
//75% 3.755 *7=26,285 (26.1)
//50% 3.575 *7=25.025 (24.95)
//25% 3.375 *7=23,625 (23.5)
//0%  3.21 * 7=22.5   (22,65)
int Estimar_capacidad_4LED(float votaje_bat)
{
  if(votaje_bat>=28.0)
  { //100%
    digitalWrite(LEDST_00, LOW);
    digitalWrite(LEDST_01, LOW);
    digitalWrite(LEDST_02, LOW);
    digitalWrite(LEDST_03, LOW);
    return 100;
  }
  else if(votaje_bat>=26.1)  
  {//75%
    digitalWrite(LEDST_00, LOW);
    digitalWrite(LEDST_01, LOW);
    digitalWrite(LEDST_02, LOW);
    digitalWrite(LEDST_03, HIGH);
    return 75;
  }
  else if(votaje_bat>=24.95)
  {//50%
    digitalWrite(LEDST_00, LOW);
    digitalWrite(LEDST_01, LOW);
    digitalWrite(LEDST_02, HIGH);
    digitalWrite(LEDST_03, HIGH);
    return 50;
  }
  else if(votaje_bat>=23.5)
  {//25%
    digitalWrite(LEDST_00, LOW);
    digitalWrite(LEDST_01, HIGH);
    digitalWrite(LEDST_02, HIGH);
    digitalWrite(LEDST_03, HIGH);
    return 25;
  }
  else if(votaje_bat<=22.65)
  { //0%
    digitalWrite(LEDST_00, HIGH);
    digitalWrite(LEDST_01, HIGH);
    digitalWrite(LEDST_02, HIGH);
    digitalWrite(LEDST_03, HIGH);
  }
  return 0;
}

int Estimar_capacidad_MAP(float votaje_bat)
{
  float out_max, out_min, in_max, in_min, result; 

  in_min=22.1;
  in_max=29;
  out_min=0;
  out_max=100;
  
  result=(votaje_bat - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

  if(result<0)
    result=0;
  return (int)result;
}
