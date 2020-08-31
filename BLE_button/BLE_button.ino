// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

//////////////
// Hardware //
//////////////
#define BTN_PIN    6 // BTN pin on 6
#define BTN_ACTIVE LOW

///////////////////////
// BLE Advertisments //
///////////////////////
const char * localName = "nRF52832 Button";
BLEPeripheral blePeriph;
BLEService bleServ("1234");
BLECharCharacteristic btnChar("1234", BLERead | BLENotify);

void setup() 
{
  Serial.begin(115200); // Set up serial at 115200 baud

  pinMode(BTN_PIN, INPUT_PULLUP);
  digitalWrite(7, HIGH);

  setupBLE();
}

void loop()
{
  blePeriph.poll();

  // read the current button pin state
  char buttonValue = digitalRead(BTN_PIN);

  // has the value changed since the last read
  bool buttonChanged = (btnChar.value() != buttonValue);

  if (buttonChanged) 
  {
    // button state changed, update characteristics
    btnChar.setValue(buttonValue);
  }
}

void setupBLE()
{
  // Advertise name and service:
  blePeriph.setDeviceName(localName);
  blePeriph.setLocalName(localName);
  blePeriph.setAdvertisedServiceUuid(bleServ.uuid());

  // Add service
  blePeriph.addAttribute(bleServ);

  // Add characteristic
  blePeriph.addAttribute(btnChar);

  // Now that device, service, characteristic are set up,
  // initialize BLE:
  blePeriph.begin(); 
}
