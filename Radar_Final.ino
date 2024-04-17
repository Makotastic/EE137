//Set up base functionality
int data_pin = A0;
int freq = 0;
float v = 0; // speed of object
float wl = 0.0285; // 3e8m/s / 10.525GHz 
float avg = 0; // avg speed
int samples = 1; // number of iterations
//System Integration Setup
int turn_on_pin = 7; // Interrupt pin for button control
bool state = true; // Changeable in ISR if button is pressed
//init display stuff
float disp_speed; //speed to be displayed in mph



void setup() {
  Serial.begin(115200);
  pinMode(turn_on_pin ,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(7),buttonOn, RISING);
  attachInterrupt(digitalPinToInterrupt(7),buttonOff, FALLING);
}

void loop() {
  disp_speed = speed();

}

void buttonOn() {
  state = true;
}

void buttonOff() {
  state = false;
}


float speed() {
  while (state) {
    freq = analogRead(data_pin);
    //need to convert this into an actual frequency (in Hz)
    v = freq * wl / 2; // this is now in m/s
    v = 2.237*v; // this is conversion to mph
    avg = (avg*(samples-1) + v)/samples;
    Serial.print("Frequency: ");
    Serial.println(freq);
    Serial.print("Speed: ");
    Serial.println(v);
    Serial.print("Average: ");
    Serial.println(avg);
    samples = samples + 1;
  }
  return avg;
  
}

