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
//#ENDREGION GetStatus para API ///////////////////////////////
      
WiFiServer server(80);

void setup() {
  
  
  EEPROM.begin(MEM_ALOC_SIZE); 
    
  pinMode(2, OUTPUT); 
  
  statusDevice = _ePFreedom.leStatusNaEEPROM(enderecoStatusEEPROM); //Verifica o status(on/off) na EEPROM
  
  dataDeUltimaModificacao = _ePFreedom.leDateTimeNaEEPROM(enderecoDataModificacao);
  
  digitalWrite(2, statusDevice);   
    
  Serial.begin(115200);
  Serial.println();

 
  WiFiManager wifiManager;

  //Convertendo string para IPAddress
  IPAddress _ip;
    if (_ip.fromString(iPDevice)) {
    }
  //    
  
  IPAddress _gw = IPAddress(192, 168, 0, 1);
  IPAddress _sn = IPAddress(255, 255, 255, 0);
    
  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);

  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  Serial.println("connected...yeey :)");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  server.begin(); //HTTP
  Serial.println("Servidor inicializado");
  
  Serial.print("Status Atualizado:   ");
  Serial.println(getStatusForApi(statusDevice));
  
  Serial.print("Data atualizada:   ");
  Serial.println(dataDeUltimaModificacao);
}



void loop() {
  
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

           } //Fim do Loop
