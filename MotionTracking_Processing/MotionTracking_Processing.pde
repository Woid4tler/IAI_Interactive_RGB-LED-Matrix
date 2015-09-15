//Import der benötigten Libaries
import processing.serial.*;
import processing.video.*;

//Variablendefiniton und Initialisierung
int videoWidth = 640; //Bildbreite
int videoHeight = 480; //Bildhöhe
int videoCols = 8, videoRows = 8; //Anzahl der Bereiche, 8x8 -> LED Matrix
 
//Array für die Bewegungswerte der 64 Quadranten (8x8)
float[] motion = new float[videoCols * videoRows];

int motionIndex; //Variable für den aktuellen Quadranten
int pxPerCol = videoWidth / videoCols; //Pixel pro Quadrant in der Breite
int pxPerRow = videoHeight / videoRows; //Pixel pro Quadrant in der Höhe

//Zwischenspeicher der letzten 3 Webcambilder
float[] picturebuffer1 = new float[videoWidth * videoHeight];
float[] picturebuffer2 = new float[picturebuffer1.length];
float[] picturebuffer3 = new float[picturebuffer1.length];

Serial serialPort; //Variable für den seriellen Port
Capture webcam = null; //Capture Objekt erstellen
 
void setup () {
  size (640, 480); //Fenstergröße definieren
 
  //Webcam initialisieren und starten
  webcam = new Capture (this, videoWidth, videoHeight);
  webcam.start();
  
  //Gibt alle verfügbaren seriellen Ports aus
  //println(Serial.list());

  //öffnet den gewünschten seriellen Port mit einer Baudrate von 9600
  serialPort = new Serial(this, Serial.list()[7], 9600);
}

void draw () {
  if (webcam.available ()) {
     
    webcam.read(); 

    // 'motion' array zurücksetzen
    for (int i=0; i < motion.length; i++) {
      motion[i] = 0;
    }
 
    //jedes Pixel des Bildes wird durchlaufen
    for (int i = 0; i < webcam.pixels.length; i++) {
       
      // x und y Position des aktuellen Pixels berechnen
      int x = (int) ((i % webcam.width) / pxPerCol);
      int y = (int) ((i / webcam.width) / pxPerRow);
       
      // in welchem Quadranten befindet sich das aktuelle Pixel
      motionIndex = y * videoCols + (7-x);
 
      // arbe des aktuellen Pixels auslesen
      color pxCol = webcam.pixels[i];
      //Summe aus allen drei Farbkanälen bilden
      float sum = red (pxCol) + green (pxCol) + blue (pxCol);
 
      // absolute Farbwertänderung des Pixels errechnen
      float deltaPixel = abs((picturebuffer1[i] + picturebuffer2[i] + picturebuffer3[i]) / 3 - sum);
 
      // Die Änderung auf den Gesamtwert des jeweiligen Quadranten addieren
      motion[motionIndex] += deltaPixel;
 
      // Verschieben der Bildpuffer nach hinten
      picturebuffer3[i] = picturebuffer2[i];
      picturebuffer2[i] = picturebuffer1[i];
      picturebuffer1[i] = sum;
    }
  
    //Bild spiegeln und ausgeben
    pushMatrix();
    scale(-1,1);
    image(webcam,-width,0);
    popMatrix();
 
    // Für jeden Quadranten
    serialPort.write('*'); //Anfangszeichen der seriellen Übertragung, danach folgt der Wert für den ersten Quadranten
    for (int i=0; i < motion.length; i++) {
       
      // Durchschnittliche Farbwertänderung berechnen
      // Mit dem Teilen der Summe durch die Anzahl der Pixel
      motion[i] /= pxPerCol* pxPerRow;
      
      //Rechtecke entsprechend der Quadranten über das Bild zeichnen, nur wenn ein gewisse Farbänderung im Quadranten passiert
      if(motion[i] > 25){
        serialPort.write('1'); //aktueller Quadrant ist aktiv
        stroke (255, 20);  //Rahmen des Rechteckes
        fill (0, 0, 255, 125);  //Füllfarbe des Rechteckes
        rect ((i % videoCols) * pxPerCol, (i / videoCols) * pxPerRow, pxPerCol, pxPerRow);  //Rechteck anzeigen
      }
      else{
        serialPort.write('0'); //aktueller Quadrant ist nicht aktiv
        stroke (255, 20); //Rahmen des Rechteckes
        fill (0, 0, 0, 175);  //Füllfarbe des Rechteckes
        rect ((i % videoCols) * pxPerCol, (i / videoCols) * pxPerRow, pxPerCol, pxPerRow); //Rechteck anzeigen
      }
    }
    serialPort.write('#'); //Endzeichen der seriellen Übertragung, davor war der Wert für den letzten Quadranten
  }
  delay(100); //Damit die serielle Schnittstelle nicht zugemüllt wird
}