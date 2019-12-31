#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#define MEM_ALOC_SIZE 512

#include <FS.h>

/* Set these to your desired credentials. */
const char *ssid = "Estudo-Carlos";
const char *password = "20192019";

int NIVEL = 321;
int TEMPERATURA = 33;
String VALOR_RECEBIDO_TELA = "";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
String html ="<!DOCTYPE html> <html> <head> <title>Tela de Estudo</title> <meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, minimumscale=1.0, maximum-scale=1.0, initial-scale=1\" /> </head> <body> <h1>Recebe Valor do Arduino:</h1> <h3>Nível: <span id=\"nivel\"></span></h3> <h3>Temperatura: <span id=\"temp\"></span></h3> <h1>Enviar valor para Arduino</h1> <input placeholder=\"Digite um Texto\" id=\"input1\"> <button onClick=\"setValor(1)\">Enviar</button> <p>Varlos Enviado: <span id=\"valorEnviado\"></span></p> <script> function getAllStatus() { console.log(\"Entrou no getAll\"); var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { var obj = JSON.parse(this.responseText); document.getElementById(\"nivel\").innerHTML = obj.data[0].nivel; document.getElementById(\"temp\").innerHTML = obj.data[1].temperatura; console.log(obj) }; }; xhttp.open(\"GET\", \"data\", true); xhttp.send(); }; function setValor(valor) { console.log(\"Set Valor\"); var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById(\"valorEnviado\").innerHTML = this.responseText; } }; var valorInput = document.getElementById(\"input1\").value; console.log(valorInput); xhttp.open(\"GET\", \"setValor?valorInput=\"+valorInput, true); xhttp.send(); }; getAllStatus(); </script> </body> </html>";

  server.send(200, "text/html", html);
}

void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/html", message);
  Serial.println(message);
}

void getData() {   
   //This is a JSON formatted string that will be served. You can change the values to whatever like.
   // {"data":[{"dataValue":"1024"},{"dataValue":"23"}]} This is essentially what is will output you can add more if you like
  String text2 = "{\"data\":[";
  text2 += "{\"nivel\":\"";
  text2 += NIVEL;
  text2 += "\"},";
  text2 += "{\"temperatura\":\"";
  text2 += TEMPERATURA;
  text2 += "\"}";
  text2 += "]}";  
  Serial.println("Realizado GETALL: " + text2);
  server.send(200, "text/html", text2);
 
}


void setValor(){
  String valor = server.arg("valorInput");
  VALOR_RECEBIDO_TELA = valor;
   Serial.print("Valor Recebido da tela: ");
   Serial.print(VALOR_RECEBIDO_TELA);
    server.send(200, "text/plane", "Resposta do Arduino: " + valor); //Send web page    
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  
  SPIFFS.begin();
  
  /* You can remove the password parameter if you want the AP to be open. */
  
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", handleRoot);
  server.onNotFound(handleWebRequests);
  server.on("/data", getData);
  server.on("/setValor", setValor);

  server.begin();
  Serial.println("HTTP server started");
    
}

 
void loop() {
  server.handleClient();
}

bool loadFromSpiffs(String path){
  String dataType = "text/html";
  if(path.endsWith("/")) path += "index.htm";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}
