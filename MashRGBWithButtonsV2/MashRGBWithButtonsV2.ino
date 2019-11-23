
// Sample using LiquidCrystal library
// https://codegists.com/code/arduino-lcd-keypad-shield-example-code-wiki/
#include <LiquidCrystal.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE

//  ************   RGB SETUP  ********************
#define DATA_PIN    3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
//#define NUM_LEDS    502
//#define NUM_LEDS    527 - Joes suit last time
#define NUM_LEDS    230 // Sam's suit
//#define NUM_LEDS    234 // Joe's suit
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120


//  ************  PANEL SETUP *****************
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define BACKLIGHTPIN 10
 
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 

void setup()
{
   //  Intialize the LCD Scrren
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0,0);
  lcd.print("Running Sinelon"); // print a simple message

  delay(1000); // 3 second delay for recovery
  analogWrite(BACKLIGHTPIN, 0);
  
  //  Initialize the LEDs
  delay(2000); // 3 second delay for recovery
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);  
}


//  ************ Setup needed to Make Demo Reel functinos work *********************
// List of patterns available, these should be mapped to buttons
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { confetti, rainbowWithGlitter, sinelon, nightrider, bpm };
uint8_t gCurrentPatternNumber = 2; // Index number of which pattern is current
uint8_t gLastPatternNumber = 2; // Used to detect change in patterns, restrict updates.
String  strCurrPatternName; 
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void loop()
{
  //  Get this value now so we check for a button change later.
  gLastPatternNumber = gCurrentPatternNumber;

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow  
//  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  
 
  lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
  lcd.print(millis()/1000);      // display seconds elapsed since power-up
 
   lcd.setCursor(0,1);            // move to the begining of the second line
  lcd_key = read_LCD_buttons();  // read the buttons
 
  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
  case btnRIGHT:
     {
     strCurrPatternName = "Rainbow ";
     gCurrentPatternNumber = 1;
     break;
     }
  case btnLEFT:
     {
     strCurrPatternName = "NightRdr";
     gCurrentPatternNumber = 3;
     break;
     }
  case btnUP:
     {
     strCurrPatternName = "BPM     ";
     gCurrentPatternNumber = 4;
     break;
     }
  case btnDOWN:
     {
     strCurrPatternName = "Confetti";
     gCurrentPatternNumber = 0;
     break;
     }
  case btnSELECT:
     {
     strCurrPatternName = "Sinelon ";
     gCurrentPatternNumber = 2;
     break;
     }
  case btnNONE:
     {
     strCurrPatternName = "";
     break;
     }
 }
 
 update_LCD_status(strCurrPatternName);
}

void update_LCD_status(String strPatternName)
{
  if (gCurrentPatternNumber != gLastPatternNumber)
   {
      lcd.setCursor(0,0);  //  Update the first line - this is current status. We do not want to overdue this.
      lcd.print("Running "); 
      lcd.print(strPatternName);
   }
  lcd.setCursor(0,1); //  This is the button pressed
  if (strPatternName!="")
  {
    FastLED.setBrightness(0); // Turn RGB Down for a second, to allow for back light to work
    analogWrite(BACKLIGHTPIN, 10);
    strPatternName.trim();
    lcd.print(strPatternName);
    lcd.print(" Pressed");
    delay(1000);
    FastLED.setBrightness(BRIGHTNESS); //  Turn LEDs back on
  }
  else
  {
    lcd.print("                ");
    analogWrite(BACKLIGHTPIN, 0);
  }
}

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 
 return btnNONE;  // when all others fail, return this...
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 40);
  int pos = beatsin16( 16, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void nightrider()
{
  for(int i = 0; i < int(NUM_LEDS/2); i++) {   
    // fade everything out
    fadeToBlackBy( leds, NUM_LEDS, 40);

    // let's set an led value
    //leds[i] = CHSV(gHue++,255,255);
    //leds[int(i+NUM_LEDS/2+1)] = CHSV(gHue++,255,255);

    // let's set an led value - Try Red
    leds[i] = CRGB::Red;
    leds[int(i+NUM_LEDS/2)] = CRGB::Red;    

    // now, let's first 20 leds to the top 20 leds, 
    //leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);

    FastLED.delay(20);
  }
}

void glitter()
{
  addGlitter(80);
}

