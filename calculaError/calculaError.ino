const int PIN_SENAL1 = A0;
const int PIN_SENAL2 = A1; 
const int PIN_SENAL3 = A2;

float valor1, valor2, valor3, resultado;
const float FACTOR_CONVERSION = 5.0/1023.0;
char operacion = '+'; // '+' para suma, '-' para resta

void setup() {
  Serial.begin(9600);
  analogReference(DEFAULT);
  Serial.println("Ingrese '+' para suma o '-' para resta");
}

void loop() {
  // Leer operación si está disponible
  if (Serial.available() > 0) {
    operacion = Serial.read();
  }
  
  // Leer y convertir valores
  valor1 = analogRead(PIN_SENAL1) * FACTOR_CONVERSION;
  valor2 = analogRead(PIN_SENAL2) * FACTOR_CONVERSION;
  valor3 = analogRead(PIN_SENAL3) * FACTOR_CONVERSION;
  
  // Realizar operación
  if (operacion == '+') {
    resultado = valor1 + valor2;
  } else if (operacion == '-') {
    resultado = valor1 - valor2;
  }
  
  // Calcular error
  float error = abs(resultado - valor3);
  float error_porcentual = (error / valor3) * 100;
  
  // Imprimir resultados
  Serial.print("V1: ");
  Serial.print(valor1);
  Serial.print("V, V2: ");
  Serial.print(valor2);
  Serial.print("V, V3: ");
  Serial.print(valor3);
  Serial.print("V, Resultado: ");
  Serial.print(resultado);
  Serial.print("V, Error: ");
  Serial.print(error);
  Serial.print("V (");
  Serial.print(error_porcentual);
  Serial.println("%)");
  
  delay(10);
}