/*
 * BDC OS 0.4 - prototype firmware for the Battery Discharge Controller (BDC) REV1
 * https://github.com/marvinramm/BDC
 *
 * Copyright (c) 2022 Marvin Ramm
 * SPDX-License-Identifier: MIT
 *
 * Board: "ESP32 Dev Module", Partition Scheme: "Default 4MB with spiffs"
 */

///////////////////Includes//////////////////

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <INA219_WE.h>
#include <CD74HC4067.h>

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"

//////////////////Variables//////////////////

long PreviousMillis;

/////////////////////////////////////////////

////////////////////Setup////////////////////

void setup(void) {
  HardwareInit();

}

////////////////////LOOP/////////////////////

void loop() {
  
  CheckRotaryEncoder();
  SetPage();
  
  if (millis() - PreviousMillis >= 500) { //Every 500ms
    PreviousMillis = millis();
    ReadINA219();
    SetBatteryLoad();
    
    UpdateDisplay();

    //PrintDataSerial();
  }
  
}
