// #include <Arduino.h>

// #define LED 2

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   pinMode(LED, OUTPUT);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   digitalWrite(LED, HIGH);
//   Serial.println("LED is on");
//   delay(1000);
//   digitalWrite(LED, LOW);
//   Serial.println("LED is off");
//   delay(1000);
// }

/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <Arduino.h>
#include <BleKeyboard.h>

#define LED 2

#define LED_POWER 4
#define LED_CONNECTED 12
#define LED_BTN_PRESSED 33

BleKeyboard bleKeyboard;

typedef struct {
    uint8_t pin;
    uint8_t key; 
    bool is_pressed;
} keymap; 

keymap key_grid[] = {
    // color buttons
    {16, 'a', false}, 
    {17, 's', false}, 
    {5, 'j', false}, 
    {18, 'k', false}, 
    {19, 'l', false}, 

    // string strum 
    {23, KEY_UP_ARROW, false}, 
    {25, KEY_DOWN_ARROW, false}, 

    // controls 
    {22, '\n', false}, 
    {21, 'h', false}, 

    {26, KEY_UP_ARROW, false}, 
    {27, KEY_DOWN_ARROW, false}, 
    {13, KEY_RIGHT_ARROW, false}, 
    {14, KEY_LEFT_ARROW, false}, 

    {15, KEY_LEFT_GUI, false},
};

constexpr size_t num_keys = sizeof(key_grid)/sizeof(keymap);

void setup() {
    for (int i = 0; i < num_keys; i++) {
        pinMode(key_grid[i].pin, INPUT_PULLUP);
    }

    pinMode(LED_POWER, OUTPUT);
    digitalWrite(LED_POWER, LOW);

    pinMode(LED_CONNECTED, OUTPUT);
    digitalWrite(LED_CONNECTED, HIGH);

    pinMode(LED_BTN_PRESSED, OUTPUT);
    digitalWrite(LED_BTN_PRESSED, HIGH);

    pinMode(LED, OUTPUT);
    bleKeyboard.begin();
}

void loop() {
    bool is_connected = bleKeyboard.isConnected();
    digitalWrite(LED_CONNECTED, !is_connected);

    if (is_connected) {
        bool key_pressed = false;
        for (int i = 0; i < num_keys; i++) {
            keymap current_key = key_grid[i];
            bool _key_pressed = digitalRead(current_key.pin) == 0;

            // pos edge
            if (!current_key.is_pressed && _key_pressed) {
                bleKeyboard.press(current_key.key);
                key_pressed = true;
            }

            // neg edge
            else if (current_key.is_pressed && !_key_pressed) {
                bleKeyboard.release(current_key.key);
                key_pressed = true;
            }

            key_grid[i].is_pressed = _key_pressed;
        }
        delay(20);
        digitalWrite(LED, key_pressed);
        digitalWrite(LED_BTN_PRESSED, !key_pressed);
    } else {
        delay(500);
    }
}