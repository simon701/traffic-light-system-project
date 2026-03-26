#include "HX711.h"
#include "Arduino.h"
#include "stdio.h"

int br = 9600;

enum State {
  S0,  // ALL_RED
  S1,  // Group A Green (L1 + L3)
  S2,  // Group A Yellow (L1 + L3)
  S3,  // Group B Green (L2 + L4)
  S4,  // Group B Yellow (L2 + L4)
};

State state = S0;  // Set the starting state to be S0

// Traffic light pin mapping: Lane 1–4 each have Red, Yellow, Green LEDs, used for 4-way intersection control
const int redPin1 = 24, yellowPin1 = 23, greenPin1 = 22;
const int redPin2 = 27, yellowPin2 = 26, greenPin2 = 25;
const int redPin3 = 44, yellowPin3 = 43, greenPin3 = 42;
const int redPin4 = 34, yellowPin4 = 35, greenPin4 = 36;
// HX711 load cell pins for each lane (DOUT = data output from sensor, SCK = clock signal to read data)
const int HX_DOUT1 = 52, HX_SCK1 = 53;
const int HX_DOUT2 = 50, HX_SCK2 = 51;
const int HX_DOUT3 = 48, HX_SCK3 = 49;
const int HX_DOUT4 = 46, HX_SCK4 = 47;

unsigned long startingTime;
unsigned long elapsedTime;                   // Time elapsed since the beginning like for example it's 5:20PM and we started counting at 5PM 20min have elapsed since 5PM
const unsigned long ALL_RED_TIME = 3000;     // Set all the lights to be red for 3 seconds
const unsigned long YELLOW_TIME = 4000;      // Set the traffic light to be yellow for 4 seconds
const unsigned long MIN_GREEN_TIME = 20000;  // Set the minimum green time to be 20 sec this will be used for sensor logic later on
const unsigned long MAX_GREEN_TIME = 30000;  // Set the maximum green time to be 30 sec
const unsigned long EXTEND_GREEN = 5000; // Time extension applied when traffic is detected (in ms)
unsigned long extended_green = MAX_GREEN_TIME; // Current dynamic green time which can be extended based on traffic conditions
const unsigned long max_green = 90000; // Max limit for green time to prevent stravation of other lanes
float threshold = 5; // Minimum weight threshold to consider that traffic is present
// HX711 objects for each load cell (1 per lane)
HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;
/*
Calibration factors for each load cell such that each factor was determined
individually by calibrating the sensor with known reference weights to ensure
accurate measurements 
*/
float calibration_factor = 115.36;
float calibration_factor2 = -217;
float calibration_factor3 = -212.5;
float calibration_factor4 = -204.5;

// Function forward declarations
void applyState(State s); // Applies LED outputs based on the current traffic light state
void handleState(State s); // Handles state transitions such as timing and sensor logic
float weightGroupA(); // Returns total traffic weight for Group A (lanes 1 & 3)
float weightGroupB(); // Returns total traffic weight for Group B (lanes 2 & 4)

void setup() {
  // Runs once at startup
  Serial.begin(br);
  // Set up the LEDs as output because they show a light hence we need them as output and not input
  pinMode(redPin1, OUTPUT);
  pinMode(yellowPin1, OUTPUT);
  pinMode(greenPin1, OUTPUT);
  pinMode(redPin2, OUTPUT);
  pinMode(yellowPin2, OUTPUT);
  pinMode(greenPin2, OUTPUT);
  pinMode(redPin3, OUTPUT);
  pinMode(yellowPin3, OUTPUT);
  pinMode(greenPin3, OUTPUT);
  pinMode(redPin4, OUTPUT);
  pinMode(yellowPin4, OUTPUT);
  pinMode(greenPin4, OUTPUT);
  startingTime = millis();  // Initialize the starting time with millis it basically tracks how long the system has been running
  applyState(state);        // Apply the initial traffic light state
  // Initialize the HX711 modules for all load cells (1 per lane)
  scale1.begin(HX_DOUT1, HX_SCK1);
  scale2.begin(HX_DOUT2, HX_SCK2);
  scale3.begin(HX_DOUT3, HX_SCK3);
  scale4.begin(HX_DOUT4, HX_SCK4);

  /*
  Wait until all load cells are ready before proceeding this allows for
  proper communication and prevents invalid readings at startup. 
  If any of the sensors are not ready it will print a message to the Serial Monitor
  indicating which sensor is not ready and suggesting to check the wiring.
  The loop will continue to check every 500ms until all sensors are ready before proceeding
  with calibration and taring.
  */
  while (!scale1.is_ready() || !scale2.is_ready() || !scale3.is_ready() || !scale4.is_ready()) {
    if (!scale1.is_ready()) {
      Serial.println("Scale 1 is not ready check wiring.");
    }
    if (!scale2.is_ready()) {
      Serial.println("Scale 2 is not ready check wiring.");
    }
    if (!scale3.is_ready()) {
      Serial.println("Scale 3 is not ready check wiring.");
    }
    if (!scale4.is_ready()) {
      Serial.println("Scale 4 is not ready check wiring.");
    }
    delay(500);
  }

  /*
  Apply calibration factors and tare each load cell
  Tare sets the current reading to zero (baseline with no load)
  */
  scale1.set_scale(calibration_factor);
  scale1.tare();
  scale2.set_scale(calibration_factor2);
  scale2.tare();
  scale3.set_scale(calibration_factor3);
  scale3.tare();
  scale4.set_scale(calibration_factor4);
  scale4.tare();
}

// This function resets all lights to LOW meaning all LEDs are off.
void resetLights() {
  digitalWrite(redPin1, LOW);
  digitalWrite(yellowPin1, LOW);
  digitalWrite(greenPin1, LOW);
  digitalWrite(redPin2, LOW);
  digitalWrite(yellowPin2, LOW);
  digitalWrite(greenPin2, LOW);
  digitalWrite(redPin3, LOW);
  digitalWrite(yellowPin3, LOW);
  digitalWrite(greenPin3, LOW);
  digitalWrite(redPin4, LOW);
  digitalWrite(yellowPin4, LOW);
  digitalWrite(greenPin4, LOW);
}

// This function sets all the traffic lights to red by first resetting all lights and then turning on the red LEDs for all lanes.
void setAllRed() {
  resetLights();
  digitalWrite(redPin1, HIGH);
  digitalWrite(redPin2, HIGH);
  digitalWrite(redPin3, HIGH);
  digitalWrite(redPin4, HIGH);
}

// Applies the given traffic light state by setting the correct LEDs for each lane
void applyState(State s) {
  switch (s) {
    // S0: All lanes are red (safe transition state)
    case S0:
      setAllRed();
      break;

    // S1: Group A (lanes 1 & 3) set to green and Group B (lanes 2 & 4) set to red
    case S1:
      resetLights();
      digitalWrite(greenPin1, HIGH);
      digitalWrite(redPin2, HIGH);
      digitalWrite(greenPin3, HIGH);
      digitalWrite(redPin4, HIGH);
      break;

    // S2: Group A (lanes 1 & 3) are set to yellow and Group B stays red
    case S2:
      resetLights();
      digitalWrite(yellowPin1, HIGH);
      digitalWrite(redPin2, HIGH);
      digitalWrite(yellowPin3, HIGH);
      digitalWrite(redPin4, HIGH);
      break;

    // S3: Group A (lanes 1 & 3) set to red and Group B (lanes 2 & 4) set to green
    case S3:
      resetLights();
      digitalWrite(redPin1, HIGH);
      digitalWrite(greenPin2, HIGH);
      digitalWrite(redPin3, HIGH);
      digitalWrite(greenPin4, HIGH);
      break;

    // S4: Group B (lanes 2 & 4) are set to yellow and Group A stays red
    case S4:
      resetLights();
      digitalWrite(redPin1, HIGH);
      digitalWrite(yellowPin2, HIGH);
      digitalWrite(redPin3, HIGH);
      digitalWrite(yellowPin4, HIGH);
      break;

    // Default: fallback safety set all to red
    default:
      setAllRed();
      break;
  }
}

// Handles state transitions using both timing and load cell based traffic conditions
void handleState(State s) {
  State next = s;

  // Calculate how long we've been in the current state
  elapsedTime = millis() - startingTime;
  switch (s) {
    
    // S0: All red: decide which group gets priority based on current traffic conditions
    case S0: {
      float groupA = weightGroupA();
      float groupB = weightGroupB();
      // No traffic detected stay in All red state
      if (groupA <= threshold && groupB <= threshold) {
        next = S0;
      }
      // If Group A has more traffic give the green light to Group A
      else if (groupA > groupB + threshold || (groupB <= threshold && groupA > threshold)) {
        extended_green = MAX_GREEN_TIME;
        next = S1;
      }
      // Otherwise give the green light to Group B
      else {
        extended_green = MAX_GREEN_TIME;
        next = S3;
      }
      break;
    }

    // S1: Group A is in green state with extension allowed based on traffic
    case S1: {
      float groupA = weightGroupA();
      float groupB = weightGroupB();
      // Debug: monitor weights and timing
      Serial.print("S1 | A: ");
      Serial.print(groupA);
      Serial.print(" B: ");
      Serial.print(groupB);
      Serial.print(" elapsed: ");
      Serial.println(elapsedTime);
      // Enforce minimum green time
      if (elapsedTime < MIN_GREEN_TIME) {
        next = S1;
      }
      else {
        // If traffic appears in Group B make sure to prepare to switch
        if (groupB > threshold) {
          next = S2;
        }
        // If Group A becomes empty: switch to Group B
        else if (groupA <= threshold) {
          next = S2;
        }
        // Extend green if we still have traffic in Group A and is within max limit this is allowed if and only if B is empty
        else if (elapsedTime >= extended_green) {
          if (extended_green + EXTEND_GREEN <= max_green) {
            extended_green += EXTEND_GREEN;
            next = S1;
          }
          else {
            next = S2;
          }
        }
        // Otherwise keep green
        else {
          next = S1;
        }
      }
      break;
    }
    // S2: Group A is in yellow state: transition to Group B
    case S2:
      if (elapsedTime >= YELLOW_TIME) {
        next = S3;
      }
      break;
    // S3: Group A is in green state with extension allowed based on traffic
    case S3: {
      float groupA = weightGroupA();
      float groupB = weightGroupB();
      // Debug: monitor weights and timing
      Serial.print("S3 | A: ");
      Serial.print(groupA);
      Serial.print(" B: ");
      Serial.print(groupB);
      Serial.print(" elapsed: ");
      Serial.print(elapsedTime);
      Serial.print(" ext: ");
      Serial.println(extended_green);
      // Enforce minimum green time
      if (elapsedTime < MIN_GREEN_TIME) {
        next = S3;
      }
      else {
        // If traffic appears in Group A make sure to prepare to switch
        if (groupA > threshold) {
          next = S4;
        }
        // If Group B becomes empty: switch to Group A
        else if (groupB <= threshold) {
          next = S4;
        }
        // Extend green if we still have traffic in Group B and is within max limit this is allowed if and only if A is empty
        else if (elapsedTime >= extended_green) {
          if (extended_green + EXTEND_GREEN <= max_green) {
            extended_green += EXTEND_GREEN;
            next = S3;
          }
          else {
            next = S4;
          }
        }
        // Otherwise keep green
        else {
          next = S3;
        }
      }
      break;
    }
    // S2: Group B is in yellow state: transition to all red
    case S4: {
      if (elapsedTime >= YELLOW_TIME) {
        next = S0;
      }
      break;
    }
  }
  // If state has changed, update the current state and update the timer
  if (next != state) {
    state = next;
    startingTime = millis();
    // Reset extended green when entering a green phase
    if (next == S1 || next == S3) {
      extended_green = MAX_GREEN_TIME;
    } 
  }
}

// Reads and returns the weight measured on lane 1 represeting traffic density per lane. The value is also printed for debugging/monitoring.
float weightlane1() {
  float weight1 = scale1.get_units(1);
  Serial.print("Weight Lane 1: ");
  Serial.println(weight1);
  return weight1;
}

// Reads and returns the weight measured on lane 2 represeting traffic density per lane. The value is also printed for debugging/monitoring.
float weightlane2() {
  float weight2 = scale2.get_units(1);
  Serial.print("Weight Lane 2: ");
  Serial.println(weight2);
  return weight2;
}

// Reads and returns the weight measured on lane 3 represeting traffic density per lane. The value is also printed for debugging/monitoring.
float weightlane3() {
  float weight3 = scale3.get_units(1);
  Serial.print("Weight Lane 3: ");
  Serial.println(weight3);
  return weight3;
}

// Reads and returns the weight measured on lane 4 represeting traffic density per lane. The value is also printed for debugging/monitoring.
float weightlane4() {
  float weight4 = scale4.get_units(1);
  Serial.print("Weight Lane 4: ");
  Serial.println(weight4);
  return weight4;
}

/*
Computes total weight for Group A (lanes 1 & 3). This will be the total combined traffic
load for this direction
*/
float weightGroupA() {
  return weightlane1() + weightlane3();
}

/*
Computes total weight for Group B (lanes 2 & 4). This will be the total combined traffic
load for this direction
*/
float weightGroupB() {
  return weightlane2() + weightlane4();
}


// Main loop: continuously updates logic and applies corresponding outputs
void loop() {
  handleState(state);  // Update the current state based on timing conditions
  applyState(state);   // Apply the LED outputs for the current state
  // Debug: print current state, elapsed time in seconds since system start to Serial Monitor
  Serial.print("State: ");
  Serial.println((int)state);
  Serial.print("Time: ");
  Serial.println(millis() / 1000);
  delay(800);
}
