#include <MIDI.h>
#include <Adafruit_NeoPixel.h>

#if defined(USBCON)
#include <midi_UsbTransport.h>

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;

UsbTransport sUsbTransport;

MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);

#else // No USB available, fallback to Serial
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

#define NUM_LEDS 53
#define NUM_KEYS 61
#define NOTE_S 36
#define NOTE_E 96
#define PIN        2
Adafruit_NeoPixel  strip(NUM_LEDS, PIN, NEO_GRB);
const int ledPin = LED_BUILTIN;
int keys[NUM_KEYS] = { 0 };//key down
float keysLocation[NUM_KEYS] = { 0.0 };
int leds[NUM_LEDS] = { 0 };//light
float ledsLocation[NUM_KEYS] = { 0.0 };

float radius = 8.33;
float disTemp = 0;
uint16_t i;
uint16_t tmpLight = 0;
uint32_t pixelHue = 0;
void handleNoteOn(byte inChannel, byte note, byte velocity)
{
	// Serial.print("NoteOn  ");
	// Serial.print(inNumber);
	//  Serial.print("\tvelocity: ");
	// Serial.println(inVelocity);
	digitalWrite(ledPin, HIGH);
	if (note >= NOTE_S&&note <= NOTE_E)
	{
		keys[note - NOTE_S] = 1;

		for (i = 0; i<strip.numPixels(); i++)
		{
			disTemp = ledsLocation[i] - keysLocation[note - NOTE_S];
			if (disTemp<0)
				disTemp = disTemp*(-1.0);
			if (disTemp <= radius)
			{	//leds[i] = velocity - 16;//-0A=-16
				leds[i] = 1;
				pixelHue += 256;
				if (pixelHue >= 65536)
					pixelHue = 0;
			}
		}
	}
}
void handleNoteOff(byte inChannel, byte note, byte velocity)
{
	// Serial.print("NoteOff ");
	// Serial.print(inNumber);
	//  Serial.print("\tvelocity: ");
	//  Serial.println(inVelocity);
	digitalWrite(ledPin, LOW);
	if (note >= NOTE_S&&note <= NOTE_E)
	{
		keys[note - NOTE_S] = 0;

		for (i = 0; i<strip.numPixels(); i++)
		{
			disTemp = ledsLocation[i] - keysLocation[note - NOTE_S];
			if (disTemp<0)
				disTemp = disTemp*(-1.0);
			if (disTemp <= radius)
				leds[i] = 0;
		}

	}
}

void setup() {
	myinit();
	pinMode(ledPin, OUTPUT);
	//  Serial.begin(115200);
	//  while (!Serial);
	MIDI.begin();
	MIDI.setHandleNoteOn(handleNoteOn);
	MIDI.setHandleNoteOff(handleNoteOff);
	strip.begin();
	for (i = 0; i<strip.numPixels(); i++)
		strip.setPixelColor(i, 0x0A0A0A);
	strip.show();
	//   Serial.println("Arduino ready.");
}

void loop() {
	MIDI.read();
	for (i = 0; i<strip.numPixels(); i++)
	{

		if (leds[i] == 1)
			strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
		else
			strip.setPixelColor(i, 0x0A0A0A);
	}
	strip.show();
}
void myinit()
{
	keysLocation[0] = 16.66;
	for (i = 1; i<12; i++)
	{
		if (i == 5)
			keysLocation[i] = keysLocation[i - 1] + 23.428;
		else
			keysLocation[i] = keysLocation[i - 1] + 23.428 / 2.0;
	}
	for (i = 12; i<NUM_KEYS; i++)
	{
		keysLocation[i] = keysLocation[i - 12] + 164;
	}

	ledsLocation[0] = 0.0;
	for (i = 1; i<strip.numPixels(); i++)
	{
		ledsLocation[i] = ledsLocation[i - 1] + 16.66666;
	}
}
