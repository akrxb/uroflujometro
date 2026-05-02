<H1>RETO UPCT: UROFLUJÓMETRO</H1>

<H3>Este repositorio es para el tener más accesible el código del proyecto, así una pequeña explicación de como funciona.</H3>

El desarrollo del proyecto lo basamos en la ESP32C3 con el módulo de groove shield de xiao steed, el sensor HX711, una pantalla redonda de 240x240 y un pulsador que dará lugar al cambio de estados de la prueba. En cuanto a su funcionamiento:   
- Pulsador inicia la toma de datos, lo que da inicio a la prueba.
- Tras 30 segundos de prueba, se pulsa de nuevo y los datos se envían por protocolo UDP.
- Con los datos sacamos la gráfica correspondiente que se mostrará en la página web.

Para el apartado del backend utilizamos Flask, SQLite para la base datos y Chart.js para las gráficas.

