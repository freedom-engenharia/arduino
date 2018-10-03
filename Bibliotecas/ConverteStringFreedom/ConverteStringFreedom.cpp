#include "ConverteStringFreedom.h"
#include "Arduino.h"


String ConverteStringFreedom::pegaDataNaRequisicaoRecebda(String data){ 
                      data.remove(0,5); // a partir da posição 0, remove 5 caracteres
                      data.remove(21);  // remore todos caracteresa partir da posição 25

                      char recebe[22];  //  array char de tamanho 22
                      
                      for(int i=0; i<22; i++){  // para cara espaço do array, adicionar um caracter da string tratada anteriormente
                      recebe[i] = data[i];
                      }
                      recebe[2] = '-';   // no espaço 2 do array é alterado para "-"
                      recebe[5] = '-';   // no espaço 25do array é alterado para "-"
	return recebe;
}
			