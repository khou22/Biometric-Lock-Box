// References:
// http://playground.arduino.cc/Code/Keypad
// http://playground.arduino.cc/Main/KeypadTutorial
// http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Components/General/SparkfunCOM-08653_Datasheet.pdf

// Libraries
#include <Key.h>
#include <Keypad.h>

const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

// Define keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4}; //connect to the column pinouts of the keypad

// Create the Keypad
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Password
String masterPassword = "1234"; // Password
char triggerKey = '#'; // Trigger to begin password collection

// User Data
String currentPassword = "";
boolean collecting = false;

void setup() {
  Serial.begin(9600);
}

void loop() {
  char key = kpd.getKey(); // Store key press
  
  if (key) { // If valid key pressed
    Serial.println(key);
    if (!collecting) { // If not already collecting
      if (key == triggerKey) { // If trigger to start password collection
        toggleCollecting(true);
      }
    } else { // If collecting
      currentPassword += key; // Add the latest key
      if (currentPassword.length() == 4) { // If password finished
        Serial.println("Entered password: " + currentPassword);
        if (currentPassword == masterPassword) { // If correct password
          Serial.println("Correct password");
        } else {
          Serial.println("Incorrect password. Please try again.");
        }
        currentPassword = ""; // Reset password
        toggleCollecting(false);
      }
    }
  }
}

void toggleCollecting(boolean state) {
  collecting = state; // Set to true/false state
  if (state) {
    Serial.println("Starting password collection");
  } else {
    Serial.println("Password collection stopped. Waiting for trigger key...");
  }
}

