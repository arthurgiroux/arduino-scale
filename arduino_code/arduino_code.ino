#include "HX711.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();

// This value was found by calling set_scale() and tare().
// putting an object of known weight (101g) and putting it on the scale.
// averaging the results of scale.get_units(10);
// you then take this average and divide it by the weight of your object.
const float calibrationFactor = 21780.0f / 101.0f;

const int scaleDOUTPin = 3;
const int scaleCLKPin = 2;

const int powerButtonPin = 8;
const int tareButtonPin = 9;
bool displayIsOn = true;
bool waitForPowerButtonReset = false;

bool waitForTareButtonReset = false;

HX711 scale;

void setup() {
  Serial.begin(9600);

  // Initialize the scale
  scale.begin(scaleDOUTPin, scaleCLKPin);
  scale.set_scale(calibrationFactor);
  scale.tare();

  // Initialize the screen
  matrix.begin(0x70);
  matrix.clear();
  matrix.writeDisplay();

  // Initialize the buttons
  pinMode(powerButtonPin, INPUT);
  pinMode(tareButtonPin, INPUT);
}

void loop() {

  // if the power button is pressed then we switch on/off the screen
  int powerButtonIsPressed = digitalRead(powerButtonPin);
  if (powerButtonIsPressed) {
    // we only do something at the rising edge
    if (!waitForPowerButtonReset) {
      Serial.println("power button pressed");
      scale.tare();
      displayIsOn = !displayIsOn;
      waitForPowerButtonReset = true;
      matrix.clear();
      matrix.writeDisplay();
    }
  }
  else {
    waitForPowerButtonReset = false;
  }

  int tareButtonIsPressed = digitalRead(tareButtonPin);
  if (tareButtonIsPressed) {
    // we only do something at the rising edge
    if (!waitForTareButtonReset) {
      Serial.println("tare button pressed");
      waitForTareButtonReset = true;
      scale.tare();
    }
  }
  else {
    waitForTareButtonReset = false;
  }

  // we retrieve the readings of the scale and display them
  if (displayIsOn) {
    int value = scale.get_units();
    matrix.print(value);
    Serial.println(value);
    matrix.writeDisplay();
  }
}
