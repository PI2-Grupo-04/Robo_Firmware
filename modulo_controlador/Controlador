#include <HX711.h>
#include <Ultrasonic.h>
#include <Servo.h>


HardwareSerial Receiver(2); // Define a  porta serial do receptor


#define Receiver_Txd_pin 27 // recepção e transmissão serial 
#define Receiver_Rxd_pin 14


#define SERVO1 9 // Porta Digital 9 PWM
#define SERVO2 10 // Porta Digital 10 PWM


Servo s1; // Variável Servo
Servo s2; // Variável Servo

int posicao; // Posição Servo


#define INTERVALO_LEITURA 250 //(ms)
// conexão pinos acelerometro
#define pinAcelX A0
#define pinAcelY A1
#define pinAcelZ A2
//conexão dos pinos para o sensor ultrasonico
#define PIN_TRIGGER   4
#define PIN_ECHO      5

// Module connection pins (Digital Pins)
#define CLK 15
#define DIO 13
#define pinDT  2
#define pinSCK  3
#define pesoMin 0.00
#define pesoMax 30.0

//variável responsável pela distância 
unsigned int distancia = 0;
// conexão para o sensor infravermelho 
const int pinoSensor1 = 7; //PINO  UTILIZADO PELO SENSOR INFRAVERMELHO 1
const int pinoSensor2 = 8; //PINO  UTILIZADO PELO SENSOR INFRAVERMELHO 2
//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(PIN_TRIGGER, PIN_ECHO);

#define escala 0.0f
// INSTANCIANDO OBJETOS
HX711 scale;
 
float medida=0;
char cmd1 = frente;
char cmd2 = atras;
char cmd3 = direita;
char cmd4 = esquerda;
char cmd5 = pare;
char cmd6 = chegou;
char cmd7 = base;

void setup() {



Serial.begin(115200);
    configurarPinos(); 
	scale.begin(pinDT, pinSCK); // CONFIGURANDO OS PINOS DA BALANÇA
    scale.set_scale(escala); // ENVIANDO O VALOR DA ESCALA CALIBRADO
    delay(2000);
    scale.tare(); // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA
	Receiver.begin(115200, SERIAL_8N1, Receiver_Txd_pin, Receiver_Rxd_pin);
	s1.attach(SERVO1);
	s2.attach(SERVO2);
	s1.write(0); // Inicia motor da tampa na  posição zero
	s2.write(0); // Inicia motor da tampa na  posição zero


//cria uma tarefa que será executada na função coreTaskZero, 
  xTaskCreatePinnedToCore(
                    coreTaskZero,   /* função que implementa a tarefa */
                    "coreTaskZero", /* nome da tarefa */
                    10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                    NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                    2,          /* prioridade da tarefa (0 a N) */
                    NULL,       /* referência para a tarefa (pode ser NULL) */
                    taskCoreZero);         /* Núcleo que executará a tarefa */
                    
  delay(500); //tempo para a tarefa iniciar
  
  
//cria uma tarefa que será executada na função coreTaskOne
  xTaskCreatePinnedToCore(
                    coreTaskOne,   /* função que implementa a tarefa */
                    "coreTaskOne", /* nome da tarefa */
                    10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                    NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                    2,          /* prioridade da tarefa (0 a N) */
                    NULL,       /* referência para a tarefa (pode ser NULL) */
                    taskCoreOne);         /* Núcleo que executará a tarefa */

    delay(500); //tempo para a tarefa iniciar
	
	
}	





void coreTaskZero( void * pvParameters ){
scale.power_up();//inicializa balança
while(true){
int valX = analogRead(pinAcelX);
int valY = analogRead(pinAcelY);
int valZ = analogRead(pinAcelZ);
      
	  
	while (Receiver.available()){

	char rxdchar= receive.read();
	
	if(rxdchar == cmd1||rxdchar == cmd2||rxdchar == cmd3||rxdchar == cmd4){
		platectrl();
		fechartampa();
		movectrl();
		obstaculo();
		delay(INTERVALO_LEITURA);
		acelerometro();
	}
	else if (rxdchar == cmd5){// parar
	
		movectrl();
	}
	else if(rxdchar == cmd6){ // chegou 
		if(destino == base){// chegou na base 
			abrirtampa();
			platectrl();
			acelerometro();
				
		}
		else{// chegou na mesa
			platectrl();
			abrirtampa();
			balanca();
			
			while(medida != 0 ){
				
				balanca();
				
			}
			delay(3000);
			fechartampa();
			platectrl();
			
						
		}
	
    } 
	else {// ir para a base 
		movectrl();
		platectrl();
		fechartampa();
		
			
	}
	
}
}
}

void coreTaskOne( void * pvParameters ){
	
while(true){
obstaculo(){ // pinoSensor1 = infrared esquerda , pinoSensor2 = infrared direita
	if (digitalRead(pinoSensor1) == LOW || digitalRead(pinoSensor2) == LOW){
		getdistance();
		if (distancia < 60 && digitalRead(pinoSensor1) == LOW  && digitalRead(pinoSensor2) == LOW){ // distância em cm, obstaculo na frente
			percentual = 0;
			pwmmov = (percentual/100)*255;
			analogWrite(motor1a, 0);
			analogWrite(motor1b, 0);
			analogWrite(motor2a, 0);
			analogWrite(motor2b, 0);
		}
		else if(distancia <60 && digitalRead(pinoSensor1) == LOW){ // obstaculo mais a  esquerda 
			percentual = 40;
			pwmmov = (percentual/100)*255;
			analogWrite(motor1a, pwmmov);// vira a direita
			analogWrite(motor1b, 0);
			analogWrite(motor2a, pwmmov);
			analogWrite(motor2b, 0);
			delay(100);
			analogWrite(motor1a, pwmmov);//segue em frente
			analogWrite(motor1b, 0);
			analogWrite(motor2a, 0);
			analogWrite(motor2b, pwmmov);
			delay(100);
			analogWrite(motor1a, 0); // vira a esquerda 
			analogWrite(motor1b, pwmmov);
			analogWrite(motor2a, 0);
			analogWrite(motor2b, pwmmov);
			delay(100);
			analogWrite(motor1a, pwmmov);//segue em frente
			analogWrite(motor1b, 0);
			analogWrite(motor2a, 0);
			analogWrite(motor2b, pwmmov);	
					
		}
		else  (distancia <60 && digitalRead(pinoSensor2) == LOW){ // obstaculo mais a direita
			percentual = 40;
			pwmmov = (percentual/100)*255;// vira a esquerda			
			analogWrite(motor1a, 0); 
			analogWrite(motor1b, pwmmov);
			analogWrite(motor2a, 0);
			analogWrite(motor2b, pwmmov);
			delay(100);
			analogWrite(motor1a, pwmmov);//segue em frente
			analogWrite(motor1b, 0);
			analogWrite(motor2a, 0);
			analogWrite(motor2b, pwmmov);
			delay(100);
			analogWrite(motor1a, pwmmov);// vira a direita
			analogWrite(motor1b, 0);
			analogWrite(motor2a, pwmmov);
			analogWrite(motor2b, 0);
			delay(100);
			analogWrite(motor1a, pwmmov);//segue em frente
			analogWrite(motor1b, 0);
			analogWrite(motor2a, 0);
			analogWrite(motor2b, pwmmov);

		} 
	}   
	else{
		delay(10);
			
	}


}
void acelerometro(){

if (valZ < 300) {
     Receiver.print("Capotado\n");
  } else if ( (valZ < 360) && (valY < 270)) {
     Receiver.print("Caiu para Esquerda\n");
  } else if ( (valZ < 360) && (valY > 380)) {
     Receiver.print("Caiu para Direita\n");
  } else  if ( (valZ < 360) && (valX < 270)) {
     Receiver.print("Caiu para Frente\n");
  } else if ( (valZ < 360) && (valX > 380)) {
     Receiver.print("Caiu para Tras\n");
  } else {
     Receiver.print("Normal\n");
  }  


}
 //pega distancia do obstaculo
 
int getDistance()
{
      //faz a leitura do sensor (em cm)
      int distanciaCM;
      long microsec = ultrasonic.timing();
      distanciaCM = ultrasonic.convert(microsec, Ultrasonic::CM);
      return distanciaCM;
}


 
}
}

// movimentação do robô -- rodas 

void movectrl(rxdchar){
  switch(rxdchar):{
    case cmd1: //frente
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, pwmmov);
    analogWrite(motor1b, 0);
    analogWrite(motor2a, 0);
    analogWrite(motor2b, pwmmov);
    break;
    case cmd3: //virar a direita
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, pwmmov);
    analogWrite(motor1b, 0);
    analogWrite(motor2a, pwmmov);
    analogWrite(motor2b, 0);
    break;
    case cmd4: //virar a esquerda
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, 0);
    analogWrite(motor1b, pwmmov);
    analogWrite(motor2a, 0);
    analogWrite(motor2b, pwmmov);
    break;
    case cmd2: //para trás
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, 0);
    analogWrite(motor1b, pwmmov);
    analogWrite(motor2a, pwmmov);
    analogWrite(motor2b, 0);
    break;
    default: //parado
    percentual = 0;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, 0);
    analogWrite(motor1b, 0);
    analogWrite(motor2a, 0);
    analogWrite(motor2b, 0);
    break;
  }
}



void abrirtampa()
{
  for(posicao = 0; posicao < 180; posicao++)
  {
    s1.write(posicao);
	s2.write(posicao);
  delay(15);
  }
  
}

void fechartampa()
{
  for(posicao = 180; posicao >= 0; posicao--)
  {
    s1.write(posicao);
	s2.write(posicao);
  delay(15);
  }
  
}

//configura pinos infrared
void configurarPinos()
{
pinMode(pinoSensor1, INPUT);
pinMode(pinoSensor2, INPUT);
 }
 
 // pega valor medido pela balança
void balanca()
{
medida = scale.get_units(5); // SALVANDO NA VARIAVEL O VALOR DA MÉDIA DE 5 MEDIDAS
 }
 


// movimentação bandeja
void platectrl(int dir2){
  switch(dir2):{
    case 1: //subindo
    analogWrite(motor3a, 1);
    analogWrite(motor3b, 0);
    break;
    case 2: //descendo
    analogWrite(motor3a, 0);
    analogWrite(motor3b, 1);
    break;
    default: //parado
    analogWrite(motor3a, 0);
    analogWrite(motor3b, 0);
    break;
  }
}



void loop() {}


