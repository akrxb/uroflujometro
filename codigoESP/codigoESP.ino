#include "pc.h"
#include "control.h"

bool finEnviado = false;
void setup() {

    Serial.begin(115200);
    while(!Serial) delay(10); 
    iniciarSensor();
    iniciarControl();
}

void loop() {

    revisarBoton();
    EstadoPrueba estado = obtenerEstadoActual();
    int id = obtenerIdPacienteActual();

    if (estado == RECORDING) {
    
        finEnviado = false; //reset
        DatosProcesados misDatos = procesamientoDatos(id);
        std::string json = salidaDatos(misDatos);
        Serial.println(json.c_str()); 
    } 
    else if (estado == FINISHED && !finEnviado) { //dato a POST
    
        DatosProcesados datosFinales = procesamientoDatos(id);
        datosFinales.estado = FINISHED;; 
        std::string json = salidaDatos(datosFinales);
        Serial.println(json.c_str()); 
        
        delay(100);

        finEnviado = true;
    }
    
    delay(100);
}