
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <math.h>
#include <SoftwareSerial.h>
#define N_PIXELS 16  // Number of pixels in strand
#define COL_HEIGHT 16 // Height of the pixel column
#define TOTAL_PIXELS 256  // Total pixels in the screen
#define N_PIXELS_HALF (N_PIXELS/2)
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define LED_PIN    6  // NeoPixel LED strand is connected to this pin
#define BUTTON_PIN  5     // the number of the pushbutton pin to rotate the effects
#define SAMPLE_WINDOW   10  // Sample window for average level
#define PEAK_HANG 24 //Time of pause before peak dot falls
#define PEAK_FALL 15 //Rate of falling peak dot
#define PEAK_FALL2 8 //Rate of falling peak dot
#define INPUT_FLOOR 10 //Lower range of analogRead input
#define INPUT_CEILING 1023  //Max range of analogRead input, the lower the value the more sensitive (1023 = max)300 (150)
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   6  // 60 Length of buffer for dynamic level adjustment 60
#define TOP       (N_PIXELS + 2) // Allow dot to go slightly off scale
#define SPEED .20       // Amount to increment RGB color by each cycle
#define TOP2      (N_PIXELS + 1) // Allow dot to go slightly off scale
#define LAST_PIXEL_OFFSET N_PIXELS-1
#define PEAK_FALL_MILLIS 10  // Rate of peak falling dot
#define POT_PIN    4  //4
#define BG 0
#define LAST_PIXEL_OFFSET N_PIXELS-1
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B     // Only use the LED_PIN for WS2812's
#define COLOR_ORDER GRB
#define COLOR_MIN           0
#define COLOR_MAX         255
#define DRAW_MAX          100
#define SEGMENTS            2  // 4 Number of segments to carve amplitude bar into
#define COLOR_WAIT_CYCLES  10  // Loop cycles to wait between advancing pixel origin
#define qsubd(x, b)  ((x>b)?b:0)
#define qsuba(x, b)  ((x>b)?x-b:0)                                              // Analog Unsigned subtraction macro. if result <0, then => 0. By Andrew Tuline.
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


struct CRGB leds[TOTAL_PIXELS];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

static uint16_t dist;         // A random number for noise generator.
uint16_t scale = 10;          // 30 Wouldn't recommend changing this on the fly, or the animation will be really blocky.
uint8_t maxChanges = 48;      // Value for blending between palettes.

CRGBPalette16 currentPalette(OceanColors_p);
CRGBPalette16 targetPalette(CloudColors_p);

// Functions declaration for Atom IDE
void All2();
void Vu();
void Vu1();
void Vu2();
void Vu3();
void Vu4();
void Vu5();
void Vu6();
void Vu7();
void Vu8();
void Vu9();
void Vu10();
void Vu11();
void Vu12();
void Vu13();
void colorWipe(uint32_t c, uint8_t wait);
uint32_t Wheel(byte WheelPos);
void drawLine(uint8_t from, uint8_t to, uint32_t c);
void soundmems();
void ripple3();
int calculateIntensity();
void updateOrigin(int intensity);
void assignDrawValues(int intensity);
void writeSegmented();
void updateGlobals();
void jugglep();
void sndwave();
void soundble();
void addGlitter(fract8 sampleavg);                          // FastLED based non-blocking delay to update/display the sequence.
void soundtun();
void soundrip();
void soundripped();
void soundripper();
void rippled();
void rippVu();
void Vu0();


//new ripple Vu
uint8_t timeval = 20;                                                           // Currently 'delay' value. No, I don't use delays, I use EVERY_N_MILLIS_I instead.
uint16_t loops = 0;                                                             // Our loops per second counter.
bool     samplepeak = 0;                                                        // This sample is well above the average, and is a 'peak'.
uint16_t oldsample = 0;                                                         // Previous sample is used for peak detection and for 'on the fly' values.
bool thisdir = 0;
//new ripple Vu

// Modes
enum
{
} MODE;
bool reverse = true;
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
  scroll_color = COLOR_MIN,
  last_intensity = 0,
  intensity_max = 0,
  origin_at_flip = 0;
uint32_t
    draw[DRAW_MAX];
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

  //LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,TOTAL_PIXELS).setCorrection(TypicalLEDStrip);
  //LEDS.setBrightness(BRIGHTNESS);
  dist = random16(12345);          // A semi-random number for our noise generator

 }

void loop() {

   //for mic
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;
  // end mic
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
     buttonPushCounter==1; {
     Vu0(); // NORMAL
      break;}

    case 2:
     buttonPushCounter==2; {
     Vu1(); // NORMAL
      break;}

     case 3:
     buttonPushCounter==3; {
       Vu2(); // Centre out
      break;}
}
//
//    case 4:
//      buttonPushCounter==4; {
//     Vu2(); // Centre Inwards
//       break;}
//
//     case 5:
//      buttonPushCounter==5; {
//     Vu3(); // Normal Rainbow
//       break;}
//
//       case 6:
//      buttonPushCounter==6; {
//     Vu4(); // Centre rainbow
//       break;}
//
//        case 7:
//      buttonPushCounter==7; {
//     Vu5(); // Shooting Star
//       break;}
//
//          case 8:
//      buttonPushCounter==8; {
//     Vu6(); // Falling star
//       break;}
//
//           case 9:
//      buttonPushCounter==9; {
//     Vu7(); // Ripple with background
//       break;}
//
//             case 10:
//      buttonPushCounter==10; {
//     Vu8(); // Shatter
//       break;}
//
//             case 11:
//      buttonPushCounter==11; {
//     Vu9(); // Pulse
//       break;}
//
//             case 12:
//      buttonPushCounter==12; {
//      Vu10(); // stream
//       break;}
//            case 13:
//      buttonPushCounter==13; {
//      Vu11(); // Ripple without Background
//       break;}
//
//             case 14:
//      buttonPushCounter==14; {
//      Vu12(); // Ripple without Background
//       break;}
//
//                   case 15:
//      buttonPushCounter==15; {
//      Vu13(); // Ripple without Background
//       break;}
//
//
//            case 16:
//      buttonPushCounter==16; {
//     colorWipe(strip.Color(0, 0, 0), 10); // Black
//       break;}
// }

}

/////////// VU1 BEGINS ////////////

void Vu1() {

  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;

  // const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
  // unsigned long startMillis= millis();
  // while (millis() - startMillis < sampleWindow) {
  //
  //   n   = analogRead(MIC_PIN);
  // }                      // Raw reading from mic

  n   = analogRead(MIC_PIN);
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top

  // shift the display left
  int newPixel = 0;
  for (int row = 0; row < COL_HEIGHT; row++) {
    for (int col = 0; col < COL_HEIGHT - 2; col++) {
      newPixel = (row * COL_HEIGHT) + col;
      uint32_t previousColor = strip.getPixelColor(newPixel + 1);
      strip.setPixelColor(newPixel, previousColor);
    }
  }

  // Color pixels based on rainbow gradient
  for(i=0; i<N_PIXELS; i++) {
    // newPixel = 240 - (16 * i);
    newPixel = 256 - (16 * i) - 1;
    if(i >= height) {
      // for (int col = 0; col < COL_HEIGHT; col++){
        strip.setPixelColor(newPixel, 0, 0, 0);
        strip.setPixelColor(newPixel - 1, 0, 0, 0);
      // }
    }
    else {
        //for (int col = 0; col < COL_HEIGHT; col++){
          strip.setPixelColor(newPixel, Wheel(map(i, 0, COL_HEIGHT - 1, 30, 150)));
          strip.setPixelColor(newPixel - 1, Wheel(map(i, 0, COL_HEIGHT - 1, 30, 150)));
        //}
    }
  }


  // Draw peak dot
  if(peak > 0 && peak <= N_PIXELS-1){
    //newPixel = 240 - (16 * peak);
    newPixel = 256 - (16 * peak) - 1;
    //for (int col = 0; col < COL_HEIGHT; col++){
      strip.setPixelColor(newPixel, Wheel(map(peak,0,COL_HEIGHT - 1,30,150)));
      //strip.setPixelColor(newPixel - 1, Wheel(map(peak,0,COL_HEIGHT - 1,30,150)));
    //}
  }

   strip.show(); // Update strip

// Every few frames, make the peak pixel drop by 1:

    if(++dotCount >= PEAK_FALL) { //fall rate

      if(peak > 0) peak--;
      dotCount = 0;
    }


  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

}

/////////// VU1 ENDS ////////////

/////////// VU0 BEGINS ////////////

void Vu0() {

  // Turns on the hole screen as one sound meter
  // All the rows go in parallel
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;

  n   = analogRead(MIC_PIN);
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top

  // Color pixels based on rainbow gradient
  int newPixel = 0;
  for(i=0; i<N_PIXELS; i++) {
    newPixel = 240 - (16 * i);
    if(i >= height) {
      for (int col = 0; col < COL_HEIGHT; col++){
        strip.setPixelColor(newPixel + col, 0, 0, 0);
      }
    }
    else {
        for (int col = 0; col < COL_HEIGHT; col++){
          strip.setPixelColor(newPixel + col, Wheel(map(i, 0, COL_HEIGHT - 1, 30, 150)));
        }
    }
  }


  // Draw peak dot
  if(peak > 0 && peak <= N_PIXELS-1){
    newPixel = 240 - (16 * peak);
    for (int col = 0; col < COL_HEIGHT; col++){
      strip.setPixelColor(newPixel + col, Wheel(map(peak,0,COL_HEIGHT - 1,30,150)));
    }
  }

   strip.show(); // Update strip

// Every few frames, make the peak pixel drop by 1:

    if(++dotCount >= PEAK_FALL) { //fall rate

      if(peak > 0) peak--;
      dotCount = 0;
    }


  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

}

/////////// VU0 ENDS ////////////

/////////// VU2 BEGINS ////////////

void Vu2() {

  // This is like Vu1 but the sampling is different
  const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)

  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0) * 2;
    if (sample < 1024)  // toss out spurious readings
    {
       if (sample > signalMax)
       {
          signalMax = sample;  // save just the max levels
       }
       else if (sample < signalMin)
       {
          signalMin = sample;  // save just the min levels
       }
    }
  }
  peakToPeak = signalMax - signalMin;

 // map 1v p-p level to the max scale of the display
 int displayPeak = map(peakToPeak, 0, 1023, 0, COL_HEIGHT);
 if(displayPeak > peak)     peak   = displayPeak; // Keep 'peak' dot at top

  // shift the display left
  int newPixel = 0;
  for (int row = 0; row < COL_HEIGHT; row++) {
    for (int col = 0; col < COL_HEIGHT - 2; col++) {
      newPixel = (row * COL_HEIGHT) + col;
      uint32_t previousColor = strip.getPixelColor(newPixel + 1);
      strip.setPixelColor(newPixel, previousColor);
    }
  }

  // Color pixels based on rainbow gradient
  for(int i = 0; i<N_PIXELS; i++) {
    // newPixel = 240 - (16 * i);
    newPixel = 256 - (16 * i) - 1;
    if(i >= displayPeak) {
      // for (int col = 0; col < COL_HEIGHT; col++){
        strip.setPixelColor(newPixel, 0, 0, 0);
        strip.setPixelColor(newPixel - 1, 0, 0, 0);
      // }
    }
    else {
        //for (int col = 0; col < COL_HEIGHT; col++){
          strip.setPixelColor(newPixel, Wheel(map(i, 0, COL_HEIGHT - 1, 30, 150)));
          strip.setPixelColor(newPixel - 1, Wheel(map(i, 0, COL_HEIGHT - 1, 30, 150)));
        //}
    }
  }


  // Draw peak dot
  if(peak > 0 && peak <= N_PIXELS-1){
    //newPixel = 240 - (16 * peak);
    newPixel = 256 - (16 * peak) - 1;
    //for (int col = 0; col < COL_HEIGHT; col++){
      strip.setPixelColor(newPixel, Wheel(map(peak,0,COL_HEIGHT - 1,30,150)));
      //strip.setPixelColor(newPixel - 1, Wheel(map(peak,0,COL_HEIGHT - 1,30,150)));
    //}
  }

   strip.show(); // Update strip

// Every few frames, make the peak pixel drop by 1:

    if(++dotCount >= PEAK_FALL) { //fall rate

      if(peak > 0) peak--;
      dotCount = 0;
    }
}

/////////// VU2 ENDS ////////////

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


int calculateIntensity() {
  int      intensity;

  reading   = analogRead(MIC_PIN);                        // Raw reading from mic
  reading   = abs(reading - 512 - DC_OFFSET); // Center on zero
  reading   = (reading <= NOISE) ? 0 : (reading - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + reading) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  intensity = DRAW_MAX * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  return constrain(intensity, 0, DRAW_MAX-1);
}

void updateOrigin(int intensity) {
  // detect peak change and save origin at curve vertex
  if (growing && intensity < last_intensity) {
    growing = false;
    intensity_max = last_intensity;
    fall_from_left = !fall_from_left;
    origin_at_flip = origin;
  } else if (intensity > last_intensity) {
    growing = true;
    origin_at_flip = origin;
  }
  last_intensity = intensity;

  // adjust origin if falling
  if (!growing) {
    if (fall_from_left) {
      origin = origin_at_flip + ((intensity_max - intensity) / 2);
    } else {
      origin = origin_at_flip - ((intensity_max - intensity) / 2);
    }
    // correct for origin out of bounds
    if (origin < 0) {
      origin = DRAW_MAX - abs(origin);
    } else if (origin > DRAW_MAX - 1) {
      origin = origin - DRAW_MAX - 1;
    }
  }
}

void assignDrawValues(int intensity) {
  // draw amplitue as 1/2 intensity both directions from origin
  int min_lit = origin - (intensity / 2);
  int max_lit = origin + (intensity / 2);
  if (min_lit < 0) {
    min_lit = min_lit + DRAW_MAX;
  }
  if (max_lit >= DRAW_MAX) {
    max_lit = max_lit - DRAW_MAX;
  }
  for (int i=0; i < DRAW_MAX; i++) {
    // if i is within origin +/- 1/2 intensity
    if (
      (min_lit < max_lit && min_lit < i && i < max_lit) // range is within bounds and i is within range
      || (min_lit > max_lit && (i > min_lit || i < max_lit)) // range wraps out of bounds and i is within that wrap
    ) {
      draw[i] = Wheel(scroll_color);
    } else {
      draw[i] = 0;
    }
  }
}

uint32_t * segmentAndResize(uint32_t* draw) {
  int seg_len = N_PIXELS / SEGMENTS;

  uint32_t segmented[N_PIXELS];
  for (int s = 0; s < SEGMENTS; s++) {
    for (int i = 0; i < seg_len; i++) {
      segmented[i + (s * seg_len) ] = draw[map(i, 0, seg_len, 0, DRAW_MAX)];
    }
  }

  return segmented;
}

void updateGlobals() {
  uint16_t minLvl, maxLvl;

  //advance color wheel
  color_wait_count++;
  if (color_wait_count > COLOR_WAIT_CYCLES) {
    color_wait_count = 0;
    scroll_color++;
    if (scroll_color > COLOR_MAX) {
      scroll_color = COLOR_MIN;
    }
  }

  vol[volCount] = reading;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(uint8_t i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < N_PIXELS) maxLvl = minLvl + N_PIXELS;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
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

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList qPatterns = {Vu, Vu1, Vu2, Vu3, Vu4, Vu5, Vu6, Vu7, Vu8, Vu9, Vu10, Vu11, Vu12, Vu13};
uint8_t qCurrentPatternNumber = 0; // Index number of which pattern is current

void nextPattern2()
{
  // add one to the current pattern number, and wrap around at the end
  qCurrentPatternNumber = (qCurrentPatternNumber + 1) % ARRAY_SIZE( qPatterns);
}
void All2()
{
  // Call the current pattern function once, updating the 'leds' array
  qPatterns[qCurrentPatternNumber]();
  EVERY_N_SECONDS( 30 ) { nextPattern2(); } // change patterns periodically
}
