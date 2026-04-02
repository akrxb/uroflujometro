import serial
import socket
import time

# Cambiar puerto y baudios si procede.
SERIAL_PORT = 'COM9' 
BAUD_RATE = 115200
UDP_IP = "127.0.0.1"
UDP_PORT = 9000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def iniciar_puente():
    print(f"Buscando ESP32 en {SERIAL_PORT}...")
    while True:
        try:
            with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1) as ser: # Timeout bajo para no bloquear
                print(f"¡Conectado! Enviando datos a UDP {UDP_PORT}")
                time.sleep(2) 
                
                while True:
                    # Usamos read_all o readline pero verificando el contenido
                    if ser.in_waiting > 0:
                        linea = ser.readline().decode('utf-8', errors='ignore').strip()
                        if linea:
                            print(f"TX -> {linea}")
                            # Enviamos inmediatamente
                            sock.sendto(linea.encode(), (UDP_IP, UDP_PORT))

        except (serial.SerialException, FileNotFoundError):
            print(f"Error en {SERIAL_PORT}. Reintentando...")
            time.sleep(3)
        except KeyboardInterrupt:
            break

if __name__ == "__main__":
    iniciar_puente()
