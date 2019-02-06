/*    
Course code : TECH1102
Semester/Year : Fall2017
Assignment code: GP1
Group name : overnight_coders
Author(s) : Anderson Resende Viana, Gurpreet Sehmbey , Khushveen Smagh, Kiranjit Bassi
BVC ID: a.resendeviana683, g.sehmbey985, k.smagh938, k.bassi437
Date created : 2017-12-05


        Title: Combination Alarm Clock
        Description: This program will emulate an alarm clock. It is required from
                     users 3 inputs using the rotation sensor to deactivate the buzzer.
                     Users should point the arrow of the rotation sensor to the borders
                     of the blue square on its base and hold each position during 
                     a certain amount of time. A LED will turn on to inform the user 
                     to move the rotation sensor to the next position. Once the user
                     repeats this 3 times, the buzzer will stop buzzing and LED's will
                     be turned off.
                     To initial Clock Time the user should press and hold SW1 button 
                     while he/she moves the rotation sensor. Once the user releases the 
                     button it will be displayed on LCD the inputed time.
                     The same procedure is used to set initial Alarm Time, but the user
                     must press and hold SW2 button instead of SW1.
*/


// Libraries
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// LCD Settings
#define I2C_ADDR  0x3f
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define NUMBER_OF_CHARACTERS 16
#define NUMBER_OF_ROWS 2
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

// Pins
int button1clock = 0;
int button2alarm = 0;
int button1 = 2;
int button2 = 3;
int notPressed = HIGH;
int pressed = LOW;
int buzzerPin = 5;
int lightPin = 1;
int rotationPin = 0;
int blueLedPin = 13;
int redLedPin = 12;

//Led Control
unsigned long previousMillis = 0;
const long interval = 1000;


//General Setup
int rotationReading = 0;
int rotationMinReading = 302;
int rotationMaxReading = 1023;
int defaultCounter = 0;
int buzzerFrequency = 2000;
int deactivationCounter = 5;

//First Input Counters
int firstInputCounter = 0;
int firstInputCounterLed = 0;
int firstLowerRotationLimit = 30;
int firstHigherRotationLimit = 320;

//Second Input Counters
int secondInputCounter = 0;
int secondInputCounterLed = 0;
int secondLowerRotationLimit = 321;
int secondHigherRotationLimit = 720;

//Third Input Counters
int thirdInputCounter = 0;
int thirdInputCounterLed = 0;
int thirdLowerRotationLimit = 721;
int thirdHigherRotationLimit = 1023;

//Clock variables
int totalRotationToMinutesConverter = 0;
int rotationToHoursConverter = 0;
int rotationToMinutesConverter = 0;
int minMinutesInDay = 0;
int maxMinutesInDay = 1439;
unsigned long elapsedTimeBeforeClockReset = 0;
int days = 0;
int currentHours = 0;
int previousHours = 0;
int currentMinutes = 0;
int previousMinutes = 0;
int currentSeconds = 0;
int previousSeconds = 0;

//Alarm variables
int alarmTime = 0;
int alarmHour = 0;
int alarmMinute = 0;
int previousAlarmHour = 0;
int previousAlarmMinute = 0;


void setup() { 
    lcd.begin (NUMBER_OF_CHARACTERS,NUMBER_OF_ROWS);
    lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
    lcd.setBacklight(HIGH);
    lcd.clear();
    pinMode(blueLedPin, OUTPUT);
    pinMode(redLedPin, OUTPUT);
    pinMode(lightPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(rotationPin, INPUT);
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    Serial.begin(9600);
    //delay(1000);
}


void loop() { 
    rotationReading = analogRead(rotationPin);
    button1clock = digitalRead(button1);
    button2alarm = digitalRead(button2);

    adjustClockTime();
    displayClock();
    countingTime();
    adjustAlarm();
    displayAlarmHour();
    if (alarmIsRinging() == true) {
        inputFirstPosition();
        lightUpBlueLed();
        inputSecondPosition();
        lightUpRedLed();
        inputThirdPosition();
        turnOffLeds();
    }
    resetInputs();
}


void inputFirstPosition() {
    if (firstInputCounter < deactivationCounter and 
        rotationReading >= firstLowerRotationLimit and 
        rotationReading <= firstHigherRotationLimit and
        button1clock == notPressed and
        button2alarm == notPressed){
        firstPositionDelay();
    }
}

void firstPositionDelay() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        firstInputCounter++;
        firstInputCounterLed++;
      }
}

void inputSecondPosition() {
    if (secondInputCounter < deactivationCounter and 
        rotationReading >= secondLowerRotationLimit and 
        rotationReading <= secondHigherRotationLimit and
        button1clock == notPressed and
        button2alarm == notPressed){
        secondPositionDelay();
    }
}

void secondPositionDelay() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        secondInputCounter++;
        secondInputCounterLed++;
      }
}

void inputThirdPosition() {
    if (thirdInputCounter < deactivationCounter and 
        rotationReading >= thirdLowerRotationLimit and 
        rotationReading <= thirdHigherRotationLimit and
        button1clock == notPressed and
        button2alarm == notPressed){
        thirdPositionDelay();
    }
}

void thirdPositionDelay() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        thirdInputCounter++;
        thirdInputCounterLed++;
      }
}

bool alarmIsRinging() {
    if (((alarmHour == currentHours) and (alarmMinute == currentMinutes)) and 
        ((firstInputCounter != deactivationCounter) or 
        (secondInputCounter != deactivationCounter) or 
        (thirdInputCounter != deactivationCounter))) {
        tone(buzzerPin, buzzerFrequency);
        return true;
    } else {
        noTone(buzzerPin);
        return false;
    }
}

void resetInputs() {
    if (rotationReading == defaultCounter) {
        firstInputCounter = defaultCounter;
        firstInputCounterLed = defaultCounter;
        secondInputCounter = defaultCounter;
        secondInputCounterLed = defaultCounter;
        thirdInputCounter = defaultCounter;
        thirdInputCounterLed = defaultCounter;
    }
}

void lightUpBlueLed() {
    if (firstInputCounterLed >= deactivationCounter) {
        digitalWrite(blueLedPin, HIGH);
    } else {
        digitalWrite(blueLedPin, LOW);
    }
}

void lightUpRedLed() {
    if (secondInputCounterLed >= deactivationCounter) {
        digitalWrite(redLedPin, HIGH);
    } else {
        digitalWrite(redLedPin, LOW);
    }
}

void turnOffLeds() {
    if (thirdInputCounterLed >= deactivationCounter) {
        firstInputCounterLed = defaultCounter;
        secondInputCounterLed = defaultCounter;
        digitalWrite(redLedPin, LOW);
        digitalWrite(blueLedPin, LOW);
    }
}

void adjustAlarm() {
    if (button2alarm == pressed) {
        alarmTime = map(rotationReading, rotationMinReading, rotationMaxReading, minMinutesInDay, maxMinutesInDay);
        if (alarmTime <= 0) {
            alarmTime = 0;
        }
    convertRotationToAlarmTime();
    } 
}

void convertRotationToAlarmTime() {
    alarmHour = int (alarmTime / 60);
    alarmMinute = int (alarmTime % 60);
}

void displayAlarmHour() {
    if (alarmHour != previousAlarmHour) {
        lcd.setCursor(0,1);
        lcd.print("ALARM:");
        if (alarmHour < 10) {
            lcd.setCursor(7,1);
            lcd.print("  ");
            lcd.setCursor(7,1);
            lcd.print("0");
            lcd.setCursor(8,1);
            lcd.print(alarmHour);
        } else {
            lcd.setCursor(7,1);
            lcd.print("  ");
            lcd.setCursor(7,1);
            lcd.print(alarmHour);
        }
        previousAlarmHour = alarmHour;
    }
    if (alarmMinute != previousAlarmMinute) {
        lcd.setCursor(9,1);
        lcd.print(":");
        if (alarmMinute < 10) {
            lcd.setCursor(10,1);
            lcd.print("  ");
            lcd.setCursor(10,1);
            lcd.print("0");
            lcd.setCursor(11,1);
            lcd.print(alarmMinute);
        } else {
        lcd.setCursor(10,1);
        lcd.print("  ");
        lcd.setCursor(10,1);
        lcd.print(alarmMinute);
        }
        previousAlarmMinute = alarmMinute;
    }
}

void adjustClockTime() {
    if (button1clock == pressed) {
        totalRotationToMinutesConverter = map(rotationReading, rotationMinReading, rotationMaxReading, minMinutesInDay, maxMinutesInDay);
        elapsedTimeBeforeClockReset = millis();
        if (totalRotationToMinutesConverter <= 0) {
            totalRotationToMinutesConverter = 0;
        }
    convertRotationToClockTime();
    }
}

void convertRotationToClockTime() {
    rotationToHoursConverter = int (totalRotationToMinutesConverter / 60);
    rotationToMinutesConverter = int (totalRotationToMinutesConverter % 60);
    currentHours = rotationToHoursConverter;
    currentMinutes = rotationToMinutesConverter;
}

void displayClock() {
    if (currentHours != previousHours) {
        lcd.setCursor(0,0);
        lcd.print("CLOCK:");
        if (currentHours < 10) {
            lcd.setCursor(7,0);
            lcd.print("  ");
            lcd.setCursor(7,0);
            lcd.print("0");
            lcd.setCursor(8,0);
            lcd.print(currentHours);
        } else {
            lcd.setCursor(7,0);
            lcd.print("  ");
            lcd.setCursor(7,0);
            lcd.print(currentHours);
        }
        previousHours = currentHours;
    }
    if (currentMinutes != previousMinutes) {
        lcd.setCursor(9,0);
        lcd.print(":");
        if (currentMinutes < 10) {
            lcd.setCursor(10,0);
            lcd.print("  ");
            lcd.setCursor(10,0);
            lcd.print("0");
            lcd.setCursor(11,0);
            lcd.print(currentMinutes);
        } else {
            lcd.setCursor(10,0);
            lcd.print("  ");
            lcd.setCursor(10,0);
            lcd.print(currentMinutes);
        }
        previousMinutes = currentMinutes;
    }
    if (currentSeconds != previousSeconds) {
        lcd.setCursor(12,0);
        lcd.print(":");
        if (currentSeconds < 10) {
            lcd.setCursor(13,0);
            lcd.print("  ");
            lcd.setCursor(13,0);
            lcd.print("0");
            lcd.setCursor(14,0);
            lcd.print(currentSeconds);
        } else {
            lcd.setCursor(13,0);
            lcd.print("  ");
            lcd.setCursor(13,0);
            lcd.print(currentSeconds);
        }
        previousSeconds = currentSeconds;
    }
}

void countingTime() {
    unsigned long runningMillis = millis();
    currentSeconds = (runningMillis - elapsedTimeBeforeClockReset)/1000;
    if (currentSeconds >= 60) {
        currentMinutes = currentMinutes + 1;
        elapsedTimeBeforeClockReset = runningMillis;
        if (currentMinutes >= 60) {
            currentHours = currentHours + 1;
            currentMinutes = 0;
            if (currentHours >= 24) {
                currentHours = 0;
            }
        }
    }
}