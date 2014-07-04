/*

BROTHER KH-940
200 steacheses

Two end lines sensors
One encoder wille with two sensors mounted on it :
  - The first sensor is composed by two photointerrupter embedded into a single component (SHARP IA06).
    This double sensor is used to sens the direction ot the carriage
  - The second sensor is a single photo interrupter used for the phase (SHARP - GP1A52LRJ00F)

To use this programme you must start to knit from thé left side of thé knitting machine.
The carriage must be out of the sticheses
This code is a fork from Sokanno KnitHack project : https://github.com/sokanno/KnitHack
My web page : http://maurin.donneaud.free.fr/
The service manual : http://maurin.box.free.fr/_DOCs/brother_kh930_knitting_machine_service_manual.pdf
*/

// HARDWARE CONST
#define BAUDRATE              57600    // vitesse du port serie
#define DATA                  200      // number of bytes per frame
#define SOLENOIDES            16       // number of solenoides

// HARDWARE INPUT SYSTEM
#define ENC_PIN_1             2        // encoder 1
#define ENC_PIN_2             3        // encoder 2
#define ENC_PIN_3             4        // phase encoder

#define END_PIN_R             0        // endLineRight for analog in
#define END_PIN_L             1        // endLineLeft for analog in

// SOFTWARE CONST
#define THRESHOLD             500      // end lines sensors threshold
#define START_POS_L           13       // 
#define START_POS_R           239      //

#define HEADER                255      // 
#define FOOTER                255      //

byte serialData[DATA];

boolean pixelBin[256] = {
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

// OUTPUT SYSTEM
const int solenoidsTemp[SOLENOIDES] = {
  22, 24, 26, 28, 30, 32, 34, 36, 37, 35, 33, 31, 29, 27, 25, 23
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

  pinMode(ENC_PIN_1, INPUT);
  pinMode(ENC_PIN_2, INPUT);
  pinMode(ENC_PIN_3, INPUT);

  for (int i = 0; i < 16; i++) {
    pinMode(solenoidsTemp[i], OUTPUT);
    digitalWrite(solenoidsTemp[i], LOW);
  }

  attachInterrupt(ENC_PIN_1, rotaryEncoder, RISING);

}

////////////////////////////////////////////////////////////////////////////////
void loop() {

  /////////////////////////////////////////////////////////
  // Start from the LEFT
  // Sens when the LEFT end ligne sensor is passed
  if ( carDirection == 1 && analogRead(END_PIN_L) >= THRESHOLD && toggel_left == true ) {
    toggel_left = false;
    stitch = START_POS_L; // set the stitch count to absolut position
    if (DEBUG) Serial.print(F("START_LEFT = ")), Serial.print(stitch), Serial.println();
    if (!DEBUG) Serial.write(64);
  }

  // Start from the LEFT and read the RIGHT end lignes sensors
  // if the sensor is hited then ask to the computer to send the next array of values
  if ( carDirection == 1 && analogRead(END_PIN_R) >= THRESHOLD && toggel_right == true ) {
    toggel_right = false;
    if (DEBUG) Serial.print(F("STOP_RIGHT = ")), Serial.print(stitch), Serial.println();
    if (!DEBUG) Serial.write(HEADER);
  }

  /////////////////////////////////////////////////////////
  // Sens when the RIGHT end ligne sensor is passed
  if ( carDirection == 2 && analogRead(END_PIN_R) >= THRESHOLD && toggel_right == false ) {
    toggel_right = true;
    stitch = START_POS_R; // set the stitch count to absolut position
    if (DEBUG) Serial.print(F("START_RIGHT = ")), Serial.print(stitch), Serial.println();
    if (!DEBUG) Serial.write(64);
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
  digitalWrite( solenoidsTemp[ pos ], pixelBin[ stitch ] );
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
  digitalWrite( solenoidsTemp[ pos ], pixelBin[ stitch ] );
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
