# -*- coding: utf-8 -*-
import serial
import requests
import time

# CONFIGURACIÓN
SERIAL_PORT = 'COM3'  # Cambia por tu puerto
BAUD_RATE = 9600
WRITE_API_KEY = '1AGEI427VHXILNVW'

# Conectar al Arduino
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
time.sleep(2)

print("Conectado al puerto", SERIAL_PORT)

while True:
    try:
        linea = ser.readline().decode().strip()

        if linea:
            print("Recibido:", linea)

            distancia, estado = linea.split(",")

            url = "https://api.thingspeak.com/update"
            payload = {
                'api_key': WRITE_API_KEY,
                'field1': distancia,
                'field2': estado
            }

            response = requests.get(url, params=payload)

            if response.text != "0":
                print("Enviado correctamente ✔")
            else:
                print("Error enviando datos ❌")

    except Exception as e:
        print("Error:", e)
        time.sleep(5)