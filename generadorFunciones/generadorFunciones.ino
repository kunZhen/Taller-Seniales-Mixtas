#include <Arduino.h>

// Definición de pines PWM (Arduino UNO: pines 9 y 10 con Timer1)
const int SINE_PIN     = 9;   // Pin de salida de onda 1
const int CONST_PIN    = 10;  // Pin de salida de onda 2

// Parámetros de la señal
const float FREQUENCY = 10000.0;   // Frecuencia de la señal (Hz)
const int TABLE_SIZE  = 50;       // Número de muestras (lookup table)

// Amplitud independiente para cada pin (0 a 255)
int amplitudePin9 = 35;    // Ajusta según la amplitud deseada en pin 9
int amplitudePin10 = 25;   // Ajusta según la amplitud deseada en pin 10

// Tablas de onda normalizadas (0.0 a 1.0)
float sineTable[TABLE_SIZE];
float squareTable[TABLE_SIZE];
float triangleTable[TABLE_SIZE];
float dcTable[TABLE_SIZE];  // Tabla DC (valor fijo)

// Variables de tiempo
unsigned long previousMicros = 0;
int waveIndex = 0;

// SELECCIÓN DE ONDA
enum WaveformType {
  WAVE_SINE,
  WAVE_SQUARE,
  WAVE_TRIANGLE,
  WAVE_DC
};

// Ajustar la onda deseada en cada pin
WaveformType waveformSelectedPin9 = WAVE_SQUARE;      // Onda para pin 9
WaveformType waveformSelectedPin10 = WAVE_SQUARE; // Onda para pin 10

void setupTables() {
  // Generar tabla seno normalizada (0..1)
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = 2.0 * PI * (float)i / (float)TABLE_SIZE;
    float sineVal = (sin(angle) + 1.0) / 2.0;   // Normalizar 0..1
    sineTable[i] = sineVal;
  }

  // Generar tabla cuadrada normalizada
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (i < TABLE_SIZE / 2)
      squareTable[i] = 1.0; // Alto
    else
      squareTable[i] = 0.0; // Bajo
  }

  // Generar tabla triangular normalizada
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (i < TABLE_SIZE / 2) {
      float frac = (float)i / (TABLE_SIZE/2);
      triangleTable[i] = frac;  // 0..1 en la primera mitad
    } else {
      float frac = (float)(i - TABLE_SIZE/2) / (TABLE_SIZE/2);
      triangleTable[i] = (1.0 - frac); // 1..0 en la segunda mitad
    }
  }

  // Generar “tabla DC” normalizada (simplemente un valor fijo)
  for (int i = 0; i < TABLE_SIZE; i++) {
    dcTable[i] = 0.5;  // Por ejemplo ~50% del rango
  }
}

void setup() {
  pinMode(SINE_PIN, OUTPUT);
  pinMode(CONST_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("Generador de señales iniciado");

  // Inicializar tablas
  setupTables();

  // Ajustar Timer1 para PWM rápido en pines 9 y 10
  // Modo Fast PWM 10 bits: 
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10); // Sin prescaler
}

void loop() {
  unsigned long currentMicros = micros();

  // Periodo de una onda completa en micros
  float periodMicros = (1.0 / FREQUENCY) * 1000000.0;   // Periodo de UNA onda en us
  float samplePeriod = periodMicros / TABLE_SIZE;       // Tiempo entre muestras

  if ((currentMicros - previousMicros) >= samplePeriod) {
    previousMicros = currentMicros;

    float outputValNormPin9, outputValNormPin10;
    
    // Seleccionar valor normalizado para pin 9
    switch (waveformSelectedPin9) {
      case WAVE_SINE:      outputValNormPin9 = sineTable[waveIndex];      break;
      case WAVE_SQUARE:    outputValNormPin9 = squareTable[waveIndex];    break;
      case WAVE_TRIANGLE:  outputValNormPin9 = triangleTable[waveIndex];  break;
      case WAVE_DC:        outputValNormPin9 = dcTable[waveIndex];        break;
    }

    // Seleccionar valor normalizado para pin 10
    switch (waveformSelectedPin10) {
      case WAVE_SINE:      outputValNormPin10 = sineTable[waveIndex];      break;
      case WAVE_SQUARE:    outputValNormPin10 = squareTable[waveIndex];    break;
      case WAVE_TRIANGLE:  outputValNormPin10 = triangleTable[waveIndex];  break;
      case WAVE_DC:        outputValNormPin10 = dcTable[waveIndex];        break;
    }

    // Escalar según amplitud
    byte outputValPin9 = (byte)(outputValNormPin9 * amplitudePin9);
    byte outputValPin10 = (byte)(outputValNormPin10 * amplitudePin10);

    // Escribir valor en pin 9
    analogWrite(SINE_PIN, outputValPin9);

    // Escribir valor en pin 10
    analogWrite(CONST_PIN, outputValPin10);

    // Avanzar el índice de la muestra
    waveIndex++;
    if (waveIndex >= TABLE_SIZE) waveIndex = 0;

    // Debug opcional
    Serial.print("Pin9 Value: ");
    Serial.print(outputValPin9);
    Serial.print(" | Pin10 Value: ");
    Serial.println(outputValPin10);
  }
}
