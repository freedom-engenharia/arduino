extern "C"{
#include "user_interface.h"
}

os_timer_t mTimer;

bool       _timeout = false;

//Nunca execute nada na interrupcao, apenas setar flags!
void tCallback(void *tCall){
    _timeout = true;
}

void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 1000, true);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  //iniciar a interrupcao
  usrInit();

}

void loop() {
  //verificacao no loop
  if (_timeout){
      Serial.println("cuco!");
      _timeout = false;
  }
  yield(); //um putosegundo soh pra respirar

}
