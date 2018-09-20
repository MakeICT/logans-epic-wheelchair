#include "audio.h"
#include "task.h"

#define HEADLIGHT_INPUT A0
#define TRICK_OR_TREAT_INPUT A1
#define SOUND_BYTE_INPUT A2
#define SNORKEL_INPUT A3

#define HEADLIGHTS_OUTPUT 6
#define FLASHERS_OUTPUT 6

#define SNORKEL_STEPPER_ENABLE 7
#define SNORKEL_STEPPER_STEP A4
#define SNORKEL_STEPPER_DIR A5

#define BUTTON_COUNT sizeof(buttons) / sizeof(Button)
#define SOUND_COUNT sizeof(sounds) / sizeof(char*)
#define TASK_COUNT sizeof(tasks) / sizeof(Task*)

int headlightBrightness = 128;

int currentSoundIndex = -1;
char* sounds[] = {"FIRE-01.WAV", "BARK-01.WAV", "TIRE-01.WAV"};

Audio audio;
LightFlasher flasher(FLASHERS_OUTPUT);
SnorkelSwiveler snorkel(SNORKEL_STEPPER_ENABLE, SNORKEL_STEPPER_STEP, SNORKEL_STEPPER_DIR);

Task* tasks[] = { &flasher, &snorkel };

typedef struct {
  int pin;
  void (*onPressed)();
  void (*onReleased)();
  bool released;
} Button;


void enableLights(){
  analogWrite(HEADLIGHTS_OUTPUT, headlightBrightness);
}

void disableLights(){
  digitalWrite(HEADLIGHTS_OUTPUT, 0);
}


void treakOrTreat(){
  audio.play("T-OR-T.WAV");
}

void nextSound(){
  if(currentSoundIndex == 0){
    flasher.stop();
  }
  
  currentSoundIndex++;
  if(currentSoundIndex >= SOUND_COUNT){
    currentSoundIndex = 0;
  }
  
  audio.play(sounds[currentSoundIndex]);
  if(currentSoundIndex == 0){
    flasher.start();
  }
}

void activateSnorkel(){
  if(snorkel.state != RUNNING){
    snorkel.start();
  }
}

Button buttons[] = {
  {HEADLIGHT_INPUT, enableLights, disableLights},
  {TRICK_OR_TREAT_INPUT, treakOrTreat},
  {SOUND_BYTE_INPUT, nextSound},
  {SNORKEL_INPUT, activateSnorkel},
};

void setup() {
  Serial.begin(9600);
  
  for(int i=0; i<BUTTON_COUNT; i++){
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  
  pinMode(HEADLIGHTS_OUTPUT, OUTPUT);
  pinMode(FLASHERS_OUTPUT, OUTPUT);
  
  audio.setup();
}

void loop() { 
  for(int i=0; i<BUTTON_COUNT; i++){
    int state = digitalRead(buttons[i].pin);
    
    if(state == LOW){
      if(buttons[i].released){
        if(buttons[i].onPressed){
          buttons[i].onPressed();
        }
        
        buttons[i].released = false;
        delay(250); // debounce
      }
    }else{
      if(buttons[i].released == false){
        if(buttons[i].onReleased){
          buttons[i].onReleased();
        }
        buttons[i].released = true;
        delay(250);
      }
    }
  }

  for(int i=0; i<TASK_COUNT; i++){
    if(tasks[i]->state == RUNNING){
      tasks[i]->update();
    }
  }
}
