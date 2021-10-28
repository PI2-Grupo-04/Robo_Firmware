/*Codigo não completo.
Falta: - triangulacao das distancias;
       - obtençao da direcao do robo (via magnetometro)
       - Comando de movimentacao das rodas para o Controlador
*/

#define pinoBMS 18 // Pino utilizado para sinal de 'alarme' do BMS (parte elétrica)
#define media_movel 10 //Largura do filtro de media movel

#include <ArduinoWebsockets.h>

const char* login_central = "SSID_roteador_central"; // Nome da rede wi-fi
const char* password = "Batatinha1,2,3"; // Senha da rede  wi-fi
const char* ssID_central = "SSID_roteador_central"; // Nome da rede wi-fi
const String ssID_base = "base_ID"; // ID da base de carregamento

const char* websockets_server_host = "192.168.1.103"; // IP local do servidor (central) websocket
const int websockets_server_port = 8091; // Porta de conexão do servidor (deve ser a mesma do back-end!)
String destino; //SSID para onde ir

int32_t rssi_central, rssi_base, rssi_mesa;//RSSI dos dispositivos
int network_central=0,network_base=0,network_mesa=0;//Numero da rede wi-fi dos dispositivos

// Utilizamos o namespace de websocket para podermos utilizar a classe WebsocketsClient
using namespace websockets;
WebsocketsClient client;// Objeto websocket client

//Fucoes utilizadas
int32_t getNetworkNumber(const String target_ssid);//Obtem o numero da rede wi-fi do SSID associado
int32_t getRSSI(const int network);//Obtem o valor de potencia do sinal (RSSI) da rede wi-fi do número associado
void callBack(void);//Rotina para funcionamento do robô como cliente
float calculaDistancia(int32_t RSSI);//Algoritmo para calcular a distancia de um dispositivo emissor baseado no seu RSSI

void setup() 
{
    // Inicia a comunicacao UART e wi-fi
    Serial.begin(115200);
    WiFi.begin();
    
    // Conectamos o wifi com o roteador
    WiFi.begin(login_central, password);
    // Enquanto não conectar printamos um "."
    while(WiFi.status() != WL_CONNECTED){
        Serial.println(".");
        delay(1000);
    }
    // Exibimos "WiFi Conectado"
    Serial.println("Conectado com Wifi. Conectando com servidor...");

    // Tentamos conectar com o websockets server
    bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    // Se foi possível conectar
    if(connected){
        // Exibimos mensagem de sucesso
        Serial.println("Conectado!");
        // Enviamos uma msg para o servidor
        client.send("Robo garçom a servico");
    }   // Se não foi possível conectar
    else{
        // Exibimos mensagem de falha
        Serial.println("Not Connected!");
        return;
    }
    //Obtem numero de rede desses dispositivos
    network_central = getNetworkNumber(ssID_central);
    network_base = getNetworkNumber(ssID_base);
    //Faz a leitura do pino_alarme do BMS e envia mensagem p/ servidor central
    if(!digitalRead(pinoBMS)){
      client.send("Erro na bateria do robo!");
      Serial.print("Erro na bateria do robo!");
      delay(10000);
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


int32_t getNetworkNumber(const String target_ssid){
  byte available_networks = WiFi.scanNetworks(); //escaneia o numero de redes
  int32_t rssi = 0;

  for (int32_t network = 0; network < available_networks; network++) {
    if (WiFi.SSID(network).compareTo(target_ssid) == 0) {return network;} //compara se alguma das redes encontradas é a que desejamos e retorna ela 
  }
  return 0;
}

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
  // Exibimos a mensagem recebida na serial
  Serial.print("Mensagem recebida: ");
  Serial.println(message.data());

  if(message.data().equalsIgnoreCase("Pare")){
    Serial.print("Pare");  
    //[Falta funcao para comando UART do controlador]
  }
    
  else if(message.data().equalsIgnoreCase("base")){
    float d_base, d_central, d_outra;
    int32_t rssi_outra;
    int network_outra=0;
    
    client.send("Voltando a base");
    Serial.println("Robo: Voltando a base");
    while((network_outra==network_central)||(network_outra==network_base)){network_outra++;}
      rssi_central = getRSSI(network_central);
      rssi_base = getRSSI(network_base);
      rssi_outra = getRSSI(network_outra);
      Serial.print("Sinal de ");
      Serial.print(ssID_central);
      Serial.print(" força: ");
      Serial.print(rssi_central);
      Serial.println("dBm");
      Serial.print("Sinal de ");
      Serial.print(ssID_base);
      Serial.print(" força: ");
      Serial.print(rssi_base);
      Serial.println("dBm");
      Serial.print("Sinal de network");
      Serial.print(network_outra);
      Serial.print(" força: ");
      Serial.print(rssi_base);
      Serial.println("dBm");       
      d_base = calculaDistancia(rssi_base);
      d_central = calculaDistancia(rssi_central);
      d_outra = calculaDistancia(rssi_outra);
      Serial.print(" distancia da base: ");
      Serial.print(d_base);
      Serial.println("m");
      Serial.print(" distancia da central: ");
      Serial.print(d_central);
      Serial.println("m");
      Serial.print(" distancia da outra: ");
      Serial.print(d_outra);
      Serial.println("m");
    }
    else{
      float d_base=0, d_central=0, d_mesa=0;
      int32_t rssi_outra;
      
      destino = String(message.data());
      network_mesa = getNetworkNumber(destino);
      Serial.print("Numero network:");
      Serial.println(network_mesa);
      if(!network_mesa){
        Serial.print("Robo diz: Nao sei para onde ir");
        delay(500);
        return;
      }
      client.send("Indo a");
      client.send(destino);
      Serial.print("Robo: Indo a ");
      Serial.println(destino);
      Serial.print("Numero de");
      Serial.print(ssID_central);
      Serial.println(network_central);
      Serial.print("Numero de");
      Serial.print(ssID_base);
      Serial.println(network_base);
      Serial.print("Numero de");
      Serial.print(destino);
      Serial.println(network_mesa);
      delay(10000);
      //Calculo da distancia dos dispositivos
      do{
        rssi_central = getRSSI(network_central);
        rssi_base = getRSSI(network_base);
        rssi_mesa = getRSSI(network_mesa);
        Serial.print("Sinal de ");
        Serial.print(ssID_central);
        Serial.print(" força: ");
        Serial.print(rssi_central);
        Serial.println("dBm");
        Serial.print("Sinal de ");
        Serial.print(ssID_base);
        Serial.print(" força: ");
        Serial.print(rssi_base);
        Serial.println("dBm");
        Serial.print("Sinal da mesa:");
        Serial.print(network_mesa);
        Serial.print(" força: ");
        Serial.print(rssi_mesa);
        Serial.println("dBm");       
        d_base = calculaDistancia(rssi_base);
        d_central = calculaDistancia(rssi_central);
        d_mesa = calculaDistancia(rssi_mesa);
        Serial.print(" distancia da base: ");
        Serial.print(d_base);
        Serial.println("m");
        Serial.print(" distancia da central: ");
        Serial.print(d_central);
        Serial.println("m");
        Serial.print(" distancia da mesa: ");
        Serial.print(d_mesa);
        Serial.println("m");
        delay(500);
      }while(d_mesa != 0); //Enquanto a distancia do alvo existir a rotina é repetida
    }
  });
  return;     
}

float calculaDistancia(int32_t RSSI){
  float distancia;
  const int32_t Pr = RSSI, P0 = -40; //dBm
  const int n = 12;
  //Esse algoritmo eh abordado no relatório
  distancia = 10^(-(P0+Pr)/(n));

  return distancia;
}
