#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

BluetoothSerial SerialBT;

// coarse  Z32091
//const String RESPONSE1 = "?+00003157m2675319+0320026d+00003156*00+00+00062\x03"; //\x0D\x0A";  // Response with ETX
// fine 34093
//const String RESPONSE2 = "(+000021760m26822425+34706355d+000021751t00+0000+000**070\x03"; //\x0D\x0A";  // Response with ETX


// Variables to store the received values
String X = "0320026";
String Y = "2675319";
String L = "00003157";

String zeroPadL() {
  String padded = L;
  while (padded.length() < 8) {
    padded = "0" + padded;
  }
  return padded;
}
String zeroPadX() {
  String padded = X;
  while (padded.length() < 7) {
    padded = "0" + padded;
  }
  return padded;
}
String zeroPadY() {
  String padded = L;
  while (padded.length() < 7) {
    padded = "0" + padded;
  }
  return padded;
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Taheo"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
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
  
  // Check for incoming Bluetooth data
  if (SerialBT.available()) {
    String btInput = SerialBT.readStringUntil('\x03'); // ETX character
    btInput.trim();
    Serial.print("Received Bluetooth command: ");
    Serial.println(btInput);
    
    if (btInput == "C067") {
      SerialBT.println("\x06 006\x03");
      Serial.println("Sent response: ACK 006␃");
    } 
    else if (btInput == "Z32091") {
      String response = "?+" + String(zeroPadL()) + "m" + String(zeroPadY()) + "+" + String(zeroPadX()) + "d+" + String(zeroPadL()) + "*00+00+00062\x03";
      SerialBT.println(response);
      Serial.print("Sent response: ");
      Serial.println(response);
    }
    else if (btInput == "Z34093") {
      String response = "(+" + String(zeroPadL()) + "0m" + String(zeroPadY()) + "+" + String(zeroPadX()) + "0d+" + String(zeroPadL()) + "t00+0000+000**070\x03";
      SerialBT.println(response);
      Serial.print("Sent response: ");
      Serial.println(response);
    }
    else {
      Serial.print("Unknown req: ");
      Serial.println(btInput);
    }
  }
  
  delay(10); // Small delay to prevent busy waiting
}