// Purpose: To create a user-programmable timer with an RTC and servo.
// Reference: https://bit.ly/2zoHIE9
// Author: A. Goldman
// Date: May 9, 2020
// Status: Working

#include <Wire.h>          // Includes I2C library
#include <LiquidCrystal.h> // Includes LCD library
#define RTCADDRESS 0x68    // I2C bus address of RTC
#define SQWREGISTER 7      // RTC SWQ control register
#define SQWPIN 2           // Defines SQW pin
#define SERVOPIN 9         // Defines servo control pin
#define UPPIN 11           // Defines increase time pin
#define DOWNPIN 12         // Defines decrease time pin
#define STOPPIN 13         // Defines start / stop pin
// Defines LCD pins; initializes display...
uint8_t rs = 8, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Variable for totalSecs counter; start at 5 mins...
volatile uint16_t totalSecs = 300;
uint8_t secs = 0; // Variable for displayed secs
uint8_t mins = 0; // Variable for displayed mins

void setup()
{
    Wire.begin();                       // Initiates the I2C bus
    Wire.beginTransmission(RTCADDRESS); // Communication with RTC
    Wire.write(SQWREGISTER);            // Accesses control register
    Wire.write(0b00010000);             // Sets register bits for 1 Hz SQW
    Wire.endTransmission();             // Ends transmission w/ stop condition
    lcd.begin(16, 2);                   // Initializes LCD
    pinMode(SQWPIN, INPUT_PULLUP);      // Sets pin as input w/ pullup
    pinMode(SERVOPIN, OUTPUT);          // Sets pin as output
    pinMode(UPPIN, INPUT);              // Sets pin as input
    pinMode(DOWNPIN, INPUT);            // Sets pin as input
    pinMode(STOPPIN, INPUT);            // Sets pin as input
    // ISR to count down secs from SQW initialized...
    attachInterrupt(digitalPinToInterrupt(SQWPIN), ISR_Count, FALLING);
}

void ISR_Count()
{ // If switch is set to START...
    if (digitalRead(STOPPIN) == LOW)
    {
        totalSecs--; // decrease secs count by 1 every falling edge
    }
}

void loop()
{
    if (digitalRead(UPPIN) == LOW)
    {                              // If pin is low...
        totalSecs = totalSecs + 5; // increase time by 5 secs
        delay(100);                // Delay to prevent over-pressing
    }
    if (digitalRead(DOWNPIN) == LOW)
    {                              // If pin is low...
        totalSecs = totalSecs - 5; // decrease time by 5 secs
        delay(100);                // Delay to prevent over-pressing
    }
    if (totalSecs == 0)
    {                                 // If time up...
        lcd.clear();                  // Clear display
        lcd.setCursor(0, 0);          // Set cursor
        lcd.print("TIMER UP");        // Display text
        lcd.setCursor(0, 1);          // Set cursor
        lcd.print("push rst button"); // Display text
        while (1)
        {                               // Runs forever; until reset
            analogWrite(SERVOPIN, 200); // Turns servo
            delay(600);                 // Delay
            analogWrite(SERVOPIN, 100); // Turns servo
            delay(600);                 // Delay
        }
    }
    mins = totalSecs / 60; // Mins value to display
    secs = totalSecs % 60; // Secs value to display
    lcd.setCursor(0, 0);   // Sets cursor... displays mins
    lcd.print(String("MINS: ") + mins + String(" "));
    lcd.setCursor(0, 1); // Sets cursor ... displays secs
    lcd.print(String("SECS: ") + secs + String(" "));
}
