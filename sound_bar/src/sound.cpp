
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <math.h>
#include <SoftwareSerial.h>
#define N_PIXELS 16  // Number of pixels in strand
#define COL_HEIGHT 16 // Height of the pixel column
#define TOTAL_PIXELS 256  // Total pixels in the screen
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define LED_PIN    6  // NeoPixel LED strand is connected to this pin
#define BUTTON_PIN  5     // the number of the pushbutton pin to rotate the effects
#define PEAK_FALL 15 //Rate of falling peak dot
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   6  // 60 Length of buffer for dynamic level adjustment 60
#define TOP       (N_PIXELS + 2) // Allow dot to go slightly off scale
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

CRGBPalette16 currentPalette(OceanColors_p);
CRGBPalette16 targetPalette(CloudColors_p);

int BRIGHTNESS_MAX = 30;
int brightness = 10;


byte
//  peak      = 0,      // Used for falling dot
//  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data
int
  reading,
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 2,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;
float
  greenOffset = 30,
  blueOffset = 150;
// cycle variables

int CYCLE_MIN_MILLIS = 2;
int CYCLE_MAX_MILLIS = 1000;
int cycleMillis = 20;
bool paused = false;
long lastTime = 0;
bool boring = true;
bool gReverseDirection = false;
int          myhue =   0;
//Vu ripple
uint8_t colour;
uint8_t myfade = 255;                                         // Starting brightness.
#define maxsteps 6                                           // 16 Case statement wouldn't allow a variable.
int peakspersec = 0;
int peakcount = 0;
uint8_t bgcol = 0;
int thisdelay = 20;
uint8_t max_bright = 255;


unsigned int sample;

//Samples
#define NSAMPLES 64
unsigned int samplearray[NSAMPLES];
unsigned long samplesum = 0;
unsigned int sampleavg = 0;
int samplecount = 0;
//unsigned int sample = 0;
unsigned long oldtime = 0;
unsigned long newtime = 0;

//Ripple variables
int color;
int center = 0;
int step = -1;
int maxSteps = 6;  //16
float fadeRate = 0.80;
int diff;

//Vu 8 variables
int
  origin = 0,
  color_wait_count = 0,
  last_intensity = 0,
  intensity_max = 0,
  origin_at_flip = 0;
boolean
  growing = false,
  fall_from_left = true;



//background color
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;
TBlendType    currentBlending;

 //Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;


    byte peak = 6;      // 16  Peak level of column; used for falling dots
//    unsigned int sample;

    byte dotCount = 0;  //Frame counter for peak dot
    byte dotHangCount = 0; //Frame counter for holding peak dot

void setup() {

  //analogReference(EXTERNAL);
  pinMode(BUTTON_PIN, INPUT);
  //initialize the BUTTON_PIN as output
  digitalWrite(BUTTON_PIN, HIGH);

  // Serial.begin(9600);
  strip.begin();
  strip.show(); // all pixels to 'off'

  Serial.begin(57600);
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

void displayMainStreamOnRightSide(int height, int colToRemainFixed) {
  // Color pixels based on rainbow gradient
  for(uint8_t i = 0; i < COL_HEIGHT; i++) {
    // newPixel = 240 - (16 * i);
    int newPixel = TOTAL_PIXELS - (COL_HEIGHT * i) - 1;
    if(i >= height) {
      for (int col = 0; col < colToRemainFixed; col++){
        strip.setPixelColor(newPixel - col, 0, 0, 0);
      }
    }
    else {
        for (uint8_t col = 0; col < colToRemainFixed; col++){
          strip.setPixelColor(newPixel - col, Wheel(map(i, 0, COL_HEIGHT - 1, 30, 150)));
        }
    }
  }
}

void drawPeakDot(int colToRemainFixed) {
  // Draw peak dot
  if(peak > 0 && peak <= N_PIXELS - 1){
    //newPixel = 240 - (16 * peak);
    int newPixel = TOTAL_PIXELS - (COL_HEIGHT * peak) - 1;
    for (int col = 0; col < colToRemainFixed; col++) {
      strip.setPixelColor(newPixel - col, Wheel(map(peak,0,COL_HEIGHT - 1,30,150)));
    }
  }
}

/////////// VU1 BEGINS ////////////

void Vu1(int colToRemainFixed, int peakInFirstColumn) {

  // Places de sound meter in the last two led columns from the right
  // Shift the history to the left without the peak (does not look good)
  if (colToRemainFixed == 0)
    colToRemainFixed = 3;

  uint8_t  i;
  int minLvl, maxLvl;
  int      n, height;

  n   = analogRead(MIC_PIN);
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if (height < 0L)
    height = 0;      // Clip output
  else
    if (height > TOP)
      height = TOP;
  if (height > peak)
    peak   = height; // Keep 'peak' dot at top

  shiftDisplayToLeft(colToRemainFixed);

  displayMainStreamOnRightSide(height, colToRemainFixed);

  drawPeakDot(colToRemainFixed - peakInFirstColumn); //colToRemainFixed - 1 => Shift the history to the left without the peak (does not look good)

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

/////////// VU1 ENDS ////////////

/////////// VU0 BEGINS ////////////
void Vu0() {
  // This mode uses all the screen as a sound meter
  Vu1(16, 0);
  //a = 0;
}
/////////// VU0 ENDS ////////////

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
// SimplePatternList qPatterns = {Vu0, Vu1};
uint8_t qCurrentPatternNumber = 0; // Index number of which pattern is current

void loop() {

  // read the pushbutton input pin:
  buttonState = digitalRead(BUTTON_PIN);
    // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes:  ");
      Serial.println(buttonPushCounter);
      if(buttonPushCounter==16) {
      buttonPushCounter=1;}
    }
    else {
      // if the current state is LOW then the button
      // wend from on to off:
      Serial.println("off");
    }
  }
  // save the current state as the last state,
  //for next time through the loop
  lastButtonState = buttonState;

  switch (buttonPushCounter){
    case 1:
      Vu0();
      break;

    case 2:
      Vu1(0, 1);
      break;

    case 3:
      Vu1(0, 0);
      break;

    default:
      lastButtonState = 0;
      break;
  }
}
