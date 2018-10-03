#ifndef EEPROMFREEDOM_h
#define EEPROMFREEDOM_h

#include <Arduino.h> 

class EEPROMFREEDOM{
    public:
        void escreveStatusNaEEPROM(int endereco, int valor);
        void escreveInteiroDe2BytesNaEEPROM(int endereco, int valor);
        int verificaSeOValorEIgualNoEndereco(int endereco);
        void escreveDateTimeNaEEPROM(int endereco, String valor);
        int leStatusNaEEPROM(int endereco);
        int leInteiroDe2BytesNaEEPROM(int endereco);
        String leDateTimeNaEEPROM(int endereco);
};
#endif