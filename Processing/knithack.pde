/*
This sketch read pixels from an image to control a BROTHER knithacked
 The image must be 200 pixels width maximum
 )c( maurin.box@gmail.com
 */

import processing.serial.*;

Serial myPort;               // Create object from Serial class

int BAUDERATE   = 38400;     // Serial port speed
int PORTNUMBER  = 0;         // Select your port number
char HEADER     = 64;        // header recived evrey knetted row 
char FOOTER     = 255;       // footer to terminate the list of pixels to knit
int PIXELWIDTH  = 0;         // 
int PIXELHEIGHT = 0;         // 
int MARGIN      = 0;         //
int MAXSIZE     = 200;       //

char inputChar;              // variable to store incoming data

PImage myImage;
PFont font;

boolean readFrame = false;
int ligneIndex = 0;          // the row to knitt
int pixelState = 0;          // store the colore for picels

byte[] serialData = new byte[MAXSIZE];

boolean DEBUG = false;
boolean COMPORT = true;  // set it true to connect your knitter

void setup() {

  size(1000, 150);

  PIXELWIDTH = int(width / MAXSIZE);
  PIXELHEIGHT = int(width / MAXSIZE);

  font = createFont("Georgia", 40);
  textFont(font);

  // Set the corect port name
  String PORTNAME = Serial.list()[PORTNUMBER];
  // println(PORTNAME);
  if (COMPORT) myPort = new Serial(this, PORTNAME, BAUDERATE);

  // Selaect your pattern by writng it name below
  myImage = loadImage("blackboxe_logo.jpg");
  // myImage = loadImage("nathalie.jpg");
  // myImage = loadImage("damier.jpg");
  // myImage = loadImage("damier_99.jpg");
  // myImage = loadImage("graphicants_petit_nb.jpg");

  // Scan all pixels from the imported picture
  // The pixels will be mapped to vector "pixels"
  myImage.loadPixels();

  ligneIndex = myImage.height;

  background(255);
  displayPicture(ligneIndex);
  displayReaadLine();

  // If the pattern is less than 200 pixel (max size)
  // Set the pattern in the middel of the knitting machine
  MARGIN = int ( MAXSIZE - myImage.width );
  println(MARGIN/2);
}

void draw() {

  if (COMPORT) {
    while (myPort.available () > 0) {
      inputChar = myPort.readChar();
      if (inputChar == HEADER && readFrame == false) {
        ligneIndex--;
        if (ligneIndex <= -1) ligneIndex = 0;
        else readFrame = true;
      }
    }
  }
  if (readFrame == true) {
    displayPicture(ligneIndex);
    displayReaadLine();
    fill(255, 0, 0);
    text(ligneIndex, 30, height/2 - 10);

    // Read and write next frame to the serial port
    for (int i=0; i<MAXSIZE; i++) {
      if (i > MARGIN / 2 && i < myImage.width + (MARGIN/2) ) {
        pixelState = myImage.pixels[ i - (MARGIN/2) + myImage.width * ligneIndex ];
        if (pixelState == -1) serialData[ i ] = 1;
        if (pixelState == -16777216) serialData[ i ] = 0;
      } else {
        serialData[i] = 1;
      }
      if (COMPORT) myPort.write( serialData[i] );
      if (DEBUG) print( serialData[i] );
      readFrame = false;
    }
    if (COMPORT) myPort.write( FOOTER );
    if (DEBUG) println( FOOTER );
  }
}

// Display all pixels 
void displayPicture(int verticalPos) {

  background(255);   // Clear trails
  // TODO : vertical symmetry must be achieved
  strokeWeight(0.5);
  // stroke(255);       // White lines  
  stroke(0);         // Black lines  

  for (int i=0; i<myImage.height; i++) {
    for (int j=0; j<myImage.width; j++) {
      pixelState = myImage.pixels[ j + i * myImage.width ];
      if (pixelState == -1) fill(255);
      if (pixelState == -16777216) fill(0);
      // rect( X1, Y1, width, height );
      rect(
      j * PIXELWIDTH + ( int( width / 2 ) - ( PIXELWIDTH * int( myImage.width / 2 ) ) ), 
      i * PIXELHEIGHT - ( verticalPos * PIXELHEIGHT - int( height / 2 ) ), 
      PIXELWIDTH, 
      PIXELHEIGHT
        );
    }
  }
}

// Draw two red lines to visualise the current frame onto the pattern
void displayReaadLine() {
  strokeWeight(1.1);
  stroke(255, 0, 0);
  line(0, height/2, width, height/2);
  line(0, height/2 + PIXELHEIGHT, width, height/2 + PIXELHEIGHT);
}

// Use keys to move the pattern and activate debug mode
void keyPressed() {

  if (key == 'd') {
    DEBUG = !DEBUG;
  }

  if (key == CODED) {
    if (keyCode == DOWN) {
      if (ligneIndex <= 0) {
        ligneIndex = 0;
      } else {
        ligneIndex--;
        readFrame = true;
      }
    }
    if (keyCode == UP) {
      if (ligneIndex >= myImage.height - 1) ligneIndex = myImage.height - 1;
      else {
        ligneIndex++;
        readFrame = true;
      }
    }
    if (keyCode == RIGHT) {
      ligneIndex = myImage.height - 1;
      readFrame = true;
    }
    if (keyCode == LEFT) {
      ligneIndex = 0;
      readFrame = true;
    }
  }
}
