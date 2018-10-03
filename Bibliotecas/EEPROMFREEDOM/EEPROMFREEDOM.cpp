#include "EEPROMFREEDOM.h"
#include "Arduino.h"
#include "EEPROM.h"


void EEPROMFREEDOM::escreveStatusNaEEPROM(int endereco, int valor){
		 Serial.println("Solicitado mudança de status...");
	 int valorAtual = verificaSeOValorEIgualNoEndereco(endereco); // Lemos o valor inteiro da memória
	 if (valorAtual == valor){ // Se o valor lido for igual ao que queremos escrever não é necessário escrever novamente
	 	Serial.println("Nada foi modificado, o valor atual é igual ao enviado!");
	 	Serial.println("\n");
	 	return;
	 }
	 else{ // Caso contrário "quebramos nosso inteiro em 2 bytes e escrevemos cada byte em uma posição da memória

	 	String myString = "";
	 	myString = String(valor);

		char dado[2];
		strcpy(dado, myString.c_str());
    	EEPROM.put(endereco, dado);			
		EEPROM.commit();
		Serial.println("Status atualizado");
	 	Serial.println("\n");
	 }
}

int EEPROMFREEDOM::verificaSeOValorEIgualNoEndereco(int endereco){  //Le o int armazenado em dois endereços de memória
     byte valorVerificado = 0; // Inicializamos nosso retorno
     valorVerificado = EEPROM.read(endereco); // Leitura do primeiro byte armazenado no endereço 1
     Serial.println("Verificando o valor...");
     return valorVerificado; // Retornamos o valor da leitura
}

void EEPROMFREEDOM::escreveDateTimeNaEEPROM(int endereco, String valor){
		char dado[25];
		strcpy(dado, valor.c_str());
    	EEPROM.put(endereco, dado);			
		EEPROM.commit();
	Serial.println("Salvou nova data de modificação");
	Serial.println();	
}

int EEPROMFREEDOM::leStatusNaEEPROM(int endereco){
		char varloNaEEPROM[2];
		String valorObtido = EEPROM.get(endereco, varloNaEEPROM);
        int resultado = atoi( valorObtido.c_str() );
        return resultado;
        Serial.println("Leu Statua na EEPROM");
}

String EEPROMFREEDOM::leDateTimeNaEEPROM(int endereco){
		char varloNaEEPROM[25];
        return EEPROM.get(endereco, varloNaEEPROM);
        Serial.println("Leu Data de modificação de status na EEPROM");
}

				