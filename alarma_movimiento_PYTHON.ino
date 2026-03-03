#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonico
const int trigPin = 9;
const int echoPin = 8;

// LEDs
const int ledVerde = 6;
const int ledAmarillo = 4;
const int ledRojo = 2;

long duracion;
float distancia;

// Parpadeo rojo
unsigned long previousMillis = 0;
const long intervaloParpadeo = 200;
bool estadoLedRojo = false;

// Control envio serial (15s)
unsigned long previousSend = 0;
const long intervaloEnvio = 15000;

int estadoSistema = 0; // 0=Normal, 1=Lejos, 2=Alerta

void setup() {

  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledRojo, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Sistema Alarma");
  delay(2000);
  lcd.clear();
}

void loop() {

  // ---- Medición ----
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duracion = pulseIn(echoPin, HIGH, 30000); // timeout 30ms

  if(duracion == 0){
    distancia = 400; // en caso de no haber eco 
  } else {
    distancia = duracion * 0.034 / 2;
  }

  lcd.setCursor(0,0);
  lcd.print("Dist: ");
  lcd.print(distancia,1);
  lcd.print(" cm   ");

  // ---- ESTADOS ----

  if(distancia > 40) {

    estadoSistema = 0;

    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledAmarillo, LOW);
    digitalWrite(ledRojo, LOW);

    lcd.setCursor(0,1);
    lcd.print("Sin obstaculos ");

  }
  else if(distancia > 20) {

    estadoSistema = 1;

    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarillo, HIGH);
    digitalWrite(ledRojo, LOW);

    lcd.setCursor(0,1);
    lcd.print("Objeto lejos   ");
  }
  else {

    estadoSistema = 2;

    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarillo, LOW);

    lcd.setCursor(0,1);
    lcd.print("!!! ALERTA !!! ");

    // Parpadeo rojo no bloqueante
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervaloParpadeo) {
      previousMillis = currentMillis;
      estadoLedRojo = !estadoLedRojo;
      digitalWrite(ledRojo, estadoLedRojo);
    }
  }

  // ---- ENVIO SERIAL CADA 15 SEGUNDOS ----
  unsigned long currentTime = millis();
  if(currentTime - previousSend >= intervaloEnvio) {
    previousSend = currentTime;

    Serial.print(distancia,1);
    Serial.print(",");
    Serial.println(estadoSistema);
  }
}
