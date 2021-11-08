/*Autor: Gabriel Borges Pinheiro
 Codigo não completo.
Falta: - obtençao da direcao do robo (via magnetometro)
       - Comando de movimentacao das rodas para o Controlador
*/
//Necessario preencher essas definicoes com os dados do estabelecimente!
#define SSID_CENTRAL  "TP-Link_8A8C" // Nome da rede wi-fi
#define PASSWORD  "4123456789" // Senha da rede  wi-fi
#define SSID_BASE "IDEIAPADnet" // ID da base de carregamento
#define SSID_MESA1 "Fulano" // ID da base de carregamento
#define SERVER_HOST "192.168.1.103" // IP local do servidor (central) websocket
#define SERVER_PORT 8091 // Porta de conexão do servidor (deve ser a mesma do back-end!)
#define LOCALIZACAO_CENTRAL {-4,-1} //Coordenadas (x,y) do roteador da rede em metros
#define LOCALIZACAO_BASE  {0.0,0.0} //Coordenadas (x,y) da base de carregamento em metros
#define LOCALIZACAO_MESA1 {-1,-3.5} //Coordenadas (x,y) da mesa 01 em metros
//Pronto! Os proximos campos nao precisam ser modificados

//ATENCAO! O pino para bms,alem de ser conectado na saida de alarme deste, deve conectado com um resistor no pino de 3,3V!
#define pinoBMS 18 // Pino utilizado para sinal de 'alarme' do BMS (parte elétrica)
#define media_movel 10 //Largura do filtro de media movel

#include <ArduinoWebsockets.h>

typedef struct{
  float x = 0;
  float y = 0;
} coordenada;

typedef struct {
  char ssid[];
  int network=0;
  int32_t rssi=0;
  float distancia;
  coordenada localizacao;
} Rede;

Rede central,base,mesa1;

// Utilizamos o namespace de websocket para podermos utilizar a classe WebsocketsClient
using namespace websockets;
WebsocketsClient client;// Objeto websocket client

//Fucoes utilizadas
void atribuiCoordenada(coordenada* localizacao, float vetor[]);
String destino; //SSID para onde ir
int32_t getNetworkNumber(const String target_ssid);//Obtem o numero da rede wi-fi do SSID associado
int32_t getRSSI(const int network);//Obtem o valor de potencia do sinal (RSSI) da rede wi-fi do número associado
void callBack(void);//Rotina para funcionamento do robô como cliente
float calculaDistancia(int32_t RSSI);//Algoritmo para calcular a distancia de um dispositivo emissor baseado no seu RSSI
coordenada trilatera(Rede* central,Rede* base,Rede* mesa);
//float trilatera(float d1,d2,d3){}

void setup(){
  float loc_central[] = LOCALIZACAO_CENTRAL, loc_base[] = LOCALIZACAO_BASE, loc_mesa1[] = LOCALIZACAO_MESA1;

  strcpy(central.ssid, SSID_CENTRAL);
  strcpy(base.ssid, SSID_BASE);
  strcpy(mesa1.ssid, SSID_MESA1);
  atribuiCoordenada(&central.localizacao, loc_central);
  atribuiCoordenada(&mesa1.localizacao, loc_mesa1);
  atribuiCoordenada(&base.localizacao, loc_base);

  // Inicia a comunicacao UART e wi-fi
  Serial.begin(115200);
  WiFi.begin();
  
  // Conectamos o wifi com o roteador
  WiFi.begin(SSID_CENTRAL, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
      delay(1000);
  }

  // Tentamos conectar com o websockets server
  bool connected = client.connect(SERVER_HOST, SERVER_PORT, "/");
  // Se foi possível conectar
  if(connected){
      // Enviamos uma msg para o servidor
      client.send("Robo garçom a servico");
  }   // Se não foi possível conectar
  else{
      return;
  }
  //Obtem numero de rede desses dispositivos
  central.network = getNetworkNumber(SSID_CENTRAL);
  base.network = getNetworkNumber(SSID_BASE);
  //Faz a leitura do pino_alarme do BMS e envia mensagem p/ servidor central
  pinMode(pinoBMS,OUTPUT);
  if(!digitalRead(pinoBMS)){
    client.send("Erro na bateria do robo!");
    delay(1000);
  }
  //Interrupção para chamar setup() e ler o pino do BMS
  attachInterrupt(pinoBMS, setup, LOW);
  //Chama rotina de cliente para realizar tarefas rotineiras do robo
  callBack();
}

void loop(){
  //  De tempo em tempo, o websocket client checa por novas mensagens recebidas e repetir rotina callback()
  if(client.available()) 
    client.poll();
  delay(500);
}

//Corpo das funcoes
void atribuiCoordenada(coordenada* localizacao, float vetor[]){
  localizacao->x = vetor[0]; 
  localizacao->y = vetor[1];
  return;
}
int32_t getNetworkNumber(const String target_ssid){
  byte available_networks = WiFi.scanNetworks(); //escaneia o numero de redes
  int32_t rssi = 0;

  for (int32_t network = 0; network < available_networks; network++) {
    if (WiFi.SSID(network).compareTo(target_ssid) == 0) {return network;} //compara se alguma das redes encontradas é a que desejamos e retorna ela 
  }
  return 0;
}
//
int32_t getRSSI(const int network) {
  int acum=0,rssi;
  for (int sample=0; sample<media_movel; sample++){
    if(rssi = WiFi.RSSI(network)){acum += rssi;}
  }
  return  acum/media_movel; ///retorna o ssid da rede  
}
void callBack(void){ //char* ssID_mesa){  
  // Iniciamos o callback onde as mesagens serão recebidas
  client.onMessage([&](WebsocketsMessage message){        
  if(message.data().equalsIgnoreCase("Pare")){
    Serial.print("Pare");  
  }
  else if(message.data().equalsIgnoreCase("base")){
    Rede outra;
    int32_t rssi_outra;
//    int network_outra=0;
    
    client.send("Voltando a base");
    destino = String(message.data());
    client.send("Indo a");
    client.send(destino);
    delay(500);
    //Rotina de calculos até chegar ao destino
    do{
      central.rssi = getRSSI(central.network);
      base.rssi = getRSSI(base.network);
      outra.rssi = getRSSI(outra.network);
      //Calculo da distancia dos dispositivos
      base.distancia = calculaDistancia(base.rssi);
      central.distancia = calculaDistancia(central.rssi);
      outra.distancia = calculaDistancia(outra.rssi);
      //Trilatarecao para descobrir a coordenada do robo
      coordenada posicao = trilatera(&central,&base,&outra);
//    FuncaoMedeDirecao(); (FALTA)
//    FuncaoComandoDirecaoControlador(); (FALTA)
      delay(500);
    }while(outra.distancia != 0); //Enquanto a distancia do alvo existir a rotina é repetida
  }
  else{
    destino = String(message.data());
    mesa1.network = getNetworkNumber(destino);
    if(!mesa1.network){
     client.send("Robo diz: Nao sei para onde ir");
      delay(500);
      return;
    }
    client.send("Indo a");
    client.send(destino);
    delay(500);
    //Rotina de calculos até chegar no destino
    do{
      central.rssi = getRSSI(central.network);
      base.rssi = getRSSI(base.network);
      mesa1.rssi = getRSSI(mesa1.network);
      //Calculo da distancia dos dispositivos
      base.distancia = calculaDistancia(base.rssi);
      central.distancia = calculaDistancia(central.rssi);
      mesa1.distancia = calculaDistancia(mesa1.rssi);
      //Trilatarecao para descobrir a coordenada do robo
      coordenada posicao = trilatera(&central,&base,&mesa1);
//    FuncaoMedeDirecao(); (FALTA)
//    FuncaoComandoDirecaoControlador(); (FALTA)
      delay(500);
    }while(mesa1.distancia != 0); //Enquanto a distancia do alvo existir a rotina é repetida
  }
  });
  return;     
}
//
float calculaDistancia(int32_t RSSI){
  float distancia;
  const int32_t Pr = RSSI, P0 = -40; //dBm
  const int n = 12;
  //Essa formula eh abordada no relatorio
  distancia = 10^(-(P0+Pr)/(n));

  return distancia;
}
//
coordenada trilatera(Rede* central,Rede* base,Rede* mesa){
  float d1,x1,y1,d2,x2,y2,d3,x3,y3, xp=0, yp=0;
  coordenada p;
  
  x1 = central->localizacao.x; y1 = central->localizacao.y;
  d1 = central->distancia;
  x2 = base->localizacao.x; y2 = base->localizacao.y;
  d2 = base->distancia;
  x3 = mesa->localizacao.x; y3 = mesa->localizacao.y;
  d3 = mesa->distancia;
  //Essa formula eh abordada no relatorio
  xp = ((powf(d2,2)-powf(d3,2)+powf(y3,2)-powf(y2,2)+powf(x3,2)-powf(x2,2))*(y2-y1)-(powf(d1,2)-powf(d2,2)+powf(y2,2)-powf(y1,2)+powf(x2,2)-powf(x1,2))*(y3-y2))/(2*((x3-x2)*(y2-y1)-(x2-x1)*(y3-y2)));
  yp = ((powf(d2,2)-powf(d3,2)+powf(y3,2)-powf(y2,2)+powf(x3,2)-powf(x2,2))*(x2-x1)-(powf(d1,2)-powf(d2,2)+powf(y2,2)-powf(y1,2)+powf(x2,2)-powf(x1,2))*(x3-x2))/(2*((y3-y2)*(x2-x1)-(y2-y1)*(x3-x2)));
  p.x = xp; p.y = yp;
  return p;
}
