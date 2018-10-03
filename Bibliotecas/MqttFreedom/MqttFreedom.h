#ifndef MqttFreedom_h
#define MqttFreedom_h

#include <Arduino.h> 

class MqttFreedom{
    public:
        void conectaMQTT();
        void recebePacote(char* topic, byte* payload, unsigned int length);
};
#endif