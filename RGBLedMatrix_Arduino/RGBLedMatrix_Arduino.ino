#include <Colorduino.h>  //Libary für das Color Shield v1.1

// Variablen 
int data[66], dataBuffer1[66], dataBuffer2[66];  //66bit Array für die serielle Übertragung
int arduinoMode = 1; //Modus des Arduinos, standardmäßig 1, Motion Tracking
const int buttonMenu = 5, buttonChoose = 2; //Pins des Buttons
int buttonMenuState = 0, buttonChooseState = 0; //aktueller Buttonstatus

void setup() {
  Colorduino.Init(); //Initialisierung des Colorduino Boards
   
  //Weißabgleich der RGB Matrix
  unsigned char whiteBalVal[3] = {25,50,50};
  Colorduino.SetWhiteBal(whiteBalVal); 
  
  Serial.begin(9600); //Serielle Übertragung starten
  
  
  //Pins auf In/Output setzen
  for(int i = 3; i <= 13; i++){
    pinMode(i, OUTPUT);
  } 
  pinMode(buttonMenu, INPUT);
  pinMode(buttonChoose, INPUT);
}

void loop() {
  buttonMenuState = digitalRead(buttonMenu);  //Menübutton einlesen
  if (buttonMenuState == HIGH) {  //Wenn der Button gedrückt ist, pausiert das aktuelle Programm
    reset();  //Matrix zurücksetzen
    Colorduino.SetPixel(0,7,0,255,0);  //Menü LED auf grün setzen
    Colorduino.SetPixel(0,arduinoMode,255,0,0);  //Modus LED nach aktuellen Modus auf Rot setzen
    Colorduino.FlipPage();  //Damit werden die LEDs auf der Matrix entsprechend der vorangegangenen Zeilen eingeschalten
    buttonChooseState = digitalRead(buttonChoose);  //Modusbutton einlesen
    if (buttonChooseState == HIGH) { //Wenn der Modusbutton gedrückt wird, geht der Modus eins weiter
      Colorduino.SetPixel(0,arduinoMode,0,0,0); //LED des alten Modus ausschalten
      arduinoMode++; //Modus weiterschalten
      if(arduinoMode > 2)  arduinoMode = 0; //Wenn der Modus auf 3 gesetzt werden würde, wird er automatisch auf 0 gesetzt, da es den Modus 3 nicht mehr gibt
      Colorduino.SetPixel(0,arduinoMode,255,0,0); //Modus LED nach aktuellen Modus auf Rot setzen
      Colorduino.FlipPage(); //Damit werden die LEDs auf der Matrix entsprechend der vorangegangenen Zeilen eingeschalten
      delay(100);  //Delay, damit die Wahl besser gelingt, ansonsten ist es fast unmöglich mit einem Buttondruck in den nächsten Modus zu wechseln
    }
  }
  else{
    //Wenn Pause/Menü-Button nicht gedrückt ist
    if (arduinoMode == 1){  //Modus 1 ist das Motion Tracking
      if (Serial.available()>0){ //Wenn eine serielle Übertragung geöffnet ist
        for (int i=0; i<66; i++){
          data[i]=data[i+1]; //der aktuelle Speicher wird eins nach vorne gesetzt
        }
        data[66] = Serial.read(); //am letzten Feld des Speichers wird der aktuelle Wert von der seriellen Schnittstelle gespeichert
        
        if ((data[0]=='*')&&(data[65]=='#')){ //Wenn das Array vollständig übertragen ist: Feld[0] = '*' und Feld[65] = '#' dazwischen sind die 64bit für die 64 LEDs auf der Matrix
          for (int i=0; i<66; i++){
            dataBuffer2[i]=dataBuffer1[i]; //Zwischenspeicher2 wird erneuert
          }
          for (int i=0; i<66; i++){
            dataBuffer1[i]=data[i]; //Zwischenspeicher1 wird erneuert
          }
          //Anzeige der 64 LEDs
          for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
              if (data[y+(x*8)+1]=='1'){
                Colorduino.SetPixel(x,y,0,0,255);
              }
              else if (dataBuffer1[y+(x*8)+1]=='1'){
                Colorduino.SetPixel(x,y,0,0,160);
              }
              else if (dataBuffer2[y+(x*8)+1]=='1'){
                Colorduino.SetPixel(x,y,0,0,80);
              }
              else{
                Colorduino.SetPixel(x,y,0,0,0);
              }
            }
          }
          Colorduino.FlipPage();
        }
      }
    }
    else if(arduinoMode == 2){ //Modus 2 ist der Farbverlauf
      //reset(); //Bild zurücksetzen
      fadeColor(); //Methode aufrufen
    }
    else{ //Modus 0 sind zufällige Farbwerte aller Pixel
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
          Colorduino.SetPixel(x,y,random(0,255),random(0,255),random(0,255));
        }
      }
      Colorduino.FlipPage();
    }
  }
}


//Reset Methode, alle LEDs werden ausgeschalten
void reset(){
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      Colorduino.SetPixel(x,y,0,0,0);
    }
  }
}


//Variablen für die fadeColor Methode
long previousMillis = 0;
long interval = 100;
int red = 0, green = 0, blue = 255;
int redDiff = 50, greenDiff = 0, blueDiff = 0;
int redBuffer[8], greenBuffer[8], blueBuffer[8];

void fadeColor(){
  //Intervallsteuerung
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    
    //Nächste Farbe berechnen
    red += redDiff;
    green += greenDiff;
    blue += blueDiff;

    if(blueDiff < 0 && blue <= 0) {
      blueDiff = 0;
      greenDiff = 50;
    }
    if(greenDiff > 0 && green >= 255) {
      greenDiff = 0;
      redDiff = -50;
    }
    if(redDiff < 0 && red <= 0) {
      redDiff = 0;
      blueDiff = 50;
    }
    if(blueDiff > 0 && blue >= 255) {
      blueDiff = 0;
      greenDiff = -50;
    }
    if(greenDiff < 5 && green <= 0) {
      greenDiff = 0;
      redDiff = 50;
    }
    if(redDiff > 0 && red >= 255) {
      blueDiff = -50;
      redDiff = 0;
    }
    
    for (int i = 0; i < 8; i++) {
      redBuffer[i] = redBuffer[i+1];
      greenBuffer[i] = greenBuffer[i+1];
      blueBuffer[i] = blueBuffer[i+1];
    }
    redBuffer[7] = red;
    greenBuffer[7] = green;
    blueBuffer[7] = blue;

    //Farbe ausgeben
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        Colorduino.SetPixel(x,y,redBuffer[y],greenBuffer[y],blueBuffer[y]);
      }
    }
    Colorduino.FlipPage();
  }

}
