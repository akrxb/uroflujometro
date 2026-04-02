#ifndef PC_H
#define PC_H

#include <Arduino.h>
#include <string>
#include <Adafruit_HX711.h>
#include "tipos.h" // solo necesita el enum

namespace PC {
  static const int HX_DT = 2;  
  static const int HX_SCK = 3;
  static const int BOTON = 4;
}

struct DatosProcesados {
    int idPaciente;
    EstadoPrueba estado;
    float volumenTotal;
    float caudalMaximo;
    float pesoActual; 
    float caudalActual;
};

void iniciarSensor();
void reiniciarMetricas();
DatosProcesados procesamientoDatos(int IDPaciente);
std::string salidaDatos(DatosProcesados datos);

#endif