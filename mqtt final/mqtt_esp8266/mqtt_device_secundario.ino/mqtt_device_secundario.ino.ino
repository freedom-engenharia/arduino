 //#REGION IP do Device //////////////////////////////////////
                const String iPDevice = "192.168.0.196";            
//#ENDREGION IP do Device //////////////////////////////////

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
                #define MEM_ALOC_SIZE 128 
                
WiFiServer server(80);
WiFiClient espClient;
PubSubClient MQTT(espClient);
ConverteStringFreedom _stringFreedom;
EEPROMFREEDOM _ePFreedom;

const char* BROKER_MQTT = "test.mosquitto.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT
#define ID_MQTT  "WEMRE00002"             //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
const char * TOPIC_SUBSCRIBE1 = "PUBLICAWEMRE00002";   //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
const char * TOPIC_SUBSCRIBE2 = "UPDATE/PUBLICAWEMRE00002";
#define TOPIC_PUBLISH "RESPOSTAWEMRE00002"


//#REGION Configuração EEPROM ////////////////////////////                
                int ENDERECO_STATUS_EEPROM = 0;
                int STATUS_DEVICE = 0;
                int ENDERECO_DATA_ULTIMA_MODIFICACAO = 2;
                String DATA_ULTIMA_MODIFICACAO = "";
//#ENDREGION Configuração EEPROM ////////////////////////////

//#REGION PORTAS DIGITAIS //////////////////////////////////
      int LED_PLACA = 2;      
//#ENDREGION/////////////////////////////////////////////////

//#REGION TIPO DA PLACA ///////////////////////////////////
      int TIPO = 1;
//#ENDREGION//////////////////////////////////////////////

//#REGION CONFIGURAÇÂO DO JSON////////////////

//ENDREGION///////////////////

void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.setTimeout(180);
  
  EEPROM.begin(MEM_ALOC_SIZE); 
  STATUS_DEVICE = _ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_EEPROM); //Verifica o status(on/off) na EEPROM
  DATA_ULTIMA_MODIFICACAO = _ePFreedom.leDateTimeNaEEPROM(ENDERECO_DATA_ULTIMA_MODIFICACAO);

  pinMode(LED_PLACA, OUTPUT); 
  digitalWrite(LED_PLACA, STATUS_DEVICE);
  
      IPAddress _ip;
       if (_ip.fromString(iPDevice)) { }
        
    
    IPAddress _gw = IPAddress(192, 168, 0, 1);
    IPAddress _sn = IPAddress(255, 255, 255, 0);
  
  //wifiManager.setAPStaticIPConfig(_ip, _gw, _sn);

  //WiFi.config(_ip, _gw, _sn);

  if(!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  } 

  Serial.println("connected...yeey :)");

  informaStatusEDataDeModificacao();
  
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(recebePacote);

  server.begin(); //HTTP
  Serial.println("Servidor HTTP inicializado");


}

void loop() {
  
  //servidorHTTP();
  mqttReconnect();
  MQTT.loop();
  yield();

  
}

void informaStatusEDataDeModificacao(){   
  Serial.println(); 
  
  String mac = WiFi.macAddress();
  Serial.print("Endereço MAC-WiFi:   ");  
  Serial.println(mac);  
      
  
  Serial.print("Status Atualizado:   ");
  Serial.println(getStatusForApi(STATUS_DEVICE));
  
  Serial.print("Data atualizada:   ");
  Serial.println(DATA_ULTIMA_MODIFICACAO);

  Serial.println();
 }
 
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

void recebePacote(char* topic, byte* payload, unsigned int length) {
    String msg;

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
    Serial.println("Menssagem recebida pelo canal : ");
    Serial.println(msg);

 //recebe json
      if (strcmp(topic,"UPDATE/PUBLICAWEMRE00002")==0){
       Serial.println("Entrou no canal de updade");
       StaticJsonBuffer<300> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(msg);
        
            const char* WEMOS = root["WEMOS"];
            String id = root["id"];
            String tipo = root["tipo"][0];
            String statusDevice = root["statusDevice"][1];
            String dataUltimaModificacao = root["dataUltimaModificacao"][2];
            
            Serial.println("Esse é JSON");
            Serial.println(WEMOS);
            Serial.println(id);
            Serial.println(tipo);
            Serial.println(statusDevice);
            Serial.println(dataUltimaModificacao);
            Serial.println("Fim do id");
  }
  
   

      

    if (msg == "1" || msg.indexOf("1") != -1) {
       digitalWrite(LED_PLACA, LOW);
       _ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_EEPROM, 0);  //Salvando status na EEPROM
       //MQTT.publish(TOPIC_PUBLISH, "ligado"); 
       responseGetAllMQTT();
    }

    if (msg == "0" || msg.indexOf("0") != -1) {
       digitalWrite(LED_PLACA, HIGH);
       _ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_EEPROM, 1);  //Salvando status na EEPROM
       responseGetAllMQTT();
       //MQTT.publish(TOPIC_PUBLISH, "desligado");
    }

    if (msg == "status" || msg.indexOf("status") != -1) {

       char c [2];
       String s;
       s = String(_ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_EEPROM));
       s.toCharArray(c,2);
       responseGetAllMQTT();
       //MQTT.publish(TOPIC_PUBLISH, c);
    }

    if(msg == "getAll" || msg.indexOf("getAll") != -1){

      responseGetAllMQTT();
   } 

}

void responseGetAllMQTT(){
       
      StaticJsonBuffer<300> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();

      char JSONmessageBuffer[200]; //Cria char com tamanho do JSON

     // Criação do json
     root["id"] = ID_MQTT;  
     root["tipo"] = String(TIPO); 
     root["statusDevice"] = String(getStatusForApi(_ePFreedom.leStatusNaEEPROM(ENDERECO_STATUS_EEPROM)));
     root["ledPlaca"] = String(LED_PLACA);
     root["dataUltimaModificacao"] = DATA_ULTIMA_MODIFICACAO;
     root.printTo(Serial);

     root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer)); //converte json em char

      MQTT.publish(TOPIC_PUBLISH, JSONmessageBuffer);
      
      }
      
void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.println("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE1);
            MQTT.subscribe(TOPIC_SUBSCRIBE2);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

void mqttReconnect(){
  if(!MQTT.connected())
  {
    Serial.println("Conectando..");
    conectaMQTT();
  }
}

void servidorHTTP(){
                  //Serial.println("Conectando HTTP");
                  //Verificando se o servidor esta pronto.
                    WiFiClient client = server.available();
                    if (!client) {
                      return;
                    }
                  
                    //Verificando se o servidor recebeu alguma requisicao
                    while (!client.available()) {
                      delay(1);
                    }
                  
                    //Obtendo a requisicao vinda do browser
                    String req = client.readStringUntil('\r');
                    
                    //Sugestao dada por Enrico Orlando
                    if(req == "GET /favicon.ico HTTP/1.1"){
                        req = client.readStringUntil('\r');
                       
                    }
                    
                    client.flush();

                    String dataTime = _stringFreedom.pegaDataNaRequisicaoRecebda(req);
                  
                    //Iniciando o buffer que ira conter a pagina HTML que sera enviada para o browser.
                    String bufLigado = "";                  
                    bufLigado += "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
                    bufLigado += iPDevice;
                    bufLigado += " Ligado ";
                    bufLigado += dataTime;
                    

                    String bufDesligado = "";
                    bufDesligado += "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
                    bufDesligado += iPDevice;
                    bufDesligado += " Desligado ";
                    bufDesligado += dataTime;
                    
                    //Enviando para o browser a 'pagina' criada.
                    if (req.indexOf("on") != -1)
                    {
                      _ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_EEPROM, 0);  //Salvando status na EEPROM
                          
                      _ePFreedom.escreveDateTimeNaEEPROM(ENDERECO_DATA_ULTIMA_MODIFICACAO, dataTime);
                      
                      client.print(bufLigado); //Enviando Resposta para API
                    }
                    else if (req.indexOf("off") != -1)
                    {
                      
                     _ePFreedom.escreveStatusNaEEPROM(ENDERECO_STATUS_EEPROM, 1);  //Salvando status na EEPROM
                     
                     _ePFreedom.escreveDateTimeNaEEPROM(ENDERECO_DATA_ULTIMA_MODIFICACAO, dataTime);
                     
                      client.print(bufDesligado);  //Enviando Resposta para API
                    }
                    
                    client.flush();
                  
                    //Analisando a requisicao recebida para decidir se liga ou desliga a lampada
                    if (req.indexOf("on") != -1)
                    {
                      digitalWrite(LED_PLACA, LOW);
                    }
                    else if (req.indexOf("off") != -1)
                    {
                      digitalWrite(LED_PLACA, HIGH);
                    }
                    else
                    {
                      //Requisicao invalida!
                      client.stop();
                    }
}           
