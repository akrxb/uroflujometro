#include "calculos.h"
#include "pantalla.h"
#include "confWifi.h"


uint32_t tInicioGrabacion = 0;

void setup() {
  Serial.begin(115200);
  inciarPeso();
  iniciarWifi(ssid,password);
  pantalla_init();
  pinMode(pin::BOTON, INPUT_PULLUP);
}


void loop() {
    bool lectura = (digitalRead(pin::BOTON) == LOW);
    Estado estadoPrevio = estado;
    Estado estadoActual = gestionarEstados(lectura);
    
    if (estadoPrevio != RECORDING && estadoActual == RECORDING) {
        tInicioGrabacion = millis();
    }

    uint32_t cronoMs = 0;
    if (estadoActual == RECORDING) {
        cronoMs = millis() - tInicioGrabacion;
    } else if (estadoActual == FINISHED) {
        // sin nada
    }

    // actualizar la interfaz visual
    pantalla_actualizar(estadoActual, cronoMs, caudalFiltrado);
    delay(10);
}

