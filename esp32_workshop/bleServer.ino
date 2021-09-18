/**
 * Bluetotth Low Energy Server
 * Power-conserving variant of Bluetooth
 *   - Transmit small data, low power
 *   - !! Different comms protocol VS bluetooth
 * 
 * GATT: Generic Attributes, defines hierarchical structure
 * exposed to connected BLE devices / how devices send and 
 * receive standard messages. 
 * 
 * 1) Give name for ESP32 at init
 * 2) Create UUIDs
 */ 

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
 
int push_button_1 = 22;
int push_button_2 = 23;
 
BLECharacteristic *button_1_Characteristic;
BLECharacteristic *button_2_Characteristic;
bool deviceConnected = false;
uint8_t pushButton_1_Value = 0;
uint8_t pushButton_2_Value = 0;
 
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
 
#define PUSHBOTTON_SERVICE_UUID      "<Generate an UUID for Service>"
#define BUTTON_1_CHARACTERISTIC_UUID "<Generate an UUID for PB1 Characteristic>"
#define BUTTON_2_CHARACTERISTIC_UUID "<Generate an UUID for PB2 Characteristic>"
 
 
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("-- Client connected --");
    }
 
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("-- Client disconnected --");
    }
};
 
 
 
void setup() {
  pinMode(push_button_1,INPUT);
  pinMode(push_button_2,INPUT);
  
  Serial.begin(115200);
  // Create the BLE Device
  BLEDevice::init("<Give your server a name>");
 
  // Create the BLE Server
  BLEServer *MyPushButtonServer = BLEDevice::createServer();
  MyPushButtonServer->setCallbacks(new MyServerCallbacks());
 
  // Create the BLE Service
  BLEService *MyPushButtonService = MyPushButtonServer->createService(PUSHBOTTON_SERVICE_UUID);
 
  // Create a BLE Characteristic
  button_1_Characteristic = MyPushButtonService->createCharacteristic(
                      BUTTON_1_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE |
                      BLECharacteristic::PROPERTY_WRITE  
 
                    );
  button_2_Characteristic = MyPushButtonService->createCharacteristic(
                      BUTTON_2_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE |
                      BLECharacteristic::PROPERTY_WRITE 
                    );
  // Create a BLE Descriptor
  button_1_Characteristic->addDescriptor(new BLE2902());
  button_2_Characteristic->addDescriptor(new BLE2902());
 
 
  // Start the service
  MyPushButtonService->start();
 
  // Start advertising
  MyPushButtonServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}
 
void loop() {
 
  if (deviceConnected) {
    pushButton_1_Value = (uint8_t)digitalRead(push_button_1);
    pushButton_2_Value = (uint8_t)digitalRead(push_button_2);
    Serial.printf("*** NOTIFY: %d Pushbutton 1 and %d Pushbutton 2 ***\n", pushButton_1_Value,pushButton_2_Value);
    button_1_Characteristic->setValue(&pushButton_1_Value, 1);
    button_1_Characteristic->notify();
    button_2_Characteristic->setValue(&pushButton_2_Value, 1);
    button_2_Characteristic->notify();
  }
  delay(200);
}