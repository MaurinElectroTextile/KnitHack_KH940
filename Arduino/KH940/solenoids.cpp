/*
  This file is part of AYAB.

    AYAB is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AYAB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AYAB.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2013 Christian Obersteiner, Andreas Müller
    https://bitbucket.org/chris007de/ayab-apparat/
*/

#include "Arduino.h"
#include "solenoids.h"

// Regular Arduino
#include <Wire.h>
#define IIC_HARD

Solenoids::Solenoids() {
  solenoidState = 0x00;
#ifdef IIC_HARD
  Wire.begin();
#endif //IIC_HARD
}


void Solenoids::setSolenoid( byte solenoid, bool state ) {

  if ( solenoid >= 0 && solenoid <= 15 ) {
    if ( state ) {
      bitSet(solenoidState, solenoid);
    }
    else {
      bitClear(solenoidState, solenoid);
    }
    // TODO optimize to act only when there is an actual change of state
    write(solenoidState);
  }
}


void Solenoids::setSolenoids( uint16_t state ) {
  solenoidState = state;
  write( state );
}


/*
   Private Methods
*/


/*
   Writes to the I2C port expanders
   Low level function, mapping to actual wiring
   is done here.
*/
void Solenoids::write( uint16_t newState ) {

  Wire.beginTransmission( I2Caddr_sol1_8 );
#ifdef IIC_HARD
  Wire.write( lowByte(newState) );
#else
  Wire.send( lowByte(newState) );
#endif //IIC_HARD
  Wire.endTransmission();

  Wire.beginTransmission( I2Caddr_sol9_16);
#ifdef IIC_HARD
  Wire.write( highByte(newState) );
#else
  Wire.send( highByte(newState) );
#endif //IIC_HARD
  Wire.endTransmission();
}
