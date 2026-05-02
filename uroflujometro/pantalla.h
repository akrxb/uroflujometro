#ifndef PANTALLA_H
#define PANTALLA_H

#include <Arduino.h>
#include "calculos.h"

// Funciones principales
void pantalla_init();
void pantalla_actualizar(Estado estado, uint32_t ms, float caudal);

#endif