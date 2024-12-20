const int sensorPin1 = A0;  // Pin analógico para la señal 1
const int sensorPin2 = A1;  // Pin analógico para la señal 2
const int sensorPin3 = A2;  // Pin analógico para la señal 3

const int samplingPeriod = 1;  // Periodo de muestreo en ms (1 kHz)
unsigned long previousMillis = 0;

void setup() {
  // Inicializar comunicación serial a 115200 baudios
  Serial.begin(9600);
  
  // Configurar resolución ADC a 10 bits (0-1023)
  analogReference(DEFAULT);

  // Configurar etiquetas con colores para el Serial Plotter
  Serial.println("Senal1:0:255:255,Senal2:255:0:0,Senal3:0:255:0");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Verificar si ha pasado el tiempo de muestreo
  if (currentMillis - previousMillis >= samplingPeriod) {
    previousMillis = currentMillis;
    
    // Leer valores analógicos
    int valor1 = analogRead(sensorPin1);
    int valor2 = analogRead(sensorPin2);
    int valor3 = analogRead(sensorPin3);
    
    // Convertir lecturas a voltaje (0-5V)
    float voltaje1 = (valor1 * 5.0) / 1023.0;
    float voltaje2 = (valor2 * 5.0) / 1023.0;
    float voltaje3 = (valor3 * 5.0) / 1023.0;
    
    // Enviar datos etiquetados al Serial Plotter
    Serial.print("Senal1:");
    Serial.print(voltaje1);
    Serial.print(",");
    Serial.print("Senal2:");
    Serial.print(voltaje2);
    Serial.print(",");
    Serial.print("Senal3:");
    Serial.println(voltaje3);
  }
}