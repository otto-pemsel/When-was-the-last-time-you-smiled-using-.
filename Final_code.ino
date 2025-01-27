#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <Control_Surface.h>

// Instantiate a MIDI over USB interface
USBMIDI_Interface midi;

// Instantiate the MPR121 object
Adafruit_MPR121 capSensor = Adafruit_MPR121();

// Define the MIDI notes for each MPR121 pin
const uint8_t midiNotes[12] = {60, 62, 64, 65, 67, 69, 71, 72, 42, 40, 38, 36};

// Variables to store touch states
uint16_t lastTouched = 0;
uint16_t currTouched = 0;

// Sensor thresholds (adjust as needed)
const uint8_t touchThreshold = 5;
const uint8_t releaseThreshold = 2;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize the MPR121 sensor
  if (!capSensor.begin(0x5A)) {
    Serial.println("MPR121 not found. Check wiring?");
    while (true) delay(1000); // Halt execution
  }
  Serial.println("MPR121 found!");

  // Set thresholds for touch and release
  setThresholdsForAllPins(touchThreshold, releaseThreshold);

  // Initialize MIDI control surface
  Control_Surface.begin();
}

void loop() {
  // Update the Control Surface
  Control_Surface.loop();

  // Read the current touch state
  currTouched = capSensor.touched();

  for (uint8_t i = 0; i < 12; i++) {
    // Determine MIDI channel based on pin
    uint8_t channel;
    if (i <= 5) {
      channel = 1; // Pins 0–5 on Channel 1
    } else if (i <= 8) {
      channel = 2; // Pins 6–8 on Channel 2
    } else {
      channel = 3; // Pins 9–11 on Channel 3
    }

    // Create a MIDIAddress object for the note and channel
    MIDIAddress noteAddress(midiNotes[i], Channel(channel));

    // Check if a pin was just touched
    if ((currTouched & _BV(i)) && !(lastTouched & _BV(i))) {
      midi.sendNoteOn(noteAddress, 127); // Send Note On
      Serial.print("Note On, Pin: ");
      Serial.print(i);
      Serial.print(", Channel: ");
      Serial.println(channel);
    }

    // Check if a pin was just released
    if (!(currTouched & _BV(i)) && (lastTouched & _BV(i))) {
      midi.sendNoteOff(noteAddress, 0); // Send Note Off
      Serial.print("Note Off, Pin: ");
      Serial.print(i);
      Serial.print(", Channel: ");
      Serial.println(channel);
    }
  }

  // Update the last touch state
  lastTouched = currTouched;

  delay(10); // Add small delay to debounce and stabilize
}

void setThresholdsForAllPins(uint8_t touchThreshold, uint8_t releaseThreshold) {
  for (uint8_t i = 0; i < 12; i++) {
    capSensor.setThresholds(touchThreshold, releaseThreshold);
  }
  Serial.println("Thresholds set for all pins.");
}
