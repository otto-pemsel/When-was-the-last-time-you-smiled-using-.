#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <Control_Surface.h>

// Instantiate a MIDI over USB interface
USBMIDI_Interface midi;

// Instantiate the MPR121 object
Adafruit_MPR121 capSensor = Adafruit_MPR121();

// Define the MIDI notes to send for each touch sensor pin
const uint8_t midiNotes[12] = {60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79};

// Variables to store touch state
uint16_t lastTouched = 0;
uint16_t currTouched = 0;

void setup() {
  // Initialize the Control Surface
  Control_Surface.begin();

  // Initialize the MPR121 sensor
  if (!capSensor.begin(0x5A)) {
    while (true) {
      Serial.println("MPR121 not found. Check wiring?");
      delay(1000);
    }
  }
  Serial.println("MPR121 found!");
  
}

void loop() {
  // Update the Control Surface
  Control_Surface.loop();

  // Get the current touch state from the sensor
  currTouched = capSensor.touched();

  for (uint8_t i = 0; i < 12; i++) {
    // Check if a pin was just touched
    if ((currTouched & _BV(i)) && !(lastTouched & _BV(i))) {
      // Create a MIDIAddress object for the note and channel
      MIDIAddress noteAddress = {midiNotes[i], Channel_1};
      // Send a Note On message
      midi.sendNoteOn(noteAddress, 127); // Velocity 127
      Serial.print("Note On, Pin: ");
      Serial.println(i);
    }

    // Check if a pin was just released
    if (!(currTouched & _BV(i)) && (lastTouched & _BV(i))) {
      // Create a MIDIAddress object for the note and channel
      MIDIAddress noteAddress = {midiNotes[i], Channel_1};
      // Send a Note Off message
      midi.sendNoteOff(noteAddress, 0); // Velocity 0
      Serial.print("Note Off, Pin: ");
      Serial.println(i);
    }
  }

  // Update the last touch state
  lastTouched = currTouched;

  delay(10); // Small delay for stability
}

