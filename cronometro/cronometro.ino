
/* Imagenes pantalla 16x16 LEDs WS2812B
by: www.elprofegarcia.com
compra de componentes en https://dinastiatecnologica.com
Pin 3 de Arduino salida de datos
/

*/
#include <Adafruit_NeoPixel.h>    // importa libreria (debe instalarla antes)


int brillo = 10; //  Brillo de los LEDS , más alto mas brillo ideal 10
int nleds=256;  // num de LEDs
int retardo=2000; // Tiempo entre imagenes de 2 segundos
int rows=16;
int colors[][3] = {
  {255,0,255}, // magenta
  {0,0,255}, //blue
  {0,255,255}, //cyan
  {255,0,0}, //red
  {255,127,80}, //orange
  {100,40,40}, //dark brown
  {0,255,0}, //green
  {120,255,120}, //fluor green,
  {255,255,0}, //yellow
  {210,105,30}, //brown
  {0,0,0}, //balck,
  {255,255,255} //white
};
int numCero[] = {16, 17, 18, 32, 34, 48, 50, 64, 66, 80, 82, 96, 98, 112, 114, 128, 130, 144, 146, 160, 162, 176, 177, 178};
int numUno[] = {18, 33, 34, 48, 50, 66, 82, 98, 114, 130, 146, 162, 178};
int numDos[] = {16, 17, 18, 34, 50, 66, 82, 96, 97, 98, 112, 128, 144, 160, 176, 177, 178};
int numTres[] = {16, 17, 18, 34, 50, 66, 81, 82, 97, 98, 113, 114, 130, 146, 162, 176, 177, 178};
int numCuatro[] = {16, 18, 32, 34, 48, 50, 64, 66, 80, 82, 96, 97, 98, 114, 130, 146, 162, 178};
int numCinco[] = {16, 17, 18, 32, 48, 64, 80, 96, 97, 98, 114, 130, 146, 162, 176, 177, 178};
int numSeis[] = {16, 17, 18, 32, 48, 64, 80, 96, 97, 98, 112, 114, 128, 130, 144, 146, 160, 162, 176, 177, 178};
int numSiete[] = {16, 17, 18, 34, 50, 66, 82, 98, 114, 130, 146, 162, 178};
int numOcho[] = {16, 17, 18, 32, 34, 48, 50, 64, 66, 80, 81, 82, 96, 97, 98, 112, 113, 114, 128, 130, 144, 146, 160, 162, 176, 177, 178};
int numNueve[] = {16, 17, 18, 32, 34, 48, 50, 64, 66, 80, 81, 82, 98, 114, 130, 146, 162, 178};
int separadorMas[] = {215, 230, 231, 232, 247};
int separadorDash[] = {230, 231, 232};
int separadorBackSlash[] = {214, 231, 248};
int separadorPipe[] = {215, 231, 247};
int separadorSlash[] = {216, 231, 246};
int separadorX[] = {214, 216, 231, 246, 248};

Adafruit_NeoPixel tira = Adafruit_NeoPixel(nleds, 3, NEO_GRB + NEO_KHZ800); // Pin 3 de Arduino salida de datos

void setup() {
  tira.begin();       // inicializacion de la tira
  tira.show();        // muestra datos en pixel
  tira.setBrightness(brillo); // Brillo de los LEDS , más alto mas brillo ideal 15
}

void loop() {
  for (int c = 1; c< 90; ++c) { //90 secs
      int arraySize = 0;
      encenderColumna(c%10, 13);
      encenderColumna(((c/10)%10), 9);
      tira.show();
      delay(950);
      apagarLeds();
  }
}

void encenderColumna(int c, int offset) {
  int numSize = 0;
  switch (c) {
        case 1:
          numSize = sizeof(numUno) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numUno[i] + offset, getColorCodeRojo());
          }
          break;
        case 2:
          numSize = sizeof(numDos) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numDos[i] + offset, getColorCodeRojo());
          }
          break;
        case 3:
          numSize = sizeof(numTres) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
          encenderPixel(numTres[i] + offset, getColorCodeRojo());
          }
          break;
        case 4:
          numSize = sizeof(numCuatro) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numCuatro[i] + offset, getColorCodeRojo());
          }
          break;
        case 5:
          numSize = sizeof(numCinco) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numCinco[i] + offset, getColorCodeRojo());
          }
          break;
        case 6:
          numSize = sizeof(numSeis) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numSeis[i] + offset, getColorCodeRojo());
          }
          break;
        case 7:
          numSize = sizeof(numSiete) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numSiete[i] + offset, getColorCodeRojo());
          }
          break;
        case 8:
          numSize = sizeof(numOcho) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numOcho[i] + offset, getColorCodeRojo());
          }
          break;
        case 9:
          numSize = sizeof(numNueve) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numNueve[i] + offset, getColorCodeRojo());
          }
          break;
        case 0:
          numSize = sizeof(numCero) / sizeof(int);
          for ( int i = 0; i < numSize; ++i ) {
            encenderPixel(numCero[i] + offset, getColorCodeRojo());
          }
          break;
        default:
          // statements
          break;  
      }
}

void encenderPixel(int pixel, int colorCode) {
  tira.setPixelColor(pixel,colors[colorCode][0],colors[colorCode][1],colors[colorCode][2]);
}

void apagarLeds() {
 for (int i=0; i<nleds; i++) {
    tira.setPixelColor(i,0,0,0); 
 }
}

int getColorCodeRojo() {
  return 3;
}
