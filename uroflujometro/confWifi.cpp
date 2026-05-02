#include "confWifi.h"


const char* ssid = "diavluras";
const char* password = "pedrito.";

WiFiUDP udp;
const int puerto_remoto = 9000;

IPAddress local_IP(10, 173, 46, 100);
IPAddress gateway(10, 173, 46, 1);   
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);  

const char* ip_remota = "10.173.46.204"; 


void iniciarWifi(const char* ssid, const char* password){
  
  WiFi.mode(WIFI_STA);
  
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Fallo al configurar IP Estática");
  }
  WiFi.begin(ssid, password);
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 

  while (WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.print('.');
  }

  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());


}
