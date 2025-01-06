// Definición de pines analógicos
const int PIN_SENAL1 = A0;
const int PIN_SENAL2 = A1; 
const int PIN_SENAL3 = A2;

// Variables para almacenar valores
float valor1, valor2, valor3;

// Factor de conversión para voltaje (0-5V)
const float FACTOR_CONVERSION = 5.0/1023.0;

void setup() {
  // Inicializar comunicación serial a 9600 baudios
  Serial.begin(9600);
  
  // Configurar resolución ADC
  analogReference(DEFAULT);
}

void loop() {
  // Leer valores analógicos
  int lectura1 = analogRead(PIN_SENAL1);
  int lectura2 = analogRead(PIN_SENAL2);
  int lectura3 = analogRead(PIN_SENAL3);
  
  // Convertir a voltaje (0-9V)
  valor1 = lectura1 * FACTOR_CONVERSION;
  valor2 = lectura2 * FACTOR_CONVERSION;
  valor3 = lectura3 * FACTOR_CONVERSION;
  
  // Enviar datos al plotter serial
  Serial.print(valor1);
  Serial.print(",");
  Serial.print(valor2);
  Serial.print(",");
  Serial.println(valor3);
  
  // Retardo para control de velocidad de muestreo
  delay(10); // 100 muestras por segundo
}