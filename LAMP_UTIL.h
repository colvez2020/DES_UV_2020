#ifndef LAMP_UTIL_H
#define LAMP_UTIL_H



void Lamp_ini(void);
void Control_UVC(char cmd);
void test_UVC(int Balastro_N, int ON_OFFdelay, int Numero_de_pruebas);
void Control_Lamp12V(void);
void Return_USO_lampara(int* Uso_lampara);

#endif
