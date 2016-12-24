#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    55
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

/******************* variables****************************/
CRGB leds[NUM_LEDS];

// pins
int analogSpeed = A3;
int analogBright = A0;
int buttonPin = 8;

//timing cars
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 500;    // the debounce time
long currentTime = 0;
int max_delay = 100; // max delay for the potentiometer

boolean DEBUG = false; // serial prints
int power_delay = 1000; // power on delay
int color_index_inc = 3; // hsv increment

int paletteNumber = 0; // which color palette is being used

// led objects
CRGBPalette16 currentPalette;
TBlendType currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
    delay(power_delay); // power-up safety delay

    // setup led object
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    
    // set led initial colors
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    // set pins
    pinMode(buttonPin, INPUT);
    Serial.begin(9600);
}

void loop()
{

    // read the state of the switch into a local variable
    int reading = digitalRead(buttonPin);
    // get current time when button was read
    currentTime = millis();
    // if button is pushed and time hs passed debounce period then chsnge color scheme
    if (reading == HIGH && (currentTime - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = currentTime;
        ChangePalette();
    }

    // motion speed
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    
    // show colors
    FillLEDsFromPaletteColors(startIndex);
    FastLED.show();
    
    // get speed of colors and birghtness
    int speed = analogRead(analogSpeed);
    int bright = analogRead(analogBright);

    // debugging info
    if(DEBUG) {
      Serial.print("speed: ");
      Serial.println(speed);
      Serial.print("bright: ");
      Serial.println(bright);
    }

    // set brightness
    FastLED.setBrightness(map(bright,0,1023,0,255));
    
    // set delay based on color speed
    FastLED.delay(map(speed,0,1023,0,max_delay));

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
