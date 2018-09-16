#include <WaveHC.h>
#include <WaveUtil.h>

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))

/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}

/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}


class Audio {
  private:
    char* lastPlay;
    
  public:
    //////////////////////////////////// SETUP
    void setup() {
      // try card.init(true) if errors occur on V1.0 Wave Shield
      if (!card.init()) {
        error("Card init. failed!");
      }
      // enable optimize read - some cards may timeout
      card.partialBlockRead(true);
      
      if (!vol.init(card)) {
        error("No partition!");
      }
      if (!root.openRoot(vol)) {
        error("Couldn't open root"); return;
      }
      putstring_nl("Files found:");
      root.ls();
    }
  
    void play(char *name) {
      // see if the wave object is currently doing something
      if (wave.isplaying) {// already playing something, so stop it!
        wave.stop(); // stop it
      }

      // look in the root directory and open the file
      if (!file.open(root, name)) {
        putstring("Couldn't open file ");
        Serial.print(name);
        return;
      }
      
      // OK read the file and turn it into a wave object
      if (!wave.create(file)) {
        putstring_nl("Not a valid WAV"); return;
      }
      
      lastPlay = name;
      // ok time to play! start playback
      wave.play();
    }

    bool toggle(char *name){
      if(strcmp(lastPlay, name) == 0 && wave.isplaying){
        wave.stop();
      }else{
        play(name);
      }
    }

    void stop(){
      wave.stop();
    }

    bool isPlaying(){
      return wave.isplaying;
    }
};
