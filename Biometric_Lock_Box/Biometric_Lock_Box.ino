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

// LED Pins
int greenPin = 1;
int yellowPin = 0;
int redPin = 9;

// User Data
String currentPassword = "";
boolean collecting = false;
boolean locked = true;
int latestConfidence = 0;

// Preferences
int unlockTime = 5000; // MS the box will be unlocked for

void setup() {
//  // Serial.begin(9600); // Begin // Serial communication
  pinMode(solenoidPin, OUTPUT); // Define solenoid output pin

  // Initialize LEDs
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  // Fingerpint sensor setup
  finger.begin(57600);

//  // Serial.println("In setup");
  if (finger.verifyPassword()) {
//    // Serial.println("Found fingerprint sensor!");
  } else {
//    // Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  // Serial.println("Waiting for valid finger...");
  toggleLED("red"); // Start with red on
}

void loop() {
  if (locked) { // If box is locked
    char key = kpd.getKey();
    if (key) {
//      // Serial.println("Pressed key");
//      // Serial.println(key);
      if (!collecting) { // If not already collecting
        if (key == triggerKey) { // If trigger to start password collection
          toggleCollecting(true); // Toggle the state of collecting
        }
      } else { // If collecting
//        toggleLED("yellow"); // Turn on just yellow
        digitalWrite(greenPin, HIGH);
        currentPassword += key; // Add the latest key
        delay(100);
        digitalWrite(greenPin, LOW);
        if (key == triggerKey) { // If hit trigger key again
          currentPassword = ""; // Reset password
          toggleCollecting(false); // Toggle the state of collecting
          toggleCollecting(true); // Restart collection
        } else {
          int passwordLength = currentPassword.length();
//          // Serial.println(currentPassword);
          if (passwordLength == 4) {
            if (currentPassword == masterPassword) {
              correctPassword(); // Unlock procedure
            } else {
              incorrectPassword(); // Reject password procedure
            }
            currentPassword = ""; // Reset password
            toggleCollecting(false); // Toggle the state of collecting
          }
        }
      }
    }
    else if (getFingerprintIDez() != -1) {
      if (latestConfidence > 100) {
        correctPassword(); // Unlock procedure
      } else {
        incorrectPassword(); // Reject password procedure
      }
    }
  }
  delay(5); // Don't need to run this at full speed.
}

void toggleCollecting(boolean state) {
  collecting = state; // Set to true/false state
  if (state) {
//    // Serial.println("Starting password collection");
    toggleLED("yellow"); // Turn on yellow light
  } else {
//    // Serial.println("Password collection stopped. Waiting for trigger key...");
  }
}

void correctPassword() {
//  // Serial.println("Correct password");
  toggleLED("green");
  unlockSolenoid(unlockTime);
  toggleLED("red");
}

void incorrectPassword() {
//  // Serial.println("Incorrect password. Please try again.");
  toggleLED("red"); // Red light indicator
}

void unlockSolenoid(int unlockTime) {
  digitalWrite(solenoidPin, HIGH); // Unlock solenoid
//  // Serial.println("Unlocking");
  delay(unlockTime);
  digitalWrite(solenoidPin, LOW);
//  // Serial.println("Locking");
}

void toggleLED(String color) {
  if (color == "green") {
    digitalWrite(greenPin, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(redPin, LOW);
  } else if (color == "yellow") {
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, LOW);
  } else if (color == "red") {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(yellowPin, LOW);
  } else {
    // Turn all LEDs off
    digitalWrite(greenPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(redPin, LOW);
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      // Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      // Serial.println("Imaging error");
      return p;
    default:
      // Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      // Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      // Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      // Serial.println("Could not find fingerprint features");
      return p;
    default:
      // Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    // Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    // Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    // Serial.println("Did not find a match");
    return p;
  } else {
    // Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  // Serial.print("Found ID #"); // Serial.print(finger.fingerID); 
  // Serial.print(" with confidence of "); // Serial.println(finger.confidence); 
}

int getFingerprintIDez() {
//  // Serial.println("Reading fingerprint");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  // Serial.print("Found ID #"); // Serial.print(finger.fingerID); 
  // Serial.print(" with confidence of "); // Serial.println(finger.confidence);
  latestConfidence = finger.confidence; // Store globally
  return finger.fingerID; 
}

