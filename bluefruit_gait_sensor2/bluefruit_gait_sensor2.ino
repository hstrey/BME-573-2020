//Use Adafruit Circuit Playgrond bluefruit as a peripheral


// Add Device Name.
const char* BLEDeviceName = "Gait Sensor";
#define NRF52_TIMER_INTERRUPT_DEBUG      1
#include <bluefruit.h>
#include "NRF52TimerInterrupt.h"
#include "NRF52_ISR_Timer.h"
#define HW_TIMER_INTERVAL_MS      1

#define OUT_PIN LED_RED

NRF52Timer ITimer(NRF_TIMER_1);

// Init NRF52_ISR_Timer
// Each NRF52_ISR_Timer can service 16 different ISR-based timers
NRF52_ISR_Timer ISR_Timer;

#define TIMER_INTERVAL_1S             1000L
#define TIMER_INTERVAL_50ms           50L
#define TIMER_INTERVAL_5ms           5L

int led_state = 0;

BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble

int taking_data = false;
int data_ready = false;
unsigned short sensorValue1 = 0;
unsigned short sensorValue2 = 0;
int adcin1 = A0;
int adcin2 = A2;

int i, factor;
int counter = 0;
uint8_t data[20];
uint8_t data_tbs[20] = {0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40};
//const char* data_const;
char* dataPointer;
char* valuePointer1;
char* valuePointer2;

const uint8_t GAIT_SERV_UUID[]  = { 0x10, 0xb4, 0xc6, 0xc6, 0xe0, 0x44, 0xaa, 0xb4, 0x67, 0x44, 0x3c, 0x1c, 0x05, 0xc6, 0x3b, 0x57 };
const uint8_t PRESS_CHAR_UUID[]  = { 0xe2, 0x3b, 0xc6, 0x05, 0x1c, 0x3c, 0x44, 0x67, 0xb4, 0xaa, 0x44, 0xe0, 0xc6, 0xc6, 0xb4, 0x10 };// create remote services
const uint8_t PERIOD_CHAR_UUID[]  = { 0x57, 0x3b, 0xc6, 0x05, 0x1c, 0x3c, 0x44, 0x67, 0xb4, 0xaa, 0x44, 0xe0, 0xc6, 0xc6, 0xb4, 0x10 };

// Create the remote sensor service
BLEService gaitService = BLEService(GAIT_SERV_UUID);

//Create remote sensor characteristics
BLECharacteristic pressure1Characteristic = BLECharacteristic(0x6ce2);
BLECharacteristic periodCharacteristic = BLECharacteristic(0x0ed1);

void setup() {
  pinMode(OUT_PIN, OUTPUT);
  digitalWrite(OUT_PIN, 1);
  pinMode(adcin1, INPUT_PULLUP);
  pinMode(adcin2, INPUT_PULLUP);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(14); // Can be 8, 10, 12 or 14

  valuePointer1 = (char*)&sensorValue1;
  valuePointer2 = (char*)&sensorValue2;
  dataPointer = (char*)&data;
  
//  Serial.begin(115200); // baud rate in bits/sec
//  while ( !Serial ) delay(10);   // For nrf52840 with native usb, and lets you print to Serial Monitor.
 
  // Initialize the Bluetooth function of board.
  setupBLE();

  // Set up the Service and Characteristics
  setupGaitServChar();

  // Advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(gaitService); //Include all service uuids
  Bluefruit.Advertising.addName(); // Include Name.
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds

  // Set up Timers
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
//    Serial.println("Starting  ITimer OK, millis() = " + String(millis()));
  }
  else {
//    Serial.println("Can't set ITimer correctly. Select another freq. or interval");

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_Timer

//  Serial.println("Setup completed. Ready for connection.");
  }
}

void loop() {
    if (data_ready) {
      pressure1Characteristic.notify(data_tbs,20);
      data_ready = false;
//      Serial.println("sent data package...");
    }
}

void TimerHandler(void)
{    
  ISR_Timer.run();
}

void setupBLE() {
  delay(500); // Delay a bit so the timestamp shows up.
  //Initialize Bluefruit module
  //Serial.println("Initialize the Bluefruit nRF52 module");
  // Set up and initialize Bluetooth
  Bluefruit.begin();
  //Configure and start device information service
  bledis.setManufacturer("Stryde");
  bledis.setModel("Gait sensor alpha");
  bledis.begin();
  // Set max power to one of the following accepted values:
  //  -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName(BLEDeviceName);

  // Set up the connect/disconnect callback handlers.
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
}



void setupGaitServChar() {
  gaitService.begin(); //BLECharacteristics cannot be added until BLEService.begin()
  //Serial.println("gait service has begun.");
  // Configure the characteristics.
  pressure1Characteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  pressure1Characteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  pressure1Characteristic.setFixedLen(20);
  pressure1Characteristic.setCccdWriteCallback(pressure1Service_cccd_callback); // Optionally capture CCCD updates
  pressure1Characteristic.begin();
  //Serial.println("pressure1Characteristic has begun.");
  
  // Configure the characteristics.
  periodCharacteristic.setProperties(CHR_PROPS_WRITE); 
  //Serial.println("period has been set to Write/Notify mode.");
  periodCharacteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  periodCharacteristic.setFixedLen(4);
  periodCharacteristic.setWriteCallback(periodService_callback);
  periodCharacteristic.begin(); 
  //Serial.println("periodCharacteristic has begun.");
}

void connect_callback(uint16_t conn_handle) {
  // Get the reference to the current connection.
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[16] = {0};
  connection->getPeerName(central_name, sizeof(central_name));

//  Serial.print("Connected to ");
//  Serial.println(central_name);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  //BLEConnection* connection = Bluefruit.Connection(conn_handle);
  
  (void) conn_handle;
  (void) reason;
  taking_data = false;
  ISR_Timer.disableAll();
  //  also reset the data taking parameters
  counter = 0;
  dataPointer = (char*)&data;
//  Serial.print("Disconnected, reason = 0x");
//  Serial.println(reason, HEX);
//  Serial.println("Ready for connection");
}

void pressure1Service_cccd_callback(uint16_t conn_hdl, BLECharacteristic* Pres1, uint16_t cccd_value) {
  if (Pres1->uuid == pressure1Characteristic.uuid) {
    if (Pres1->notifyEnabled(conn_hdl)) {
      taking_data = true;
      ISR_Timer.setInterval(TIMER_INTERVAL_50ms,  Timer_callback);
//      Serial.println("notify...");
    } else {
//      Serial.println("stopped notify...");
      taking_data = false;
      ISR_Timer.disableAll();
      //  also reset the data taking parameters
      counter = 0;
      dataPointer = (char*)&data;
    }
  }
}

void periodService_callback(uint16_t conn_hdl, BLECharacteristic* Period, uint8_t* per, uint16_t cccd_value) {
/*  if (Period->uuid == periodCharacteristic.uuid) {
    Serial.print("0");
   // if (Period->notifyEnabled(conn_hdl)) {
    if (Serial.available()) {
      Serial.print("1");

    //delay to wait for enough input
    delay(2);
   
    uint8_t buf[64];
    int count = Serial.readBytes(buf, sizeof(buf));
    bleuart.write( buf, count );

    //Forward from BLEUART to serial monitor
    if (bleuart.available()){
      Serial.print("2");
      uint8_t per;
      per = (uint8_t) bleuart.read();
      Serial.print("Period setting:");
      Serial.println(per);
      }
    }
  }
//  } */
int value = *per;
// Serial.println(value);
} 

void Timer_callback() {
    if (taking_data) {
      sensorValue1 = analogRead(adcin1);
      sensorValue2 = analogRead(adcin2);
//      Serial.print(sensorValue1);
//      Serial.print(",");
//      Serial.println(sensorValue2);

//      if(ecg_index < ArrayLength){
//        sensorValue = ECG[ecg_index];
//        ecg_index++;
//      }
//      else {
//        ecg_index = 0;
//        sensorValue = ECG[ecg_index];
//        ecg_index++;
//      }
      
      // copy value into char buffer
      for(int i = 0; i < 2; i++) {
        dataPointer[i] = valuePointer1[i];
      }
      dataPointer +=2;
      counter +=1;
      for(int i = 0; i < 2; i++) {
        dataPointer[i] = valuePointer2[i];
      }
      dataPointer +=2;
      counter +=1;
      if (counter>9) {
        memcpy(data_tbs,data,sizeof(data));
        data_ready = true;
        counter = 0;
        dataPointer = (char*)&data;
      }
   }
}
