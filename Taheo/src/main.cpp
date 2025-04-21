#include <Arduino.h>
#include "BluetoothSerial.h"

// Check if Bluetooth is properly enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

BluetoothSerial SerialBT;

// Delimiters and patterns
const char START_CHAR = 'Z';
const char END_CHAR = '\x03'; // ETX character (␃)
const String REQUEST_PATTERN1 = "32091";
// coarse
const String RESPONSE1 = "?+00003157m2675319+0320026d+00003156*00+00+00062\x03"; //\x0D\x0A";  // Response with ETX
// fine
const String REQUEST_PATTERN2 = "34093";
//const String RESPONSE2 = "(+000021760m26822425+34706355d+000021751t00+0000+000**070\x03"; //\x0D\x0A";  // Response with ETX
const String RESPONSE2 = "(+000165690m09022425+22930134d+000021751t00+0000+000**070\x03"; //\x0D\x0A";  // Response with ETX


void setup()
{
    Serial.begin(115200);

    // Start Bluetooth Serial with a device name
    SerialBT.begin("Taheo");

    Serial.println("Bluetooth device started, waiting for connections...");
}

void processRequest(String request)
{
    if (request == REQUEST_PATTERN1)
    {
        Serial.print("Received valid request: ");
        Serial.println(request);

        SerialBT.println(RESPONSE1);
        Serial.print("Sent response: ");
        Serial.println(RESPONSE1);
    }
    else if (request == REQUEST_PATTERN2)
    {
        Serial.print("Received valid request: ");
        Serial.println(request);

        SerialBT.println(RESPONSE2);
        Serial.print("Sent response: ");
        Serial.println(RESPONSE2);
    }
    else
    {
        Serial.print("Received invalid request: ");
        Serial.println(request);
    }
}

void loop()
{
    static String buffer = "";

    // Read from Bluetooth
    while (SerialBT.available())
    {
        char c = SerialBT.read();

        if (c == START_CHAR)
        {
            buffer = ""; // Start new message
        }
        else if (c == END_CHAR)
        {
            processRequest(buffer);
            buffer = "";
        }
        else
        {
            buffer += c;
        }
    }

    // Optional: Add small delay to prevent watchdog triggers
    delay(10);
}