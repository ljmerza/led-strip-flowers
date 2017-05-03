#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 3

#include <IRremote.h>
#include <FastLED.h>




/***************** define variables******************************/
#define RECV_PIN 11
#define LED_PIN  2
#define BUTTON_PIN 8
#define NUM_LEDS 25
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

#define POWER 0x10EFD827 
#define A 0x10EFF807 
#define B 0x10EF7887
#define C 0x10EF58A7
#define UP 0x10EFA05F
#define DOWN 0x10EF00FF
#define LEFT 0x10EF10EF
#define RIGHT 0x10EF807F
#define SELECT 0x10EF20DF
/***************** IR variables******************************/
IRrecv irrecv(RECV_PIN); // create IR object
decode_results results; // the results of the IR signal
/***************** IR controlled variables******************************/
uint8_t brightness = 128; // LED brightness -> need two to save last brightness before turning off/on
uint8_t default_speed = 50; // default speed of LEDs
uint8_t led_speed = led_speed; // hoew long to delay before updating LED colors
uint8_t max_led_speed = 100; // max speed of LEDs
uint16_t current_time = 0; // current time of MCU
boolean turn_off = true; // do we want to 'turn off' LEDs? (brightness = 0)
uint8_t start_index = 0; // for color pattern incrementing

uint8_t delay_inc = 5;
uint8_t bright_inc = 20;
uint8_t speed_inc = 5;
/***************** misc variables******************************/
uint16_t power_delay = 3000; // power up delay
/***************** LED variables******************************/
CRGBPalette16 current_palette; // color palette object
TBlendType current_blending; // color blending object
CRGB leds[NUM_LEDS]; // led strip object for fastLED
uint16_t last_time = 0; // keep track of last time we updated LED colors
uint8_t actual_brightness; // the brightness that gets applied to the LED strip
uint8_t palette_number = 1; // which color palette is being used



/********************************************************
*   void setup
*
*
*
*********************************************************/
void setup()
{
    delay(power_delay); // power-up safety delay

    pinMode(BUTTON_PIN, INPUT); // reset button

    irrecv.enableIRIn(); // Start the receiver

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

    // make sure led_speed is in range
    if(led_speed > max_led_speed) { led_speed = max_led_speed; }

    // read from IR if data is there
    read_reciever();

    current_time = millis();
    // if led_speed has passed and IR is idle then update LEDs
    if( (current_time - last_time > (max_led_speed - led_speed)) && irrecv.isIdle() ){

        // reset new last time updated LEDs
        last_time = current_time; 
        // increment starting index
        start_index = start_index + 1;

        // update LED colors and send to LED strip
        fill_led_colors(start_index);
        FastLED.show();
    }
}


/*********************************************************
*   void fill_led_colors
*
*
*
*********************************************************/

void fill_led_colors(uint8_t color_index){ 

    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(current_palette, color_index, actual_brightness, current_blending);
        color_index += 3;
    } 
}

/*********************************************************
*   void turn_off_on
*
*
*
*********************************************************/
void turn_off_on(){  
  // if lights are turned off then set brightness to zero
  if(turn_off){
    actual_brightness = 0;
   } else {
    actual_brightness = brightness;
  }
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
    if (irrecv.decode(&results)) 
    {
        switch (results.value) {
          case POWER:
             turn_off = !turn_off;
             turn_off_on();
             break;
          case A:
            led_speed -= speed_inc;
            break;
          case B:
            led_speed = default_speed;
            break;
          case C:
            led_speed += delay_inc;
            break;
          case UP:
            brightness += bright_inc;
            break;
          case DOWN:
            brightness -= bright_inc;
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
        irrecv.resume();
    }
}

/*********************************************************
*   void warm_light
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