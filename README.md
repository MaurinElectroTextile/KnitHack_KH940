KnitHack KH940
=========

- This project is part of the e-textile Summec camp 2014 : http://etextile-summercamp.org/2014
- The Arduino firmware is a fork of Sokanno KnitHack project : https://github.com/sokanno/KnitHack

"Since the 19th century, home knitting machines were a popular manufacturing tool which allowed knitters to produce rapidly knitted items to sell or for household crafts.  The possibility to use jacquard patterns started with the use of punched hole cards and began to be computerised in the 80's. The immediate and low raw material resource needed to produce a knitted surface is what make knitting machines mobile and accessible for experimenting and prototyping. Since the late 80's these knitting machines have stopped been produced.
These last years hackers and makers have started to open up knitting machines and found new ways to hack them by allowing the machines to be controlled directly by a modern computer. These hacked knitting machines become a sort of textile printer. Having a direct communication between digital tools and the knitting machine pushes the boundaries of traditional knitted patterns and gives us the opportunity to think of new way's in conceiving patterns . 

In this workshop we will explore how our knitting machine can become a way of printing out (generated) data on a textile surface.
First we will show how the monitoring system and the arduino controls the knitting machine as well as how the mechanics of the machine works in general.
We will then experiment hands on with field recordings of our surrounding environment. Our sound cartography’s will then be translated and visualised with Pure Data and then sent to the machine to be knitted out. These patterns can be read as a visual translation of rhythms, silences and noises embedded in a palpable knitted surface." Claire Williams.
        

BROTHER KH-940
=========

For technicals specifications see the service manual : http://maurin.box.free.fr/_DOCs/brother_kh930_knitting_machine_service_manual.pdf
- 200 steacheses
- Two end lines sensors
- One encoder wille with two sensors mounted on it :
  - The first sensor is composed by two photointerrupter embedded into a single component (SHARP IA06). This double sensor is used to sens the direction ot the carriage
  - The second sensor is a single photo interrupter used for the phase (SHARP - GP1A52LRJ00F)

Arduino firmware
=========

The Git repository : https://github.com/MaurinElectroTextile/KnitHack_KH940

Before to flash the Arduino with the main code you will need to set up the THRESHOLD value of the main sketch by using this calibration sketch : KnitHack_KH940/Arduino/Calibration/endLignes/endLignes.ino
This sketch will give you the endStop sensor values. You will need to see the max value of the two endLignes sensors and choose your THRESHOLD under this max values.


The next step is to flash the main code and perform the calibration :
- KnitHack_KH940 / Arduino / KH940 / KH940.ino
These two variables allow you to set the knitter's stitches positions.
- START_POS_L value 
- START_POS_R value
...


Softs
=========

This knitHack sound pilot is running with multiple software that will run in addition with Pure Data.
 - Pure Data : PD is an open source software that offers an accessible visual programming solution.
 - Jackd : is a low latency sound server. It allow multiple applications to connect to an audio device, and to share audio between themselves.
 - QjackCtl : user interface to controll the JACK sound server. At the same time, it is a patch bay and monitoring tool for JACK.
 - Audacity : multitrack audio editor for Linux / Unix, MacOS and Windows. It is designed to easily record, play, and edit digital audio files.

        http://puredata.info/downloads/pd-extended
        http://audacity.sourceforge.net/
        http://www.jackaudio.org/downloads/
        

Step by step 
=========

 - start QjackCtl
 - start Pd-extande
 - open the patch : /PureData/KH940_soundKniting.pd
 - Start Audacity
 - Load a sound file
 
The PD patch will analyse the frequencies of the sound with an FFT algorithm
If the volume of a specific frequency is above a certain threshold the corresponding stitch will be triggered.

 - A : Select the good comport by clicking on 'devices' message
 - B : look the available serial ports on the Pd-extended window
  - [comport]: available serial ports:
  - 1	/dev/tty.Bluetooth-PDA-Sync
  - 2   /dev/ttyACM1
 - C : edit the 'open 0' message to change the corresponding serial port number
  - édition mode : Alt-Ctrl+E
  - change the number of the message 'open 0' by the one you want
 - D : clic on the 'open 0' to open the good comport
 - E : load a sound
 - F : play this sound from the beginning to the end
 - G : start knitting

To start knitting, the carriage must be out of the sticheses on the left side.
Each time you make a row you must go from one side to the other.
At the end of the row you will hear two bip, one wen you pass the end stop sensor end one juste after that tell you when all stitch's positions are set.

If you have questions : maurin.box@gmail.com
