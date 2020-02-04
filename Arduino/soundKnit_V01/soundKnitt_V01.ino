/*
  BROTHER KH-940
  For explanations see the README
  )c( maurin.box@gmail.com
  I chosed to update the hardwear to https://github.com/AllYarnsAreBeautiful/ayab-hardware
*/
#include <Wire.h>

// HARDWARE CONST
#define BAUDRATE              38400    // vitesse du port serie
#define DATA                  200      // number of bytes per frame
#define SOLENOIDES            16       // number of solenoides

// #define DATA_PIN           A4       // I2C hardhare / Connected to the IO expander
// #define CLOCK_PIN          A5       // I2C hardhare / Connected to the IO expander

// HARDWARE INPUT SYSTEM
#define ENC_PIN_1             2        // encoder 1 - interrupt driven 
#define ENC_PIN_2             3        // encoder 2
#define ENC_PIN_3             4        // phase encoder

#define END_PIN_R             0        // end Of Line Right for analog in
#define END_PIN_L             1        // end Of Line Left for analog in

// SOFTWARE CONST
#define THRESHOLD             400      // end lines sensors threshold
#define START_POS_L           12       // 12
#define START_POS_R           239      // 239

#define HEADER                64       // 
#define FOOTER                255      //

#define I2Caddr_sol1_8        0x20     // IO expander chip addres 
#define I2Caddr_sol9_16       0x21     // IO expander chip addres 

byte serialData[DATA];

uint8_t pixelBin[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 16
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

int byte_index = 0;                      // index for incomming serial datas
int offset = 28; // serialData[256];


boolean encState1 = false;               // encoder 1 state
boolean encState2 = false;               // encoder 2 state
boolean phaseEncoder = false;            // phase encoder state
boolean lastPhaseEncoder = false;        // last phase encoder state
int carDirection = 0;                    // direction of carriage　0:unknown　1:right　2:left
int stitch = 0;                          // the carriage stitch position

uint8_t pos = 0;                         // solenoid index position

boolean toggel_right = true;             // boolean to sens the RISING age of the phase encoder when the carriage going RIGHT
boolean toggel_left = true;              // boolean to sens the RISING age of the phase encoder when the carriage going LEFT
boolean toggel_start_pos_right = false;  // boolean to set stitch absolut position
boolean toggel_start_pos_left = false;   // boolean to set stitch absolut position

uint16_t solenoids = 0;
boolean ISR_flag = false;                //
boolean DEBUG = false;                    // boolean for serial DEBUGING

////////////////////////////////////////////////////////////////////////////////
void setup() {

  Wire.begin();

  Serial.begin(BAUDRATE);

  pinMode(ENC_PIN_1, INPUT);
  pinMode(ENC_PIN_2, INPUT);
  pinMode(ENC_PIN_3, INPUT);

  attachInterrupt(0, setISR_flag, RISING); // Interrupt 0 is associated to digital pin 2
}

////////////////////////////////////////////////////////////////////////////////
void loop() {

  //////////////////////
  // Start from the LEFT
  // Sens when the LEFT end ligne sensor is passed
  if (carDirection == 1 && analogRead(END_PIN_L) >= THRESHOLD && toggel_left == true) {
    toggel_left = false;
    stitch = START_POS_L; // set the stitch count to absolut position
    if (DEBUG) Serial.print(F("START_LEFT = ")), Serial.print(stitch), Serial.println();
  }

  // Start from the LEFT and read the RIGHT end lignes sensors
  // if the sensor is hited then ask to the computer to send the next array of values
  if (carDirection == 1 && analogRead(END_PIN_R) <= THRESHOLD && toggel_right == true) {
    toggel_right = false;
    if (DEBUG) Serial.print(F("STOP_RIGHT = ")), Serial.print(stitch), Serial.println();
    if (!DEBUG) Serial.write(HEADER);
  }

  //////////////////////
  // Sens when the RIGHT end ligne sensor is passed
  if (carDirection == 2 && analogRead(END_PIN_R) <= THRESHOLD && toggel_right == false) {
    toggel_right = true;
    stitch = START_POS_R; // set the stitch count to absolut position
    if (DEBUG) Serial.print(F("START_RIGHT = ")), Serial.print(stitch), Serial.println();
  }

  // Start from the RIGHT and read the LEFT end lignes sensors
  // if the sensor is hited then ask to the computer to send the next array of values
  if (carDirection == 2 && analogRead(END_PIN_L) >= THRESHOLD && toggel_left == false) {
    toggel_left = true;
    if (DEBUG) Serial.print(F("STOP_LEFT = ")), Serial.print(stitch), Serial.println();
    if (!DEBUG) Serial.write(HEADER);
  }

  if (ISR_flag) {
    rotaryEncoder();
  }
}

//////////////////////////////////////////////////////
void serialEvent() {
  byte inputValue = 0;

  if (Serial.available() > 0) {
    inputValue = Serial.read();

    if (inputValue != FOOTER) {
      serialData[byte_index] = inputValue;
      byte_index++;
    }
    else {
      for (int i = 0; i < DATA; i++) {
        pixelBin[i + offset] = serialData[i];
      }
      byte_index = 0;
    }
  }
}

void setISR_flag() {
  ISR_flag = true;
}

//////////////////////////////////////////////////////
// This sensor is used to analyse the direction of the carriage
void rotaryEncoder() {

  lastPhaseEncoder = phaseEncoder;
  encState2 = digitalRead(ENC_PIN_2);
  phaseEncoder = digitalRead(ENC_PIN_3);

  if (!encState2) { // If carriage go RIGHT to LEFT
    carDirection = 2;
    out1();
  }
  else if (encState2) { // If carriage go LEFT to RIHT
    carDirection = 1;
    out2();
  }
}

//////////////////////////////////////////////////////
// Carriage go LEFT to RIHT
// Phase encoder is used to synchronise stitch count cycle
inline void out1() {

  stitch--; // decrease stitch count
  if (stitch < 0) stitch = 0;
  
  if (pos < 15) pos++;
  if (phaseEncoder != lastPhaseEncoder && lastPhaseEncoder == 0) {
    pos = 0;
  }
  if (pixelBin[stitch] == 1) {
    bitSet(solenoids, pos);
  }
  else {
    bitClear(solenoids, pos);
  }
  writeSolenoids(solenoids);
  if (DEBUG) printOut();
}

//////////////////////////////////////////////////////
// Carriage go RIGHT to LEFT
// Phase encoder is used to synchronise stitch count cycle
inline void out2() {

  stitch++; // increase stitch count
  if (stitch > 255) stitch = 255;

  if (pos > 0) pos--;
  if (phaseEncoder != lastPhaseEncoder && lastPhaseEncoder == 0) {
    pos = 15;
  }
  if (pixelBin[stitch] == 1) {
    bitSet(solenoids, pos);
  }
  else {
    bitClear(solenoids, pos);
  }
  writeSolenoids(solenoids);
  if (DEBUG) printOut();
}

inline void writeSolenoids(uint16_t newState) {
  Wire.beginTransmission(I2Caddr_sol1_8);
  Wire.write(lowByte(newState));
  Wire.endTransmission();
  Wire.beginTransmission(I2Caddr_sol9_16);
  Wire.write(highByte(newState));
  Wire.endTransmission();
  ISR_flag = false;
}

// Print out DEBUGING values
void printOut() {
  Serial.print(carDirection);
  Serial.print(F(" "));
  Serial.print(phaseEncoder);
  Serial.print(F(" "));
  Serial.print(pos);
  Serial.print(F("\t"));
  Serial.println(stitch);
}
