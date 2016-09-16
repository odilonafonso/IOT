/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 * 
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 * 
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

// Number of known default keys (hard-coded)
// NOTE: Synchronize the NR_KNOWN_KEYS define with the defaultKeys[] array
#define NR_KNOWN_KEYS   9
// Known keys, see: https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0x14, 0x15, 0x92, 0x65, 0x35, 0x89},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = factory default
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};

/**
 * Initialize.
 */
void setup() {
    Serial.begin(115200); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read all sectors."));
/*   
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
*/    
}


/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Main loop.
 */
void loop() {
   // Look for some cards
//   if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
//      return;
//   }
/*
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
*/
   // In this sample we use all sector,
   byte sector         = 1;
   byte blockAddr      = 4;
   byte trailerBlock   = 7;
   byte buffer[18];
   byte size = sizeof(buffer);
   MFRC522::StatusCode status;
   MFRC522::MIFARE_Key key;

   for (sector = 0; sector < 16; sector++){
      blockAddr = sector*4;
      trailerBlock = blockAddr+3;

      // Try the known default keys
      for (byte k = 0; k < NR_KNOWN_KEYS; k++) {
         // Copy the known key into the MIFARE_Key structure
         for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
            key.keyByte[i] = knownKeys[k][i];
         }

         mfrc522.PCD_Init(); // Init MFRC522 card
    
         Serial.print("Sector ");Serial.print(sector);

         // Look for new cards
         int trys = 0;
         while ( ! mfrc522.PICC_IsNewCardPresent()) {
            if (trys++ > 5) {
               Serial.println(F(" No new card..."));
               break;
            }
            delay(100);
         }
         if (trys > 5) {
            break;
         }
      
         // Select one of the cards
         if ( ! mfrc522.PICC_ReadCardSerial()) {
            Serial.println(F(" Read card error..."));
            break;
         }
         
          // Show some details of the PICC (that is: the tag/card)
          Serial.print(F(" Card UID:"));
          dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
          Serial.print(F(" ; PICC type: "));
          MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
          Serial.println(mfrc522.PICC_GetTypeName(piccType));
      
          // Check for compatibility
          if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
              &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
              &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
              Serial.println(F("This sample only works with MIFARE Classic cards."));
              return;
          }
      
         // Try the key
         Serial.print("try key# ");
         dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
         Serial.print(" ");
         // Authenticate using key A
         status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
         if (status != MFRC522::STATUS_OK) {
           Serial.print(F("PCD_Authenticate() failed: "));
           Serial.println(mfrc522.GetStatusCodeName(status));
         }
         else {
            Serial.println("Ok");
            // Show the whole sector as it currently is
            mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
            Serial.println();
            break;
         }
      }
      mfrc522.PICC_HaltA();       // Halt PICC
      mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
      mfrc522.PCD_Reset();        // Reset PCD
      delay(1000);
   }
   Serial.println("Again...");
   delay(2000);
}

