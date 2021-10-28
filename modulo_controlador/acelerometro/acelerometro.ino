#define pinAcelX A0
#define pinAcelY A1
#define pinAcelZ A2

void setup() {

    Serial.begin(9600);
}

void loop() {

  int valX = analogRead(pinAcelX);
  int valY = analogRead(pinAcelY);
  int valZ = analogRead(pinAcelZ);

  /*
  Serial.print("X: ");
  Serial.print(valX);
  Serial.print("  Y: ");
  Serial.print(valY);
  Serial.print("  Z: \n");
  Serial.println(valZ);
  */
  
  if (valZ < 300) {
     Serial.println("Capotado\n");
  } else if ( (valZ < 360) && (valY < 270)) {
     Serial.println("Caiu para Esquerda\n");
  } else if ( (valZ < 360) && (valY > 380)) {
     Serial.println("Caiu para Direita\n");
  } else  if ( (valZ < 360) && (valX < 270)) {
     Serial.println("Caiu para Frente\n");
  } else if ( (valZ < 360) && (valX > 380)) {
     Serial.println("Caiu para Tras\n");
  } else {
     Serial.println("Normal\n");
  }  
}
