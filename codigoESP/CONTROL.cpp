#include "control.h"
#include "pc.h"

EstadoPrueba estadoActual = IDLE;
int idPacienteActual = 1;
bool botonPresionadoAnterior = false;

void iniciarControl() {
    pinMode(PC::BOTON, INPUT_PULLUP);
}

void revisarBoton() {
    bool lecturaBoton = (digitalRead(PC::BOTON) == LOW); 

    if (lecturaBoton && !botonPresionadoAnterior) {
        delay(50); //para boton

        if (estadoActual == IDLE) {

            reiniciarMetricas(); 
            estadoActual = RECORDING;
            Serial.println("--- INICIO DE PRUEBA ---");

        } 
        else if (estadoActual == RECORDING) {

            estadoActual = FINISHED;
            Serial.println("--- PRUEBA FINALIZADA ---");

        } 
        else if (estadoActual == FINISHED) {

            estadoActual = IDLE;
            idPacienteActual++; 
            Serial.println("--- SISTEMA EN ESPERA ---");

        }
    }
    
    botonPresionadoAnterior = lecturaBoton;
}

EstadoPrueba obtenerEstadoActual() {
    return estadoActual;
}

int obtenerIdPacienteActual() {
    return idPacienteActual;
}