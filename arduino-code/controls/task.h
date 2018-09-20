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
    LightFlasher(int pin){
      this->pin = pin;
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
    long timeBucket = 0l;
    int enablePin;
    int stepPin;
    int dirPin;
    
    long period = 1000l;

    void step(boolean direction){
      digitalWrite(dirPin, direction);
      
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(500); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(500);
    }
    
  public:
    SnorkelSwiveler(int enablePin, int stepPin, int dirPin){
      this->enablePin = enablePin;
      this->stepPin = stepPin;
      this->dirPin = dirPin;
      
      pinMode(this->enablePin, OUTPUT);
      digitalWrite(this->enablePin, HIGH);
    }

    void start(){
      pinMode(this->enablePin, OUTPUT);
      pinMode(this->stepPin, OUTPUT);
      pinMode(this->dirPin, OUTPUT);

      digitalWrite(this->enablePin, LOW);
      timeBucket = 0l;      
      Task::start();
    }
    
    void update(long elapsedTime){
      timeBucket += elapsedTime;

      if(timeBucket < period || (timeBucket > period*3 && timeBucket < period*4)){
        this->step(HIGH);
      }else if(timeBucket < period*3){
        this->step(LOW);
      }else{
        digitalWrite(this->enablePin, HIGH);
        this->state = DONE;
      }
    }
};
