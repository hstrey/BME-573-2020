
// Copyright (c) Helmut H. Strey.  All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <nrf.h>
#include "nrf_timer.h"
#include "Timer.h"

#define OUT_PIN  7

#define nrf_timer_num   (1)
#define cc_channel_num  (0)
#define anio2 2

#include <SPI.h>
#include <BLEPeripheral.h>
#include <BLEUtil.h>

BLEPeripheral gaitPeripheral = BLEPeripheral();
TimerClass timer(nrf_timer_num, cc_channel_num);

int taking_data = false;
unsigned short sensorValue = 0;  // variable to store the value coming from the sensor
int intervalmicsec = 5000;
int factor=1;
int i;
int counter = 0;
unsigned char data[20];
char* dataPointer;
char* sensorPointer;

//create remote service
BLEService GaitService = BLEService("573bc6051c3c4467b4aa44e0c6c6b410");

// create remote characteristics
BLECharacteristic pressureCharacteristic = BLECharacteristic("5a8ac39c0ce142128b46d589ba126ce2", BLERead | BLENotify ,20);
BLECharacteristic periodCharacteristic = BLECharacteristic("f8bc0798e4474625959fce6970b70ed1", BLERead | BLEWrite,4);

void setup() {
  pinMode(OUT_PIN, OUTPUT);
  digitalWrite(OUT_PIN, 1);
  pinMode(anio2, INPUT_PULLUP);

  dataPointer = (char*)&data;
  sensorPointer = (char*)&sensorValue;
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  gaitPeripheral.setLocalName("Pressure-Sensor1");

  // set device name and appearance
  gaitPeripheral.setDeviceName("Pressure-Sensor");
  gaitPeripheral.setAppearance(0x0080);

  gaitPeripheral.addAttribute(GaitService);
  gaitPeripheral.addAttribute(pressureCharacteristic);
  gaitPeripheral.addAttribute(periodCharacteristic);

  // assign event handlers for connected, disconnected to peripheral
  gaitPeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  gaitPeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  pressureCharacteristic.setEventHandler(BLESubscribed, pressureCharacteristicSubscribedHandle);
  pressureCharacteristic.setEventHandler(BLEUnsubscribed, pressureCharacteristicUnSubscribedHandle);
  periodCharacteristic.setEventHandler(BLEWritten, pressureCharacteristicValueWrittenHandle);

  // begin initialization
  gaitPeripheral.begin();
  
  Serial.println(F("BLE started..."));
}

void loop() {
  // put your main code here, to run repeatedly:
  gaitPeripheral.poll();
     // Sleep
    __WFE(); // Enter System ON sleep mode (OFF mode would stop timers)
    __SEV(); // Make sure any pending events are cleared
    __WFE(); // More info about this sequence:
// devzone.nordicsemi.com/f/nordic-q-a/490/how-do-you-put-the-nrf51822-chip-to-sleep/2571
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}

void pressureCharacteristicValueWrittenHandle(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Written characteristic value: "));

  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
  factor = 1;
  intervalmicsec = 0;
  for (i=3;i>=0;--i) {
    intervalmicsec += int(characteristic.value()[i])*factor;
    factor = factor *256;
  }
  Serial.print(F("Value after conversion: "));
  Serial.println(intervalmicsec);
}

void pressureCharacteristicSubscribedHandle(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Subscribed to characteristic value: "));
  taking_data = true;
  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
  timer.attachInterrupt(&Timer_callback, intervalmicsec); // microseconds
  taking_data = true;
  digitalWrite(OUT_PIN, 0);
}

void pressureCharacteristicUnSubscribedHandle(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Unsubscribed to characteristic value: "));
  taking_data = false;
  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
  digitalWrite(OUT_PIN, 1);
}

void Timer_callback() {
    if (taking_data) {
      timer.attachInterrupt(&Timer_callback, intervalmicsec); // microseconds
      sensorValue = analogRead(anio2);
      // copy value into char buffer
      for (i=0;i<2;i++){
        dataPointer[i] = sensorPointer[i];
      }
      dataPointer += 2;
      counter +=1;
      if (counter>9) {
        pressureCharacteristic.setValue(data,20);
        counter=0;
        dataPointer = (char*)&data;
        Serial.println("sent data package...");
      }
      
      Serial.print(F("Measuring... "));
      Serial.println(sensorValue);
   }
}
