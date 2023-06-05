
#include <SPI.h>
#include "max6675.h"
#include "pid.h"


int thermoDO = 4;
int thermoCS = 3;
int thermoCLK = 2;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

#define HEATER_PIN 6
#define FAN_PIN 5

// Define the target temperature and PID constants
double targetTemperature = 50.0;
#define Kp 0.1
#define Ki 0.00001
#define Kd 1500.0

// Creo el controlador pid
Pid control(Kp,Ki,Kd);


// Create the MAX6675 object


void setup() {
  //TCCR0B=TCCR0B & B11111000 | B00000100;//PRESCALER CANVIAR FREQUENCIA PER TREURE EL SOROLL
  // Initialize the serial communication
  Serial.begin(9600);

  // Set the heater and fan pins to outputs
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Turn on the fan
  digitalWrite(FAN_PIN, LOW);
}

void loop() {
  // Read the temperature from the MAX6675 amplifier
  double temperature = thermocouple.readCelsius();

  control.set_setpt(targetTemperature);
  control.set_input(temperature);

  int output = control.get_output();
  output = output>255? 255 : output;
  output = output<0? 0 : output;

  analogWrite(HEATER_PIN, output);

  // Update the heater and fan based on the PID output
  if (control.get_error() < -5) {
    digitalWrite(FAN_PIN, HIGH);
    //Serial.println("on");
    
  } else {
    digitalWrite(FAN_PIN, LOW);
    //Serial.println("off");
  }

  // Print the temperature and PID output to the serial monitor
  //Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(targetTemperature);
  Serial.print(",");
  Serial.println(output);
  //Serial.print(", Output: ");
  //Serial.println(output);
  //Serial.print(", error: ");
  //Serial.println(error);
  
  // Wait for a short time before repeating the loop
  delay(500);
}
