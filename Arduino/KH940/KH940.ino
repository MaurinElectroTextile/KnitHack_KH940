/*
  BROTHER KH-940
  For explanations see the README
  )c( maurin.box@gmail.com
  I chosed to update the hardwear to https://github.com/AllYarnsAreBeautiful/ayab-hardware
*/

#include <Wire.h>
#define IIC_HARD
#include "solenoids.h"

Solenoids m_solenoids;

// HARDWARE CONST
#define BAUDRATE              38400    // vitesse du port serie
#define DATA                  200      // number of bytes per frame
#define SOLENOIDES            16       // number of solenoides

// #define DATA_PIN              A4       // I2C hardhare / Connected to the IO expander
// #define CLOCK_PIN             A5       // I2C hardhare / Connected to the IO expander

// HARDWARE INPUT SYSTEM
#define ENC_PIN_1             2        // encoder 1
#define ENC_PIN_2             3        // encoder 2
#define ENC_PIN_3             4        // phase encoder

#define END_PIN_R             0        // end Of Line Right for analog in
#define END_PIN_L             1        // end Of Line Left for analog in

// SOFTWARE CONST
#define THRESHOLD             400      // end lines sensors threshold
#define START_POS_L           13       // 
#define START_POS_R           239      //

#define HEADER                64       // 
#define FOOTER                255      //

byte serialData[DATA];

uint8_t pixelBin[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
boolean encState1 = false;               // encoder 1 state
boolean encState2 = false;               // encoder 2 state
boolean phaseEncoder = false;            // phase encoder state
boolean lastPhaseEncoder = false;        // last phase encoder state
int carDirection = 0;                    // direction of carriage　0:unknown　1:right　2:left
int stitch = 0;                          // the carriage stitch position
int pos = 0;                             // solenoid index position

boolean toggel_right = true;             // boolean to sens the RISING age of the phase encoder when the carriage going RIGHT
boolean toggel_left = true;              // boolean to sens the RISING age of the phase encoder when the carriage going LEFT
boolean toggel_start_pos_right = false;  // boolean to set stitch absolut position
boolean toggel_start_pos_left = false;   // boolean to set stitch absolut position

boolean DEBUG = false;                   // boolean for serial DEBUGING

////////////////////////////////////////////////////////////////////////////////
void setup() {

  Serial.begin(BAUDRATE);

  Wire.begin();

  pinMode(ENC_PIN_1, INPUT);
  pinMode(ENC_PIN_2, INPUT);
  pinMode(ENC_PIN_3, INPUT);

  for (int i = 0; i < 16; i++) {
    0xFFFF;
    // pinMode(solenoidsTemp[i], OUTPUT);     // Added shift out register
    // digitalWrite(solenoidsTemp[i], LOW);   // Added shift out register
  }

  attachInterrupt(ENC_PIN_1, rotaryEncoder, RISING);

}

////////////////////////////////////////////////////////////////////////////////
void loop() {

  //////////////////////
  // Start from the LEFT
  // Sens when the LEFT end ligne sensor is passed
  if ( carDirection == 1 && analogRead(END_PIN_L) >= THRESHOLD && toggel_left == true ) {
    toggel_left = false;
    stitch = START_POS_L; // set the stitch count to absolut position
    if (DEBUG) Serial.print(F("START_LEFT = ")), Serial.print(stitch), Serial.println();
  }

  // Start from the LEFT and read the RIGHT end lignes sensors
  // if the sensor is hited then ask to the computer to send the next array of values
  if ( carDirection == 1 && analogRead(END_PIN_R) >= THRESHOLD && toggel_right == true ) {
    toggel_right = false;
    if (DEBUG) Serial.print(F("STOP_RIGHT = ")), Serial.print(stitch), Serial.println();
    if (!DEBUG) Serial.write(HEADER);
  }

  //////////////////////
  // Sens when the RIGHT end ligne sensor is passed
  if ( carDirection == 2 && analogRead(END_PIN_R) >= THRESHOLD && toggel_right == false ) {
    toggel_right = true;
    stitch = START_POS_R; // set the stitch count to absolut position
    if (DEBUG) Serial.print(F("START_RIGHT = ")), Serial.print(stitch), Serial.println();
  }

  // Start from the RIGHT and read the LEFT end lignes sensors
  // if the sensor is hited then ask to the computer to send the next array of values
  if ( carDirection == 2 && analogRead(END_PIN_L) >= THRESHOLD && toggel_left == false ) {
    toggel_left = true;
    if (DEBUG) Serial.print(F("STOP_LEFT = ")), Serial.print(stitch), Serial.println();
    if (!DEBUG) Serial.write(HEADER);
  }
}

//////////////////////////////////////////////////////
void serialEvent() {
  byte inputValue = 0;
  int offset = 28;

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

//////////////////////////////////////////////////////
//
// This sensor is used to analyse the direction of the carriage
void rotaryEncoder() {

  lastPhaseEncoder = phaseEncoder;

  encState2 = digitalRead( ENC_PIN_2 );

  phaseEncoder = digitalRead( ENC_PIN_3 );

  // Serial.println(phaseEncoder);

  // If carriage go LEFT to RIHT
  if ( encState2 == false ) {
    carDirection = 1;
    out2();
  }
  // If carriage go RIGHT to LEFT
  else if ( encState2 == true ) {
    carDirection = 2;
    out1();
  }
}

//////////////////////////////////////////////////////
// Carriage go LEFT to RIHT
// Phase encoder is used to synchronise stitch count cycle
void out1() {

  stitch--; // decrease stitch count
  if ( pos < 15 ) pos++;
  if ( phaseEncoder != lastPhaseEncoder && lastPhaseEncoder == 0 ) {
    pos = 0;
  }
  // digitalWrite( solenoidsTemp[pos], pixelBin[stitch] ); // Added shift out register
  m_solenoids.setSolenoid( pos, pixelBin[stitch]);

  if ( DEBUG ) printOut();
}

//////////////////////////////////////////////////////
// Carriage go RIGHT to LEFT
// Phase encoder is used to synchronise stitch count cycle
void out2() {

  stitch++; // increase stitch count
  if ( pos > 0 ) pos--;
  if ( phaseEncoder != lastPhaseEncoder && lastPhaseEncoder == 0 ) {
    pos = 15;
  }
  // digitalWrite( solenoidsTemp[pos], pixelBin[stitch] ); // Added shift out register
  m_solenoids.setSolenoid( pos, pixelBin[stitch]);

  if ( DEBUG ) printOut();
}

// Print out DEBUGING values
void printOut() {
  Serial.print( carDirection );
  Serial.print( F(" ") );
  Serial.print( phaseEncoder );
  Serial.print( F(" ") );
  Serial.print( pos );
  Serial.print( F(" ") );
  Serial.println( stitch );
}
