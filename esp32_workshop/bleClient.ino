/**
 * Bluetotth Low Energy Client
 * Power-conserving variant of Bluetooth
 *   - Transmit small data, low power
 *   - !! Different comms protocol VS bluetooth
 * 
 * GATT: Generic Attributes, defines hierarchical structure
 * exposed to connected BLE devices / how devices send and 
 * receive standard messages. 
 * 
 * 1) Use same name and UUID as server.
 */ 

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
 
#define bleServerName "serverEsp"
 
//UUID's of the service, characteristic that we want to read and characteristic that we want to write.
static BLEUUID MyPushButtonServiceUUID("a20c412d-f325-416d-bdc1-da4fdcbb5b4c");
static BLEUUID button_1_CharacteristicUUID("9df66b03-ae85-4f4b-b249-451cc4d60ad0");
static BLEUUID button_2_CharacteristicUUID("a727f699-f683-4611-a27d-9d3c9894aebd");
 
//Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;
 
//LED pins number
int LED_1 = 22;
int LED_2 = 23;
 
//Pushbutton states storage varable
char* push_button_1_state;
char* push_button_2_state;
 
//Address of the PushButtonServer. Address will be found during scanning... Hopefully.
static BLEAddress *MyPushButtonServerAddress;
 
//Characteristic at the PushButtonServer that we want to read the pushbutton state in it.
static BLERemoteCharacteristic* button_1_Characteristic;
static BLERemoteCharacteristic* button_2_Characteristic;
 
//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};
 
//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
   BLEClient* pClient = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
 
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(MyPushButtonServiceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(MyPushButtonServiceUUID.toString().c_str());
    return (false);
  }
 
  // Obtain a reference to the characteristics in the service of the remote BLE server.
  button_1_Characteristic = pRemoteService->getCharacteristic(button_1_CharacteristicUUID);
  button_2_Characteristic = pRemoteService->getCharacteristic(button_2_CharacteristicUUID);
 
  if (button_1_Characteristic == nullptr || button_2_Characteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
 
  //Assign callback functions for the Characteristics
  button_1_Characteristic->registerForNotify(led_1_NotifyCallback);
  button_2_Characteristic->registerForNotify(led_2_NotifyCallback);
  return true;
}
 
//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) { //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop(); //Scan can be stopped, we found what we are looking for
      MyPushButtonServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser is the one we need
      doConnect = true; //Set indicator, stating that we are ready to connect
      Serial.println("Device found. Connecting!");
    }
  }
};
 
//When the BLE Server sends a new push button 1 state reading with the notify property
static void led_1_NotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store push_button 1 state value
  push_button_1_state = (char*)pData;
  Serial.println("LED 1 state :");
  Serial.println((int)*push_button_1_state);
  if ((int)*push_button_1_state == 1)
  {
    digitalWrite(LED_1,HIGH);
  }
  else if ((int)*push_button_1_state == 0)
  {
    digitalWrite(LED_1,LOW);
  }
}
 
//When the BLE Server sends a new push button 2 state reading with the notify property
static void led_2_NotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                    uint8_t* pData, size_t length, bool isNotify) {
  //store push_button 2 state value
  push_button_2_state = (char*)pData;
  Serial.println("LED 2 state :");
  Serial.println((int)*push_button_2_state);
 
   if ((int)*push_button_2_state == 1)
  {
   digitalWrite(LED_2,HIGH);
  }
  else if ((int)*push_button_2_state == 0)
  {
    digitalWrite(LED_2,LOW);
  }
}
 
void setup() {
  pinMode(LED_1,OUTPUT);
  pinMode(LED_2,OUTPUT);
  //Start serial communication
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
 
  //Init BLE device
  BLEDevice::init("Jason_ESP_Client");
 
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 60 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(60);
}
 
void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*MyPushButtonServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      button_1_Characteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      button_2_Characteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  
  delay(1000); // Delay a second between loops.
}