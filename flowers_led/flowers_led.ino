#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 3

#include <IRremote.h>
#include <FastLED.h>

/********************************** pin variables ***********************************************/
#define RECV_PIN 11
#define LED_PIN  2
#define BRIGHT_PIN A0
#define BUTTON_PIN 8
/********************************** FastLED variables ***********************************************/
#define NUM_LEDS 25
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
/********************************** IR variables ***********************************************/
#define POWER 0x10EFD827 
#define A 0x10EFF807 
#define B 0x10EF7887
#define C 0x10EF58A7
#define UP 0x10EFA05F
#define DOWN 0x10EF00FF
#define LEFT 0x10EF10EF
#define RIGHT 0x10EF807F
#define SELECT 0x10EF20DF

IRrecv irrecv(RECV_PIN); // create IR object
decode_results results; // the results of the IR signal
/********************************** timing variables ***********************************************/
uint8_t default_speed = 51; // default speed of LEDs
uint8_t led_speed = default_speed; // hoew long to delay before updating LED colors
uint8_t max_led_speed = 99; // max speed of LEDs
uint16_t current_time = 0; // current time of MCU
uint8_t delay_inc = 5; // time delay incrementing
uint16_t debounce_delay = 500; // how much time to wait before accepting a button press again
uint16_t last_bounce_time = 0; // last time color change button was pressed
/********************************** LED variables ***********************************************/
CRGBPalette16 current_palette; // color palette object
TBlendType current_blending; // color blending object
CRGB leds[NUM_LEDS]; // led strip object for fastLED
uint16_t last_time = 0; // keep track of last time we updated LED colors
uint8_t palette_number = 1; // which color palette is being used
uint8_t lamp_light_number = 1; // which lamp color scheme
uint16_t start_index = 0; // for color pattern incrementing
uint8_t speed_inc = 3; // speed incrementing
/********************************** brightness variables ***********************************************/
uint16_t analog_bright = 0; // val from analog brightness input
uint16_t last_analog_bright = 0; // last brightness annlog read
uint8_t  max_analog_diff = 20; // max diff before we accept the analog read input as a 'real' change
uint8_t brightness = 128; // LED brightness -> need two to save last brightness before turning off/on
uint8_t actual_brightness = brightness; // the brightness that gets applied to the LED strip
uint8_t bright_inc = 20; // incrementing of brightness
uint8_t turn_off = 1; // do we want to 'turn off' LEDs? (brightness = 0)
/********************************** misc variables ***********************************************/
uint16_t power_delay = 3000; // power up delay



/******************************************************************************************
*   void setup
*
*
*
*******************************************************************************************/
void setup()
{
    delay(power_delay); // power-up safety delay
    Serial.begin(9600);

    pinMode(BUTTON_PIN, INPUT); // color button

    irrecv.enableIRIn(); // Start the receiver

    // setup LED object
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(brightness);
    
    // set LED initial colors
    current_palette = RainbowColors_p;
    current_blending = LINEARBLEND;
}


/*******************************************************************************************
*   void loop
*
*
*
*******************************************************************************************/
void loop() { 

    // read from IR if data is there
    read_reciever();

    // get current run time
    current_time = millis();

    // read button input and change color if pressed
    int reading = digitalRead(BUTTON_PIN);
    if (reading == HIGH && (current_time - last_bounce_time) > debounce_delay) {
        last_bounce_time = current_time;
        palette_number++;
        update_color_scheme();
    }

    // read brightness and see if changed
    // analog_bright = analogRead(BRIGHT_PIN);
    // if (analog_bright - last_analog_bright < max_analog_diff || analog_bright - last_analog_bright > max_analog_diff) {
    //     brightness += bright_inc;
    // }
    
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


/*******************************************************************************************
*   void fill_led_colors
*       fills the LED color array with the curent color palette
*   param:
        uint8_t color_index - the starting index of the LED array
*
*******************************************************************************************/

void fill_led_colors(uint16_t color_index){ 

    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(current_palette, color_index, actual_brightness, current_blending);
        color_index += 3;
    } 
}

/*******************************************************************************************
*   void turn_off_on
*       turns off/on the lights
*******************************************************************************************/
void turn_off_on(){ 

    // if lights are turned off then set brightness to zero
    if(turn_off){
        actual_brightness = 0;
    } else {
        actual_brightness = brightness;
    }
}

/*******************************************************************************************
*   void update_color_scheme
*       updates the current color palette
*******************************************************************************************/
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
            lamp_light_colors();
            break;
        default:
            palette_number = 1;
            current_palette = RainbowColors_p;
            break;
    }
}



/*******************************************************************************************
*   void lamp_light_colors
*       for testing out different lamp colors
*******************************************************************************************/
void lamp_light_colors() {

    
    if(lamp_light_number > 19){ lamp_light_number = 1; }
    if(lamp_light_number < 1){ lamp_light_number = 19; }

    switch(lamp_light_number) {
        case 1:
            // Candle 
            lamp_light(255, 147, 41);
            break;
        case 2:
            // 40W Tungsten 
            lamp_light(255, 197, 143);
            break;
        case 3:
            // 100W Tungsten 
            lamp_light(255, 214, 170);
            break;
        case 4:
            // Halogen 
            lamp_light(255, 241, 224);
            break;
        case 5:
            // Carbon Arc 
            lamp_light(255, 250, 244);
            break;
        case 6:
            // High Noon Sun 
            lamp_light(255, 255, 251);
            break;
        case 7:
            // Direct Sunlight 
           lamp_light( 255, 255, 25);
            break;
        case 8:
            // Overcast Sky 
            lamp_light(201, 226, 255);
            break;
        case 9:
            // Clear Blue Sky 
            lamp_light(64, 156, 255);
            break;
        case 10:
            // Warm Fluorescent 
            lamp_light(255, 244, 229);
            break;
        case 11:
            // Standard Fluorescent 
            lamp_light(244, 255, 250);
            break;
        case 12:
            // Cool White Fluorescent 
            lamp_light(212, 235, 255);
            break;
        case 13:
            // Full Spectrum Fluorescent 
            lamp_light(255, 244, 242);
            break;
        case 14:
            // Grow Light Fluorescent 
            lamp_light(255, 239, 247);
            break;
        case 15:
            // Black Light Fluorescent 
            lamp_light(167, 0, 255); 
            break;
        case 16:
            // Mercury Vapor 
            lamp_light(216, 247, 255);
            break;
        case 17:
            // Sodium Vapor 
            lamp_light(255, 209, 178);
            break;
        case 18:
            // Metal Halide 
            lamp_light(242, 252, 255);
            break;
        case 19:
            // High Pressure Sodium 
            lamp_light(255, 183, 76);
            break;
    }
}



/*******************************************************************************************
*   void read_reciever
*
*
*
*******************************************************************************************/
void read_reciever() {

    // if we read something from the reciver then decode results
    if (irrecv.decode(&results)) {

        switch (results.value) {
          case POWER:
             turn_off = turn_off ? 0 : 1;
             turn_off_on();
             break;
          case A:
            led_speed -= speed_inc;
            break;
          case B:
            led_speed = default_speed;
            break;
          case C:
            led_speed += speed_inc;
            break;
          case UP:
            brightness += bright_inc;
            actual_brightness = brightness;
            break;
          case DOWN:
            brightness -= bright_inc;
            actual_brightness = brightness;
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
            lamp_light_number++;
            update_color_scheme();
            break;
        }

        // make sure led_speed is in range
        if(led_speed > max_led_speed) { led_speed = 3; }
        if(led_speed < 1) { led_speed = max_led_speed; }

        if(true){
            Serial.print("results.value: ");
            Serial.println(results.value);
            Serial.print("palette_number: ");
            Serial.println(palette_number);
            Serial.print("brightness: ");
            Serial.println(brightness);
            Serial.print("actual_brightness: ");
            Serial.println(actual_brightness);
            Serial.print("led_speed: ");
            Serial.println(led_speed);
            Serial.print("lamp_light_number: ");
            Serial.println(lamp_light_number);
            Serial.println("");
        }

        // resume reciever listening
        irrecv.resume();
    }
}

/*******************************************************************************************
*   void lamp_light
*
*
*
*******************************************************************************************/
void lamp_light(uint8_t red, uint8_t green, uint8_t blue){

    CRGB rgb = CRGB( red, green, blue);

    current_palette = CRGBPalette16(
        rgb, rgb, rgb, rgb,
        rgb, rgb, rgb, rgb,
        rgb, rgb, rgb, rgb,
        rgb, rgb, rgb, rgb
    );
}

