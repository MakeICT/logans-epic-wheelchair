#include "audio.h"

#define BUTTON_COUNT sizeof(buttons) / sizeof(Button)

Audio audio;

typedef struct {
  int pin;
  void (*callback)();
  bool released;
} Button;


void toggleSiren(){
  audio.toggle("FIRE-01.WAV");
}

void bark(){
  audio.play("BARK-01.WAV");
}

void tires(){
  audio.play("TIRE-01.WAV");
}

Button buttons[] = {
  {6, toggleSiren},
  {7, bark},
  {8, tires},
};

void setup() {
  Serial.begin(9600);
  
  for(int i=0; i<BUTTON_COUNT; i++){
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  
  audio.setup();
}

void loop() { 
  for(int i=0; i<BUTTON_COUNT; i++){
    int state = digitalRead(buttons[i].pin);
    
    if(state == LOW){
      if(buttons[i].released){
        buttons[i].callback();
        
        buttons[i].released = false;
        delay(250); // debounce
      }
    }else{
      if(buttons[i].released == false){
        buttons[i].released = true;
        delay(250);
      }
    }
  }
}
