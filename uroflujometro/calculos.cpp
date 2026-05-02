#include "calculos.h"
#include "confWifi.h"

Adafruit_HX711 HX711_PESO(pin::HX_DT, pin::HX_SCK);


Estado estado = IDLE;
float peso, varPeso, caudal, pesoAnt, tAnt, tActual, tVar, vol, qMax = 0;
bool btnP, finEnviado = false;

//Filtro exp
float caudalFiltrado = 0;
float alpha = 0.2;


void inciarPeso(){
  HX711_PESO.begin();
}

void resetMetricas(){
    tAnt = millis() / 1000.f;
    peso = 0;
    long raw = HX711_PESO.readChannelRaw(CHAN_A_GAIN_128);
    pesoAnt = (raw * 0.0014) - 1015.7;
    varPeso = 0; 
    caudal = 0;
    tVar = 0;vol = 0;
    qMax = 0;
    caudalFiltrado = 0;

  }

void calculos(){
    tActual = millis() / 1000.f;
    long raw = HX711_PESO.readChannelRaw(CHAN_A_GAIN_128);
    peso = (raw * 0.0014) - 1015.7;
    varPeso = peso - pesoAnt;
    tVar = tActual - tAnt;
    if(tVar > 0.01){
      if (varPeso > 0.001){
        caudal = abs(varPeso/tVar);
        vol += varPeso;
      }else{
        caudal = 0;
      }
      caudalFiltrado = alpha * caudal + (1 - alpha) * caudalFiltrado;
      if (caudalFiltrado > qMax) qMax = caudalFiltrado;
      pesoAnt = peso;
      tAnt = tActual;
    }
}


Estado gestionarEstados(bool btn) {
    if (btn && !btnP) { 
        delay(50); // debounce
        switch (estado) {
            case Estado::IDLE:
                resetMetricas();
                estado = Estado::RECORDING; 
                break;

            case Estado::RECORDING:
                // si pulsamos mientras grabamos, terminamos la prueba
                estado = Estado::FINISHED;
                // enviar un último paquete indicando el fin
                enviarDatos(peso, 0, vol, qMax, Estado::FINISHED);
                break;

            case Estado::FINISHED:
                // preparamos el sistema para el paciente siguiente y volvemos al inicio
                finEnviado = true;
                estado = Estado::IDLE;
                break;
                
            default:
                break;
        }
    }
    btnP = btn;

        if (estado == Estado::RECORDING) {
        calculos(); // leemos el sensor y calculamos
        // enviamos los datos constantemente mientras dure la prueba
        enviarDatos(peso, caudalFiltrado, vol, qMax, Estado::RECORDING); 
    }

    return estado;
}

void enviarDatos(float peso, float caudal, float vol, float qmax, Estado estado) {
    char buffer[200];
        snprintf(buffer, sizeof(buffer), "{\"p\":%.2f,\"c\":%.4f,\"v_total\":%.2f,\"q_max\":%.2f,\"estado\":%d}", 
              peso, caudal, vol, qmax, (int)estado);
        udp.beginPacket(ip_remota, puerto_remoto);
        udp.write((uint8_t*)buffer, strlen(buffer));
        udp.endPacket();

        Serial.println(buffer);

}


