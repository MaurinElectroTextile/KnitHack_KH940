# soundKnit V01
## Computer controlled knitting machine
This project explore how to hack knitting machines to use them as audio sound files textile printer.
The Arduino Firmware & Software is fuly compatible with the AYAB Aruino extantion board (shield).
You can purchase this Arduino extantion bord from our shop (comming soon) or the one from AYAB if it is still avalable.

![Picture](TODO)
- Git repository : https://github.com/etextile.org/soundKnit

## About the project
Since the 19th century, home knitting machines were a popular manufacturing tool which allowed knitters to produce rapidly knitted items to sell or for household crafts. The possibility to use jacquard patterns started with the use of punched hole cards and began to be computerised in the 80's. The immediate and low raw material resource needed to produce a knitted surface is what make knitting machines mobile and accessible for experimenting and prototyping. Since the late 80's these knitting machines have stopped been produced. These last years hackers and makers have started to open up knitting machines and found new ways to hack them by allowing the machines to be directly controlled by a modern computer. These hacked knitting machines become a sort of textile printer. Having a direct communication between digital tools and the knitting machine pushes the boundaries of traditional knitted patterns and gives us the opportunity to think of new way's in conceiving patterns. The Software translat sound in to visual patern and then send it to the Arduino to control the knitting machine.

## Project content
 - Hardware : we are using the [AYAB Arduino Shieldv1x](https://ayab-knitting.com/)
 - Firmware : Arduino inspired by the [KnitHack project](https://github.com/sokanno/KnitHack) 
 - Software : a PureData patch dedicated to translate sound into visual sound spectrum (fft)

## Arduino firmware
First of all you need to upload our Arduino soundKnit firmware to the Arduino extended with the AYAB shield.
Before flashing the Arduino firmware you will need to set up the THRESHOLD value by using this calibration sketch :
``` KnitHack_KH940/Arduino/Calibration/endLignes/endLignes.ino ```
This sketch will give you the endStop sensor values.
You will need to see the max value of the two endStop sensors and choose your THRESHOLD under this values.
The next step is to flash the firmware and perform the calibration :
``` KnitHack_KH940/Arduino/KH940/KH940.ino ```
These two variables allow you to set the knitter's stitches positions :

```
 #define START_POS_L value 
 #define START_POS_R value
```
h
## Softsware
### Pure Data
The soundKnit.pd file is working with [Pure Data Vanilla V5.0](http://msp.ucsd.edu/)
Pure Data is an open source software that offer an accessible visual programming environment.
This soundKnit.pd patch analyse the sound frequencies using FFT algorithm to generate the textile pattern.

https://packages.debian.org/buster/gem

### Step by step
- Start PureData software
- Open the PureData patch : /PureData/soundKnit.pd
- Plugin your Arduino with the AYAB extension
- Click the first toggle (CHECK_DEVICES) of the PD patch to print all available port in the PD window
- Look for the available serial ports on the Pd window
- [comport]: available serial ports: ...
- Select the corresponding serial port number with the radio GUI
- Start the serial connection by clicking the next toggle (PORT_ON_OFF)
- Load your sound file by clicking ...
- Load the pattern visualisation by clicking the next toggle (SIMULATOR)

### Start knitting
To start knitting, the carriage must be out of the sticheses on the left side.
Each time you make a row you must pass the end ligne sensor.
At the end of the row you will hear two bip.
A bip when you pass the end ligne sensor end a bip to tell you when all stitch's positions are seted.

## BROTHER KH-940 technicals specifications
- 200 steacheses
- Two end lines sensors
- One encoder wheel with two sensors mounted on it :
 - The first sensor is composed by two photointerrupter embedded into a single component (SHARP IA06). This double sensor is used to sens the direction of the carriage
 - The second sensor is a single photo interrupter used for the phase (SHARP - GP1A52LRJ00F)
- For more specifications you can read the [service manual](TODO)

### Processing
This simple sketch allow you to send any black and white image
# TODO
