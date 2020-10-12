
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
#include <string.h> 

BLEPeripheral gaitPeripheral = BLEPeripheral();
TimerClass timer(nrf_timer_num, cc_channel_num);

int taking_data = false;
unsigned short sensorValue = 0;  // variable to store the value coming from the sensor
int intervalmicsec = 5000;
int i,factor;
int counter = 0;
unsigned char data[20];
char* dataPointer;
char* valuePointer;
int ecg_index = 0;

unsigned int ECG[] = {1035,
            1035,
            1041,
            1041,
            1041,
            1046,
            1057,
            1068,
            1078,
            1084,
            1089,
            1089,
            1089,
            1094,
            1105,
            1116,
            1126,
            1143,
            1164,
            1175,
            1180,
            1180,
            1196,
            1223,
            1250,
            1266,
            1287,
            1309,
            1335,
            1357,
            1378,
            1410,
            1448,
            1475,
            1485,
            1512,
            1533,
            1555,
            1582,
            1608,
            1630,
            1646,
            1646,
            1651,
            1651,
            1646,
            1619,
            1582,
            1555,
            1528,
            1496,
            1453,
            1410,
            1384,
            1362,
            1325,
            1287,
            1266,
            1234,
            1218,
            1201,
            1169,
            1148,
            1132,
            1126,
            1126,
            1121,
            1121,
            1116,
            1116,
            1116,
            1116,
            1110,
            1110,
            1105,
            1094,
            1094,
            1094,
            1100,
            1105,
            1105,
            1110,
            1110,
            1100,
            1100,
            1105,
            1105,
            1105,
            1105,
            1105,
            1094,
            1089,
            1089,
            1089,
            1078,
            1078,
            1073,
            1073,
            1073,
            1068,
            1062,
            1057,
            1057,
            1046,
            1052,
            1057,
            1057,
            1052,
            1046,
            1046,
            1035,
            1035,
            1041,
            1041,
            1035,
            1035,
            1035,
            1041,
            1041,
            1041,
            1041,
            1041,
            1035,
            1035,
            1030,
            1030,
            1035,
            1035,
            1035,
            1030,
            1030,
            1030,
            1030,
            1030,
            1030,
            1035,
            1035,
            1030,
            1035,
            1035,
            1025,
            1025,
            1025,
            1025,
            1035,
            1035,
            1035,
            1035,
            1035,
            1030,
            1030,
            1035,
            1041,
            1041,
            1041,
            1041,
            1041,
            1041,
            1035,
            1035,
            1041,
            1052,
            1078,
            1089,
            1089,
            1089,
            1084,
            1105,
            1132,
            1169,
            1185,
            1185,
            1185,
            1185,
            1180,
            1175,
            1148,
            1110,
            1084,
            1068,
            1062,
            1057,
            1035,
            1035,
            1025,
            1030,
            1030,
            1025,
            1025,
            1014,
            1014,
            1014,
            1019,
            1014,
            1003,
            998,
            1003,
            1003,
            998,
            993,
            918,
            880,
            902,
            1110,
            1496,
            1999,
            2583,
            3129,
            3531,
            3595,
            3161,
            2272,
            1303,
            666,
            500,
            570,
            666,
            682,
            661,
            612,
            607,
            607,
            607,
            623,
            612,
            629,
            682,
            757,
            827,
            880,
            918,
            944,
            982,
            998,
            1014,
            1019,
            1025,
            1030,
            1025,
            1030,
            1035};
int ArrayLength = (sizeof(ECG)/sizeof(*ECG));

// create remote services
BLEService GaitService = BLEService("573bc6051c3c4467b4aa44e0c6c6b410");

// create remote characteristics
BLECharacteristic pressureCharacteristic = BLECharacteristic("5a8ac39c0ce142128b46d589ba126ce2", BLERead | BLENotify ,20);
BLECharacteristic periodCharacteristic = BLECharacteristic("f8bc0798e4474625959fce6970b70ed1", BLERead | BLEWrite,4);


void setup() {
  pinMode(OUT_PIN, OUTPUT);
  digitalWrite(OUT_PIN, 1);
//  pinMode(anio2, INPUT_PULLUP);
  
  valuePointer = (char*)&sensorValue;
  dataPointer = (char*)&data;
  

  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  
  // trying to find out what the size of int is for RF52
  //Serial.print("sizeof int: ");
  //Serial.println(sizeof(intervalmicsec));
  
  gaitPeripheral.setLocalName("Pressure-Sensor");

  // set device name and appearance
  gaitPeripheral.setDeviceName("Pressure-Sensor1");
  gaitPeripheral.setAppearance(0x0080);

  gaitPeripheral.addAttribute(GaitService);
  gaitPeripheral.addAttribute(pressureCharacteristic);
  gaitPeripheral.addAttribute(periodCharacteristic);
  
  gaitPeripheral.setAdvertisedServiceUuid(GaitService.uuid());

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
  BLEUtil::printBuffer(characteristic.value(),characteristic.valueLength());
  intervalmicsec = 0;
  factor = 1;
  for (i=3;i>=0;--i) {
    intervalmicsec += int(characteristic.value()[i])*factor;
    factor = factor*256;
  }
  Serial.print(F("after conversion: "));
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
//      sensorValue = analogRead(anio2);

    if(ecg_index < ArrayLength){
      sensorValue = ECG[ecg_index];
      ecg_index++;
    }
    else {
      ecg_index = 0;
      sensorValue = ECG[ecg_index];
      ecg_index++;
    }

      
      // copy value into char buffer
      for(int i = 0; i < 2; i++) {
        dataPointer[i] = valuePointer[i];
      }
      dataPointer +=2;
      counter +=1;
      if (counter>9) {
        pressureCharacteristic.setValue(data,20);
        counter = 0;
        dataPointer = (char*)&data;
        Serial.print("sent data package...");
      }
      Serial.print(F("Measuring... "));
      Serial.println(sensorValue);
   }
}
