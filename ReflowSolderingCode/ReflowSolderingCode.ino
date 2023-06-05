//Include all the libraries needed
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "max6675.h"
#include "pid.h"

// Pin configuration (Hardrware SPI)
#define TFT_DC 9
#define TFT_CS 10
#define TOUCH_CS 7
#define TFT_RST 8

// Create the MAX6675 object
int thermoDO = 4;
int thermoCS = 3;
int thermoCLK = 2;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// define heater and fan pins
#define HEATER_PIN 6
#define FAN_PIN 5

// Define the temperature profile and time profile
int tempProfile[] = {21, 150, 183, 217, 240, 240, 217, 183, 150, 100, 60};
int timeProfile[] = {1, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120};
double currentTime = 1.0;
double dt = 0.1;

// Define the target temperature and PID constants
double targetTemperature = 30;
#define Kp 0.01
#define Ki 0.00001
#define Kd 100.0


// Creo el controlador pid
Pid control(Kp,Ki,Kd);




// Inicialize the pins from the display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
XPT2046_Touchscreen ts(TOUCH_CS);

void setup() {
  //PRESCALER TO CHANGE THE FREQUENCY ON PINS 6 AND 5
  //It is not used as it efects timer 0 which is the one used for delay and millis
  //TCCR0B=TCCR0B & B11111000 | B00000100;
  
  //reset pantalla
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(1000);
  digitalWrite(TFT_RST, HIGH);
  delay(1000);
  // Initialize the serial communication 
  Serial.begin(9600);
  //init screen
  tft.begin(1000000);
  tft.setRotation(3);
  //init Touch
  ts.begin();
  ts.setRotation(3);
  
  // Set the heater and fan pins to outputs
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Turn OOFF the fan
  digitalWrite(FAN_PIN, LOW);
  //DRAW ALL THE STATIC GRAPHICS IN THE SCREEN
  dibujaGraficos();
  
////////////  
//When the screen is inicialized it allways gets one point
//This is the way for avoiding this point
  while (!ts.touched()) {
  }
  TS_Point p = ts.getPoint();
  delay(1000);
////////////
}
unsigned long lastFrame = millis();
int state = 0;
void loop() {
  if (!ts.touched()) {
    //First, get the value of the temperature and show it on the display
    double temperature = thermocouple.readCelsius();
    tft.fillRect(200, 140, 110, 35, ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(210, 145); tft.setTextSize(3);
    tft.println(temperature);
    
    if (state == 1) {
      
///////////////////
      currentTime += dt;
      int profileIndex = 1;
      int profileIndexANT =0;
      while (currentTime > timeProfile[profileIndex]) {
        profileIndexANT=profileIndex;
        profileIndex++;
      }
      // Create the target temperature value
      targetTemperature=((tempProfile[profileIndex]-tempProfile[profileIndexANT])/(timeProfile[profileIndex]-timeProfile[profileIndexANT]))*(currentTime-timeProfile[profileIndexANT])+ tempProfile[profileIndexANT];
////////////////////

  // Read the temperature from the MAX6675 amplifier
      control.set_setpt(targetTemperature);
      control.set_input(temperature);

      int output = control.get_output();
      output = output>255? 255 : output;
      output = output<0? 0 : output;
      
  // Control the heater based on the PID output
      analogWrite(HEATER_PIN, output);

  //Control the fan based on the error 
      if (control.get_error() < -5) {
        digitalWrite(FAN_PIN, HIGH);
       
        
      } else {
        digitalWrite(FAN_PIN, LOW);
        
      }

      // Print the temperature and PID output to the serial monitor
      Serial.print(",");
      Serial.print(temperature);
      Serial.print(",");
      Serial.print(targetTemperature);
      Serial.print(",");
      Serial.println(output);
      //Target temperature update
      tft.fillRect(210, 80, 110, 35, ILI9341_BLACK);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(210, 90); tft.setTextSize(3);
      tft.println(targetTemperature);
      //REAL TIME update
      tft.fillRect(210, 200, 110, 35, ILI9341_BLACK);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(210, 210); tft.setTextSize(3);
      tft.println(((millis()-lastFrame)/1000)-8);
      
      //When the profile finish restore the screen and go to state 0
      if (profileIndex>=11){
        state=0;
        profileIndex=1;
        verd();
      }
    }
  }
  //When detects somethig touching the screen
  else {
    TS_Point p = ts.getPoint();

    // Botón
    if ( (p.x >= 1500) && (p.x < 2500) && (p.y >= 1600) && (p.y < 2400) ) // Coordenadas del botón
    {
      if (state == 0) {
        state = 1;
      }
      else {
        //Here should be 0 but it took some random points (sometimes) which made some errors and stopped the process
        state = 1;
      }
    }
    if (state == 0)
    {
      verd();
      // green button

    }
    else
    {
      red();
      // Red button

    }

  }


  // Wait for a short time before repeating the loop
  //This time is the freqüency of the update
  delay(300);
}

//All the static interace is printed here
void dibujaGraficos()
{
  //Black background
  tft.fillScreen(ILI9341_BLACK);
  yield();

  // Title
  tft.fillRect(0, 0, 319, 60, ILI9341_BLUE);
  tft.drawRect(0, 0, 319, 60, ILI9341_WHITE);

  tft.setCursor(4, 30);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("REFLOW SOLDERING");

  tft.fillRect(0, 280, 180, 240, ILI9341_BLUE);
  tft.drawRect(0, 280, 180, 240, ILI9341_WHITE);
  
  //Information in the bottom
  tft.setCursor(4, 200);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("Soldering");
  tft.setCursor(4, 220);
  tft.println(" Machine");

  // Names of the people in the group
  char texto[15];
  sprintf(texto, "juni.ana.jordi");
  tft.setCursor(320 - (strlen(texto) * 6), 5);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println(texto);


  //TARGET TEMPERATURA
  tft.setCursor(180, 70);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("TARGET TEMPERATURE");
  // REAL TEMPERATURA
  tft.setCursor(180, 125);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("REAL TEMPERATURE");
  // REAL TIME
  tft.setCursor(180, 190);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("REAL TIME");

  // First button
  tft.fillRect(20, 100, 110, 70, ILI9341_GREEN); 
  tft.setCursor(50, 125);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.println("START");

  
  delay(1000);

}
//Geen button
void verd() {
  tft.fillRect(210, 80, 110, 40, ILI9341_BLACK);
  tft.fillRect(20, 100, 110, 70, ILI9341_GREEN);
  tft.setCursor(50, 125);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.println("START");
  currentTime = 1;
}
//Red button
void red() {
  tft.fillRect(20, 100, 110, 70, ILI9341_RED);
  tft.setCursor(50, 125);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("STOP");
}
