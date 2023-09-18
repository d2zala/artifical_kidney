#define INTERRUPT_PIN 3
#define CLEAR_EMERGENCY_STATE 6 //RED BUTTON
#define USED_WASHROOM 7 //GREEN BUTTON
#define CONTACT_DOCTOR_LED 8
#define EMERGENCY_STATE_LED 2
#define USE_WASHROOM_LED 10
#define ANP_LED 11
#define RENIN_LED 12
#define ADP_LED 13

//Global variables
int anp_level = 1; // 0 is low, 1 is medium, 2 is high
int anpledState = LOW;
int renin_level = 1; // 0 is low, 1 is medium, 2 is high
int reninledState = LOW; 
int adp_level = 1; // 0 is low, 1 is medium, 2 is high
int adpledState = LOW;
int blood_pressure = 1; // 0 is low, 1 is medium, 2 is high, 3 is emergency
int osmolarity = 1; // 0 is low, 1 is medium, 2 is high

// LDR sensor
int LDRPin = A0;
int LDRVal = 0;

// water level sensor 1
int waterlevelPin1 = A1;
int liquid_level1;

// water level sensor 2
int waterlevelPin2 = A2;
int liquid_level2;

// water level sensor 2
int waterlevelPin3 = A3;
int liquid_level3;

// rates to blink LED
unsigned long previousMillis2 = 0;

const long interval2 = 750;

// timer for use washroom
long interval4 = 10000;
long when2go = 10000;
unsigned long currentMillis4 = 0;

//Volatile variables
volatile bool emergency_state = false;
volatile bool used_washroom = false;

void setup() {
  // setting prescalar to 256 which is 100
  TCCR0B |= (1<<CS02);
  TCCR0B &= ~((1<<CS01) | (1<<CS00));

  TCCR1A = 0; //sets to normal mode
  TCCR1B = 0; 
  OCR1A = 31249; // compare value for 1/2 sec
  OCR1B = 15624; // comapre for 1/4 sec

  TCCR1B |= (1<<CS12) | (1<<WGM12); //prescaler 256, sets to clear timer on compare mode
  TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B); //enable OCR1A/OCR1B Interrupter

  Serial.begin(9600);  // initialize serial communication, 9600 is the baud rate
  Serial.print('\n');  // starts the program on a new line in the serial monitor
  pinMode(LDRPin, INPUT);
  pinMode(EMERGENCY_STATE_LED, OUTPUT);
  pinMode(USED_WASHROOM, OUTPUT);
  pinMode(ANP_LED, OUTPUT);
  pinMode(RENIN_LED, OUTPUT);
  pinMode(ADP_LED, OUTPUT);
  pinMode(USE_WASHROOM_LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), system_interrupted, RISING);
}

void loop() {
  unsigned long currentMillis4 = millis() * 4;
  if (currentMillis4 >= when2go) {
    // set the use washroom LED to HIGH
    Serial.println("Time to blink washroom LED since 10 seconds have passed");
    digitalWrite(USE_WASHROOM_LED, HIGH);
  }
  if (anp_level == 1){
    digitalWrite(ANP_LED, LOW);
  }
  if (renin_level == 1){
    digitalWrite(RENIN_LED, LOW);
  }
  if (adp_level == 1){
    digitalWrite(ADP_LED, LOW);
  }
  if (anp_level == 2){
    digitalWrite(ANP_LED, HIGH);
  }
  if (renin_level == 2){
    digitalWrite(RENIN_LED, HIGH);
  }
  if (adp_level == 2){
    digitalWrite(ADP_LED, HIGH);
  }
  if (anp_level == 0){
    unsigned long currentMillis1 = millis();
      if (currentMillis1 - previousMillis1 >= interval1) {
        previousMillis1 = currentMillis1;
        // if the LED is off turn it on and vice-versa:
        if (anpledState == LOW) {
          anpledState = HIGH;
        } else {
          anpledState = LOW;
        }
        // set the LED with the ledState of the variable:
        digitalWrite(ANP_LED, anpledState);
      }
  }
  if (adp_level == 0){
    unsigned long currentMillis2 = millis() * 4;
      if (currentMillis2 - previousMillis2 >= interval2) {
        Serial.println("Time to blink ADP LED since 750 ms have elapsed");
        previousMillis2 = currentMillis2;
        // if the LED is off turn it on and vice-versa:
        if (adpledState == LOW) {
          adpledState = HIGH;
        } else {
          adpledState = LOW;
        }
        // set the LED with the ledState of the variable:
        digitalWrite(ADP_LED, adpledState);
      }
  }
  if (renin_level == 0){
    unsigned long currentMillis3 = millis();
      if (currentMillis3 - previousMillis3 >= interval3) {
        previousMillis3 = currentMillis3;
        // if the LED is off turn it on and vice-versa:
        if (reninledState == LOW) {
          reninledState = HIGH;
        } else {
          reninledState = LOW;
        }
        // set the LED with the ledState of the variable:
        digitalWrite(RENIN_LED, reninledState);
      }
  }
  if (emergency_state == false){
    // Serial.print("Blood pressure is ");
    // Serial.println(blood_pressure);
    // Serial.print("Osmolarity is ");
    // Serial.println(osmolarity);
    digitalWrite(EMERGENCY_STATE_LED, LOW);
    readLDR();
    read_water_level();
    calculate_state();
    // Serial.println("State was calculated");
  }
}

void system_interrupted(){
  if (digitalRead(CLEAR_EMERGENCY_STATE) == HIGH){
    emergency_state = false;
    clear_emergency_state(emergency_state);    
  }    
  if (digitalRead(USED_WASHROOM) == HIGH){
    used_washroom = true;
    user_used_washroom(used_washroom);  
  }
  if (digitalRead(waterlevelPin3) == true){
    emergency_state = true;
    dangerous_blood_pressure(emergency_state);
  }
} 

void readLDR(){
  LDRVal = analogRead(LDRPin);
  // Serial.print("LDR Value is ");
  // Serial.println(LDRVal);
  if (LDRVal > 75){
    // Serial.println("osmolarity is high");
    osmolarity = 2;
  }
  else if (LDRVal > 2){
    // Serial.println("osmolarity is medium");
    osmolarity = 1;
  }
  else {
    // Serial.println("osmolarity is low");
    osmolarity = 0;
  }
}

void read_water_level(){
  liquid_level1 = digitalRead(waterlevelPin1);
  liquid_level2 = digitalRead(waterlevelPin2);
  liquid_level3 = digitalRead(waterlevelPin3);
  if (liquid_level2 == true){
    // Serial.println("Blood pressure is high");
    blood_pressure = 2;
  }
  else if (liquid_level1 == true){
    // Serial.println("Blood pressure is medium");
    blood_pressure = 1;
  }
  else{
    // Serial.println("Blood pressure is low");
    blood_pressure = 0;
  }
}

void calculate_state(){
  if (blood_pressure == 1 && osmolarity == 1){    
    // Serial.println("Changing to state 0");
    state0();
  }
  if (blood_pressure == 0 && osmolarity == 0){
    // Serial.println("Changing to state 1");
    state1();
  }
  if (blood_pressure == 0 && osmolarity == 1){
    // Serial.println("Changing to state 2");
    state2();
  }
  if (blood_pressure == 1 && osmolarity == 0){
    // Serial.println("Changing to state 3");
    state3();
  }
  if (blood_pressure == 1 && osmolarity == 2){
    // Serial.println("Changing to state 4");
    state4();
  }
  if (blood_pressure == 2 && osmolarity == 0){
    // Serial.println("Changing to state 5");
    state5();
  }
  if (blood_pressure == 2 && osmolarity == 1){
    // Serial.println("Changing to state 6");
    state6();
  }
  if (blood_pressure == 2 && osmolarity == 2){
    // Serial.println("Changing to state 7");
    state7();
  }
  if (blood_pressure == 0 && osmolarity == 2){
    // Serial.println("Changing to state 8");
    state8();
  }
}

void state0(){
  anp_level = 1;
  renin_level = 1;
  adp_level = 1;
  // Serial.println("State changed to 0");
}

void state1(){
  anp_level = 2;
  renin_level = 0;
  adp_level = 0;
  // Serial.println("State changed to 1");
}

void state2(){
  anp_level = 1;
  renin_level = 1;
  adp_level = 0;
  // Serial.println("State changed to 2");
}

void state3(){
  anp_level = 0;
  renin_level = 0;
  adp_level = 0;
  // Serial.println("State changed to 3");
}

void state4(){
  anp_level = 0;
  renin_level = 0;
  adp_level = 1;
  // Serial.println("State changed to 4");
}

void state5(){
  anp_level = 2;
  renin_level = 0;
  adp_level = 1;
  // Serial.println("State changed to 5");
}

void state6(){
  anp_level = 0;
  renin_level = 1;
  adp_level = 2;
  // Serial.println("State changed to 6");
}

void state7(){
  anp_level = 2;
  renin_level = 0;
  adp_level = 2;
  // Serial.println("State changed to 7");
}

void state8(){
  anp_level = 0;
  renin_level = 2;
  adp_level = 0;
  emergency_state = true;
  // Serial.println("State changed to 8");
  digitalWrite(EMERGENCY_STATE_LED,HIGH);
}

void clear_emergency_state(bool emergency_state){
  if (emergency_state == false){
    digitalWrite(EMERGENCY_STATE_LED,LOW);
    osmolarity = 1;
    blood_pressure = 1;
    // Serial.println("Emergency state cleared");
    state0();
  }
  else{
    return;
  }
}

void user_used_washroom(bool used_washroom){
  if (used_washroom == true){
    digitalWrite(USE_WASHROOM_LED,LOW);
    currentMillis4 = millis() * 4;
    when2go = currentMillis4 + interval4;
    osmolarity = 2;
    blood_pressure = 1;
    // Serial.println("User used washroom");
    state4();
  }
  else{
    return;
  }
}
void dangerous_blood_pressure(bool emergency_state){
  if (emergency_state == true){
    // Serial.println("Blood pressure is very dangerous");
    osmolarity = 1;
    blood_pressure = 2;
    state8();
  }
}

ISR (TIMER1_COMPA_vect){
  // Serial.println(millis());
  // if the LED is off turn it on and vice-versa:
  if (anp_level == 0){
    Serial.println("Time to blink ANP LED since 500 secs have elapsed");
    if (anpledState == LOW) {
    anpledState = HIGH;
    } 
    else {
    anpledState = LOW;
    }
  }
  
  // set the LED with the ledState of the variable:
  digitalWrite(ANP_LED, anpledState);
}

ISR (TIMER1_COMPB_vect){
  // Serial.println(millis());
  // if the LED is off turn it on and vice-versa:
  if (renin_level == 0){
    Serial.println("Time to blink RENIN LED since 250 secs have elapsed");
    if (reninledState == LOW) {
      reninledState = HIGH;
    } else {
      reninledState = LOW;
    }
  }
  // set the LED with the ledState of the variable:
  digitalWrite(RENIN_LED, reninledState);
}