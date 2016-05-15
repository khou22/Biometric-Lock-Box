// Biometric Lock Box Final Arduino Code
// April 8, 2016
// References:
// Keypad Schematic: http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Components/General/SparkfunCOM-08653_Datasheet.pdf

// Libraries
#include <Key.h> // Keypad
#include <Keypad.h> // Keypad
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

int getFingerprintIDez();

// pin #11 is IN from sensor (GREEN wire)
// pin #12 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(11, 12);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const byte ROWS = 4; // Four keypad rows
const byte COLS = 3; // Three keypad columns

// Define keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

const int solenoidPin = 13; // Soilenoid pin
byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4}; //connect to the column pinouts of the keypad
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Create the Keypad

// Keypad Password
String masterPassword = "1234"; // Password
char triggerKey = '#'; // Trigger to begin password collection

// User Data
String currentPassword = "";
boolean collecting = false;

// Preferences
int unlockTime = 4000; // MS the box will be unlocked for

void setup() {
  Serial.begin(9600); // Begin Serial communication
  pinMode(solenoidPin, OUTPUT); // Define solenoid output pin

  // Fingerpint sensor setup
  finger.begin(57600);

  Serial.println("In setup");
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial.println("Waiting for valid finger...");
}

void loop() {
  Serial.println("Looping");
  
  // Keypad password collection
  char key = kpd.getKey(); // Store key press

  if (key) { // If valid key pressed
    Serial.println(key);
    if (!collecting) { // If not already collecting
      if (key == triggerKey) { // If trigger to start password collection
        toggleCollecting(true); // Toggle the state of collecting
      }
    } else { // If collecting
      currentPassword += key; // Add the latest key
      if (currentPassword.length() == 4) { // If password finished
        Serial.println("Entered password: " + currentPassword);
        if (currentPassword == masterPassword) { // If correct password
          correctPassword(); // Run correct password function
        } else {
          incorrectPassword(); // Run incorrect password function
        }
        currentPassword = ""; // Reset password
        toggleCollecting(false); // Toggle the state of collecting
      }
    }
  } else {
    Serial.println("Waiting...");
    // Fingerprint password
    getFingerprintIDez();
  }
  delay(50); // Don't ned to run this at full speed.
}

void toggleCollecting(boolean state) {
  collecting = state; // Set to true/false state
  if (state) {
    Serial.println("Starting password collection");
  } else {
    Serial.println("Password collection stopped. Waiting for trigger key...");
  }
}

void correctPassword() {
  Serial.println("Correct password");
  unlockSolenoid(unlockTime);
}

void incorrectPassword() {
  Serial.println("Incorrect password. Please try again.");
}

void unlockSolenoid(int unlockTime) {
  digitalWrite(solenoidPin, HIGH);
  Serial.println("Unlocking");
  delay(unlockTime);
  digitalWrite(solenoidPin, LOW);
  Serial.println("Locking");
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
}

int getFingerprintIDez() {
  Serial.println("Reading fingerprint");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}

