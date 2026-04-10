#include "pc.h"
#include "control.h"

//DECLARAMOS objeto para HX711
Adafruit_HX711 HX711_PESO(PC::HX_DT, PC::HX_SCK);

float tiempo_anterior = 0;
float lectura_peso_anterior = 0;
float lectura_caudal = 0;
float tiempo_actual = 0;
unsigned long VADC = 0;
float lectura_peso = 0;
float variacion_peso = 0;
float variacion_tiempo = 0;
float volumenAcumulado = 0;
float qMaximo = 0;

DatosProcesados misDatos;

void iniciarSensor(){

    HX711_PESO.begin();
    Serial.println("HX711 iniciado.");
}

void reiniciarMetricas() {

    volumenAcumulado = 0;
    qMaximo = 0;
    lectura_peso_anterior = 0;
}

DatosProcesados procesamientoDatos(int IDPaciente) {

 EstadoPrueba estadoSist = obtenerEstadoActual();
    
    tiempo_actual = millis() / 1000.0;
    VADC = HX711_PESO.readChannelRaw(CHAN_A_GAIN_128);
    lectura_peso = (VADC * 0.0014) - 903.48 + 2.36;
    
    variacion_peso = lectura_peso - lectura_peso_anterior;
    variacion_tiempo = tiempo_actual - tiempo_anterior;

    // SOLO CALCULAMOS CAUDAL SI ESTAMOS GRABANDO
    if (estadoSist == RECORDING && variacion_tiempo > 0.0001) {
        if (variacion_peso > 0.000001) {
            lectura_caudal = abs(variacion_peso / variacion_tiempo);
            if (lectura_caudal > qMaximo) qMaximo = lectura_caudal;
            volumenAcumulado += variacion_peso;
        } else {
            lectura_caudal = 0;
        }
        lectura_peso_anterior = lectura_peso;
        tiempo_anterior = tiempo_actual;
    } 
    // Si no es RECORDING, el caudal es 0 y el volumen se congela
    else {
        lectura_caudal = 0;
    }

    DatosProcesados resultado;
    resultado.idPaciente = IDPaciente;
    resultado.pesoActual = lectura_peso;
    resultado.caudalActual = lectura_caudal;
    resultado.estado = estadoSist;
    resultado.volumenTotal = volumenAcumulado;
    resultado.caudalMaximo = qMaximo;
    
    return resultado;
}

std::string salidaDatos(DatosProcesados datos){
    return "{\"id\":" + std::to_string(datos.idPaciente) + 
           ",\"p\":" + std::to_string(datos.pesoActual) + 
           ",\"c\":" + std::to_string(datos.caudalActual) + 
           ",\"v_total\":" + std::to_string(datos.volumenTotal) + 
           ",\"q_max\":" + std::to_string(datos.caudalMaximo) + 
           ",\"estado\":" + std::to_string(datos.estado) + "}";
}






