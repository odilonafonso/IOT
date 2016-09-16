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

    Serial.println(F("Scan a sector 10 from MIFARE Classic PICC."));
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
void loop() {
   // Look for some cards
   if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
//      Serial.println(F("No card..."));
      return;
   }
/*
   // Look for new cards
   if ( ! mfrc522.PICC_IsNewCardPresent())
     return;

   // Select one of the cards
   if ( ! mfrc522.PICC_ReadCardSerial())
     return;
*/
   if (firstTime) {
      firstTime = false;
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F("Card UID:"));
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
   }
//   return;


/*
   for (sector = 0; sector < 16; sector++){
      blockAddr = sector*4;
      trailerBlock = blockAddr+3;

      Serial.print("Currente data in sector: "); Serial.println(sector);
*/

//gerador de todas as keys possiveis
//   if (allKeys()) {
//      Serial.print("Achou key !! ");
//      dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
//      Serial.println();
//      return;
//   }

   Serial.println("keys mais conhecidas...");
   delay(3000);
//gerador de keys mais conhecidas e utilizadas
   if (makeKnownKeys()) {
      Serial.print("Achou key !! ");
      dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
      Serial.println();
      return;
   }

//gerador de keys algoritmo odilon 
   if (odiKeys()) {
      Serial.print("Achou key !! ");
      dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
      Serial.println();
      return;
   }
   delay(3000);

/*
   }
*/
   Serial.println("Again...");
}

boolean makeKnownKeys() {
   // Try the known default keys
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

boolean odiKeys() {
/*   
 * loop 1
 * variar 256 combinações de keys com os 6 bytes idênticos.
 * 00-00-00-00-00-00
 * 01-01-01-01-01-01
 * ...
 * ff-ff-ff-ff-ff-ff
 * 
 */
   Serial.println("loop1...");
   delay(3000);
   for (int k=0; k<256; k++) {
      for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
         key.keyByte[i] = k;
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
   Serial.println("loop2...");
   delay(3000);
/*   
 * loop 2
 * variar 256 combinações onde:
 * k0: 00 a FF
 * k1: k0+1
 * k2: k1+1
 * ...
 * k5: k4+1
 * 
 * 00-01-02-03-04-05
 * 01-02-03-04-05-06
 * ...
 * ff-00-01-02-03-04
 * 
 */
   int k[6];
   for (k[0]=0; k[0]<256; k[0]++) {
      for (byte i = 1; i < 6; i++)
         k[i] = (k[i-1] == 255 ? 0 : k[i-1] + 1);
      for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
         key.keyByte[i] = k[i];
      }
      if (tryKey()) {
         return true;
      }
   }

/*   
 * loop 3
 * variar 256 combinações onde:
 * k0: ff a 00
 * k1: k0-1
 * k2: k1-1
 * ...
 * k5: k4-1
 * 
 * ff-fe-fd-fc-fb-fa
 * fe-fd-fc-fb-fa-f9
 * ...
 * 00-ff-fe-fd-fc-fb
 * 
 */
//   int k[6];
   Serial.println("loop3...");
   delay(3000);
   for (k[0]=255; k[0]>0; k[0]--) {
      for (byte i = 1; i < 6; i++)
         k[i] = (k[i-1] == 0 ? 255 : k[i-1] - 1);
      for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
         key.keyByte[i] = k[i];
      }
      if (tryKey()) {
         return true;
      }
   }
}

boolean allKeys() {
/*
 * k - varia de 0 a 5; bytes da key gerada
 * l - varia de 0 a 16; valores que cada byte pode receber
 * 
 * 00-00-00-00-00-00
 * 00-00-00-00-00-01
 * ...
 * 00-00-00-00-00-ff
 * 00-00-00-00-01-00
 * 00-00-00-00-01-01
 * ...
 * 00-00-00-00-01-ff
 * 00-00-00-00-02-00
 * 00-00-00-00-02-01
 * ...
 * 00-00-00-00-02-ff
 * ...
 * 00-00-00-00-ff-00
 * 00-00-00-00-ff-01
 * ...
 * 00-00-00-00-ff-ff
 * 
 * 
 */
   int k[6];
   for (k[0] = 0; k[0] < 256; k[0]++) {
      for (k[1] = 0; k[1] < 256; k[1]++) {
         for (k[2] = 0; k[2] < 256; k[2]++) {
            for (k[3] = 0; k[3] < 256; k[3]++) {
               for (k[4] = 0; k[4] < 256; k[4]++) {
                  for (k[5] = 0; k[5] < 256; k[5]++) {
//                     printa(k[0]);Serial.print(";");
//                     printa(k[1]);Serial.print(";");
//                     printa(k[2]);Serial.print(";");
//                     printa(k[3]);Serial.print(";");
//                     printa(k[4]);Serial.print(";");
//                     printa(k[5]);Serial.println();
                     for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
                        key.keyByte[i] = k[i];
                     }
                     //key e global; não precisa ser passada
//                     if (tryKey(&key)) {
                     if (tryKey()) {
                        return true;
                     }
                  }
               }
            }
         }
      }
   }
}

boolean tryKey() //MFRC522::MIFARE_Key *key)
{
   // In this sample we use sector 10,(blocked)
   byte sector         = 10;
   byte blockAddr      = 40;
   byte trailerBlock   = 43;
   byte buffer[18];
   byte byteCount = sizeof(buffer);
   boolean result = false;
   MFRC522::StatusCode status;
    
   // Serial.println(F("Authenticating using key A..."));
//   if (key.keyByte[5] == 0) {
      dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
//      Serial.println();
//   }
   status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
   if (status != MFRC522::STATUS_OK) {
       Serial.print(F(" PCD_Authenticate() failed: "));
       Serial.println(mfrc522.GetStatusCodeName(status));
      return false;
   }

   // Read block
   status = mfrc522.MIFARE_Read(blockAddr, buffer, &byteCount);
   if (status != MFRC522::STATUS_OK) {
      // Serial.print(F("MIFARE_Read() failed: "));
      // Serial.println(mfrc522.GetStatusCodeName(status));
   }
   else {
      // Successful read
      result = true;
      Serial.print(F("Success with key:"));
      dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
      Serial.println();
      // Dump block data
      Serial.print(F("Block ")); Serial.print(blockAddr); Serial.print(F(":"));
      dump_byte_array(buffer, 16);
      Serial.println();
      mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
      Serial.println();
   }
   Serial.println();

   mfrc522.PICC_HaltA();       // Halt PICC
   mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
   return result;
}

void printa(int i){
   Serial.print(i < 0x10 ? " 0" : " ");
   Serial.print(i, HEX);
}


