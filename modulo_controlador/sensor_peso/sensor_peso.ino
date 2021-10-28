// INCLUSÃO DE BIBLIOTECAS
#include <HX711.h>

// DEFINIÇÕES DE PINOS
#define pinDT  2
#define pinSCK  3

// DEFINIÇÕES
#define pesoMin 0.00
#define pesoMax 30.0

#define escala 0.0f

// INSTANCIANDO OBJETOS
HX711 scale;

// DECLARAÇÃO DE VARIÁVEIS  
float medida=0;
 
void setup() {
    Serial.begin(9600);
    scale.begin(pinDT, pinSCK); // CONFIGURANDO OS PINOS DA BALANÇA
    scale.set_scale(escala); // ENVIANDO O VALOR DA ESCALA CALIBRADO
    delay(2000);
    scale.tare(); // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA
}

void loop() {

  if(colocar lógica da localização ){ // se o robô chegou no local da mesa faça 
     
        scale.power_up(); // LIGANDO O SENSOR
    delay(1500);    
        medida = scale.get_units(5); // SALVANDO NA VARIAVEL O VALOR DA MÉDIA DE 5 MEDIDAS
    
        if (medida <= pesoMin ){ // CONFERE O PESO NA BALANÇA
      for (int j= 0;j<20;j++){  // TEMPO DE ESPERA ANTES DA TAMPA FECHAR --30segundos
        get_time();
      }
    
    delay(100);
    }
    
      } else {
    
    delay(100);
    }

   }
  
// IMPLEMENTO DE FUNÇÕES
void get_time()
{
    delay(1500);
}
