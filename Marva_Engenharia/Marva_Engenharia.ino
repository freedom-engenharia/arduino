// Libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi network
const char* subCorrenteid = "Samuel-Home";
const char* pasubCorrenteword = "20182018";

//Bomba 01
String pB1 = "30"; // % da bomba
String sB1 = "Ligada"; // Ligada , desligada, manutenção ou defeito
float cB1 = 99.9;//COrrente da bomba 1
String tMB1 = "";// Trifasico ou Monofasico Bomba 1
float soCB1 = 0.0; // Sobre corrente
float suCB1 = 0.0; // sub corrente

//Bomba 02
String pB2 = "70"; // % da bomba
String sB2 = "Ligada"; // Ligada , desligada, manutenção ou defeito
float cB2 = 88.88;//corrente da bomba 2
String tMB2 = ""; // Trifasico ou Monofasico Bomba 2
float soCB2 = 0.0; // Sobre corrente
float suCB2 = 0.0; // sub corrente

int ledPin = 2; // GPIO2
int ledPin2 = D3; // GPIO2

ESP8266WebServer server ( 80 );



char htmlResponse[10000];

void handleRoot() {
snprintf ( htmlResponse, 10000,
"<!DOCTYPE html>\
<html>\
    <head>\
        <meta charset=\"utf-8\">\
    </head>\
    <body>\
        <link rel='stylesheet' href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.3/css/bootstrap.min.css\" integrity='sha384-Zug+QiDoJOrZ5t4lssLdxGhVrurbmBWopoEl+M6BdEfwnCJZtKxi1KgxUyJq13dy' crossorigin='anonymous'>\
        <link rel='stylesheet' href=\"https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css\">\
        <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>\
        <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js\" integrity'sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q' crossorigin='anonymous'></script>\
        <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.3/js/bootstrap.min.js\" integrity='sha384-a5N7Y/aK3qNeh15eJKGWxsqtnX/wWdSZSKp+81YjTmS15nvnvxKHuzaWwXHDli+4' crossorigin='anonymous'></script>\
        <script>\
            $(document).ready(function() {\
                $('#atualizarStatus').click(function(){\
                    $.get('/pB1', function(data){\
                        console.log(data);\
                        $('#porBomba01').html(data);\
                    }).fail(function(){\
                        console.log('deu erro');\
                    });\
                });\
                $('#atualizarStatus').click(function(){\
                    $.get('/pB2', function(data){\
                        console.log(data);\
                        $('#porBomba02').html(data);\
                    }).fail(function(){\
                        console.log('deu erro');\
                    });\
                });\
                $('#atualizarStatus').click(function(){\
                    $.get('/sB1', function(data){\
                        console.log(data);\
                        $('#statusBomba01').html(data);\
                    }).fail(function(){\
                        console.log('deu erro');\
                    });\
                });\
                $('#atualizarStatus').click(function(){\
                    $.get('/sB2', function(data){\
                        console.log(data);\
                        $('#statusBomba02').html(data);\
                    }).fail(function(){\
                        console.log('deu erro');\
                    });\
                });\
                $('#atualizarStatus').click(function(){\
                    $.get('/cB1', function(data){\
                        console.log(data);\
                        $('#correnteBomba01').html(data);\
                    }).fail(function(){\
                        console.log('deu erro');\
                    });\
                });\
                $('#atualizarStatus').click(function(){\
                    $.get('/cB2', function(data){\
                        console.log(data);\
                        $('#correnteBomba02').html(data);\
                    }).fail(function(){\
                        console.log('deu erro');\
                    });\
                });\
                $('#saveB01').click(function(e){\
                    e.preventDefault();\
                    var tMB1 = $('#date_tMB1').val();\
                    var soCB1 = $('#date_soCB1').val();\
                    var suCB1 = $('#date_suCB1').val();\
                    $.get('/saveB01?tMB1=' + tMB1 + '&soCB1=' + soCB1 + '&suCB1=' + suCB1, function(data){\
                        console.log(data);\
                    });\
                });\
                $('#saveB02').click(function(e){\
                    e.preventDefault();\
                    var tMB2 = $('#date_tMB2').val();\
                    var soCB2 = $('#date_soCB2').val();\
                    var suCB2 = $('#date_suCB2').val();\
                    $.get('/saveB02?tMB2=' + tMB2 + '&soCB2=' + soCB2 + '&suCB2=' + suCB2, function(data){\
                        console.log(data);\
                    });\
                });\
                $('#onB1').click(function(e){\
                    e.preventDefault();\
                    var onB1 = true;\
                    $.get('/onB01?onB1=' + onB1, function(data){\
                        console.log(data);\
                    });\
                });\
                $('#offB1').click(function(e){\
                    e.preventDefault();\
                    var offB1 = false;\
                    $.get('/offB01?offB1=' + offB1, function(data){\
                        console.log(data);\
                    });\
                });\
                $('#onB2').click(function(e){\
                    e.preventDefault();\
                    var onB2 = true;\
                    $.get('/onB02?onB2=' + onB2, function(data){\
                        console.log(data);\
                    });\
                });\
                $('#offB2').click(function(e){\
                    e.preventDefault();\
                    var offB2 = false;\
                    $.get('/offB02?offB2=' + offB2, function(data){\
                        console.log(data);\
                    });\
                });\
                $('#saveB01, #saveB02, #onB1, #onB2, #offB1, #offB2, #atualizarStatus').click(function(){\
                  $('.modal').modal('show');\
                    setTimeout(function () {\
                   console.log('hejsan');\
                 $('.modal').modal('hide');\
                  }, 800);\
                });\
            });\
        </script>\
        \
        <div class='modal fade bd-example-modal-lg' data-backdrop='static' data-keyboard='false' tabindex='-1'>\
            <div class='modal-dialog modal-sm'>\
                  </br></br></br></br></br></br></br></br></br></br></br></br>\
                  <div class='modal-content' style='width:48px; height:50px; margin-left:130px;'>\
                    <span class='fa fa-spinner fa-spin fa-3x'></span>\
                  </div>\
            </div>\
        </div>\
        \
        <div class='panel'>\
            <h1 class='text-center'>Marva Engenharia</h1>\
            </br>\
            <h2 class='text-center'>Residencial Forte Iracema</h2>\
            <h3 class='text-center'>S.A.S.P. - Sistema de Automação, Sinalização e Proteção de Bombas</h3>\
        </div>\
        \
        <div class='container'>\
            <div class='row'>\
                <div class='col-md-2'>\
                    <div><button class='btn btn-primary' id=\"atualizarStatus\">Atualizar tela</button>\
                    </div>\
                </div>\
                \   
                <div class='col-md-4' style='border-style: groove; padding: 20px'>\
                    <div class='alert alert-info text-center' role='alert' style='font-size:20'><b>Bomba 01</b>\
                    </div>\
                    <div>\
                        <a class='btn btn-danger' aria-haspopup='true' href='#' id='offB1'> Desligar </a>\
                        <a class='btn btn-success' style='margin-left:10px' \aria-haspopup='true' href='#' id='onB1'> Ligar </a>\
                    </div>\
                    <div>\
                        </br>\
                        <p>Status : <spam id='statusBomba01'></spam></p> \
                        <p>Corrente de Bomba : <spam id='correnteBomba01'></spam></p> \
                            Trifásica/Monofásica\
                        <input type='text' name='date_tMB1' id='date_tMB1' size=3 autofocus>\
                        </br>\
                            Corrente Máxima\   
                        <input type='text' name='date_soCB1' id='date_soCB1' size=3 autofocus>\
                        </br>\
                            Corrente Mínima\
                        <input type='text' name='date_suCB1' id='date_suCB1' size=3 autofocus>\
                        </br>\
                        <div><button class='btn btn-primary' id=\"saveB01\">Enviar</button>\
                        </div>\
                        </br>\
                        </br>\
                        <div class='progress' id='porBomba01'>\
                        </div>\
                    </div>\
                </div>\
                \
                <div class='col-md-4' style='border-style: groove; padding: 20px'>\
                    <div class='alert alert-info text-center' role='alert' style='font-size:20'><b>Bomba 02</b>\
                    </div>\
                        <div>\
                            <a class='btn btn-danger' aria-haspopup='true' href='#' id='offB2'> Desligar </a>\
                            <a class='btn btn-success' style='margin-left:10px' aria-haspopup='true' href='#' id='onB2'> Ligar </a>\
                        </div>\
                    <div>\
                        </br>\
                        <p>Status : <spam id='statusBomba02'></spam></p>\
                        <p>Corrente de Bomba : <spam id='correnteBomba02'></spam></p> \
                            Trifásica/Monofásica\
                        <input type='text' name='date_tMB2' id='date_tMB2' size=3 autofocus>\
                        </br>\
                            Corrente Máxima\
                        <input type='text' name='date_soCB2' id='date_soCB2' size=3 autofocus>\
                        </br>\
                            Corrente Mínima\
                        <input type='text' name='date_suCB2' id='date_suCB2' size=3 autofocus>\
                        </br>\
                        <div><button class='btn btn-primary' id=\"saveB02\">Enviar</button>\
                        </div>\
                        </br>\
                        </br>\
                        <div class='progress' id='porBomba02'>\
                        </div>\
                    </div>\
                </div>\
            </div>\
        </div>\
    </body>\
</html>");

  server.send ( 200, "text/html", htmlResponse );

}


void handleSave() {
  if (server.arg("tMB1") != "") {
    Serial.println("Trifásica/Monofásica Bomba 01: " + server.arg("tMB1"));
    tMB1 = server.arg("tMB1");
    Serial.println(tMB1);
  }

  if (server.arg("soCB1") != "") {
    Serial.println("Sobre Corrente Bomba 01: " + server.arg("soCB1"));
    soCB1 = server.arg("soCB1").toFloat();
    Serial.println(soCB1);
  }

  if (server.arg("suCB1") != "") {
    Serial.println("Sub Corrente Bomba 01: " + server.arg("suCB1"));
    suCB1 = server.arg("suCB1").toFloat();
    Serial.println(suCB1);
  }

}

void handleSave2() {
  if (server.arg("tMB2") != "") {
    Serial.println("Trifásica/Monofásica Bomba 02: " + server.arg("tMB2"));
    tMB2 = server.arg("tMB2");
    Serial.println(tMB2);
  }

  if (server.arg("soCB2") != "") {
    Serial.println("Sobre Corrente Bomba 02: " + server.arg("soCB2"));
    soCB2 = server.arg("soCB2").toFloat();
    Serial.println(soCB2);
  }
  if (server.arg("suCB2") != "") {
    Serial.println("Sub Corrente Bomba 02: " + server.arg("suCB2"));
    suCB2 = server.arg("suCB2").toFloat();
    Serial.println(suCB2);
  }

}

void handleOnB1(){
  digitalWrite(ledPin, LOW);
  Serial.println("Bomba 01 ligada " + server.arg("onB1"));
}

void handleOffB1(){
  digitalWrite(ledPin, HIGH);
  Serial.println("Bomba 01 desligada " + server.arg("offB1"));
}

void handleOnB2(){
  Serial.println("Bomba 02 ligada " + server.arg("onB2"));
}

void handleOffB2(){
  Serial.println("Bomba 02 Desligada " + server.arg("offB2"));
}

void sendSB1() {
  String StatusBomba01 = sB1; 
  //ex json
  // String json = "{\"name\":\"Bomba 01" "\",";
  // json += "\"statusB1\":\"" + String(StatusB1) + "\"}";
  //server.send(200, "application/json", json);
  server.send(200, "application/String", StatusBomba01);
  Serial.println("Status da Bomba 01 é: " + StatusBomba01);
}

void sendSB2() {
  String StatusBomba02 = sB2; 
  server.send(200, "application/String", StatusBomba02);
  Serial.println("Status da Bomba 02 é: " + StatusBomba02);
}

void sendcB1() {
   String CorrenteBomba01 = "";
   char mess[100];
   CorrenteBomba01 = dtostrf(cB1, 2, 2, mess );
   server.send(200, "application/float", CorrenteBomba01);
   Serial.println("Corrente da Bomba 01 é: " + CorrenteBomba01);
}

void sendcB2() {
   String CorrenteBomba02 = "";
   char mess[100];
   CorrenteBomba02 = dtostrf(cB2, 2, 2, mess );
   server.send(200, "application/float", CorrenteBomba02);
   Serial.println("Corrente da Bomba 02 é: " + CorrenteBomba02);
}

void sendpB1() {
   String porcentagemBomba01 = pB1;
   String valor = "<div class='progress-bar' role='progress-bar' aria-valuenow='80' aria-valuemin='0' aria-valuemax='100' style='width: " + porcentagemBomba01 + "%'> "+ porcentagemBomba01 +" % Cheia</div>";
   server.send(200, "application/String", valor);
   //server.send(200, "application/String", porcentagemBomba01);
   Serial.println("% da Bomba 01 é: " + porcentagemBomba01);
}

void sendpB2() {
   String porcentagemBomba02 = pB2;
   String valor = "<div class='progress-bar' role='progress-bar' aria-valuenow='80' aria-valuemin='0' aria-valuemax='100' style='width: " + porcentagemBomba02 + "%'> "+ porcentagemBomba02 +" % Cheia</div>";
   server.send(200, "application/String", valor);
   //server.send(200, "application/String", porcentagemBomba01);
   Serial.println("% da Bomba 02 é: " + porcentagemBomba02);
}
void sendAllStatus() {
  //String StatusB1 = sB1;
 // String StatusB2 = sB1;
 // String json = "{\"name1\":\"Bomba01" "\",";
 // json += "\"Status\":\"" + String(StatusB1) + "\"}";
//  "{\"name2\":\"Bomba02" "\",";
 // json += "\"statusB2\":\"" + String(StatusB2) + "\"}";
  //server.send(200, "application/String", json);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, LOW);
  // Start serial
  Serial.begin(115200);
  delay(10);

  // Connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(subCorrenteid);

  WiFi.begin(subCorrenteid, pasubCorrenteword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP addresubCorrente: ");
  Serial.println(WiFi.localIP());

  server.on ( "/", handleRoot );
  server.on ("/saveB01", handleSave);
  server.on ("/saveB02", handleSave2);
  server.on ("/onB01", handleOnB1);
  server.on ("/offB01", handleOffB1);
  server.on ("/onB02", handleOnB2);
  server.on ("/offB02", handleOffB2);

  server.on("/pB1", sendpB1);
  server.on("/pB2", sendpB2);
  server.on("/sB1", sendSB1);
  server.on("/sB2", sendSB2);
  server.on("/cB1", sendcB1);
  server.on("/cB2", sendcB2);
  
  
  server.on("/sendAllS.Json", sendAllStatus);

  server.begin();
  Serial.println ( "HTTP server started" );


}

void loop() {
  server.handleClient();
}


