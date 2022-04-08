
/*Global Variables*/
int timer = 0;            // Variable for Timer

const int PIR_1 = 6;      // Name buttons
const int PIR_2 = 7;
const int PIR_3 = 8;
const int PIR_4 = 9;

const int LED1 = 10;      // Name LEDs
const int LED2 = 11;
const int LED3 = 12;
const int LED4 = 13;

bool dir;                     // Boolean variable to store direction of pedestrian
const float distance = 800;   // Distance between buttons for speed calculation
int PIR1, PIR2, PIR3, PIR4;   // Initialize placeholder variables as int
float LEDspeed;               // Variable to store calculated pedestrian speed

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);       // For serial monitor

  pinMode(LED4, OUTPUT);    // Setup LEDs as output
  pinMode(LED3, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED1, OUTPUT);

  pinMode(PIR_4, INPUT);    // Setup buttons as input
  pinMode(PIR_3, INPUT);
  pinMode(PIR_2, INPUT);
  pinMode(PIR_1, INPUT);

  TCCR0A = (1 << WGM01);    // Set the CTC mode
  OCR0A = 0xF9;             // value for OCR0A for 1ms

  TIMSK0 |= (1 << OCIE0A);  // Set interrupt request
  sei();                    // enable interrupts

  TCCR0B |= (1 << CS01);    // Set prescale for 1/64 clock
  TCCR0B |= (1 << CS00);
}


void loop() {

  PIR1 = 0; //Initialize placeholder variables to zero
  PIR2 = 0;
  PIR3 = 0;
  PIR4 = 0;

WAIT:
  Serial.println("Entered Wait");       // Print for debugging
  digitalWrite(LED4, LOW);              // Set all LEDs off in WAIT mode
  digitalWrite(LED3, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
  int light_value = analogRead(A0);     // Read level of ambient light
  if (light_value > 700) {              // If it is daytime
    goto DAYLIGHT;                      // Jump to DAYLIGHT
  }
PIR4:                                   // PIR4 mode
  if (digitalRead(PIR_4) == HIGH) {       // If PIR_4?
    Serial.print("button 4 pushed \n");   // Print for debugging
    delay(50);                            // Delay
    timer = 0;                            // Reset timer
    PIR4 = 1;                             // Indicate that PIR_4 has been triggered
    if (PIR3 == 1) {                      // If PIR3 was previously triggered (pedestrian walking away from light path)
      PIR3 = 0;                           // Clear variables
      PIR4 = 0;
      goto WAIT;                          // Jump to WAIT mode
    }
    delay(50000);                         // delay
    while (timer < 5000) {                // Clock invariant for 5 seconds
      Serial.println(timer);
      if (digitalRead(PIR_3) == HIGH) {   // if PIR_3? (pedestrian has walked through the second sensor towards path)
        digitalWrite(LED4, HIGH);         // All lights turn on
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, HIGH);
        delay(500);                       // delay
        goto CALCULATE_SPEED_L;           // Jump to CALCULATE_SPEED_L mode
      }
      else if ((PIR3 == 0) & (digitalRead(PIR_4) == HIGH)) { // if PIR_3==0 ∧ PIR_4? (a group of pedestrians is detected)
        goto ALL_LIGHTS_ON;                                 // Jump to ALL_LIGHTS_ON
      }
    }
    goto WAIT;    // jump to WAIT
  }

PIR3:                                   // PIR3 mode
  if (digitalRead(PIR_3) == HIGH) {       // if PIR_3?
    PIR3 = 1;                             // indicate PIR_3 was triggered
    goto WAIT;
  }

PIR2:                                   // PIR2 mode
  if (digitalRead(PIR_2) == HIGH) {       // if PIR_2?
    PIR2 = 1;                             // indicate PIR_2 was triggered
    goto WAIT;
  }

PIR1:
  if (digitalRead(PIR_1) == HIGH) {       // if PIR_1?
    Serial.print("button 1 pushed \n");
    delay(50);
    timer = 0;                            // Reset timer
    PIR1 = 1;                             // Indicate PIR_1 was triggered
    if (PIR2 == 1) {                      // If PIR2 was previously triggered (pedestrian walking away from light path)
      PIR1 = 0;                           // Clear variables
      PIR2 = 0;
      goto WAIT;
    }
    delay(50000);
    while (timer < 5000) {                // Clock invariant for 5 seconds
      Serial.println(timer);
      if (digitalRead(PIR_2) == HIGH) {   // if PIR_2?
        digitalWrite(LED4, HIGH);         // all LEDS on
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, HIGH);
        goto CALCULATE_SPEED_R;
      }
      else if ((PIR2 == 0) & (digitalRead(PIR_1) == HIGH)) { // if PIR_2==0 ∧ PIR_1? (a group of pedestrians is detected)
        goto ALL_LIGHTS_ON;
      }
    }
    goto WAIT;
  }
  goto WAIT;

DAYLIGHT:                           // DAYLIGHT mode
  while (light_value > 700) {         // While it is daylight
    Serial.println("DAYLIGHT");
    light_value = analogRead(A0);     // Check ambient light level
  }
  goto WAIT;                          // Jump back to WAIT when it is dark

CALCULATE_SPEED_R:                  // CALCULATE_SPEED_R mode
  LEDspeed = timer / distance;        // Calculate delay value
  LEDspeed = LEDspeed * 1000;         // scale value to be read as milliseconds instead of seconds
  dir = true;                         // Indicate direction to the right
  timer = 0;                          // Reset timer
  while (timer < LEDspeed) {          // Create delay equal to LEDspeed value
    Serial.println(timer);
  }
  goto LED1;                          // Start sequence from LED1

CALCULATE_SPEED_L:                  // CALCULATE_SPEED_L mode
  LEDspeed = timer / distance;        // Calculate delay value
  LEDspeed = LEDspeed * 1000;         // scale value to be read as milliseconds instead of seconds
  dir = false;                        // Indicate direction to the left
  timer = 0;                          // Reset timer
  while (timer < LEDspeed) {          // Create delay equal to LEDspeed value
    Serial.println(timer);
  }
  goto LED4;                          // Start sequence from LED1


LED1:                               // LED1 mode
  timer = 0;                          // Reset timer
  digitalWrite(LED1, LOW);            // Turn off LED1
  while (timer < LEDspeed) {          // Create delay equal to LEDspeed value
    Serial.println(timer);
    if ((digitalRead(PIR_1) == HIGH) | (digitalRead(PIR_4) == HIGH)) {  // if PIR_1? ∧ PIR_4?
      goto ALL_LIGHTS_ON;
    }
    else if ((digitalRead(PIR_2) == HIGH) | (digitalRead(PIR_3) == HIGH)) { // if PIR_2? ∧ PIR_3?
      goto WAIT;
    }
  }
  if (dir) goto LED2;                 // If going right move to LED2, otherwise go to WAIT mode
  else goto WAIT;

LED2:                               // LED2 mode
  timer = 0;                          // Reset Timer
  digitalWrite(LED2, LOW);            // Turn off LED2
  while (timer < LEDspeed) {          // Create delay equal to LEDspeed value
    Serial.println(timer);
    if ((digitalRead(PIR_1) == HIGH) | (digitalRead(PIR_4) == HIGH)) {  // if PIR_1? ∧ PIR_4?
      goto ALL_LIGHTS_ON;
    }
    else if ((digitalRead(PIR_2) == HIGH) | (digitalRead(PIR_3) == HIGH)) { // if PIR_2? ∧ PIR_3?
      goto WAIT;
    }
  }
  if (dir) goto LED3;                 // If going right move to LED3, otherwise go to LED1
  else goto LED1;

LED3:                               // LED3 mode
  timer = 0;                          // Reset timer
  digitalWrite(LED3, LOW);            // Turn off LED3
  while (timer < LEDspeed) {          // Create delay equal to LEDspeed value
    Serial.println(timer);
    if ((digitalRead(PIR_1) == HIGH) | (digitalRead(PIR_4) == HIGH)) {  // if PIR_1? ∧ PIR_4?
      goto ALL_LIGHTS_ON;
    }
    else if ((digitalRead(PIR_2) == HIGH) | (digitalRead(PIR_3) == HIGH)) { // if PIR_2? ∧ PIR_3?
      goto WAIT;
    }
  }
  if (dir) goto LED4;                 // If going right move to LED4, otherwise go to LED2
  else goto LED2;

LED4:                               // LED4 mode
  timer = 0;                          // Reset timer
  digitalWrite(LED4, LOW);            // Turn off LED4
  while (timer < LEDspeed) {          // Create delay equal to LEDspeed value
    Serial.println(timer);
    if ((digitalRead(PIR_1) == HIGH) | (digitalRead(PIR_4) == HIGH)) {  // if PIR_1? ∧ PIR_4?
      goto ALL_LIGHTS_ON;
    }
    else if ((digitalRead(PIR_2) == HIGH) | (digitalRead(PIR_3) == HIGH)) { // if PIR_2? ∧ PIR_3?
      goto WAIT;
    }
  }
  if (dir) goto WAIT;                 // If going right move to WAIT mode, otherwise go to LED3
  else goto LED3;

ALL_LIGHTS_ON:                      // ALL_LIGHTS_ON mode
  digitalWrite(LED4, HIGH);           // Turn on all LEDs
  digitalWrite(LED3, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED1, HIGH);
  Serial.println("ALL_LIGHTS_ON");
  while (timer < 6000) {              // Delay for 6 seconds
    if (digitalRead(PIR_1) == HIGH) { // if PIR_1?
      goto PIR1;
    }
    else if (digitalRead(PIR_4) == HIGH) { // if PIR_4?
      goto PIR4;
    }
  }
  goto WAIT;

}


ISR(TIMER0_COMPA_vect) {              // Interrupt for Timer 0
  timer++;                            // Increment timer variable (occurs every millisecond)

}
