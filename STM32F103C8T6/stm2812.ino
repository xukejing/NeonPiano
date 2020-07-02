#include <WS2812B.h>
#include <USBComposite.h>

#define NUM_LEDS 53
#define NUM_KEYS 61
#define NOTE_S 36
#define NOTE_E 96
/*
 * Note. Library uses SPI1
 * Connect the WS2812B data input to MOSI on your board.
 *  SPI1  PA5->CLK, PA6->MISO, PA7->MOSI PA2->CSN
 * 
 */
WS2812B strip = WS2812B(NUM_LEDS);
int keys[NUM_KEYS]={0};//key down
float keysLocation[NUM_KEYS]={0.0};
int leds[NUM_LEDS]={0};//light
float ledsLocation[NUM_KEYS]={0.0};

int pedal=0;
float radioL=8.33;
float disTemp=0;
uint16_t i;
uint16_t tmpLight=0;
class myMidi : public USBMIDI {
 virtual void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) 
 {
//36~96
  if(note>=NOTE_S&&note<=NOTE_E)
  { keys[note-NOTE_S]=0;
    
   for(i=0; i<strip.numPixels(); i++)
    {
      disTemp=ledsLocation[i]-keysLocation[note-NOTE_S];
  if (disTemp<0)
  disTemp=disTemp*(-1.0);
    if(disTemp<=radioL)
    leds[i]=0;
    }

  }

 }
 virtual void handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity) 
 {
  if(note>=NOTE_S&&note<=NOTE_E)
  { keys[note-NOTE_S]=1;
   
   for(i=0; i<strip.numPixels(); i++)
    {
      disTemp=ledsLocation[i]-keysLocation[note-NOTE_S];
  if (disTemp<0)
  disTemp=disTemp*(-1.0);
    if(disTemp<=radioL)
    leds[i]=velocity;//-0A=-16
    }
  }
 }
 virtual void handleControlChange(unsigned int channel, unsigned int controller, unsigned int value)
 {
  if(controller==64)//pedal
  {
    if(value==0)//pedal lifted
    {
      pedal=0;
    }
    else
    pedal=1;
  }
 }
  
};

myMidi midi;
USBCompositeSerial CompositeSerial;

void setup() 
{
  myinit();

  strip.begin();// Sets up the SPI
   for(i=0; i<strip.numPixels(); i++) 
  {
      strip.setPixelColor(i, strip.Color(0, 0, 5));
  }
  strip.show();// Clears the strip, as by default the strip data is set to all LED's off.
 // strip.setBrightness(8);
   USBComposite.setProductId(0x0030);
    midi.registerComponent();
    CompositeSerial.registerComponent();
    USBComposite.begin();
}

void loop() 
{ 
midi.poll();
for(i=0; i<strip.numPixels(); i++)
 {
  if(pedal==1)
  {
    tmpLight=leds[i]*2;
    if (tmpLight>255)
    tmpLight=255;
    strip.setPixelColor(i, strip.Color(tmpLight/2, tmpLight/2, tmpLight));
  }
  else
  {
    tmpLight=leds[i]*3;
    if (tmpLight>255)
    tmpLight=255;
    strip.setPixelColor(i, strip.Color(tmpLight, tmpLight/2, tmpLight/2));
  }
    
 }
 delay(10);
 strip.show();
}

void myinit()
{
  keysLocation[0]=16.66;
  for(i=1; i<12 ; i++) 
  {
    if(i==5)
    keysLocation[i]=keysLocation[i-1]+23.428;
    else
    keysLocation[i]=keysLocation[i-1]+23.428/2.0;
  }
  for(i=12;i<NUM_KEYS;i++)
  {
     keysLocation[i]=keysLocation[i-12]+164;
  }
    
  ledsLocation[0]=0.0;
  for(i=1; i<strip.numPixels(); i++) 
  {
    ledsLocation[i]=ledsLocation[i-1]+16.66666;
  }
}
