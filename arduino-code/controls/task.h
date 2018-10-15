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
    int stepPin;
    int dirPin;
    int limitPin;
    int direction;
    int limitStatus;
    
    long period = 1000l;

    void step(boolean direction){
      digitalWrite(dirPin, direction);
      
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(500); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(500);
    }
    
  public:
    SnorkelSwiveler(int enablePin, int stepPin, int dirPin, int limitPin){
      this->enablePin = enablePin;
      this->stepPin = stepPin;
      this->dirPin = dirPin;      
      this->limitPin = limitPin;
      
      pinMode(this->enablePin, OUTPUT);
      digitalWrite(this->enablePin, HIGH);
    }

    void start(){
      pinMode(this->enablePin, OUTPUT);
      pinMode(this->stepPin, OUTPUT);
      pinMode(this->dirPin, OUTPUT);
      pinMode(this->limitPin, INPUT_PULLUP);

      digitalWrite(this->enablePin, LOW);
      direction = 0;
      Task::start();
    }
    
    void update(long elapsedTime){
      bool lastLimitStatus = this->limitStatus;

      this->limitStatus = digitalRead(this->limitPin);      
      if(limitStatus != lastLimitStatus){
        if(limitStatus){
          direction++;
        }
      }

      if(direction == 0){
        this->step(HIGH);
      }else if(direction == 1){
        this->step(LOW);
      }else{
        if(limitStatus){
          this->step(HIGH);
        }else{
          digitalWrite(this->enablePin, HIGH);
          this->state = DONE;        
        }
      }
    }
};
