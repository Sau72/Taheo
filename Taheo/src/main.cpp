#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// coarse  Z32091
//const String RESPONSE1 = "?+00003157m2675319+0320026d+00003156*00+00+00062\x03"; //\x0D\x0A";  // Response with ETX
// fine 34093
//const String RESPONSE2 = "(+000021760m26822425+34706355d+000021751t00+0000+000**070\x03"; //\x0D\x0A";  // Response with ETX


// Variables to store the received values
String X = "493015";
String Y = "880217";
String L = "00016577";

// Forward declaration
String zeroPadL(String input);
String zeroPadX(String input);
String zeroPadY(String input); 


BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
      // Restart advertising to allow reconnection
      pServer->getAdvertising()->start();
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      String btInput = String(value.c_str());
      btInput.trim();
      Serial.print("Received BLE command: ");
      Serial.println(btInput);

      if (btInput == "C067") {
        String response = "\x06 006\x03";
        pCharacteristic->setValue(response.c_str());
        pCharacteristic->notify();
        Serial.print("Sent response: ");
        Serial.println(response);
      } 
      else if (btInput == "Z32091") {
        String paddedL = zeroPadL(L);
        String response = "?+" + String(zeroPadL(L)) + "m" + String(zeroPadY(Y)) + "+" + String(zeroPadX(X)) + "d+" + String(zeroPadL(L)) + "*00+00+00062\x03";
        pCharacteristic->setValue(response.c_str());
        pCharacteristic->notify();
        Serial.print("Sent response: ");
        Serial.println(response);
      }
      else if (btInput == "Z34093") {
        String paddedL = zeroPadL(L);
        String response = "(+" + String(zeroPadL(L)) + "0m" + String(zeroPadY(Y)) + "0+" + String(zeroPadX(X)) + "0d+" + String(zeroPadL(L)) + "t00+0000+000**070\x03";
        pCharacteristic->setValue(response.c_str());
        pCharacteristic->notify();
        Serial.print("Sent response: ");
        Serial.println(response);
      }
      else {
        String response = "Unknown req";
        pCharacteristic->setValue(response.c_str());
        pCharacteristic->notify();
        Serial.print("Sent response: ");
        Serial.println(response);
      }
    }
  }
};

String zeroPadL(String input) {
  String padded = input;
  while (padded.length() < 8) {
    padded = "0" + padded;
  }
  return padded;
}
String zeroPadX(String input) {
  String padded = input;
  while (padded.length() < 7) {
    padded = "0" + padded;
  }
  return padded;
}
String zeroPadY(String input) {
  String padded = input;
  while (padded.length() < 7) {
    padded = "0" + padded;
  }
  return padded;
}

void setup() {
  Serial.begin(115200);
  // Initialize BLE
  BLEDevice::init("Taheo_BLE");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("BLE service started. Waiting for connections...");
}

void loop() {
 // Check for incoming serial data (X,Y,L format)
 if (Serial.available()) {
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  // Parse the input in "X,Y,L" format
  int firstComma = input.indexOf(',');
  int secondComma = input.indexOf(',', firstComma + 1);
  
  if (firstComma != -1 && secondComma != -1) {
    X = input.substring(0, firstComma);
    Y = input.substring(firstComma + 1, secondComma);
    L = input.substring(secondComma + 1);
    
    Serial.print("Received values - X: ");
    Serial.print(X);
    Serial.print(", Y: ");
    Serial.print(Y);
    Serial.print(", L: ");
    Serial.println(L);
    }
}

delay(10); // Small delay to prevent busy waiting
}
