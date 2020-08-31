
// Copyright (c) Helmut H. Strey.  All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <BLEUtil.h>

BLEPeripheral gaitPeripheral = BLEPeripheral();

// create remote services
BLEService GaitService = BLEService("573bc6051c3c4467b4aa44e0c6c6b410");

// create remote characteristics
BLECharacteristic pressure1Characteristic = BLECharacteristic("5a8ac39c0ce142128b46d589ba126ce2", BLERead | BLENotify ,20);
BLECharacteristic pressure2Characteristic = BLECharacteristic("f8bc0798e4474625959fce6970b70ed1", BLEWrite | BLEWriteWithoutResponse,20);

int taking_data = false;
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  gaitPeripheral.setLocalName("Pressure-Sensor1");

  // set device name and appearance
  gaitPeripheral.setDeviceName("Pressure-Sensor");
  gaitPeripheral.setAppearance(0x0080);

  gaitPeripheral.addAttribute(GaitService);
  gaitPeripheral.addAttribute(pressure1Characteristic);
  gaitPeripheral.addAttribute(pressure2Characteristic);

  // assign event handlers for connected, disconnected to peripheral
  gaitPeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  gaitPeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  gaitPeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralServicesDiscoveredHandler);

  // assign event handlers for characteristic
  pressure1Characteristic.setEventHandler(BLESubscribed, pressureCharacteristicSubscribedHandle);
  pressure1Characteristic.setEventHandler(BLEUnsubscribed, pressureCharacteristicUnSubscribedHandle);
  pressure2Characteristic.setEventHandler(BLEWritten, pressureCharacteristicValueWrittenHandle);

  // begin initialization
  gaitPeripheral.begin();
  


  Serial.println(F("BLE Peri"));
}

void loop() {
  // put your main code here, to run repeatedly:
  gaitPeripheral.poll();
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

void blePeripheralServicesDiscoveredHandler(BLECentral& central) {
  // central remote services discovered event handler
  Serial.print(F("Services discovered event, central: "));
  Serial.println(central.address());
}

void pressureCharacteristicValueWrittenHandle(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Written characteristic value: "));

  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
}

void pressureCharacteristicSubscribedHandle(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Subscribed to characteristic value: "));
  taking_data = true;
  timer.attachInterrupt(&Timer_callback, 1000000); // microseconds
  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
}

void pressureCharacteristicUnSubscribedHandle(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Unsubscribed to characteristic value: "));
  taking_data = false;
  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
}
