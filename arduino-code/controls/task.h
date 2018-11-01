enum TaskState {
  NOT_STARTED, RUNNING, DONE
};

class Task {
  public:
    long lastUpdate = 0l;
    TaskState state = NOT_STARTED;
    
    virtual void start(){
      lastUpdate = millis();
      state = RUNNING;
    }
    
    virtual void update(){
      long currentTime = millis();
      this->update(currentTime - lastUpdate);
      lastUpdate = currentTime;
    }
    
    virtual void update(long elapsedTime){}
    
    virtual void stop(){
      this->state = DONE;
    }
};

class LightFlasher: public Task{
  private:
    long timeBucket = 0l;
    int pin;
    boolean blink = false;
    
  public:
    /**
     * controlBlink:  when true, task will enable/disable pin periodically.
     *                When false, task will enable pin, hold, disable pin, then quit.
     **/
    LightFlasher(int pin, boolean controlBlink){
      this->pin = pin;
      this->blink = controlBlink;
    }

    void start(){
      pinMode(this->pin, OUTPUT);
      digitalWrite(this->pin, HIGH);
      
      this->timeBucket = 0l;
      Task::start();
    }
    
    void update(long elapsedTime){
      this->timeBucket += elapsedTime;
      
      if(this->blink){
        while(this->timeBucket > 1000){
          this->timeBucket -= 1000;
        }
        digitalWrite(this->pin, this->timeBucket < 500);
      }else if(this->timeBucket > 10000){
        this->stop();
      }
    }

    void stop(){
      Task::stop();
      digitalWrite(this->pin, LOW);
    }
};

class SnorkelSwiveler: public Task{
  private:
    int enablePin;
    int bubblePin;
    int stepPin;
    int dirPin;
    int limitPin;
    int direction;
    int limitStatus;
    
    int maxSteps = 150;
    int stepCount = 0;
    
    long period = 1000l;

    void step(boolean direction){
      digitalWrite(dirPin, direction);
      
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(10000); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(10000);
      this->stepCount++;
    }
    
  public:
    SnorkelSwiveler(int bubblePin, int enablePin, int stepPin, int dirPin, int limitPin){
      this->bubblePin = bubblePin;
      this->enablePin = enablePin;
      this->stepPin = stepPin;
      this->dirPin = dirPin;      
      this->limitPin = limitPin;
      
      pinMode(this->bubblePin, OUTPUT);
      pinMode(this->enablePin, OUTPUT);

      digitalWrite(this->bubblePin, LOW);
      digitalWrite(this->enablePin, HIGH);
    }

    void start(){
      pinMode(this->bubblePin, OUTPUT);
      pinMode(this->enablePin, OUTPUT);
      pinMode(this->stepPin, OUTPUT);
      pinMode(this->dirPin, OUTPUT);
      pinMode(this->limitPin, INPUT_PULLUP);

      digitalWrite(this->bubblePin, HIGH);
      digitalWrite(this->enablePin, LOW);
      
      this->direction = 0;
      this->stepCount = 0;
      this->limitStatus = HIGH;
      Task::start();
    }
    
    void update(long elapsedTime){
      bool lastLimitStatus = this->limitStatus;

      this->limitStatus = digitalRead(this->limitPin);      
      bool stateChange = ((limitStatus == LOW) && (lastLimitStatus == HIGH)) || (this->stepCount >= this->maxSteps);
      if(stateChange){
        this->direction++;
        this->stepCount = 0;
        for(int i=0; i<10; i++){
          this->step((this->direction % 2) + 1);
        }
      }

      if(this->direction == 0 || this->direction == 2){
        this->step(HIGH);
      }else if(this->direction == 1 || this->direction == 3){
        this->step(LOW);
      }else{
        /*
        for(int i=0; i<20; i++){
          this->step(HIGH);
        }
        */
        digitalWrite(this->bubblePin, LOW);
        digitalWrite(this->enablePin, HIGH);
        this->state = DONE;        
      }
    }
};
