//#REGION Importações /////////////////////////////
                #include <FS.h>                   //this needs to be first, or it all crashes and burns...
                #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
                //needed for library
                #include <DNSServer.h>
                #include <ESP8266WebServer.h>
                #include <ConexaoWiFiFreedom.h>          //https://github.com/tzapu/WiFiManager
                #include <WiFiServer.h>
//#ENDREGION Importações /////////////////////////////

//#REGION IP do Device /////////////////////////////
const String iPDevice = "192.168.0.196";
//#ENDREGION IP do Device ////////////////////////////

WiFiServer server(80);

void setup() {
  pinMode(2, OUTPUT); 
  digitalWrite(2, 0); 

  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print("."); 
  }

  server.begin();
  Serial.begin(115200);
  delay(10);
  Serial.println();

//#REGION Conexão automatica na rede WiFi /////////////////////////////

                  ConexaoWiFiFreedom wifiFreedom;

                  //Convertendo string para IPAddress
                    IPAddress _ip;
                      if (_ip.fromString(iPDevice)) {
                      }
                  //    
                  IPAddress _gw = IPAddress(192,168,0,1);
                  IPAddress _sn = IPAddress(255, 255, 255, 0);
                  
                  wifiFreedom.setSTAStaticIPConfig(_ip, _gw, _sn);
                
                  if (!wifiFreedom.autoConnect("FreedomAP", "senha")) {
                    Serial.println("Falha na conexão, devemos reinicializar para ver se conecta");
                    delay(3000);
                    ESP.reset();
                    delay(5000);
                  }
                  Serial.println("conectado :)");
                  Serial.println("local ip");
                  Serial.println(WiFi.localIP());
}
//#ENDREGION Conexão automatica na rede WiFi /////////////////////////////

//#REGION GetStatus na API /////////////////////////////

void loop() {
  
//#REGION Conexão automatica na rede WiFi /////////////////////////////
                  
                  //Verificando se o servidor esta pronto.
                    WiFiClient client //= server.available();
//                    if (!client) {
//                      return;
//                    }
//                  
//                    //Verificando se o servidor recebeu alguma requisicao
//                    while (!client.available()) {
//                      delay(1);
//                    }
//                  
//                    //Obtendo a requisicao vinda do browser
//                    String req = client.readStringUntil('\r');
//                    
//                    //Sugestao dada por Enrico Orlando
//                    if(req == "GET /favicon.ico HTTP/1.1"){
//                        req = client.readStringUntil('\r');
//                       
//                    }
//                    
//                    client.flush();
//                  
//                    //Iniciando o buffer que ira conter a pagina HTML que sera enviada para o browser.
//                    String bufLigado = "";                  
//                    bufLigado += "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
//                    bufLigado += iPDevice;
//                    bufLigado += " Ligado";
//
//                    String bufDesligado = "";
//                    bufDesligado += "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
//                    bufDesligado += iPDevice;
//                    bufDesligado += " Desligado";
//                    
//                    //Enviando para o browser a 'pagina' criada.
//                    if (req.indexOf("on") != -1)
//                    {
//                      client.print(bufLigado);
//                    }
//                    else if (req.indexOf("off") != -1)
//                    {
//                      client.print(bufDesligado);
//                    }
//                    
//                    client.flush();
//                  
//                    //Analisando a requisicao recebida para decidir se liga ou desliga a lampada
//                    if (req.indexOf("on") != -1)
//                    {
//                      digitalWrite(2, LOW);
//                    }
//                    else if (req.indexOf("off") != -1)
//                    {
//                      digitalWrite(2, HIGH);
//                    }
//                    else
//                    {
//                      //Requisicao invalida!
//                      //client.stop();
//                    }
//#ENDREGION Conexão automatica na rede WiFi /////////////////////////////

                    

if (!client.connect("server.io", 80)) {
  Serial.println("Connection failed");
}

client.print("GET /https://localhost:44301/api/Hardware/Device/Status/192.168.0.196 HTTP/1.1\r\n\r\n");

while (client.connected()) {
  String response = client.readStringUntil('\n');
  if (response == "\r") {
    Serial.println("<-- Headers received");
    break;
  }
}
String response = client.readStringUntil('\n');
Serial.println(response);


        }
                    

  


