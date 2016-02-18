#include <FastSPI_LED.h>

#include <time.h>;

const int r =  21;
const int g =  20;
const int b =  22;
char mode = '5';
char charByte;
unsigned char inByte;
long startTime;
typedef struct
{
  byte  x, y, z;
} coord;
static coord  v;
        

void setup()   {                
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  analogWrite(r, 0);
  analogWrite(g, 0);
  analogWrite(b, 0);
  Serial.begin(9600);
}

void loop()                     
{
  switch(mode){
    case '0':                    //LEDs OFF
        analogWrite(r, 0);
        analogWrite(g, 0);
        analogWrite(b, 0);
        getByte();
      break;
    case '1':                    //LEDs Solid Blue
        analogWrite(r, 0);
        analogWrite(g, 0);
        analogWrite(b, 255);
        getByte();
      break;
    case '2':                    //LEDs Solid Red
        analogWrite(r, 255);
        analogWrite(g, 0);
        analogWrite(b, 0);
        getByte();
      break;
    case '3':                    //LEDs Solid Green
        analogWrite(r, 0);
        analogWrite(g, 255);
        analogWrite(b, 0);
        getByte();
      break;  
    case '4':                    //LEDs Solid Purple
        analogWrite(r, 255);
        analogWrite(g, 0);
        analogWrite(b, 255);
        getByte();
      break;
    case '5':                    //LEDs Rainbow
        {
        #define  MIN_RGB_VALUE  10   // no smaller than 0. 
        #define  MAX_RGB_VALUE  255  // no bigger than 255.
        #define  TRANSITION_DELAY  70   // in milliseconds, between individual light changes
        #define  WAIT_DELAY        500  // in milliseconds, at the end of each traverse

        
        const coord vertex[] = 
        {
         //x  y  z      name
          {0, 0, 0}, // A or 0
          {0, 1, 0}, // B or 1
          {0, 1, 1}, // C or 2
          {0, 0, 1}, // D or 3
          {1, 0, 0}, // E or 4
          {1, 1, 0}, // F or 5
          {1, 1, 1}, // G or 6
          {1, 0, 1}  // H or 7
        };

        const byte path[] =
        {
          0x01, 0x23, 0x76, 0x54, 0x03, 0x21, 0x56, 0x74,  // trace the edges
          0x13, 0x64, 0x16, 0x02, 0x75, 0x24, 0x35, 0x17, 0x25, 0x70,  // do the diagonals
        };
        
        #define  MAX_PATH_SIZE  (sizeof(path)/sizeof(path[0]))  // size of the array
        
        while (mode == '5'){
            int    v1, v2=0;    // the new vertex and the previous one

            // initialise the place we start from as the first vertex in the array
            v.x = (vertex[v2].x ? MAX_RGB_VALUE : MIN_RGB_VALUE);
            v.y = (vertex[v2].y ? MAX_RGB_VALUE : MIN_RGB_VALUE);
            v.z = (vertex[v2].z ? MAX_RGB_VALUE : MIN_RGB_VALUE);
          
            // Now just loop through the path, traversing from one point to the next
            for (int i = 0; i < 2*MAX_PATH_SIZE; i++)
            {
            // !! loop index is double what the path index is as it is a nybble index !!
            v1 = v2;
            if (i&1)  // odd number is the second element and ...
              v2 = path[i>>1] & 0xf;  // ... the bottom nybble (index /2) or ...
            else      // ... even number is the first element and ...
              v2 = path[i>>1] >> 4;  // ... the top nybble
              
            traverse(vertex[v2].x-vertex[v1].x, 
                     vertex[v2].y-vertex[v1].y, 
                     vertex[v2].z-vertex[v1].z);
            getByte();
            }
        getByte();
        }
        break;
        }
    case '6':                    //LEDs White Pulse
        {
        int pwm = 21;
        int dir = 1;
        while (mode == '6'){
          analogWrite(r, pwm);
          analogWrite(g, pwm);
          analogWrite(b, pwm);
          if (pwm == 20){
            dir = 1;
            startTime = millis();
            while (millis() < startTime + 20 && mode =='6'){
              getByte();
            }
          }
          if (pwm == 255){
            dir = 0;
            startTime = millis();
            while (millis() < startTime + 20 && mode =='6'){
              getByte();
            }
          }
          if (dir == 1){
            pwm++;
          } else{
            pwm = pwm - 1;
          }
          startTime = millis();
          while (millis() < startTime + 40 && mode =='6'){
            getByte();
          }
          getByte();
        }
        break;
        }
    case '7':                    //LEDs Light Blue Pulse
        {
        float ratio = 0.2;
        int pwm2 = 21;
        float pwm1 = pwm2 * ratio;
        int dir = 1;
        while (mode == '7'){
          analogWrite(r, pwm1);
          analogWrite(g, pwm2);
          analogWrite(b, pwm2);
          if (pwm2 == 20){
            dir = 1;
            startTime = millis();
            while (millis() < startTime + 100 && mode =='7'){
              getByte();
            }
          }
          if (pwm2 == 255){
            dir = 0;
            while (millis() < startTime + 100 && mode =='7'){
              getByte();
            }
          }
          if (dir == 1){
            pwm2++;
            pwm1 = pwm2 * ratio;
          } else{
            pwm2 = pwm2 - 1;
            pwm1 = pwm2 * ratio;
          }
          startTime = millis();
          while (millis() < startTime + 40 && mode =='7'){
            getByte();
          }
        }
        getByte();
        break;
        }
    default:                    //LEDs OFF
        analogWrite(r, 0);
        analogWrite(g, 0);
        analogWrite(b, 0);
        getByte();
      break;
  }
}

void traverse(int dx, int dy, int dz)
{
  if ((dx == 0) && (dy == 0) && (dz == 0))   // no point looping if we are staying in the same spot!
    return;
    
  for (int i = 0; i < MAX_RGB_VALUE-MIN_RGB_VALUE; i++, v.x += dx, v.y += dy, v.z += dz)
  {
    // set the colour in the LED
    analogWrite(r, v.x);
    analogWrite(g, v.y);
    analogWrite(b, v.z);
    
    startTime = millis();
    while (millis() < startTime + TRANSITION_DELAY && mode == '5'){   // wait fot the transition delay
      getByte();
    }
    if (mode != '5'){
      break;
    }
  }

  startTime = millis();
  while (millis() < startTime + WAIT_DELAY && mode =='5'){           // give it an extra rest at the end of the traverse
      getByte();
  }      
}


void getByte(){
  if (Serial.available() > 0){
    inByte = Serial.read();
    charByte = char(inByte);
    Serial.println(charByte);
    mode = charByte;
    if (mode == 'c'){
      getColors();
    }
  }
}

void getColors(){
  //while (true){
          
  //}
}
