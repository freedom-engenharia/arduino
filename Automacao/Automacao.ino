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
                #include <EEPROMFREEDOM.h>
                #include <ConverteStringFreedom.h>
                #define MEM_ALOC_SIZE 128 
//#ENDREGION Importações /////////////////////////////
ConverteStringFreedom _stringFreedom;
EEPROMFREEDOM _ePFreedom;

//#REGION Configuração EEPROM ////////////////////////////
                int enderecoStatusEEPROM = 0;
                int statusDevice = 0;
                int enderecoDataModificacao = 2;
                String dataDeUltimaModificacao = "";
//#ENDREGION Configuração EEPROM ////////////////////////////

//#REGION GetStatus para API ///////////////////////////////


//#ENDREGION GetStatus para API ///////////////////////////////
      
WiFiServer server(80);
WiFiManager wifiManager;
WiFiClient wificlient;
/////////////////////MQTT//////////////////////////////////
                      
const char* BROKER_MQTT = "iot.eclipse.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT
#define ID_MQTT  "WEMRE00001"             //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_SUBSCRIBE "FREEDOMLED01"   //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wificlient);



//PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient

void setup() {
  Serial.begin(115200); 
  WiFiClient wifiClient; 
  
  EEPROM.begin(MEM_ALOC_SIZE); 
  statusDevice = _ePFreedom.leStatusNaEEPROM(enderecoStatusEEPROM); //Verifica o status(on/off) na EEPROM
  dataDeUltimaModificacao = _ePFreedom.leDateTimeNaEEPROM(enderecoDataModificacao);
  
  pinMode(2, OUTPUT); 
  digitalWrite(2, statusDevice); 
      
  conectaWiFi();
  informaStatusEDataDeModificacao();

}

void loop() {
  
//  Serial.println("entrou aio do serviro http");
//  delay(1000);
//  servidorHTTP();
//  Serial.println("saiu do Servidor http");
//  delay(1000);

/////////////////////MQTT//////////////////////////////////
  mantemConexoes();
  MQTT.loop();
  yield();


} //Fim do Loop

void servidorHTTP(){
//#REGION Requisição HTTP /////////////////////////////
                  
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
                      _ePFreedom.escreveStatusNaEEPROM(enderecoStatusEEPROM, 0);  //Salvando status na EEPROM
                          
                      _ePFreedom.escreveDateTimeNaEEPROM(enderecoDataModificacao, dataTime);
                      
                      client.print(bufLigado); //Enviando Resposta para API
                    }
                    else if (req.indexOf("off") != -1)
                    {
                      
                     _ePFreedom.escreveStatusNaEEPROM(enderecoStatusEEPROM, 1);  //Salvando status na EEPROM
                     
                     _ePFreedom.escreveDateTimeNaEEPROM(enderecoDataModificacao, dataTime);
                     
                      client.print(bufDesligado);  //Enviando Resposta para API
                    }
                    
                    client.flush();
                  
                    //Analisando a requisicao recebida para decidir se liga ou desliga a lampada
                    if (req.indexOf("on") != -1)
                    {
                      digitalWrite(2, LOW);
                    }
                    else if (req.indexOf("off") != -1)
                    {
                      digitalWrite(2, HIGH);
                    }
                    else
                    {
                      //Requisicao invalida!
                      client.stop();
                    }
//#ENDREGION Requisição HTTP /////////////////////////////
}           

void informaStatusEDataDeModificacao(){          
  Serial.print("Status Atualizado:   ");
  Serial.println(getStatusForApi(statusDevice));
  
  Serial.print("Data atualizada:   ");
  Serial.println(dataDeUltimaModificacao);
 }

void conectaWiFi() {
   //wifiManager.resetSettings();
   wifiManager.setTimeout(180);
    //Convertendo string para IPAddress
  IPAddress _ip;
    if (_ip.fromString(iPDevice)) { }
  //    
  
  IPAddress _gw = IPAddress(192, 168, 0, 1);
  IPAddress _sn = IPAddress(255, 255, 255, 0);
  
  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  
  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  Serial.println("connected...yeey :)");

  /////////////////////MQTT//////////////////////////////////
  MQTT.setServer("iot.eclipse.org", 1883);  
  MQTT.setCallback(recebePacote);

  server.begin(); //HTTP
  Serial.println("Servidor inicializado
 }

int getStatusForApi(int status) {
  if(statusDevice == 1)
    {
      return 0;
    }
    else if (statusDevice == 0)
    {
      return 1;
    }  
}


/////////////////////MQTT//////////////////////////////////

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT();   
    }
    Serial.println("Reconectando WiFi");
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
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

    Serial.print(msg);

    if (msg == "0") {
       digitalWrite(2, LOW);
    }

    if (msg == "1") {
       digitalWrite(2, HIGH);
    }
}
