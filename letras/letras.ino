
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
Adafruit_NeoPixel tira = Adafruit_NeoPixel(nleds, 3, NEO_GRB + NEO_KHZ800); // Pin 3 de Arduino salida de datos

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

// 28 representa la cantidad maxima de leds que puede ocupar un 'caracter'
// representado por 4 leds horizontales y 7 verticales.
byte letras [][4][7] = {
  {
    {2, 3, 4, 5, 6, 7},
    {1, 5},
    {1, 5},
    {2, 3, 4, 5, 6, 7}
  }, // A
  {}, //B
  {
    {2, 3, 4, 5, 6},
    {1, 7},
    {1, 7},
    {1, 7}
  }, //C
  {
    {1, 2, 3, 4, 5, 6, 7},
    {1, 7},
    {2, 3, 4, 5, 6},
    {}
  }, // D
  {}, //E
  {}, //F
  {
    {1, 2, 3, 4, 5, 6, 7},
    {1, 7},
    {1, 4, 5, 7},
    {1, 4, 5, 6, 7}
  }, //G
  {}, //H
  {
    {1, 7},
    {1, 2, 3, 4, 5, 6, 7},
    {1, 7},
    {}
  }, // I
  {}, //J
  {}, //K
  {}, //L
  {}, //M
  {}, //N
  {
    {2, 3, 4, 5, 6, 6},
    {1, 7},
    {1, 7},
    {2, 3, 4, 5, 6, 6}
  }, // O
  {}, //P
  {}, //Q
  {
    {1, 2, 3, 4, 5, 6, 7},
    {1, 4},
    {1, 3, 4, 5},
    {1, 2, 3, 6, 7}
  }, //R
  {
    {2, 3, 4, 7},
    {1, 4, 7},
    {1, 4, 7},
    {1, 4, 5, 6}
  }, // S
  {}, //T
  {}, //U
  {}, //V
  {}, //W
  {}, //X
  {}, //Y
  {}, //Z
  {
    {},
    {1,2,3,4,5,7},
    {},
    {},
  }, // !
  };

char mensaje[] = "GRACIAS!!";
String mensajeVertical = "ADIOS!";

// Los caracteres son de 4 x 7
void encenderCoracter(int c, int offset) {
  for ( int x = 0; x < 4; ++x ) {
    if (offset >= cols || offset < 0) {
      return;
    }
    for ( int y = 0; y < 7; ++y ) {  
      if (letras[c][x][y] != '\0') {
        int ledN = ((letras[c][x][y] - 1) * cols) + offset;
        encenderPixel(ledN, x);
      }
    }
    offset ++;
  }
}

void encenderCaracterVertical(int c, int offset, int vOffset) {
  for ( int x = 0; x < 4; ++x ) {
    if (offset >= cols || offset < 0) {
      return;
    }
    for ( int y = 0; y < 7; ++y ) {  
      if (letras[c][x][y] != '\0') {
        int ledN = ((letras[c][x][y] - 1) * cols) + offset + vOffset;
        encenderPixel(ledN, x);
      }
    }
    offset ++;
  }
}


void setup() {
  Serial.begin(9600);
  tira.begin();       // inicializacion de la tira
  tira.show();        // muestra datos en pixel
  tira.setBrightness(brillo); // Brillo de los LEDS , más alto mas brillo ideal 15
}

void loop() {

//  encenderPixel(255, 0);
//  tira.show();
  int mensajeSize = strlen(mensaje);
//  Serial.print("Mensaje Size: ");
//  Serial.print(mensaje);
//  Serial.print(" | ");
//  Serial.println(mensajeSize);
  int espacio = 5;
  for (int i=16; i>= -(mensajeSize * espacio); i--) {
    for(int cpos = 0; cpos<mensajeSize; cpos++){
      char caracter = mensaje[cpos];
//      Serial.print("Caracter: ");
//      Serial.println(caracter);
//  Serial.print(" | ");
//  Serial.println(mensajeSize);
      encenderCoracter(getCaracterFromIndex(caracter), i + (cpos * espacio));
    }
    tira.show();
    delay(100);
    apagarLeds();
  }  
//
  mensajeSize = sizeof(mensajeVertical);
//  Serial.print("Mensaje Size: ");
//  Serial.print(mensajeVertical);
//  Serial.print(" | ");
//  Serial.println(mensajeSize);
  espacio = 8;

  for (int i=16; i>= -(mensajeSize * espacio); i--) {
    for(int cpos = 0; cpos<mensajeSize; cpos++){
      char caracter = mensajeVertical.charAt(cpos);
      encenderCaracterVertical(getCaracterFromIndex(caracter), 6, (i * cols) + (cpos * espacio * cols));
    }
    tira.show();
    delay(100);
    apagarLeds();
  }
}

int getCaracterFromIndex(char caracter) {
  switch (caracter) {
    case 'A':
      return 0;
      break;
    case 'C':
      return 2;
      break;
    case 'D':
      return 3;
      break;
    case 'G':
      return 6;
      break;
    case 'I':
      return 8;
      break;
    case 'O':
      return 14;
      break;
    case 'R':
      return 17;
      break;
    case 'S':
      return 18;
      break;
    case '!':
      return 26;
      break;
    default:
      return;
      break;
  }
}

void encenderColumna(int c, int offset) {
  if (offset < 16) {
    for ( int i = 0; i < 28; ++i ) {
      if (letras[c][i] != '\0') {
        encenderPixel(letras[c][i] + offset, getColorCodeRojo());
      }
    }
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
