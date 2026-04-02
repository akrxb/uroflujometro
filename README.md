# Uroflujómetro

Este el repositorio respectivo a la primera parte del reto, en concreto, a la parte de la uroflujometría.
En este se utilizan las siguientes tecnologías:  
- Docker: creamos un contenedor con las imagenes necesarias por simplificidad.  
- Node-Red: para definir el flujo que llevará a cabo el programa.  
- InfluxDB: como base de datos temporal, necesaria para tener registro temporal de todas las medidas.  
- Grafana: con la que obtenemos las gráficas de las medidas.
- Flask: backend necesario para la parte web (esta en HTML y CSS).
Por otra parte, hardware utilizado es una ESP32C3 de steeed studio conectada a un groove shield para la conexión física de un botón que dará los distintos estados del programa, sensor HX711 (1 kg) y una pantalla redonda de 240 x 240 píxeles de resolución. Más adelante se comentará los pines utilizados.

### GUÍA DE INSTALACIÓN COMPLETA
Primero debemos de instalar Docker y utilizaremos el docker-compose.yml del repositorio para que se creen las imagenes que necesitamos dentro del contenedor.
