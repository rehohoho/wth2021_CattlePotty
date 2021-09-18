/**
 * Blink functionality to test ESP32
 *   - Ensure Node32s board
 *   - Ensure COM port correct
 *   - !! May have to hold boot button to upload the code
 * 
 * Wiring:
 * Connect using two boards (side chopped off if required)
 *   22 - resistor - LED }
 *   23 - resistor - LED }
 *   GND                 } <- connected in parallel
 */

int LED_1 = 22;
int LED_2 = 23;
 
void setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
}
 
void loop() {
  digitalWrite(LED_1, HIGH);
  delay(1000);
  digitalWrite(LED_2, HIGH);
  delay(1000);
  digitalWrite(LED_2, LOW);
  delay(1000);
  digitalWrite(LED_1, LOW);
  delay(1000);
}
