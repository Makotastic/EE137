#include <Arduino.h>
#include <Fonts/FreeMono18pt7b.h>
#include "Adafruit_SSD1306.h"

//Set up base functionality
int analogReadPin = A0;
int ANALOG_THREASHOLD = 700; //0-1023
long freqCount = 0;
int data_pin = 4;
int prev_data = 0;
unsigned long samplingStopTime;
unsigned long samplingTimeInMircos = 2000000;
unsigned long MircoToSecFactor = 1000000;
bool isFalling = false;
float freq = 0;
float v = 0; // speed of object
float wl = 0.0285; // 3e8m/s / 10.525GHz 
float avg = 0; // avg speed
int samples = 1; // number of iterations
bool needToReset = false;

//System Integration Setup
int turn_on_pin = D7; // Interrupt pin for button control
bool state = false; // Changeable in ISR if button is pressed
//init display stuff
float disp_speed; //speed to be displayed in mph

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void analogSampleFreq(){
  int data = analogRead(analogReadPin);
  if(data > ANALOG_THREASHOLD)
  {
    if(!isFalling)
    {
      isFalling = true;
      freqCount = freqCount + 1;
    }
    
  }
  else if(data <= ANALOG_THREASHOLD)
  {
    if(isFalling)
    {
      isFalling = false;
    }
  }
}

float calcFreq(){
  freqCount = 0;
  samplingStopTime = micros() + samplingTimeInMircos;
  while (micros() < samplingStopTime)
  {
    analogSampleFreq();
    Serial.println(freqCount);
  }
  return MircoToSecFactor * ((float)freqCount / samplingTimeInMircos);
}

void buttonOn() {
  state = true;
  Serial.println("Button On");
}

void buttonOff() {
  state = false;
  Serial.println("Button Off");
}

void setup() {
  Serial.begin(115200);
  /* while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setFont(&FreeMono18pt7b);
  delay(4000);
  Serial.println("Starting...");
  pinMode(turn_on_pin ,INPUT_PULLUP);
}

void loop() {
  if(digitalRead(D7) == 1)
  {
    state = true;
    //Serial.println("Button On");
  }
  else if(digitalRead(D7) == 0)
  {
    state = false;
    needToReset = false;
    //Serial.println("Button off");
  }
  if(state && !needToReset)
  {
    freq = calcFreq();
    v = freq * wl / 2;
    v = 2.237*v;
    if(samples % 1 == 0)
    {
    Serial.print("Frequency: ");
    Serial.println(freq);
    Serial.print("Speed: ");
    Serial.println(v);
    Serial.print("\n\n\n");
    }
    samples = samples + 1;
    display.clearDisplay();
    display.setCursor(20,20);
    display.println(v);
    display.print("MPH");
    display.display();
    needToReset = true;
  }

}






