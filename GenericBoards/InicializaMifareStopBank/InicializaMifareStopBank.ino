/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to try read the sector 10 blocked from Mifare classic tag
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


#define NR_KNOWN_KEYS 2
byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0x14, 0x15, 0x92, 0x65, 0x35, 0x89},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} // FF FF FF FF FF FF = factory default
};

/**
 * Initialize.
 */
void setup() {
    Serial.begin(115200); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    Serial.println(F("Initialize sector 10-15 from MIFARE Classic PICC."));
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

boolean tryKey(MFRC522::MIFARE_Key *key);
/**
 * Main loop.
 */
bool firstTime=true;
byte sector;
byte blockAddr;
byte trailerBlock;
void loop() {
   Serial.println("keys mais conhecidas...");
   delay(3000);
//gerador de keys mais conhecidas e utilizadas
   for (sector = 10; sector < 16; sector++) {
      blockAddr = sector*4;
      trailerBlock = blockAddr+3;
      if (makeKnownKeys()) {
      }
   }
   Serial.println("Again...");
   firstTime = true;
}

boolean makeKnownKeys() {
   for (byte k = 0; k < NR_KNOWN_KEYS; k++) {
      // Copy the known key into the MIFARE_Key structure
      for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
         key.keyByte[i] = knownKeys[k][i];
      }
//      printa(key.keyByte[0]);Serial.print(";");
//      printa(key.keyByte[1]);Serial.print(";");
//      printa(key.keyByte[2]);Serial.print(";");
//      printa(key.keyByte[3]);Serial.print(";");
//      printa(key.keyByte[4]);Serial.print(";");
//      printa(key.keyByte[5]);Serial.println();
      if (tryKey()) {
         return true;
      }
   }
   return false;
}

boolean tryKey() //MFRC522::MIFARE_Key *key)
{
   //stopBank keyA
   byte newKeyA[16] = {
      0x14, 0x15, 0x92, 0x65, 0x35, 0x89,  //keyA
      0xff, 0x07, 0x80, 0x69,              //access bits
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff   //keyB
      };

   byte buffer[18];
   byte byteCount = sizeof(buffer);
   boolean result = false;
   MFRC522::StatusCode status;
    
   mfrc522.PCD_Init(); // Init MFRC522 card

   Serial.print("Sector ");Serial.print(sector);
   Serial.print(F(" try key:"));
   dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
//   Serial.println();

   // Look for new cards
   int trys = 0;
   while ( ! mfrc522.PICC_IsNewCardPresent()) {
      if (trys++ > 5) {
         Serial.println(F(" No new card..."));
         return false;
      }
      delay(100);
   }

   // Select one of the cards
   if ( ! mfrc522.PICC_ReadCardSerial()) {
      Serial.println(F(" Read card error..."));
      return false;
   }

   Serial.println();

   if (firstTime) {
      firstTime = false;
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
         return false;
      }
   }

   status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
   if (status != MFRC522::STATUS_OK) {
       Serial.print(F(" PCD_Authenticate() failed: "));
       Serial.println(mfrc522.GetStatusCodeName(status));
      return false;
   }

   // Read block
   status = mfrc522.MIFARE_Read(blockAddr, buffer, &byteCount);
   if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
   }
   else {
      // Successful read
      result = true;
      // Dump block data
      Serial.print(F("Block ")); Serial.print(blockAddr); Serial.print(F(":"));
      dump_byte_array(buffer, 16);
      Serial.println();
      mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
//      Serial.println();
   }

   //Grava no trailer block
   status = mfrc522.MIFARE_Write(trailerBlock, newKeyA, 16);
   if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      //return;
   }
   else {
      Serial.println(F("KeyA gravada com sucesso!"));
   }
   Serial.println();

   mfrc522.PICC_HaltA();       // Halt PICC
   mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
   mfrc522.PCD_Reset();        // Reset PCD
   return result;
}

void printa(int i){
   Serial.print(i < 0x10 ? " 0" : " ");
   Serial.print(i, HEX);
}


