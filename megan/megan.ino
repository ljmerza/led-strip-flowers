#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    55
#define BRIGHTNESS  255s
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

int analogInPin = A3;
int buttonPin = 8;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 500;    // the debounce time
long currentTime = 0;
int max_delay = 100; // max delay for the potentiometer

boolean DEBUG = false;
int power_delay = 1000; // power on delay
int color_index_inc = 3; // hsv increment

int paletteNumber = 0;

CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
    delay( power_delay ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    pinMode(buttonPin, INPUT);

    Serial.begin(9600);
}

void loop()
{

    // read the state of the switch into a local variable:
    int reading = digitalRead(buttonPin);

    currentTime = millis();

    if (reading == HIGH && (currentTime - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = currentTime;
        ChangePalette();
    }

    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors(startIndex);
    
    FastLED.show();
    
    int pot = analogRead(analogInPin);

    if(DEBUG) {
      Serial.print("pot: ");
      Serial.println(pot);
    }
    
    FastLED.delay(map(pot,0,1023,0,max_delay));

}

void FillLEDsFromPaletteColors( uint8_t colorIndex){
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += color_index_inc;
    }
}

void ChangePalette(){  
    paletteNumber++;

    switch (paletteNumber) {
        case 0:
            currentPalette = RainbowColors_p;
            break;
        case 1:
            currentPalette = CloudColors_p;
            break;
        case 2:
            currentPalette = OceanColors_p;
            break;
        case 3:
            currentPalette = LavaColors_p;
            break;
        case 4:
            currentPalette = ForestColors_p;
            break;
        default:
            paletteNumber = 0;
            currentPalette = RainbowColors_p;
            break;
    }
}
