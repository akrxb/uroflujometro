# Uroflujómetro

Este el repositorio respectivo a la primera parte del reto, en concreto, a la parte de la uroflujometría.
En este se utilizan las siguientes tecnologías:  
- Docker: creamos un contenedor con las imagenes necesarias por simplificidad.  
- Node-Red: para definir el flujo que llevará a cabo el programa.  
- InfluxDB: como base de datos temporal, necesaria para tener registro temporal de todas las medidas.  
- Grafana: con la que obtenemos las gráficas de las medidas.
- Flask: backend necesario para la parte web (esta en HTML y CSS).  
Por otra parte, hardware utilizado es una ESP32C3 de steeed studio conectada a un groove shield para la conexión física de un botón que dará los distintos estados del programa, sensor HX711 (1 kg) y una pantalla redonda de 240 x 240 píxeles de resolución. Más adelante se comentará los pines utilizados. Para leer lo que viene del serial, utilizamos comunicación UDP por el puerto 9000, y el script de python indicado.  
Comentar que se espera la entrada por serial de un JSON de tal forma que contenga: {"id": id_paciente,"p":peso,"c":caudal,"v_total":volumen_total,"q_max":caudal_max,"estado":estado_proceso}.

### GUÍA DE INSTALACIÓN COMPLETA
Primero debemos de instalar Docker y utilizaremos el docker-compose.yml del repositorio para que se creen las imagenes que necesitamos dentro del contenedor. Crear una carpeta donde se vaya a trabajar el proyecto (incluir aquí el .yml), y una vez la tengamos creada, en el mismo path de esta carpeta ejecutamos el siguiente comando: ```docker-compose up --build```.    
Con esto, ya tendríamos las imagenes creadas de las diferentes herramientas, lo que nos toca hacer ahora ir configurando cada una.
#### NODE-RED
Inicialmente al entrar al puerte que tengamos configurado, nos debe de salir un "lienzo" en vacío, así que vamos a ir integrando los diferentes nodos.  
<img width="900" height="900" alt="image" src="https://github.com/user-attachments/assets/c4bdd6e8-bd36-4489-95b2-c633c7cbf27d" />    
Necesario instalar la librería node-red que incluye el nodo udp, y además node-red-contrib-influxdb para la base de datos. Una vez instalados:
- **UDP**: puerto de escucha 9000, ipv4 y ponemos como salida "un Texto".
- **Switch**: dejamos como te viene por defecto la propiedad, solo en la regla buscamos la opción "match regex" y escribimos "^\{.*\}$".
- **JSON**: importante aquí marcar la opción "Convertir siempre a objeto de javascript", lo demás por defecto.
- **Switch**: la propiedad ahora será "payload.estado", añadimos una nueva regla, y en la primera (que será primera salida), escribimos 2 (opción a..z), y en la segunda 3 (opción a..z).
- **Funciones de control de flujo** - influxDB: copiar y pegar el código en la sección mensaje.
```
if (!msg.payload || typeof msg.payload !== 'object' || msg.payload.id === undefined) {
    return null; // Si no tiene ID, ignoramos este mensaje específico
}

let data = msg.payload;

msg.payload = {
    peso: data.p || 0,
    caudal: data.c || 0,
    volumen: data.v_total || 0
};

msg.tags = {
    paciente_id: String(data.id)
};

return msg;
```
- **Funciones de control de flujo** - HTTP Request: copiar y pegar el código en la sección mensaje.
```
if (!msg.payload || msg.payload.estado !== 3) {
    return null;
}

let data = msg.payload;
msg.payload = {
    id: data.id,
    v_total: data.v_total,
    q_max: data.q_max,
    ts_inicio: Date.now() - 30000, // Ajusta este cálculo si el ESP32 no lo envía
    ts_fin: Date.now()
};

msg.headers = {
    "Content-Type": "application/json"
};

msg.url = "http://flaskBack:5000/api/finalizar_prueba";

return msg;
```
También se puede descargar directamente el archivo JSON para su posterior importación
- **InfluxDB**: buscamos influx out y lo añadimos, su configuración la definiremos más adelante.
- **HTTP request**: seleccionamos el método "POST" y que devuelva la opción "un JSON analizado".
#### INFLUXDB
Nos toca configurar la base datos, entonces creamos una cuenta si no tenemos y además nos pedirá nombre de organización, ponemos en este caso "reto_upct". Una vez creada la cuenta, nos logueamos.  
Ahora vamos a "Load Data" - "API token" y creamos uno. IMPORTANTE GUARDAR EL TOKEN. Posteriomente, creamos un "Bucket" de nombre "uroflujometro".
Los nombres pueden ser los que queramos pero debemos de tenerlos en cuenta para conectar node-red con influxdb.  
Podemos ir devuelta a node-red, y completar las opciones del nodo:  
- **Servidor**: creamos uno nuevo, importante para la url escribir "http://influxdb:puerto" y pegar el token.
- **Otras opciones**: tanto organización, como bucket tienen que ser los mismo que los de la base de datos, mediciones puedes darle el nombe que quieras.
#### GRAFANA
Esto es más simple, creamos una nueva dashboard y utilizando el *time series plotter*, y en query la secuencia que se proporciona lo tenemos todo casi hecho, solo hay que en las opciones, hacer que la consulta se actualice cada 5 segundos en auto. Además, para la gráfica debemos de escribir en la sección "Standard options" - "Display Name": "${__field.labels._field}".  
El código necesario para realizar la consulta a la base de datos es:  
``` 
from(bucket: "nombre_bucket")
  |> range(start: -5m) //se puede cambiar por "now" o "5m".
  |> filter(fn: (r) => r["_measurement"] == "nombre_mediciones")
  |> filter(fn: (r) => r["_field"] == "caudal" or r["_field"] == "peso" or r["_field"] == "paciente_id")
  |> group(columns: ["paciente_id", "_field"])
  |> yield(name: "uroflujometro")
```
#### FLASK
Como todo está en los archivos y carpetas correctas, solo asegurarse que todo está en su sitio.
### Página Web
Montamos la siguiente web:
<img width="900" height="900" alt="image" src="https://github.com/user-attachments/assets/8e1f0a21-df3b-4c53-9353-b06eb215a198" />
<img width="900" height="900" alt="image" src="https://github.com/user-attachments/assets/1803713e-12ed-40b5-89d5-2d5e4845db9c" />
<img width="900" height="900" alt="image" src="https://github.com/user-attachments/assets/51a602ad-234a-4050-bc86-1f930de4ef93" />
<img width="900" height="900" alt="image" src="https://github.com/user-attachments/assets/caadeda3-f6fa-4ca1-9938-e69987631aa8" />
### Conexión física en ESP32C3
El botón se conectará en el D2.  
Sensor HX711 en el D0 (DT) - D1 (SCK), y la pantalla en el D7 y el D5 (comunicación).
<br/><img width="900" height="900" alt="image" src="https://github.com/user-attachments/assets/3f2b89c3-48b3-4a6e-9b1f-13499ee60c41" /><br/>
El botón se conectará en el D2.
Sensor HX711 en el D0 (DT) - D1 (SCK), y la pantalla en 

### Cómo se debe de proceder
El usuario debe conectar la ESP32C3 con las conexiones indicadas, comprobando que la asignación de pines es la correcta, y después deberá de revisar el puerto asignado por el ordenador a la placa. Una vez lo tenga, modificar el COM el script de python si procede y ejecutarlo antes de que empiece con la captura de medidas. Después ejecutar el comando de *build* que se comentó anteriormente, y una vez este todo listo, pulsar el botón para que inicie el paso de datos. Con eso ya se podrían ver los datos tanto en la base de datos como en grafana. Una vez se vuelva a pulsar el botón, se debe haber realizado el POST, y haber un nuevo informe en la página de consultas. Si no fuera así, comprobar el log y conexiones.
