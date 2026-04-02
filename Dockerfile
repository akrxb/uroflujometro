# 1. Usamos una imagen ligera de Python
FROM python:3.9-slim

# 2. Establecemos el directorio de trabajo dentro del contenedor
WORKDIR /flask_app

# 3. Copiamos el archivo de librerías primero (para aprovechar la caché de Docker)
COPY requirements.txt .

# 4. Instalamos las dependencias
RUN pip install --no-cache-dir -r requirements.txt

# 5. Copiamos el resto del código de la carpeta local a /app
COPY . .

# 6. Exponemos el puerto 5000 que es el de Flask
EXPOSE 5000

# 7. Comando para arrancar la web
CMD ["python", "app.py"]
