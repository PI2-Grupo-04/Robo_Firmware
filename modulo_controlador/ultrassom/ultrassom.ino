// codigo do sensor ultrassonico 
#include <Ultrasonic.h>

#define INTERVALO_LEITURA 250 //(ms)

//conexão dos pinos para o sensor ultrasonico
#define PIN_TRIGGER   4
#define PIN_ECHO      5

// Module connection pins (Digital Pins)
#define CLK 14
#define DIO 13

//variável responsável pela distância 
unsigned int distancia = 0;
// conexão para o sensor infravermelho 
const int pinoSensor1 = 7; //PINO  UTILIZADO PELO SENSOR INFRAVERMELHO 1
const int pinoSensor2 = 8; //PINO  UTILIZADO PELO SENSOR INFRAVERMELHO 2

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(PIN_TRIGGER, PIN_ECHO);

void setup()
{
    Serial.begin(9600);
   configurarPinos();  
}

void loop()
{
    verificarDistancia();
    delay(INTERVALO_LEITURA);
}

void configurarPinos()
{
pinMode(pinoSensor1, INPUT);
pinMode(pinoSensor2, INPUT);
 }

/*
verificar a distância entre o objeto e o robô se a distância  for alta continua movimentando, 
se a distância for média diminui o pwm dos motores e se a distancia for baixa para na mesma hora
  */
void verificarDistancia()
{
      //pega  a distância atual lida pelo sensor
    distancia = getDistance();// distancia em cm 

  //caso a distância lida seja menor ou igual a 15 considerada uma distância de perigo
    // se a distância for de 15 cm parar os motores 
    if( digitalRead(pinoSensor) == LOW && distancia <= 15 ) 
  {
      //Colocar código do motor aqui
      }
    //caso a distância seja maior que 15 e menor ou igual a 60,
    //uma distância de atenção diminuir o pwm dos motores 
    else if(distancia <=60)  
  {
    // Colocar código do motor aqui

  }
  
    //para distâncias maiores que 60 ,uma distância sem perigo pwm dos motores continua alto
  
  else 
  {
    // Colocar código do motor aqui
  }
}

//  Função para a leitura da distância medida pelo sensor ultrassônico
int getDistance()
{
      //faz a leitura do sensor (em cm)
      int distanciaCM;
      long microsec = ultrasonic.timing();
      distanciaCM = ultrasonic.convert(microsec, Ultrasonic::CM);
      return distanciaCM;
}
