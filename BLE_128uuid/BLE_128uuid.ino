#include <bluefruit.h>

 // Custom UUID used to differentiate this device.
const uint8_t CUSTOM_UUID[]  = { 0x57, 0x3b, 0xc6, 0x05, 0x1c, 0x3c, 0x44, 0x67, 0xb4, 0xaa, 0x44, 0xe0, 0xc6, 0xc6, 0xb4, 0x10 };

BLEService        bsvc = BLEService(CUSTOM_UUID);

void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  // Initialise the Bluefruit module
  Serial.println("Initialise the Bluefruit nRF52 module");
  Bluefruit.begin();

  // Set the advertised device name (keep it short!)
  Serial.println("Setting Device Name to 'Bluefruit52 Custom'");
  Bluefruit.setName("Bluefruit52 Custom");

  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Setup the Custom service using
  Serial.println("Configuring the Custom Service");
  setupService();

  // Setup the advertising packet(s)
  Serial.println("Setting up the advertising payload(s)");
  startAdv();

  Serial.println("\nAdvertising");
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include Service UUID
  Bluefruit.Advertising.addService(bsvc);

  // Include Name
  Bluefruit.Advertising.addName();
 
  // Start Advertising
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds 
}

void setupService(void)
{
  bsvc.begin();

}

void connect_callback(uint16_t conn_handle) {
  // Get the reference to the current connection.
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[16] = {0};
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  //BLEConnection* connection = Bluefruit.Connection(conn_handle);
  
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
  Serial.println("Ready for connection");
}

void loop() {
}
