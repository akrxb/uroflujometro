#ifndef CALCULOS_H
#define CALCULOS_H

#include <Arduino.h>
#include <Adafruit_HX711.h>

namespace pin {
  static const int HX_DT = 4;  
  static const int HX_SCK = 3;
  static const int BOTON = 2;
} 

enum Estado {IDLE, TARING, RECORDING, FINISHED};

extern Estado estado;
extern float peso, varPeso, caudal, pesoAnt, tAnt, tActual, tVar, vol, qMax;
extern bool btnP, finEnviado;


//Filtro exp
extern float caudalFiltrado, alpha;

void inciarPeso();
void resetMetricas();
Estado gestionarEstados(bool btn);
void calculos();
void enviarDatos(float peso, float caudal, float vol, float qmax, Estado estado);


#endif