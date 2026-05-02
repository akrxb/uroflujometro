<H1>RETO UPCT: UROFLUJÓMETRO</H1>

<h4>Este repositorio es para el tener más accesible el código del proyecto, así una pequeña explicación de como funciona.</h4>

El desarrollo del proyecto lo basamos en la ESP32C3 con el módulo de groove shield de xiao steed, el sensor HX711, una pantalla redonda de 240x240 y un pulsador que dará lugar al cambio de estados de la prueba. En cuanto a su funcionamiento:   
- Pulsador inicia la toma de datos, lo que da inicio a la prueba.
- Tras 30 segundos de prueba, se pulsa de nuevo y los datos se envían por protocolo UDP.
- Con los datos sacamos la gráfica correspondiente que se mostrará en la página web.

Para el apartado del backend utilizamos Flask, SQLite para la base datos y Chart.js para las gráficas, de modo que el resultado final obtenido es el siguiente:    

<img width="1919" height="910" alt="image" src="https://github.com/user-attachments/assets/1b308da2-64e6-408a-9229-603c9e99b996" />   
       
El listado de consultas:
      
<img width="1903" height="907" alt="image" src="https://github.com/user-attachments/assets/470198f3-b9a6-4372-81a7-d3612c40c9eb" />      
        
Ejemplo de prueba, en este caso es la simulación de un patrón reconocido como dientes de sierra:  
        
<img width="1901" height="910" alt="image" src="https://github.com/user-attachments/assets/42a7a60e-b4f6-44e1-8944-d60bb408c44d" />      

Profundizando un poco en el apartado técnico, comentar de que se ha utilizado un filtro exponencial con un valor alpha de 0.2, escogido tras varias pruebas experimentales. Este proyecto se desarrolla en local, pero como queremos que sea accesible para cualquier usuario, hemos utilizado ngrok.    



