
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
int cols=16;
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
// 33 representa la cantidad maxima de leds que puede ocupar un 'caracter'
// representado por 3 leds horizontales y 11 verticales.
byte numeros[][33] = {
  {16, 17, 18, 32, 34, 48, 50, 64, 66, 80, 82, 96, 98, 112, 114, 128, 130, 144, 146, 160, 162, 176, 177, 178}, // 0
  {18, 33, 34, 48, 50, 66, 82, 98, 114, 130, 146, 162, 178}, // 1
  {16, 17, 18, 34, 50, 66, 82, 96, 97, 98, 112, 128, 144, 160, 176, 177, 178},
  {16, 17, 18, 34, 50, 66, 81, 82, 97, 98, 113, 114, 130, 146, 162, 176, 177, 178},
  {16, 18, 32, 34, 48, 50, 64, 66, 80, 82, 96, 97, 98, 114, 130, 146, 162, 178}, //4
  {16, 17, 18, 32, 48, 64, 80, 96, 97, 98, 114, 130, 146, 162, 176, 177, 178}, //5
  {16, 17, 18, 32, 48, 64, 80, 96, 97, 98, 112, 114, 128, 130, 144, 146, 160, 162, 176, 177, 178}, //6
  {16, 17, 18, 34, 50, 66, 82, 98, 114, 130, 146, 162, 178}, //7
  {16, 17, 18, 32, 34, 48, 50, 64, 66, 80, 81, 82, 96, 97, 98, 112, 113, 114, 128, 130, 144, 146, 160, 162, 176, 177, 178}, //8
  {16, 17, 18, 32, 34, 48, 50, 64, 66, 80, 81, 82, 98, 114, 130, 146, 162, 178} //9
};

int letraAncho = 3;
int espacioEntreNumeros = 1;

Adafruit_NeoPixel tira = Adafruit_NeoPixel(nleds, 3, NEO_GRB + NEO_KHZ800); // Pin 3 de Arduino salida de datos

void setup() {
  //Serial.begin(9600);
  tira.begin();       // inicializacion de la tira
  tira.show();        // muestra datos en pixel
  tira.setBrightness(brillo); // Brillo de los LEDS , más alto mas brillo ideal 15
  //encenderColumna(0,13);
}

int getColorCodeRojo() {
  return 3;
}

void encenderPixel(int pixel, int colorCode) {
  tira.setPixelColor(pixel, colors[colorCode][0],colors[colorCode][1],colors[colorCode][2]);
}

void encenderColumna(int c, int offset) {
  int numSize = sizeof(numeros[c]);
  for ( int i = 0; i < numSize; ++i ) {
    if (numeros[c][i] == '\0') {
      break;
    }
    encenderPixel(numeros[c][i] + offset, getColorCodeRojo());
  }
}

void apagarLeds() {
 for (int i=0; i<nleds; i++) {
    tira.setPixelColor(i,0,0,0);
 }
}

void loop() {
  for (int c = 1; c< 90; ++c) { //90 secs
      encenderColumna(c%10, (cols - letraAncho));
      encenderColumna(((c/10)%10), (cols - (2 * letraAncho) - espacioEntreNumeros));
      tira.show();
      delay(950);
      apagarLeds();
  }
}
