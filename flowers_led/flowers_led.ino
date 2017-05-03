#include <FastLED.h>
#include <IRremote.h>


int RECV_PIN = 11; // pin for IR
IRrecv irReceiver(RECV_PIN); // create IR object
decode_results results; // the results of the IR signal

#define LED_PIN  2
#define NUM_LEDS 25
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
//#define FASTLED_ALLOW_INTERRUPTS 1

#define POWER 0x10EFD827 
#define A 0x10EFF807 
#define B 0x10EF7887
#define C 0x10EF58A7
#define UP 0x10EFA05F
#define DOWN 0x10EF00FF
#define LEFT 0x10EF10EF
#define RIGHT 0x10EF807F
#define SELECT 0x10EF20DF
/*****************pin variables******************************/
int buttonPin = 8; // button for changing colors
/***************** LED variables******************************/
CRGBPalette16 current_palette; // color palette object
TBlendType current_blending; // color blending object
CRGB leds[NUM_LEDS]; // led strip object for fastLED
int last_time = 0; // keep track of last time we updated LED colors
uint8_t actual_brightness; // the brightness that gets applied to the LED strip
uint8_t palette_number = 1; // which color palette is being used
/***************** IR controlled variables******************************/
uint8_t brightness = 128; // LED brightness -> need two to save last brightness before turning off/on
uint8_t delay_time = 50; // hoew long to delay before updating LED colors
int current_time = 0;
boolean turn_off = false; // do we want to 'turn off' LEDs? (brightness = 0)


/********************************************************
*   void setup
*
*
*
*********************************************************/
void setup()
{
    delay(3000); // power-up safety delay
    pinMode(buttonPin, INPUT); // reset button setup
    Serial.begin(9600); // begin serial output for debugging
    irReceiver.enableIRIn(); // Start the receiver

    // setup LED object
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(brightness);
    
    // set LED initial colors
    current_palette = RainbowColors_p;
    current_blending = LINEARBLEND;
}


/*********************************************************
*   void loop
*
*
*
*********************************************************/
void loop() { 

    // make sure brightness and delay_time are in range
    check_delay_time();
    check_brightness();

    read_reciever();

    // if IR reciver is idle then update LEDs
    if( irReceiver.isIdle() ){
        last_time = current_time; // reset new last time updated LEDs

        static uint8_t start_index = 0; // for color pattern incrementing
        start_index = start_index + 1;
        fill_led_colors(start_index);
        FastLED.show();
        FastLED.delay(delay_time);
    }

   

}


/*********************************************************
*   void fill_led_colors
*
*
*
*********************************************************/

void fill_led_colors( uint8_t color_index){  
    for( int i=0; i< NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(current_palette, color_index, actual_brightness, current_blending);
        color_index += 3;
    } 
}

/*********************************************************
*   void check_brightness
*
*
*
*********************************************************/
void check_brightness(){  
  // make sure brightness is in range 
    if(brightness > 255){ 
      brightness = 255;
    }
    else if(brightness < 0){ brightness = 0; }

  // if lights are turned off then set brightness to zero
  if(turn_off){
    actual_brightness = 0;
   } else {
    actual_brightness = brightness;
  }
}


/*********************************************************
*   void check_delay_time
*
*
*
*********************************************************/
void check_delay_time(){   
  if(delay_time > 100) { delay_time = 100; }
  if(delay_time < 1){ delay_time = 1; }
}


/*********************************************************
*   void update_color_scheme
*       updates the current color palette
*
*********************************************************/
void update_color_scheme(){ 
   
  switch (palette_number) {
    case 0:
        palette_number = 5;
        current_palette = ForestColors_p;
        break;
    case 1:
        current_palette = RainbowColors_p;
        break;
    case 2:
        current_palette = CloudColors_p;
        break;
    case 3:
        current_palette = OceanColors_p;
        break;
    case 4:
        current_palette = LavaColors_p;
        break;
    case 5:
        current_palette = ForestColors_p;
        break;
    case 6:
        warm_light();
        break;
    default:
        palette_number = 1;
        current_palette = RainbowColors_p;
        break;
  }
}

/*********************************************************
*   void read_reciever
*
*
*
*********************************************************/
void read_reciever() {

    // if we read something from the reciver then decode results
    if (irReceiver.decode(&results)) 
    {
        switch (results.value) {
          case POWER:
             turn_off = !turn_off;
             break;
          case A:
            delay_time -= 2;
            break;
          case B:
            delay_time = 50;
            break;
          case C:
            delay_time += 2;
            break;
          case UP:
            brightness += 20;
            break;
          case DOWN:
            brightness -= 20;
            break;
          case LEFT:
            palette_number--;
            update_color_scheme();
            break;
          case RIGHT:
            palette_number++;
            update_color_scheme();
            break;
          case SELECT:
            palette_number = 6;
            update_color_scheme();
            break;
        }

        // resume reciever listening
        irReceiver.resume();
    }
}

/*********************************************************
*
*
*
*
*********************************************************/
void warm_light(){
    CRGB warm = CRGB( 244, 255, 250);
    
    current_palette = CRGBPalette16(
        warm, warm, warm, warm,
        warm, warm, warm, warm,
        warm, warm, warm, warm,
        warm, warm, warm, warm
    );
}




