                #include <FS.h>                   //this needs to be first, or it all crashes and burns...
                #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
                //needed for library
                #include <DNSServer.h>
                #include <ESP8266WebServer.h>
                #include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
                #include <WiFiServer.h>
                #include <EEPROM.h>   
                #include <PubSubClient.h>     
                #include <ArduinoJson.h>        
                #include <EEPROMFREEDOM.h>
                #include <ConverteStringFreedom.h>
               
                
                #define MEM_ALOC_SIZE 512
                
WiFiServer server(80);
WiFiClient espClient;
PubSubClient MQTT(espClient);
ConverteStringFreedom _stringFreedom;
EEPROMFREEDOM _ePFreedom;
WiFiManager wifiManager;

//############ 1.. CONFIGURAÇÃO MQTT
const char* BROKER_MQTT = "broker.hivemq.com"; //URL DO SERVIDOR MQTT
int BROKER_PORT = 1883;                      // Porta do Broker MQTT
#define ID_MQTT  "FREEDOMBOARD0003"             //ID DO DISPOSITIVO PARA MQTT, DEVE SER ÚNICO
const char * TOPIC_SUBSCRIBE1 = "FREEDOM/ESCUTA/MSG/FREEDOMBOARD0003";   //TÓPICO QUE O DISPOSITIVO SE ESCREVE, DEVE SOLOCAR O TÓPICO NO MQTT.CONECT() PARA FUNCIONAR
const char * TOPIC_SUBSCRIBE2 = "FREEDOMBOARD/ESCUTA/GETALL/FREEDOMBOARD0003";   //TÓPICO QUE O DISPOSITIVO SE ESCREVE  DEVE SOLOCAR O TÓPICO NO MQTT.CONECT() PARA FUNCIONAR
const char * TOPIC_SUBSCRIBE3 = "FREEDOMBOARD/ESCUTA/UPDATE/FREEDOMBOARD0003";
#define TOPIC_PUBLISH_ANGULAR "FREEDOMBOARD/RESPOSTA/GETALL/ANGULAR"  //TÓPICO QUE O DISPOSITIVO RESPONDE
#define TOPIC_PUBLISH_API "FREEDOM/RESPOSTA/GETALL/ANGULAR"  //TÓPICO QUE O DISPOSITIVO RESPONDE


//############ .. DEFINIÇÃO DO TIPO DA PLACA
const String TIPO = "FB01";
const String NOME = "Freedom Board 03";

//############ 2.. CONFIGURAÇÃO EEPROM               
int ENDERECO_STATUS_EEPROM = 0;  //ENDEREÇO NA EEPROM ONDE O VALOR DO STATUS ESTA SALVO
int STATUS_DEVICE = 0;    //VARIÁVEL QUE ARMAZENA STATUS DO DISPOSITIVO "QUANDO O DISPOSITIVO FOR LIGADO"

int ENDERECO_STATUS_RELE_01_EEPROM = 2;
int ENDERECO_STATUS_RELE_02_EEPROM = 4;
int ENDERECO_STATUS_RELE_03_EEPROM = 6;
int ENDERECO_STATUS_RELE_04_EEPROM = 8;

int STATUS_RELE_01 = 0;
int STATUS_RELE_02 = 1;
int STATUS_RELE_03 = 0;
int STATUS_RELE_04 = 1;

int ENDERECO_DATA_ULTIMA_MODIFICACAO = 10;  //ENDEREÇO NA EEPROM DA DATA DE ULTIMA MODIFICAÇÃO DO DISPOSITIVO
String DATA_ULTIMA_MODIFICACAO = "";  // VARIÁVEL QUE ARMAZENA O VALOR DA DATA DA ÚLTIMA MODIFICAÇÃO DO DISPOSITIVO
                

//############ 3.. DEFINIÇÃO DAS PINOS DO DISPOSITIVO
int LED_PLACA = 2; 

//############ 3.1.. DEFINIÇÃO VARIÁVEIS QUE RECEBEM VALORES DE SONSORES
String NOME_SENSOR_O1 = "Salão principal";
float VALOR_SENSOR_01 = 34.5;




//############ id

String id = ID_MQTT;


//############ INTERRUPÇÃO
extern "C"{
#include "user_interface.h"
}
os_timer_t mTimer;
bool       _timeout = false;
//Nunca execute nada na interrupcao, apenas setar flags!
void tCallback(void *tCall){
    _timeout = true;
}
void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 15000, true);
}


void setup() {

    usrInit();

  pinMode(LED_PLACA, OUTPUT);
   
  Serial.begin(115200);
  
  wifiManager.setTimeout(30);

  //wifiManager.resetSettings();

//############ 5.. COLETANDO DADOS SALVOS NA EEPROM  
  EEPROM.begin(MEM_ALOC_SIZE); 
  STATUS_DEVICE = _ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_EEPROM); //VERIFICA ÚLTIMO ESTADO SALVO NA EEPROM
  DATA_ULTIMA_MODIFICACAO = _ePFreedom.leDateTimeNaEEPROM(ENDERECO_DATA_ULTIMA_MODIFICACAO); //VERIFICA ÚLTIMA SALVA DATA NA EEPROM


//############ 6.. INICIALIZANDO PINOS, CONFORME CONGIGURAÇÃO ==> 3 <==


  digitalWrite(LED_PLACA, STATUS_DEVICE);  //


//############ 7.. CONECTA NA REDE WIFI 
 
  if(!wifiManager.autoConnect("FreedomBoard")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  } 

  Serial.println("connected...yeey :)");

//############ 8.. PRINT NO SERIAL DAS INFORMAÇÕES INICIAIS ==> 12 <==
  informaStatusEDataDeModificacao();


//############ 9.. CONFIGURANDO MQTT COM VALORES CONFIGURADOS EM ==> 1 <==  
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(recebePacote);


//############ 10.. CONECTANDO SERVIDOR HTTP (PRECISA VER SE HÁ NECESSIDADE)
  server.begin(); //HTTP
  Serial.println("Servidor HTTP inicializado");
}

void loop() {


//############ 11.. CONECTA MQTT, FICA ESCUTANDO MSG
    mqttReconnect();
    MQTT.loop();
    yield();

  
  ///// teste simula sensor de temperatura
if(VALOR_SENSOR_01 <= 50.0){
  VALOR_SENSOR_01 = (VALOR_SENSOR_01 + 1.0);
  }
if(VALOR_SENSOR_01 >= 49.0){
  VALOR_SENSOR_01 = 1;
  } 


  
//INTERRUPÇÃO
     if (_timeout){
      Serial.println("15 segundos, responde GetAll");
      responseGetAllMQTT();
      _timeout = false;
  }
  
}

//############ 12.. INFORMAÇÕES PARA VOID ==> 8 <==
void informaStatusEDataDeModificacao(){   
  Serial.println(); 
  
  String mac = WiFi.macAddress();
  Serial.println("Endereço MAC-WiFi:   ");  
  Serial.println(mac);     
  
  Serial.println("Status Atualizado:   ");
  Serial.println(getStatusForApi(STATUS_DEVICE));
  
  Serial.println("Data atualizada:   ");
  Serial.println(DATA_ULTIMA_MODIFICACAO);

  Serial.println();
 }


//############ 13.. NA WEMOS O VALOR LOW E HIGH  SÃO "INVERTIDOS": SÃO RESPECTIVAMENTE 1 PARA LOW E 0 PARA HIGH, ESSA FUNÇÃO MUDA O STATUS PARA MELHOR ENTENDIMENTO NO MQTT OU API
int getStatusForApi(int valor) {
  if(valor == 1)
    {
      return 0;
    }
    else if (valor == 0)
    {
      return 1;
    }  
}



//############ 14.. RECEBE MSG E JSON PELO TÓPICOS MQTT
void recebePacote(char* topic, byte* payload, unsigned int length) {
    String msg;

    for(int i = 0; i < length; i++)   //CONVERTE A MSG EM UMA STRING
    {
       char c = (char)payload[i];
       msg += c;
    }

    


if (strcmp(topic, TOPIC_SUBSCRIBE3)==0){ // SE O CANAL QUE OUVE MSG RECEBIDA FOR O SELECIONADO, ENTÃO..
    Serial.println("-----------update solicitado---------------");
    Serial.println("................");
    Serial.println(msg);
    Serial.println("................");

    StaticJsonBuffer<300> JSONBuffer;
    JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message

    if (!parsed.success()) { //Check for errors in parsing
      Serial.println("Parsing failed");
      return;
     }

//Atualizando led da placa
int statusLedPlaca = parsed ["ledPlaca"];
    digitalWrite(LED_PLACA, statusLedPlaca);
    _ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_EEPROM, statusLedPlaca);

int statusRele01 = parsed["statusRele01"];
int statusRele02 = parsed["statusRele02"];
int statusRele03 = parsed["statusRele03"];
int statusRele04 = parsed["statusRele04"];

STATUS_RELE_01 = statusRele01;
STATUS_RELE_02 = statusRele02;
STATUS_RELE_03 = statusRele03;
STATUS_RELE_04 = statusRele04;

        yield();
        responseGetAllMQTT();    
            
  }

if(strcmp(topic, TOPIC_SUBSCRIBE1)==0){
       if (msg == "1" || msg.indexOf("1") != -1) {
       digitalWrite(LED_PLACA, LOW);
       _ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_EEPROM, 0);  //Salvando status na EEPROM
       //MQTT.publish(TOPIC_PUBLISH_ANGULAR, "ligado"); 
       responseGetAllMQTT();
    }

    if (msg == "0" || msg.indexOf("0") != -1) {
       digitalWrite(LED_PLACA, HIGH);
       _ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_EEPROM, 1);  //Salvando status na EEPROM
       responseGetAllMQTT();
       //MQTT.publish(TOPIC_PUBLISH_ANGULAR, "desligado");
    }

    if (msg == "status" || msg.indexOf("status") != -1) {

       char c [2];
       String s;
       s = String(_ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_EEPROM));
       s.toCharArray(c,2);
       responseGetAllMQTT();
       //MQTT.publish(TOPIC_PUBLISH_ANGULAR, c);
    }
}

if(strcmp(topic, TOPIC_SUBSCRIBE2)==0){
  Serial.println("GetAll solicitado");
if(msg == "GETALL" || msg.indexOf("GETALL") != -1){

      
      responseGetAllMQTT();
      
   } 

}
}


//############ 15.. FUNÇÃO QUE CRIA O JSON PARA RESPOSTA GETALL
void responseGetAllMQTT(){
       
      StaticJsonBuffer<350> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();

      char JSONmessageBuffer[350]; //CRIA UM CHAR PARA COLOCAR O JSON

     //CRIAÇÃO DO JSON E SUAS PROPRIEDADES
     root["id"] = id;  
     root["nome"] = NOME;
     root["tipo"] = TIPO; 
     root["ledPlaca"] = _ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_EEPROM);
     root["dataUltimaModificacao"] = DATA_ULTIMA_MODIFICACAO;
     root["statusRele01"] = STATUS_RELE_01;
     root["statusRele02"] = STATUS_RELE_02;
     root["statusRele03"] = STATUS_RELE_03;
     root["statusRele04"] = STATUS_RELE_04;
     root["topicoMQTTEscutaUpdate"] = TOPIC_SUBSCRIBE3;
     
     JsonArray& sensor01 = root.createNestedArray("sensor01");
      sensor01.add(NOME_SENSOR_O1);
      sensor01.add(VALOR_SENSOR_01);
     
     root.printTo(Serial);

     root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer)); // CONVERT O JSON CRIADO PARA O CHAR CRIADO
     
     yield();
      MQTT.publish(TOPIC_PUBLISH_ANGULAR, JSONmessageBuffer);  //PUBLICA JSON, LEMBRANDO QUE DEVE MUDAR O TAMANHO DO BUFFER NA BIBLIOTECA.H DO MQTT "TEM Q PESQUISAR, EU ESQUECI ;D"  
      MQTT.publish(TOPIC_PUBLISH_API, JSONmessageBuffer);
      
      

}


//############ 16.. CONECTA AO MQTT, E ASSINA OS TÓPICOS PRÉ DETERMINADOS NO =>> 1.. <==      
void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.println("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE1);
            MQTT.subscribe(TOPIC_SUBSCRIBE2);
            MQTT.subscribe(TOPIC_SUBSCRIBE3);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 2s");
            delay(2000);
            reconectaWiFi();
        }
    }
}

void reconectaWiFi(){
    WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("Nova conexao requisitada...");
  while(!client.available()){
    delay(1);
  }
 }


//############ 17. RECONECTA AO MQTT
void mqttReconnect(){
  if(!MQTT.connected())
  {
    Serial.println("Conectando..");
    conectaMQTT();
  }
}

void resetaConfiguracaoWiFI(void) {
  //Reset das definicoes de rede
  wifiManager.resetSettings();
  delay(1500);
  ESP.reset();
}
