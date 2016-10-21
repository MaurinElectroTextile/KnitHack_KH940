#KnitHack KH940

![alt tag](https://farm9.staticflickr.com/8830/18264782205_2756bc8632_z_d.jpg)

- This project is part of the e-textile Summec camp 2014 : http://etextile-summercamp.org/2014
- The Arduino firmware is a fork of Sokanno KnitHack project : https://github.com/sokanno/KnitHack
- The Git repository : https://github.com/MaurinElectroTextile/KnitHack_KH940

##About The project
"Since the 19th century, home knitting machines were a popular manufacturing tool which allowed knitters to produce rapidly knitted items to sell or for household crafts.
The possibility to use jacquard patterns started with the use of punched hole cards and began to be computerised in the 80's.
The immediate and low raw material resource needed to produce a knitted surface is what make knitting machines mobile and accessible for experimenting and prototyping.
Since the late 80's these knitting machines have stopped been produced.
These last years hackers and makers have started to open up knitting machines and found new ways to hack them by allowing the machines to be controlled directly by a modern computer.
These hacked knitting machines become a sort of textile printer.
Having a direct communication between digital tools and the knitting machine pushes the boundaries of traditional knitted patterns and gives us the opportunity to think of new way's in conceiving patterns.
In this workshop we will explore how our knitting machine can become a way of printing out (generated) data on a textile surface.
First we will show how the monitoring system and the arduino controls the knitting machine as well as how the mechanics of the machine works in general.
We will then experiment hands on with field recordings of our surrounding environment.
Our sound cartography’s will then be translated and visualised with Pure Data and then sent to the machine to be knitted out.
These patterns can be read as a visual translation of rhythms, silences and noises embedded in a palpable knitted surface.
[Claire Williams](http://xxxclairewilliamsxxx.wordpress.com/workshops/)

##Project content
 - Arduino firmware
 - Eagle PCB : I use the [AYAB Arduino shield](https://bitbucket.org/chris007de/ayab-apparat/src/f6e368fbe0a6?at=master)
 - Puredata software
 - Processing software

##BROTHER KH-940 technicals specifications
- 200 steacheses
- Two end lines sensors
- One encoder wille with two sensors mounted on it :
 - The first sensor is composed by two photointerrupter embedded into a single component (SHARP IA06). This double sensor is used to sens the direction of the carriage
 - The second sensor is a single photo interrupter used for the phase (SHARP - GP1A52LRJ00F)
- For more specifications you can see the [service manual]( http://maurin.box.free.fr/_DOCs/brother_kh930_knitting_machine_service_manual.pdf)

##Arduino firmware
Before to flash the Arduino firmware you will need to set up the THRESHOLD value by using this calibration sketch :
``̀
KnitHack_KH940/Arduino/Calibration/endLignes/endLignes.ino
```
This sketch will give you the endStop sensor values.
You will need to see the max value of the two endStop sensors and choose your THRESHOLD under this values.

The next step is to flash the firmware and perform the calibration :
```
KnitHack_KH940/Arduino/KH940/KH940.ino
```
These two variables allow you to set the knitter's stitches positions :
```
#define START_POS_L value 
#define START_POS_R value
```

##Softs

###Pure Data
This knitHack Pure Data patch is working in combination with others sound softwares like Audacity.
The PD patch will analyse the frequencies of the sound with an FFT algorithm to generate the knitting pattern.

- [Pure Data](http://puredata.info/downloads/pd-extended) : PD is an open source software that offers an accessible visual programming solution.
- [Jackd](http://www.jackaudio.org/downloads/) : is a low latency sound server. It allow multiple applications to connect to an audio device, and to share audio between themselves.
- [QjackCtl](http://qjackctl.sourceforge.net/) : user interface to controll the JACK sound server. At the same time, it is a patch bay and monitoring tool for JACK.
- [Audacity](http://audacity.sourceforge.net/) : multitrack audio editor for Linux / Unix, MacOS and Windows. It is designed to easily record, play, and edit digital audio files.

###Step by step 
- A add user to the audio group
 - in a terminal : sudo gpasswd -a "user name" audio
 - restart your user session
- B load QjackCtl to start jack audio server
 - select Alsa driver in config
 - clic on "START"
- C load Pd-extande
 - Media > Audio configurations > JACK Audio Connection Kit
- D load Audacity
 - Edition > Preferances > Peripheriques > Hote > Jack Audio Connection Kit
 - save and restart Audacity
 - Edition > Preferances > Peripheriques > Peripherique > pd_extended_0
 - load a sound file
 - play the sound
- E configure QjackCtl connexions
 - clic on "CONNECT" to do the audio patchning
 - Audacity OUTPUT > INPUT Pd-extended OUTPUT > INPUT System
- F open the patch : /PureData/KH940_soundKniting.pd
- G select the good comport by clicking on 'CHECK_DEVICES'
 - look the available serial ports on the Pd-extended window
 - [comport]: available serial ports: ...
 - set the corresponding serial port number in PORT numbrer box
 - clic PORT_ON_OFF
- H play the sound in Audacity
- I clic REC
- J clic SIMULATOR to visualise the pattern
- ...
- K start knitting

###Processing
This simple sketch allow you to send any black and white image.

##knitt
To start knitting, the carriage must be out of the sticheses on the left side.
Each time you make a row you must pass the end ligne sensor.
At the end of the row you will hear two bip.
A bip when you pass the end ligne sensor end a bip to tell you when all stitch's positions are seted.

