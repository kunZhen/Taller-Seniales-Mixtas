const int inputPin1 = A0;  // Primera señal de entrada
const int inputPin2 = A1;  // Segunda señal de entrada
const int opSelectPin = A2; // Pin para seleccionar operación

// Variables para el muestreo
const int samplingRate = 1000;  // 1000 Hz
const unsigned long samplingInterval = 1000000 / samplingRate; // en microsegundos
unsigned long lastSampleTime = 0;

// Variables para procesamiento
float lastSample = 0;  // Para derivada
float integral = 0;    // Para integral
const float dt = 1.0 / samplingRate;

void setup() {
  Serial.begin(9600);  // Alta velocidad para mejor visualización
  analogReference(DEFAULT);  // 5V reference
}

void loop() {
  unsigned long currentTime = micros();
  
  // Control de frecuencia de muestreo
  if (currentTime - lastSampleTime >= samplingInterval) {
    lastSampleTime = currentTime;
    
    // Lectura de entradas
    float signal1 = -(analogRead(inputPin1) - 512) * (6.0 / 1023.0);  // Centrado en 0
    float signal2 = -(analogRead(inputPin2) - 512) * (6.0 / 1023.0);
    int operation = analogRead(opSelectPin);
    
    float result = 0;
    
    // Selección de operación basada en el valor leído
    if (operation < 256) {  // Suma
      result = signal1 + signal2;
    }
    else if (operation < 512) {  // Resta
      result = signal1 - signal2;
    }
    else if (operation < 768) {  // Integral
      integral += signal1 * dt;
      result = integral;
    }
    else {  // Derivada
      result = (signal1 - lastSample) / dt;
      lastSample = signal1;
    }
    
    // Envío al Serial Plotter
    Serial.print(signal1);
    Serial.print(" ");
    Serial.print(signal2);
    Serial.print(" ");
    Serial.println(result);
  }
}