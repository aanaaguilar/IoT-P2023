#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 22 // Pin 22 for RC522 reset
#define SS_PIN 21  // Pin 21 for RC522 SS (SDA)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
    Serial.begin(9600); // Start serial communication
    SPI.begin();        // Start SPI bus
    mfrc522.PCD_Init(); // Initialize MFRC522
    Serial.println("Reading UID");
}

void loop() {
    delay(500); // Wait for half a second

    // Check if there are new cards present
    if (mfrc522.PICC_IsNewCardPresent()) {
        delay(50); // Wait a little bit

        // Select one of the cards
        if (mfrc522.PICC_ReadCardSerial()) {
            Serial.println("Card detected");

            // Print the card UID
            Serial.println("Card UID:");
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                Serial.print(mfrc522.uid.uidByte[i], HEX);
            }
            Serial.println();

            // Halt the current card
            mfrc522.PICC_HaltA();
        }
    } else {
        Serial.println("No card detected... waiting for one");
    }
}
