#include <Arduino.h>
#include <stdlib.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "ADCReadings.h"
#include "Interruptsetup.h"

// Pis and constants
#define voltage A1
#define relais 13

volatile bool execute = 0;

float setTemp = 7;          // Desired temperature in fridge
float hystTemp = 2;         // Allowed tolerance band for the temperature +/-
int numbMeasur = 100;       // Number of measurements for averaging adc values
float vA0off = 0.0;         // Offset A0
float vA0gain = 1.0;        // Gain for A0
float m = -22.493;          // Gain of liner regression of NTC value
float t = 79.25;            // Temperature offset of liner regression of NTC value
bool debug = true;          // True for debugging
bool relaisstate = 0;       // State of the relais
String relaisStatus = "OFF";// State message relais

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Create sensor objects
ADCReadings volval(voltage, numbMeasur, vA0off, vA0gain);

void setup() {
  pinMode(relais, OUTPUT);
  interruptSetup(1024, 15624);      // prescaler and compare value https://tic.simsso.de/
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("Fridge");
  lcd.setCursor(3, 1);
  lcd.print("Controller");
  delay(5000);
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("by");
  lcd.setCursor(2, 1);
  lcd.print("Matze aka Boss");
  delay(5000);  
}

void loop() {

  if (execute) {
    execute = 0; // reset flag
        
    // Read voltage
    float valV = volval.readVoltageADC();
        
    // Calculate temperature
    float ntcTemp = m*valV + t;

    // Algorythm for temperature hysteresis
  
    if (ntcTemp > setTemp+hystTemp) {
      relaisstate = 1;
      digitalWrite(relais,relaisstate);
      relaisStatus = "ON ";
      Serial.println("To hot");
    }

    else if (ntcTemp < setTemp-hystTemp) {
      relaisstate = 0;
      digitalWrite(relais,relaisstate);
      relaisStatus = "OFF";
      Serial.println("To cold");
    }

    else if ((ntcTemp > setTemp+hystTemp) || (ntcTemp < setTemp-hystTemp)) {
      digitalWrite(relais,relaisstate);
      Serial.println("Optimal Window");
    }
  
    // Print values
    lcd.setCursor(0, 0);
    lcd.print("Vol  Temp  State");
    lcd.setCursor(0, 1);
    lcd.print(valV,2); lcd.print("  ");
    lcd.print(ntcTemp,1); lcd.print("  ");
    lcd.print(relaisStatus);

    // Debug values via serial communication
    if (debug) {
      Serial.println((String)"valV "+valV);
      Serial.println((String)"NTC "+ntcTemp);
    }
  
  }// end if

}// end loop

ISR(TIMER1_COMPA_vect) { // function which will be called when an interrupt occurs at timer 1

  execute = 1; // set flag
}
