
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <math.h>
#include <SoftwareSerial.h>
#define COL_HEIGHT   16             // Height of the column by number of leds
#define TOTAL_PIXELS 256            // Total pixels in the screen
#define MIC_PIN      4             // Microphone is attached to this analog pin
#define LED_PIN      16              // NeoPixel LED strand is connected to this pin
#define LED_BUTTON   2
#define BUTTON_PIN   5              // the number of the pushbutton pin to rotate the effects
#define PEAK_FALL    15             // Rate of falling peak dot
#define DC_OFFSET    0              // DC offset in mic signal - if unusure, leave 0
#define NOISE        10             // Noise/hum/interference in mic signal
#define SAMPLES      6              // 60 Length of buffer for dynamic level adjustment 60
#define TOP       (COL_HEIGHT + 2)  // Allow dot to go slightly off scale
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

CRGBPalette16 currentPalette(OceanColors_p);
CRGBPalette16 targetPalette(CloudColors_p);

int BRIGHTNESS_MAX = 30;
int brightness = 10;


byte volCount  = 0;      // Frame counter for storing past volume data
int
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 2,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;

int buttonPushCounter = 0;  // counter for the number of button presses
int buttonState = 0;        // current state of the button
int lastButtonState = 0;
byte peak = 6;              // 16  Peak level of column; used for falling dots
byte dotCount = 0;          //Frame counter for peak dot
byte dotHangCount = 0;      //Frame counter for holding peak dot

void setup() {

  pinMode(BUTTON_PIN, INPUT);
  //initialize the BUTTON_PIN as output
  //digitalWrite(BUTTON_PIN, HIGH);
  pinMode(LED_BUTTON, OUTPUT);

  strip.begin();
  strip.show(); // all pixels to 'off'

  //Serial.begin(57600);
  delay(3000);

 }

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, float opacity) {

  if(WheelPos < 85) {
    return strip.Color((WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity, 0);
  }
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color((255 - WheelPos * 3) * opacity, 0, (WheelPos * 3) * opacity);
  }
  else {
    WheelPos -= 170;
    return strip.Color(0, (WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity);
  }
}

void shiftDisplayToLeft(uint8_t columnLimit) {
  // shift the display left from columnLimit
  uint8_t newPixel = 0;
  for (uint8_t row = 0; row < COL_HEIGHT; row++) {
    for (uint8_t col = 0; col < COL_HEIGHT - columnLimit; col++) {
      newPixel = (row * COL_HEIGHT) + col;
      uint32_t previousColor = strip.getPixelColor(newPixel + 1);
      strip.setPixelColor(newPixel, previousColor);
    }
  }
}

void displayMainStreamOnRightSide(int height, int numberOfColsToFix) {
  // Color pixels based on rainbow gradient
  for (uint8_t i = 0; i < COL_HEIGHT; i++) {
    // newPixel = 240 - (16 * i);
    int newPixel = TOTAL_PIXELS - (COL_HEIGHT * i) - 1;
    if(i >= height) {
      for (int col = 0; col < numberOfColsToFix; col++){
        strip.setPixelColor(newPixel - col, 0, 0, 0);
      }
    }
    else {
        for (uint8_t col = 0; col < numberOfColsToFix; col++){
          strip.setPixelColor(newPixel - col, Wheel(map(i, 0, COL_HEIGHT - 1, 30, 150)));
        }
    }
  }
}

void drawPeakDotRightToLeft(int numberOfColsToFix) {
  // Draw peak dot
  if(peak > 0 && peak <= COL_HEIGHT - 1){
    //newPixel = 240 - (16 * peak);
    int newPixel = TOTAL_PIXELS - (COL_HEIGHT * peak) - 1;
    for (int col = 0; col < numberOfColsToFix; col++) {
      strip.setPixelColor(newPixel - col, Wheel(map(peak,0,COL_HEIGHT - 1,30,150)));
    }
  }
}

/////////// ModeRightToLeft BEGINS ////////////

void ModeRightToLeft(int numberOfColsToFix, int numberOfColsToDrawPeak) {

  // Places de sound meter in the last two led columns from the right
  // Shift the history to the left without the peak (does not look good)
  if (numberOfColsToFix == 0)
    numberOfColsToFix = 3;

  uint8_t  i;
  int minLvl, maxLvl;
  int      n, height;

  n   = analogRead(MIC_PIN);
  n   = abs(n - 512 - DC_OFFSET);         // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);   // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;             // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if (height < 0L)
    height = 0;      // Clip output
  else
    if (height > TOP)
      height = TOP;
  if (height > peak)
    peak   = height; // Keep 'peak' dot at top

  shiftDisplayToLeft(numberOfColsToFix);

  displayMainStreamOnRightSide(height, numberOfColsToFix);

  drawPeakDotRightToLeft(numberOfColsToDrawPeak); //numberOfColsToFix - 1 => Shift the history to the left without the peak (does not look good)

  strip.show(); // Update strip

  // Every few frames, make the peak pixel drop by 1:
  if(++dotCount >= PEAK_FALL) { //fall rate

    if(peak > 0) peak--;
    dotCount = 0;
  }

  vol[volCount] = n;                      // Save sample for dynamic leveling
  if( ++volCount >= SAMPLES)
    volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for (i=1; i<SAMPLES; i++) {
    if (vol[i] < minLvl)
      minLvl = vol[i];
    else
      if(vol[i] > maxLvl)
        maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if ((maxLvl - minLvl) < TOP)
    maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

}

/////////// ModeRightToLeft ENDS ////////////

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
// SimplePatternList qPatterns = {Mode0_allBarsAsOne, ModeRightToLeft};
uint8_t qCurrentPatternNumber = 0; // Index number of which pattern is current

void loop() {

  // read the pushbutton input pin:
  buttonState = digitalRead(BUTTON_PIN);
    // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      buttonPushCounter++;
      digitalWrite(LED_BUTTON, HIGH);
    } else {
      digitalWrite(LED_BUTTON, LOW);
    }
  }
  // save the current state as the last state,
  //for next time through the loop
  lastButtonState = buttonState;

  switch (buttonPushCounter){
    case 1:
      // This mode uses the 16 columns of the screen as ONE sound meter
      // places the peak in all the bars
      ModeRightToLeft(16, 16);
      break;

    case 2:
      // This mode uses the last 3 columns of the screen as ONE sound meter
      // places the peak in the last TWO bars
      // display the history to the leaft without the peak
      ModeRightToLeft(3, 2);
      break;

    case 3:
      // This mode uses the last 3 columns of the screen as ONE sound meter
      // places the peak on the 3
      // display the history to the leaft with the peak
      ModeRightToLeft(3, 3);
      break;

    default:
      lastButtonState = 0;
      break;
  }
}
