enum State {
  S0,  // ALL_RED
  S1,  // Group A Green (L1 + L3)
  S2,  // Group A Yellow (L1 + L3)
  S3,  // Group B Green (L2 + L4)
  S4,  // Group B Yellow (L2 + L4)
};

// Pins are set to -1 for now as placeholders and will be updated once hardware is implemented
const int redPin1 = -1, yellowPin1 = -1, greenPin1 = -1;
const int redPin2 = -1, yellowPin2 = -1, greenPin2 = -1;
const int redPin3 = -1, yellowPin3 = -1, greenPin3 = -1;
const int redPin4 = -1, yellowPin4 = -1, greenPin4 = -1;

unsigned long startingTime;
unsigned long elapsedTime;                   // Time elapsed since the beginning like for example it's 5:20PM and we started counting at 5PM 20min have elapsed since 5PM
const unsigned long ALL_RED_TIME = 30000;    // Set all the lights to be red for 30 seconds
const unsigned long YELLOW_TIME = 4000;      // Set the traffic light to be yellow for 4 seconds
const unsigned long MIN_GREEN_TIME = 20000;  // Set the minimum green time to be 20 sec this will be used for sensor logic later on
const unsigned long MAX_GREEN_TIME = 30000;  // Set the maximum green time to be 30 sec

int br = 9600;
State state = S0;  // Set the starting state to be S0

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
  startingTime = millis(); // Initialize the starting time with millis it basically tracks how long the system has been running
  applyState(state); // Apply the initial traffic light state
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
