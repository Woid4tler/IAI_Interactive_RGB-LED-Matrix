#include <Colorduino.h>

int data[66], dataBuffer1[66], dataBuffer2[66];
int value = 1, arduinoMode = 1;
const int buttonMenu = 5, buttonChoose = 2;
int buttonMenuState = 0, buttonChooseState = 0;
int r = 0, g = 0, b = 0;
int waitingState = 0;

void setup() {
  Colorduino.Init(); //Initialisierung des Colorduino Boards
   
  //Weißabgleich der RGB Matrix
  unsigned char whiteBalVal[3] = {25,50,50};
  Colorduino.SetWhiteBal(whiteBalVal); 
  
  Serial.begin(9600);
  
  for(int i = 3; i <= 13; i++){
    pinMode(i, OUTPUT);
  } 
  pinMode(buttonMenu, INPUT);
  pinMode(buttonChoose, INPUT);
}

void loop() {
  buttonMenuState = digitalRead(buttonMenu);
  if (buttonMenuState == HIGH) {
    reset();
    Colorduino.SetPixel(0,7,0,255,0);
    Colorduino.SetPixel(0,arduinoMode,255,0,0);
    Colorduino.FlipPage();
    buttonChooseState = digitalRead(buttonChoose);
    if (buttonChooseState == HIGH) {
      Colorduino.SetPixel(0,arduinoMode,0,0,0);
      arduinoMode++;
      if(arduinoMode > 2)  arduinoMode = 0;
      Colorduino.SetPixel(0,arduinoMode,255,0,0);
      Colorduino.FlipPage();
      delay(100);
    }
  }
  else{
    //Wenn Pause/Menü-Button nicht gedrückt ist
    if (arduinoMode == 1){
      if (Serial.available()>0){
        for (int i=0; i<66; i++){
          data[i]=data[i+1];
        }
        data[66] = Serial.read();
        
        if ((data[0]=='*')&&(data[65]=='#')){
          for (int i=0; i<66; i++){
            dataBuffer2[i]=dataBuffer1[i];
          }
          for (int i=0; i<66; i++){
            dataBuffer1[i]=data[i];
          }
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
    else if(arduinoMode == 2){
      reset();
      fadeColor();
    }
    else{
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
          Colorduino.SetPixel(x,y,random(0,255),random(0,255),random(0,255));
        }
      }
      Colorduino.FlipPage();
    }
  }
}

void reset(){
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      Colorduino.SetPixel(x,y,0,0,0);
    }
  }
}

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
