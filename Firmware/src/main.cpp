#include <Arduino.h>
#include <FastLED.h>

// ============================================================
// MINI MUSIC STUDIO
// Controller: Waveshare ESP32-S3-Zero
// Framework: Arduino / PlatformIO
//
// Features:
//   1. 12-button matrix
//   2. 12 addressable RGB LEDs
//   3. Two rotary encoders
//   4. Two potentiometers
//   5. Two encoder push buttons
//   6. Four additional capacitive-touch inputs
//   7. MIDI note and control-change output over UART
//
// IMPORTANT:
// Replace the -1 GPIO placeholders below with the actual
// GPIO numbers from your KiCad schematic before using hardware.
// GPIO 10 is the LED data pin from the existing firmware.
// ============================================================


// ============================================================
// 1. RGB LED CONFIGURATION
// ============================================================

constexpr uint8_t LED_DATA_PIN = 10;
constexpr uint8_t LED_COUNT = 12;
constexpr uint8_t LED_BRIGHTNESS = 40;

CRGB leds[LED_COUNT];


// ============================================================
// 2. BUTTON MATRIX CONFIGURATION
// ============================================================

// Four columns and three rows = 12 buttons.
//
// Replace -1 with the GPIO numbers assigned in KiCad.
// Leave a pin at -1 until you have confirmed its assignment.

const int8_t ROW_PINS[3] = {
    -1, -1, -1
};

const int8_t COL_PINS[4] = {
    -1, -1, -1, -1
};

// MIDI notes assigned to the 12 buttons.
const uint8_t BUTTON_NOTES[12] = {
    60, 61, 62, 63,
    64, 65, 66, 67,
    68, 69, 70, 71
};

bool buttonState[12] = {false};
bool previousButtonState[12] = {false};

unsigned long lastButtonScan = 0;
constexpr unsigned long BUTTON_SCAN_INTERVAL = 5;


// ============================================================
// 3. ROTARY ENCODERS
// ============================================================

// Encoder 1: A, B and push button
// Encoder 2: A, B and push button
//
// Replace -1 with the corresponding GPIO numbers.

const int8_t ENCODER1_A_PIN = -1;
const int8_t ENCODER1_B_PIN = -1;
const int8_t ENCODER1_SW_PIN = -1;

const int8_t ENCODER2_A_PIN = -1;
const int8_t ENCODER2_B_PIN = -1;
const int8_t ENCODER2_SW_PIN = -1;

int previousEncoder1A = HIGH;
int previousEncoder2A = HIGH;

bool previousEncoder1Button = HIGH;
bool previousEncoder2Button = HIGH;


// ============================================================
// 4. POTENTIOMETERS
// ============================================================

// Connect each potentiometer's wiper to its assigned ADC GPIO.
// The other two terminals should connect to the appropriate
// power and ground rails specified by the circuit design.

const int8_t POT1_PIN = -1;
const int8_t POT2_PIN = -1;

int previousPot1Value = -1;
int previousPot2Value = -1;

constexpr int POT_CHANGE_THRESHOLD = 12;


// ============================================================
// 5. ADDITIONAL CAPACITIVE-TOUCH INPUTS
// ============================================================

// These are intended for external touch-sensing circuits or
// GPIOs that support the selected touch-sensing implementation.
//
// Verify the actual ESP32-S3 GPIO assignments and touch hardware
// before enabling these inputs.

const int8_t TOUCH_PINS[4] = {
    -1, -1, -1, -1
};

bool previousTouchState[4] = {
    false, false, false, false
};

// Adjust after testing your actual touch-sensing hardware.
constexpr uint16_t TOUCH_THRESHOLD = 30;


// ============================================================
// 6. MIDI CONFIGURATION
// ============================================================

// This implementation sends standard MIDI bytes over UART.
// A suitable MIDI output circuit/interface is required.
//
// Do not connect a 5-pin DIN MIDI connector directly to a GPIO.

const int8_t MIDI_TX_PIN = -1;

constexpr uint8_t MIDI_CHANNEL = 1;

constexpr uint8_t MIDI_NOTE_ON = 0x90;
constexpr uint8_t MIDI_NOTE_OFF = 0x80;
constexpr uint8_t MIDI_CONTROL_CHANGE = 0xB0;

constexpr uint8_t MIDI_VELOCITY = 100;

HardwareSerial MIDI(1);

bool midiEnabled = false;


// ============================================================
// 7. GENERAL SETTINGS
// ============================================================

constexpr unsigned long SERIAL_BAUD_RATE = 115200;

constexpr unsigned long POT_READ_INTERVAL = 20;
constexpr unsigned long ENCODER_DEBOUNCE_INTERVAL = 3;

unsigned long lastPotRead = 0;


// ============================================================
// 8. MIDI HELPER FUNCTIONS
// ============================================================

void sendMidiMessage(
    uint8_t status,
    uint8_t data1,
    uint8_t data2
) {
    if (!midiEnabled) {
        return;
    }

    MIDI.write(status | ((MIDI_CHANNEL - 1) & 0x0F));
    MIDI.write(data1 & 0x7F);
    MIDI.write(data2 & 0x7F);
}


void sendMidiNoteOn(uint8_t note, uint8_t velocity) {
    sendMidiMessage(MIDI_NOTE_ON, note, velocity);

    Serial.print("MIDI Note ON: ");
    Serial.println(note);
}


void sendMidiNoteOff(uint8_t note) {
    sendMidiMessage(MIDI_NOTE_OFF, note, 0);

    Serial.print("MIDI Note OFF: ");
    Serial.println(note);
}


void sendMidiControlChange(uint8_t controller, uint8_t value) {
    sendMidiMessage(MIDI_CONTROL_CHANGE, controller, value);
}


// ============================================================
// 9. RGB LED HELPER FUNCTIONS
// ============================================================

void setAllLeds(const CRGB &color) {
    fill_solid(leds, LED_COUNT, color);
    FastLED.show();
}


void showStartupAnimation() {
    setAllLeds(CRGB::Black);

    for (uint8_t i = 0; i < LED_COUNT; i++) {
        leds[i] = CRGB::Blue;
        FastLED.show();
        delay(40);
    }

    setAllLeds(CRGB::Black);
}


void showButtonLed(uint8_t buttonIndex, bool pressed) {
    if (buttonIndex >= LED_COUNT) {
        return;
    }

    if (pressed) {
        leds[buttonIndex] = CRGB::Green;
    } else {
        leds[buttonIndex] = CRGB::Black;
    }

    FastLED.show();
}


// ============================================================
// 10. BUTTON MATRIX INITIALIZATION
// ============================================================

bool matrixPinsConfigured() {
    for (uint8_t row = 0; row < 3; row++) {
        if (ROW_PINS[row] < 0) {
            return false;
        }
    }

    for (uint8_t col = 0; col < 4; col++) {
        if (COL_PINS[col] < 0) {
            return false;
        }
    }

    return true;
}


void initializeButtons() {
    if (!matrixPinsConfigured()) {
        Serial.println(
            "Button matrix disabled: assign the GPIO pins first."
        );
        return;
    }

    for (uint8_t row = 0; row < 3; row++) {
        pinMode(ROW_PINS[row], INPUT_PULLUP);
    }

    for (uint8_t col = 0; col < 4; col++) {
        pinMode(COL_PINS[col], INPUT_PULLUP);
    }

    Serial.println("Button matrix initialized.");
}


// ============================================================
// 11. BUTTON MATRIX SCANNING
// ============================================================

// Assumes a diode-isolated matrix with a compatible electrical
// arrangement. Confirm diode orientation and scan polarity in KiCad.

void scanButtons() {
    if (!matrixPinsConfigured()) {
        return;
    }

    if (millis() - lastButtonScan < BUTTON_SCAN_INTERVAL) {
        return;
    }

    lastButtonScan = millis();

    for (uint8_t row = 0; row < 3; row++) {

        // Return all rows to their inactive state.
        for (uint8_t r = 0; r < 3; r++) {
            pinMode(ROW_PINS[r], INPUT_PULLUP);
        }

        // Activate one row at a time.
        pinMode(ROW_PINS[row], OUTPUT);
        digitalWrite(ROW_PINS[row], LOW);

        delayMicroseconds(50);

        for (uint8_t col = 0; col < 4; col++) {

            uint8_t index = row * 4 + col;

            bool pressed =
                digitalRead(COL_PINS[col]) == LOW;

            buttonState[index] = pressed;

            if (pressed != previousButtonState[index]) {

                if (pressed) {
                    sendMidiNoteOn(
                        BUTTON_NOTES[index],
                        MIDI_VELOCITY
                    );

                    showButtonLed(index, true);
                } else {
                    sendMidiNoteOff(BUTTON_NOTES[index]);

                    showButtonLed(index, false);
                }

                previousButtonState[index] = pressed;
            }
        }
    }

    // Leave every row inactive.
    for (uint8_t row = 0; row < 3; row++) {
        pinMode(ROW_PINS[row], INPUT_PULLUP);
    }
}


// ============================================================
// 12. ROTARY ENCODER INITIALIZATION
// ============================================================

void initializeEncoders() {

    if (ENCODER1_A_PIN >= 0 && ENCODER1_B_PIN >= 0) {
        pinMode(ENCODER1_A_PIN, INPUT_PULLUP);
        pinMode(ENCODER1_B_PIN, INPUT_PULLUP);

        previousEncoder1A = digitalRead(ENCODER1_A_PIN);
    }

    if (ENCODER2_A_PIN >= 0 && ENCODER2_B_PIN >= 0) {
        pinMode(ENCODER2_A_PIN, INPUT_PULLUP);
        pinMode(ENCODER2_B_PIN, INPUT_PULLUP);

        previousEncoder2A = digitalRead(ENCODER2_A_PIN);
    }

    if (ENCODER1_SW_PIN >= 0) {
        pinMode(ENCODER1_SW_PIN, INPUT_PULLUP);
    }

    if (ENCODER2_SW_PIN >= 0) {
        pinMode(ENCODER2_SW_PIN, INPUT_PULLUP);
    }

    Serial.println("Encoder initialization complete.");
}


// ============================================================
// 13. ROTARY ENCODER ROTATION
// ============================================================

void readEncoder1() {

    if (ENCODER1_A_PIN < 0 || ENCODER1_B_PIN < 0) {
        return;
    }

    int currentA = digitalRead(ENCODER1_A_PIN);

    if (currentA != previousEncoder1A && currentA == LOW) {

        int bState = digitalRead(ENCODER1_B_PIN);

        if (bState != currentA) {
            sendMidiControlChange(20, 65);
            Serial.println("Encoder 1: clockwise");
        } else {
            sendMidiControlChange(20, 63);
            Serial.println("Encoder 1: counter-clockwise");
        }
    }

    previousEncoder1A = currentA;
}


void readEncoder2() {

    if (ENCODER2_A_PIN < 0 || ENCODER2_B_PIN < 0) {
        return;
    }

    int currentA = digitalRead(ENCODER2_A_PIN);

    if (currentA != previousEncoder2A && currentA == LOW) {

        int bState = digitalRead(ENCODER2_B_PIN);

        if (bState != currentA) {
            sendMidiControlChange(21, 65);
            Serial.println("Encoder 2: clockwise");
        } else {
            sendMidiControlChange(21, 63);
            Serial.println("Encoder 2: counter-clockwise");
        }
    }

    previousEncoder2A = currentA;
}


// ============================================================
// 14. ENCODER PUSH BUTTONS
// ============================================================

void readEncoderButtons() {

    if (ENCODER1_SW_PIN >= 0) {

        bool currentState = digitalRead(ENCODER1_SW_PIN);

        if (currentState == LOW &&
            previousEncoder1Button == HIGH) {

            Serial.println("Encoder 1 push button pressed.");

            // Example: toggle a MIDI control value.
            sendMidiControlChange(22, 127);
        }

        if (currentState == HIGH &&
            previousEncoder1Button == LOW) {

            sendMidiControlChange(22, 0);
        }

        previousEncoder1Button = currentState;
    }


    if (ENCODER2_SW_PIN >= 0) {

        bool currentState = digitalRead(ENCODER2_SW_PIN);

        if (currentState == LOW &&
            previousEncoder2Button == HIGH) {

            Serial.println("Encoder 2 push button pressed.");

            sendMidiControlChange(23, 127);
        }

        if (currentState == HIGH &&
            previousEncoder2Button == LOW) {

            sendMidiControlChange(23, 0);
        }

        previousEncoder2Button = currentState;
    }
}


// ============================================================
// 15. POTENTIOMETER INPUTS
// ============================================================

void initializePotentiometers() {

    if (POT1_PIN >= 0) {
        pinMode(POT1_PIN, INPUT);
    }

    if (POT2_PIN >= 0) {
        pinMode(POT2_PIN, INPUT);
    }

    Serial.println("Potentiometer initialization complete.");
}


void readPotentiometers() {

    if (millis() - lastPotRead < POT_READ_INTERVAL) {
        return;
    }

    lastPotRead = millis();


    if (POT1_PIN >= 0) {

        int rawValue = analogRead(POT1_PIN);

        int midiValue = map(rawValue, 0, 4095, 0, 127);
        midiValue = constrain(midiValue, 0, 127);

        if (previousPot1Value < 0 ||
            abs(midiValue - previousPot1Value) >=
                POT_CHANGE_THRESHOLD / 32) {

            sendMidiControlChange(16, midiValue);

            previousPot1Value = midiValue;

            Serial.print("Potentiometer 1: ");
            Serial.println(midiValue);
        }
    }


    if (POT2_PIN >= 0) {

        int rawValue = analogRead(POT2_PIN);

        int midiValue = map(rawValue, 0, 4095, 0, 127);
        midiValue = constrain(midiValue, 0, 127);

        if (previousPot2Value < 0 ||
            abs(midiValue - previousPot2Value) >=
                POT_CHANGE_THRESHOLD / 32) {

            sendMidiControlChange(17, midiValue);

            previousPot2Value = midiValue;

            Serial.print("Potentiometer 2: ");
            Serial.println(midiValue);
        }
    }
}


// ============================================================
// 16. CAPACITIVE-TOUCH INPUTS
// ============================================================

// This section uses the Arduino-ESP32 touchRead() API.
// Confirm that the selected GPIOs support touch sensing on your
// ESP32-S3 board and test the returned values before use.

void initializeTouchInputs() {

    for (uint8_t i = 0; i < 4; i++) {

        if (TOUCH_PINS[i] >= 0) {
            previousTouchState[i] = false;
        }
    }

    Serial.println("Touch input initialization complete.");
}


void readTouchInputs() {

    for (uint8_t i = 0; i < 4; i++) {

        if (TOUCH_PINS[i] < 0) {
            continue;
        }

        uint16_t touchValue = touchRead(TOUCH_PINS[i]);

        // Touch thresholds vary by board, pad size, wiring and
        // environmental conditions. Calibrate before use.
        bool touched = touchValue < TOUCH_THRESHOLD;

        if (touched != previousTouchState[i]) {

            if (touched) {

                Serial.print("Touch input ");
                Serial.print(i + 2);
                Serial.println(" activated.");

                sendMidiControlChange(
                    30 + i,
                    127
                );

            } else {

                sendMidiControlChange(
                    30 + i,
                    0
                );
            }

            previousTouchState[i] = touched;
        }
    }
}


// ============================================================
// 17. MIDI OUTPUT INITIALIZATION
// ============================================================

void initializeMidi() {

    if (MIDI_TX_PIN < 0) {

        midiEnabled = false;

        Serial.println(
            "MIDI UART disabled: assign MIDI_TX_PIN first."
        );

        return;
    }

    // Standard MIDI UART speed is 31,250 baud.
    MIDI.begin(
        31250,
        SERIAL_8N1,
        -1,
        MIDI_TX_PIN
    );

    midiEnabled = true;

    Serial.println("MIDI UART initialized.");
}


// ============================================================
// 18. SETUP
// ============================================================

void setup() {

    Serial.begin(SERIAL_BAUD_RATE);

    delay(500);

    Serial.println();
    Serial.println("==============================");
    Serial.println(" MINI MUSIC STUDIO");
    Serial.println(" ESP32-S3-Zero Firmware");
    Serial.println("==============================");

    // Initialize the RGB LED strip.
    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(
        leds,
        LED_COUNT
    );

    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.clear(true);

    showStartupAnimation();

    Serial.println("RGB LED controller initialized.");

    initializeButtons();
    initializeEncoders();
    initializePotentiometers();
    initializeTouchInputs();
    initializeMidi();

    Serial.println("Firmware startup complete.");
    Serial.println(
        "Unassigned GPIO features remain disabled."
    );
}


// ============================================================
// 19. MAIN LOOP
// ============================================================

void loop() {

    scanButtons();

    readEncoder1();
    readEncoder2();

    readEncoderButtons();

    readPotentiometers();

    readTouchInputs();

    delay(1);
}