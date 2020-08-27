/* octoPedal!
 *  this is a simple joystick that takes input from a basic hall sensor
 *  A3144 A3144E OH3144E, whatever. 
 *  
 *  We use the digispark core for "digijoystick" to make our single button device
 *  to change the USB id you have to go into the files a bit, they're in the digijoystick configuration files.
 *  
 *  ws2812.h is a minimalistic addressable LED controller, fastLED is too big and extensive for the attiny85
 * 
 * we used a metal body foot pedal:
 * https://www.amazon.com/gp/product/B00EF9D2DY/
 * there's a whole family of these products, basically as long as it can move a magnet in
 * or out of the way of the sensor it's fine. Hell you could even use a switch
 * I'm a comment not a cop do what you want.
 */


#include <DigiJoystick.h>

#include <WS2812.h>

//settings
#define FADETIME 400 //ms
#define FPS 80 // frames/sec
#define BLINKTIME 200 //ms delay
#define BLINK 4 //nr of blinks
cRGB color1 = {255, 0, 0};
cRGB color2 = {0, 0 , 255};
cRGB readyColorOn = {0, 255, 0};
cRGB readyColorOff = {0,0,0};

//hardware
#define INVERT 1
#define LEDPIN 0
#define MAGPIN 2
#define LEDCOUNT 1


#define FNUM (FPS * FADETIME/1000)
#define FRAMEDEL (FADETIME/FNUM)

//Globals
bool flux = true;
bool prev_flux = 1; //initial state = no magnet
uint8_t iterater = 0;
unsigned long timerF = 0;

//library Stuff
WS2812 LED(LEDCOUNT);
cRGB value;

const float stepSize_r = (color2.r-color1.r)/((float) FNUM);
const float stepSize_g = (color2.g-color1.g)/((float) FNUM);
const float stepSize_b = (color2.b-color1.b)/((float) FNUM);

//=============Setup===========
void setup() {
  //pins
  pinMode(MAGPIN, INPUT_PULLUP);

  //libraries
  LED.setOutput(LEDPIN);
  LED.setColorOrderRGB();

  //jstick
  DigiJoystick.setX((byte) 127); // scroll X left to right repeatedly
  DigiJoystick.setY((byte) 127);
  DigiJoystick.setXROT((byte) 127);
  DigiJoystick.setYROT((byte) 127);
  DigiJoystick.setZROT((byte) 127);
  DigiJoystick.setSLIDER((byte) 127);

  doneSetup();
}

//=============LOOP===========
void loop() {
  //read inputs
  readStuff();
  fadeStuff();
  DigiJoystick.delay(1);
}

//============FUNCTIONS=========
//readStuff reads the pins with no timer and sets globals
void readStuff(){
  flux = digitalRead(MAGPIN);
  if(INVERT){
    flux = !flux;
  }
  
  if(flux != prev_flux){
    if(flux){
      DigiJoystick.setButtons((byte) 0x00, (byte) 0x00);
    }else{
      DigiJoystick.setButtons((byte) 0x01, (byte) 0x00);
    }
    
    prev_flux = flux;
    
  }
}

//changes ghue value based on flux variable
void fadeStuff(){
  if( millis() - timerF > FRAMEDEL ){
    iterate();
    
    for(int i = 0; i < LEDCOUNT; i++){    
      value.r = color1.r + (int) (stepSize_r * iterater);
      value.g = color1.g + (int) (stepSize_g * iterater);
      value.b = color1.b + (int) (stepSize_b * iterater);
      
      LED.set_crgb_at(i, value);  

    }
    
    timerF = millis();
    LED.sync(); 
  }
}

//acts on global variable iterater based on flux boolean
void iterate(){
  if(flux && iterater >= (FNUM-1)){
    iterater = (FNUM-1);
  }else if (!flux && iterater <= 0){
    iterater = 0;
  }else if (flux){
    iterater++; 
  }else{
    iterater--;
  }
}

void doneSetup(){
  for(int j = 0; j < BLINK; j++){
    
    DigiJoystick.delay(BLINKTIME);  
    
    for(int i = 0; i < LEDCOUNT; i++){
      LED.set_crgb_at(i, readyColorOn);      
      LED.sync();    
    }
    
    DigiJoystick.delay(BLINKTIME);
      
    for(int i = 0; i < LEDCOUNT; i++){
      LED.set_crgb_at(i, readyColorOff);      
      LED.sync();    
    }
  }
}
