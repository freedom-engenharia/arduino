#include "ConverteStringFreedom.h"
#include "Arduino.h"


String ConverteStringFreedom::pegaDataNaRequisicaoRecebda(String data){ 
                      data.remove(0,5);
                      data.remove(21);

                      char recebe[22];
                      
                      for(int i=0; i<22; i++){
                      recebe[i] = data[i];
                      }
                      recebe[2] = '-'; 
                      recebe[5] = '-';
	return recebe;
}
			