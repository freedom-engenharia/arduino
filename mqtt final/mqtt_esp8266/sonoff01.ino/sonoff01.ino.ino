#include <FS.h>                   
#include <ESP8266WiFi.h>          
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          
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

//############ VARIÁVEIS GLOBAIS

//-------------CONFIGURAÇÃO MQTT
const char* BROKER_MQTT = "broker.hivemq.com"; //URL DO SERVIDOR MQTT
int BROKER_PORT = 1883;                      // Porta do Broker MQTT
const char * ID_MQTT =  "ab5e7bd5-8ff8-40eb-8b20-a89a7d82716c" ;            //ID DO DISPOSITIVO PARA MQTT, DEVE SER ÚNICO
const char * TOPIC_SUBSCRIBEGETALL = "S1R/ESCUTA/GETALL/";   //TÓPICO QUE O DISPOSITIVO SE ESCREVE  DEVE SOLOCAR O TÓPICO NO MQTT.CONECT() PARA FUNCIONAR
const char * TOPIC_SUBSCRIBEGETTHIS = "S1R/ESCUTA/GETALL/ab5e7bd5-8ff8-40eb-8b20-a89a7d82716c";
const char * TOPIC_SUBSCRIBEUPDATE = "S1R/ESCUTA/UPDATE/ab5e7bd5-8ff8-40eb-8b20-a89a7d82716c" ;
#define TOPIC_PUBLISH_ANGULAR "FREEDOM/RESPOSTA/GETALL/ANGULAR"  //TÓPICO QUE O DISPOSITIVO RESPONDE
#define TOPIC_PUBLISH_API "S1R/RESPOSTA/GETALL/API"  //TÓPICO QUE O DISPOSITIVO RESPONDE


//-------------CONFIGURAÇÃO EEPROM               
int ENDERECO_STATUS_EEPROM = 0;  //ENDEREÇO NA EEPROM ONDE O VALOR DO STATUS ESTA SALVO
int STATUS_DEVICE = 0;    //VARIÁVEL QUE ARMAZENA STATUS DO DISPOSITIVO "QUANDO O DISPOSITIVO FOR LIGADO"
int ENDERECO_STATUS_RELE_01_EEPROM = 2;
int STATUS_RELE = 0;
int ENDERECO_DATA_ULTIMA_MODIFICACAO = 10;  //ENDEREÇO NA EEPROM DA DATA DE ULTIMA MODIFICAÇÃO DO DISPOSITIVO
String DATA_ULTIMA_MODIFICACAO = "";  // VARIÁVEL QUE ARMAZENA O VALOR DA DATA DA ÚLTIMA MODIFICAÇÃO DO DISPOSITIVO
               
//-------------MODELO
const String ID = String(ID_MQTT);
const String TIPO = "S1R";
const String NOME = "Sonoff 1 Relé";
const String TOPICOMQTTESCUTAUPDATE = String(TOPIC_SUBSCRIBEUPDATE);
const int LED_PLACA = 13; 
//STATUS_RELE;
const int RELE = 12;
const int BOTAO = 0;
const int PINOIO = 14;


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
    os_timer_arm(&mTimer, 5000, true);
}


void setup() {

    usrInit();

  pinMode(LED_PLACA, OUTPUT);
  
  DATA_ULTIMA_MODIFICACAO = _ePFreedom.leDateTimeNaEEPROM(ENDERECO_DATA_ULTIMA_MODIFICACAO); //VERIFICA ÚLTIMA SALVA DATA NA EEPROM
  STATUS_RELE = _ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_RELE_01_EEPROM);
  Serial.begin(115200);
  
  wifiManager.setTimeout(30);

  //wifiManager.resetSettings(); 

//############ 7.. CONECTA NA REDE WIFI 
 
  if(!wifiManager.autoConnect("FreedomBoard")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  } 

  Serial.println("connected...yeey :)");

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
//INTERRUPÇÃO
     if (_timeout){
      Serial.println("15 segundos, responde GetAll");
      responseGetAllMQTT();
      _timeout = false;
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

    


if (strcmp(topic, TOPIC_SUBSCRIBEUPDATE)==0){ // SE O CANAL QUE OUVE MSG RECEBIDA FOR O SELECIONADO, ENTÃO..
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
int ledPlaca = parsed ["statusLedPlaca"];
    digitalWrite(LED_PLACA, ledPlaca);

//Atualizando Status rele
int statusRele = parsed["statusRele"];
STATUS_RELE = statusRele;
digitalWrite(STATUS_RELE, statusRele);
_ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_RELE_01_EEPROM, statusRele);
        
        responseGetAllMQTT();    
            
  }


if(strcmp(topic, TOPIC_SUBSCRIBEGETALL)==0){
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
     root["id"] = ID;  
     root["nome"] = NOME;
     root["tipo"] = TIPO; 
     root["statusLedPlaca"] = _ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_EEPROM);
     root["statusRele"] = STATUS_RELE;
     root["topicoMQTTEscutaUpdate"] = TOPIC_SUBSCRIBEUPDATE;
     
    // JsonArray& sensor01 = root.createNestedArray("sensor01");
     // sensor01.add(NOME_SENSOR_O1);
    //  sensor01.add(VALOR_SENSOR_01);
     
     root.printTo(Serial);

     root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer)); // CONVERT O JSON CRIADO PARA O CHAR CRIADO

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
            MQTT.subscribe(TOPIC_SUBSCRIBEGETALL);
            MQTT.subscribe(TOPIC_SUBSCRIBEUPDATE);
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
