#include <Arduino.h>

// Definición de pines PWM (Arduino UNO: pines 9 y 10 con Timer1)
const int SINE_PIN     = 9;  // Generación de onda principal
const int CONST_PIN    = 10; // Onda constante o cuadrada (o cualquier otra)

// Parámetros de la señal
const float FREQUENCY = 1000.0;   // Frecuencia de la señal (Hz)
const int TABLE_SIZE  = 50;       // Número de muestras (lookup table)

// Ajuste de amplitud (PWM de 8 bits: 0..255)
// Para ~1 V pico sobre 5 V, usamos un rango máximo ~51 (20% de 255).
const int MAX_AMP = 51;  

// Tablas de onda (sugerencia: 50 muestras en vez de 256 para simplificar)
byte sineTable[TABLE_SIZE];
byte squareTable[TABLE_SIZE];
byte triangleTable[TABLE_SIZE];
byte dcTable[TABLE_SIZE];  // Si quisieras una “tabla DC”, o simplemente un valor fijo

// Variables de tiempo
unsigned long previousMicros = 0;
int waveIndex = 0;

// ----- SELECCIÓN DE ONDA A GENERAR EN SINE_PIN -----
enum WaveformType {
  WAVE_SINE,
  WAVE_SQUARE,
  WAVE_TRIANGLE,
  WAVE_DC
};

WaveformType waveformSelected = WAVE_TRIANGLE; // Cambiar según la onda deseada
WaveformType waveformSelectedForPin10 = WAVE_TRIANGLE; // Tipo de onda para el pin 10 (puede ser cuadrada, triangular, etc.)

// ---------------------------------------------------
void setupTables() {
  // Generar tabla seno (0..MAX_AMP)
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = 2.0 * PI * (float)i / (float)TABLE_SIZE;
    float sineVal = (sin(angle) + 1.0) / 2.0;   // Normalizar 0..1
    sineTable[i] = (byte)(sineVal * MAX_AMP);
  }

  // Generar tabla cuadrada
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (i < TABLE_SIZE / 2)
      squareTable[i] = MAX_AMP; // Alto
    else
      squareTable[i] = 0;       // Bajo
  }

  // Generar tabla triangular
  // Subir lineal mitad, bajar lineal mitad
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (i < TABLE_SIZE / 2) {
      float frac = (float)i / (TABLE_SIZE/2);
      triangleTable[i] = (byte)(frac * MAX_AMP);
    } else {
      float frac = (float)(i - TABLE_SIZE/2) / (TABLE_SIZE/2);
      triangleTable[i] = (byte)((1.0 - frac) * MAX_AMP);
    }
  }

  // Generar “tabla DC” (simplemente un valor fijo)
  for (int i = 0; i < TABLE_SIZE; i++) {
    dcTable[i] = MAX_AMP / 2;  // p. ej. ~0.5 V
  }
}

// ---------------------------------------------------
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

// ---------------------------------------------------
void loop() {
  unsigned long currentMicros = micros();

  // Periodo = 1 / (FREQUENCY), pero tenemos TABLE_SIZE muestras por ciclo
  float periodMicros = (1.0 / FREQUENCY) * 1000000.0;   // Periodo de UNA onda en us
  float samplePeriod = periodMicros / TABLE_SIZE;       // Tiempo entre muestras

  if ((currentMicros - previousMicros) >= samplePeriod) {
    previousMicros = currentMicros;

    // Seleccionar qué tabla usar según waveformSelected para el pin 9
    byte outputValSine = 0;
    switch (waveformSelected) {
      case WAVE_SINE:      outputValSine = sineTable[waveIndex];      break;
      case WAVE_SQUARE:    outputValSine = squareTable[waveIndex];    break;
      case WAVE_TRIANGLE:  outputValSine = triangleTable[waveIndex];  break;
      case WAVE_DC:        outputValSine = dcTable[waveIndex];        break;
    }

    // Seleccionar qué tabla usar según waveformSelectedForPin10 para el pin 10
    byte outputValPin10 = 0;
    switch (waveformSelectedForPin10) {
      case WAVE_SINE:      outputValPin10 = sineTable[waveIndex];     break;
      case WAVE_SQUARE:    outputValPin10 = squareTable[waveIndex];   break;
      case WAVE_TRIANGLE:  outputValPin10 = triangleTable[waveIndex]; break;
      case WAVE_DC:        outputValPin10 = dcTable[waveIndex];       break;
    }

    // Escribir valor en pin 9 (seno u onda seleccionada)
    analogWrite(SINE_PIN, outputValSine);

    // Escribir valor en pin 10 (onda cuadrada o cualquier otra)
    analogWrite(CONST_PIN, outputValPin10);

    // Avanzar index
    waveIndex++;
    if (waveIndex >= TABLE_SIZE) waveIndex = 0;

    // Mostrar cuál es la onda actual para el pin 10
    String waveform = "";
    switch (waveformSelectedForPin10) {
      case WAVE_SINE:      waveform = "Seno";      break;
      case WAVE_SQUARE:    waveform = "Cuadrada";  break;
      case WAVE_TRIANGLE:  waveform = "Triangular";break;
      case WAVE_DC:        waveform = "DC";        break;
    }
    Serial.print("Onda actual en pin 10: ");
    Serial.println(waveform);

    // Debug opcional
    Serial.print("WAVE Value Sine (Pin 9): ");
    Serial.print(outputValSine);
    Serial.print("   WAVE Value Pin 10: ");
    Serial.println(outputValPin10);
  }
}
