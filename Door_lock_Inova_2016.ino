/*
 * ----------------------------------------------------------------------------
 * This sketch uses the MFRC522 library ; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * This sketch show a simple locking mechanism using the RC522 RFID module.
 * ----------------------------------------------------------------------------
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
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

String read_rfid;
String ok_rfid_1="9166a85";
//String ok_rfid_2="ffffffff"; //add as many as you need.
//Use the lines below if you plan on using a servo as a locking mechanism.
#include <Servo.h> 
Servo myservo;  // create servo object to control a servo 
int posClosed = 0;    // variable to store the servo position for locked
int posOpen = 360;    //same for open...
 
char* password = "7298";
int position = 0;
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

byte rowPins[ROWS] = { A1, A2, 6, 5 };
byte colPins[COLS] = { 4, 3, 2 };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int RedpinLock = 8;
int GreenpinUnlock = 7;
/*
 * Initialize.
 */
void setup() {
    Serial.begin(9600);         // Initialize serial communications with the PC
    while (!Serial);            // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();                // Init SPI bus
    mfrc522.PCD_Init();         // Init MFRC522 card
    myservo.attach(A0);  // attaches the servo on pin 2 to the servo object 

    lcd.begin(16,2);
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("INOVA 2016");
    lcd.setCursor(0, 1);
    lcd.print("CTI Tesla");
    delay(2000);
    lcd.clear();
    myservo.write(0);

    digitalWrite(RedpinLock, OUTPUT);
    digitalWrite(GreenpinUnlock, OUTPUT);

    LockedPosition(true);
}

/*
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    read_rfid="";
    for (byte i = 0; i < bufferSize; i++) {
        read_rfid=read_rfid + String(buffer[i], HEX);
    }
}

void open_lock() {
  //Use this routine when working with Servos.
  lcd.setCursor(0, 0);
  lcd.print("RFID used");
  lcd.setCursor(0, 1);
  lcd.print("Welcome");
  LockedPosition(false);
  myservo.write(180); 
  delay(5000);
  myservo.write(0);
  LockedPosition(true);
  lcd.clear();
}

void LockedPosition(int locked)
{
if (locked)
{
digitalWrite(RedpinLock, HIGH);
digitalWrite(GreenpinUnlock, LOW);
myservo.write(11);
}
else
{
digitalWrite(RedpinLock, LOW);
digitalWrite(GreenpinUnlock, HIGH);
myservo.write(90);
}
}

void RFID(){
// Look for new cards
    LockedPosition(true);
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println(read_rfid);
    if (read_rfid==ok_rfid_1) {
      //ok, open the door.
      open_lock();
    }
    else  {
      lcd.setCursor(0, 0);
      lcd.print("RFID used");
      lcd.setCursor(0, 1);
      lcd.print("Get out You!");
      delay(1000);
      lcd.clear();
    }  
}

void myKeypad(){
  LockedPosition(true);
  char key = keypad.getKey();

    Serial.println(password[position]);

    if (key == password[position])
    {
    position ++;
    }
    if (position == 4)
    {
    lcd.setCursor(0, 0);
    lcd.print("Keypad used");
    lcd.setCursor(0, 1);
    lcd.print("Welcome");
    LockedPosition(false);
    delay(5000);
    LockedPosition(true);
    position = 0;
    lcd.clear();
    }
    delay(50);
}
void loop() {
 RFID();
  myKeypad();
    //Add below as many "keys" as you want
    //if (read_rfid==ok_rfid_2) {
      //also ok, open the door
    //  open_lock();
    //}
    // else not needed. Anything else is not ok, and will not open the door...
}
