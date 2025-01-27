#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <Control_Surface.h>

// Instantiate a MIDI over USB interface
USBMIDI_Interface midi;

// Instantiate the MPR121 object
Adafruit_MPR121 capSensor = Adafruit_MPR121();

// Define the MIDI notes to send for each touch sensor pin
const uint8_t midiNotes[12] = {60, 62, 64, 65, 67, 69, 71, 72, 42, 40, 38, 36}; //first 8 are notes for the piano, last 4 for the bouncy balls

// Variables to store touch state
uint16_t lastTouched = 0;
uint16_t currTouched = 0;

// Button on digital pin 2 to play note C4
NoteButton drumpad1 {
  2, 
  {MIDI_Notes::C[2], Channel_4} // Play note C4 on Channel 1
};

// Button on digital pin 2 to play note C4
NoteButton drumpad2 {
  3, 
  {MIDI_Notes::E[2], Channel_4} // Play note C4 on Channel 1
};

// Button on digital pin 2 to play note C4
NoteButton drumpad3 {
  4, 
  {MIDI_Notes::F[2], Channel_4} // Play note C4 on Channel 1
};

// Button on digital pin 2 to play note C4
NoteButton drumpad4 {
  5, 
  {MIDI_Notes::B[2], Channel_4} // Play note C4 on Channel 1
};

CCButton PianoPedal1 {
  // Push button on pin 5:
  7,
  // General Purpose Controller #1 on MIDI channel 1:
  {MIDI_CC::General_Purpose_Controller_1, Channel_1},
};

CCButton PianoPedal2 {
  // Push button on pin 5:
  8,
  // General Purpose Controller #1 on MIDI channel 1:
  {MIDI_CC::General_Purpose_Controller_2, Channel_1},
};

int button2 = 9; // toggle for LED 
int led2 = 10;
int button3 = 11; // toggle for LED 
int led3 = 12;


CCButton PianoInstrument1 {
  // Push button on pin 5:
  9,
  // General Purpose Controller #1 on MIDI channel 1:
  {MIDI_CC::General_Purpose_Controller_3, Channel_1},
};

CCButton PianoInstrument2 {
  // Push button on pin 5:
  11,
  // General Purpose Controller #1 on MIDI channel 1:
  {MIDI_CC::General_Purpose_Controller_4, Channel_1},
};

// Potentiometer on A0
CCPotentiometer potentiometer {
  A0, 
  {MIDI_CC::Channel_Volume, Channel_1} // CC 7 (Channel Volume) on Channel 1
};

// // Potentiometer on A0
// CCPotentiometer potentiometer {
//   A1, 
//   {MIDI_CC::Channel_Volume1, Channel_1} // CC 7 (Channel Volume) on Channel 1
// };

// // Potentiometer on A0
// CCPotentiometer potentiometer {
//   A2, 
//   {MIDI_CC::Channel_Volume2, Channel_1} // CC 7 (Channel Volume) on Channel 1
// };

// // Potentiometer on A0
// CCPotentiometer potentiometer {
//   A3, 
//   {MIDI_CC::Channel_Volume3, Channel_1} // CC 7 (Channel Volume) on Channel 1
// };

// // Potentiometer on A0
// CCPotentiometer potentiometer {
//   A4, 
//   {MIDI_CC::Channel_Volume4, Channel_1} // CC 7 (Channel Volume) on Channel 1
// };

// // Potentiometer on A0
// CCPotentiometer potentiometer {
//   A5, 
//   {MIDI_CC::Channel_Volume5, Channel_1} // CC 7 (Channel Volume) on Channel 1
// };

void setup() {
  
  // Initialize the Control Surface
  Control_Surface.begin();
  
  pinMode(2, INPUT_PULLUP);// Enable internal pull-up resistor for the button
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP); 
  //pinMode(9, INPUT_PULLUP);
  pinMode(led2, OUTPUT);
  pinMode(9, INPUT_PULLUP);
  pinMode(led3, OUTPUT);
  pinMode(11, INPUT_PULLUP); 


  // Initialize the MPR121 sensor
  if (!capSensor.begin(0x5A)) {
    while (true) {
      Serial.println("MPR121 not found. Check wiring?");
      delay(1000);
    }
  }
  Serial.println("MPR121 found!");
  setThresholdsForAllPins(15,9);
}


void loop() {
  // Update the Control Surface
  Control_Surface.loop();

  // Get the current touch state from the sensor
  currTouched = capSensor.touched();

  for (uint8_t i = 0; i < 12; i++) {
    // Determine the channel (1 for pins 0-7, 2 for pins 8-11)
    uint8_t channel = (i < 8) ? 1 : 2;

    // Create a MIDIAddress object for the note and channel
    MIDIAddress noteAddress(midiNotes[i], Channel(channel));

    // Check if a pin was just touched
    if ((currTouched & _BV(i)) && !(lastTouched & _BV(i))) {
      // Send a Note On message
      midi.sendNoteOn(noteAddress, 127); // MIDIAddress, Velocity
      Serial.print("Note On, Pin: ");
      Serial.print(i);
      Serial.print(", Channel: ");
      Serial.println(channel);
    }

    // Check if a pin was just released
    if (!(currTouched & _BV(i)) && (lastTouched & _BV(i))) {
      // Send a Note Off message
      midi.sendNoteOff(noteAddress, 0); // MIDIAddress, Velocity
      Serial.print("Note Off, Pin: ");
      Serial.print(i);
      Serial.print(", Channel: ");
      Serial.println(channel);
    }
  }

  // Update the last touch state
  lastTouched = currTouched;


if (digitalRead(button2) == LOW) {  // Button pressed
    digitalWrite(led2, !digitalRead(led2));  // Toggle the LED state
    while (digitalRead(button2) == LOW);   // Wait until the button is released
}

if (digitalRead(button3) == LOW) {  // Button pressed
    digitalWrite(led3, !digitalRead(led3));  // Toggle the LED state
    while (digitalRead(button3) == LOW);   // Wait until the button is released
}
  delay(2); // Small delay for stability
}

void setThresholdsForAllPins(uint8_t touchThreshold, uint8_t releaseThreshold) {
  for (uint8_t i = 0; i < 12; i++) { // MPR121 has 12 electrodes
    capSensor.setThresholds(touchThreshold, releaseThreshold);
  }

}

