//Import der benötigten Libaries
import processing.serial.*;
import processing.video.*;

//Variablendefiniton und Initialisierung
int videoWidth = 640; //Bildbreite
int videoHeight = 480; //Bildhöhe
int videoCols = 8, videoRows = 8; //Anzahl der Bereiche, 8x8 -> LED Matrix
 
//Array für die Bewegungswerte der 64 Quadranten
float[] motion = new float[videoCols * videoRows];

//Zwischenspeicher der letzten 3 Webcambilder
float[] picturebuffer1 = new float[videoWidth * videoHeight];
float[] picturebuffer2 = new float[picturebuffer1.length];
float[] picturebuffer3 = new float[picturebuffer1.length];

Serial serialPort; //Variable für den seriellen Port
Capture webcam = null; //Capture Objekt erstellen

int numPixels;
 
void setup () {
  size (640, 480); //Fenstergröße definieren
 
  //Webcam initialisieren und starten
  webcam = new Capture (this, videoWidth, videoHeight);
  webcam.start();
 
  numPixels = webcam.width * webcam.height;
  
  //Gibt alle verfügbaren seriellen Ports aus
  //println(Serial.list());

  //öffnet den gewünschten seriellen Port mit einer Baudrate von 9600
  serialPort = new Serial(this, Serial.list()[7], 9600);
}

void draw () {
  if (webcam.available ()) {
     
    webcam.read(); 
     
    int motionIndex;
    int pxPerCol = videoWidth / videoCols;
    int pxPerRow = videoHeight / videoRows;
     
    // 'motion' array zurücksetzen
    for (int i=0; i < motion.length; i++) {
      motion[i] = 0;
    }
 
    //jedes Pixel des Bildes wird durchlaufen
    for (int i = 0; i < webcam.pixels.length; i++) {
       
      // x und y Position des Quadranten berechnen
      int x = (int) ((i % webcam.width) / pxPerCol);
      int y = (int) ((i / webcam.width) / pxPerRow);
       
      // Quadranten-Zugehörigkeit des Pixels herausfinden.
      // Später wichtig für das 'motion' array.
      motionIndex = y * videoCols + (7-x);
 
      // Farbe an der Position 'i' im Kamerabild
      color col = webcam.pixels[i];
      // Die Summe aus allen drei Farbkanälen bilden
      float sum = red (col) + green (col) + blue (col);
 
      // Farbwertänderung des Pixels bezogen auf alle Bilder errechnen
      float deltaPixel = (picturebuffer1[i] + picturebuffer2[i] + picturebuffer3[i]) / 3 - sum;
 
      if (deltaPixel < 0) {
        deltaPixel *= -1;
      }
 
      // Die Änderung auf den Gesamtwert des Quadranten addieren
      motion[motionIndex] += deltaPixel;
 
      // Verschiebe das 'Bildgedächtnis' um einen Rutsch nach hinten.
      picturebuffer3[i] = picturebuffer2[i];
      picturebuffer2[i] = picturebuffer1[i];
      picturebuffer1[i] = sum;
    }
    
    /*loadPixels();
    for ( int i = 0; i < numPixels; i++ ) {
      float b = brightness( webcam.pixels[i] );
      webcam.pixels[i] = color( b+100 );
    }
    updatePixels();*/
  
    //Bild spiegeln und ausgeben
    pushMatrix();
    scale(-1,1);
    image(webcam,-width,0);
    popMatrix();
 
    // Für jeden Quadranten
    serialPort.write('*');
    for (int i=0; i < motion.length; i++) {
       
      // Durchschnittliche Farbwertänderung berechnen.
      // Mit dem Teilen der Summe durch die Anzahl der Pixel
      motion[i] /= pxPerCol* pxPerRow;
      
      //Rechtecke entsprechend der Quadranten über das Bild zeichnen, nur wenn ein gewisse Farbänderung im Quadranten passiert
      if(motion[i] > 25){
        serialPort.write('1'); 
        stroke (255, 20);
        fill (0, 0, 255, 140);
        rect ((i % videoCols) * pxPerCol, (i / videoCols) * pxPerRow, pxPerCol, pxPerRow);
      }
      else{
        serialPort.write('0');
        stroke (255, 20);
        fill (0, 0, 0, 180);
        rect ((i % videoCols) * pxPerCol, (i / videoCols) * pxPerRow, pxPerCol, pxPerRow);
      }
    }
    serialPort.write('#');
  }
  delay(100); //Damit die serielle Schnittstelle nicht zugemüllt wird
}