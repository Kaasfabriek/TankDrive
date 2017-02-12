#include <Servo.h>

#define debug false
#define LED_BUILTIN 13

///////////////////////////////////////////////////////////
// joystick potmeters are used for input - settings to ignode middle zone
/////////////////////////////////////////////////////////////
#define pot_x_min 30
#define pot_x_dead_min 500
#define pot_x_dead_max 570
#define pot_x_max 1010

#define pot_y_min 30
#define pot_y_dead_min 500
#define pot_y_dead_max 570
#define pot_y_max 1010

#define pot_pwr_min 0
#define pot_pwr_max 1024

// output goes to two servo channels to which a chinese 30A ESC is connected, 
//    can also controll a servo to move fwd/mid/back position matched with joystick fwd/mid/back position 
Servo ESC_L;  // create servo object to control a servo
Servo ESC_R;  // create servo object to control a servo  // twelve servo objects can be created on most boards

int prev_power_perc_L = 0;
int prev_power_perc_R = 0;
int prev_ESC_L_value = 90;
int prev_ESC_R_value = 90;

///////////////////////////////////////////////////////////
//  joystick   the 5 connections are wired to adjacent connections on the arduino pro mini
///////////////////////////////////////////////////////////
#define pin_joy_btn 13
#define pin_joy_RY A0
#define pin_joy_RX A1
#define pin_joy_5V A2
#define pin_joy_GND A3

///////////////////////////////////////////////////////////
// Esc or servo pins
///////////////////////////////////////////////////////////
#define pin_ESC_L_signal 4    // ESC/servo L -->   brown ground= GND,   red power= RAW,      orange signal= (as defined here)   digital 9 is common in examples
#define pin_ESC_R_signal 6   // ESC/servo R -->   brown ground= GND,   red power= RAW,      orange signal= (as defined here)

///POWER options...:
// If using ESC:
//    --> the first ESC is also powering the arduino
//    --> for other esc, leave "power" unconnected; connect gnd and signal only
// if using Servo:
//    an external power source is needed to power all servos (5 to 12 volts) and is also connected to the RAW pin to power the arduino


void setup() {

  // init servo L. ground=pin29, 5V=5v pin27, signal=D9  --> the ESC is also powering the device
  ESC_L.attach(pin_ESC_L_signal);  // attaches the servo on pin 9 to the servo object
  
  // init servo R. ground=D12, 5V=D11, signal=D10   
  ESC_R.attach(pin_ESC_R_signal);  
  //pinMode(11, OUTPUT);           //  for easier solderig of your connections, can use neigboring digital ports as GND and VCC by making them low or high. Can power tiny servo
  //digitalWrite(11, HIGH);
  //pinMode(12, OUTPUT); 
  //digitalWrite(12, HIGH);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  // init joystick    A0=GND A1=5V A2=X A3=y A4=btn
  pinMode(pin_joy_GND, OUTPUT);  
  pinMode(pin_joy_5V, OUTPUT);    
  pinMode(pin_joy_RX, INPUT_PULLUP); 
  pinMode(pin_joy_RY, INPUT_PULLUP); 
  pinMode(pin_joy_btn, INPUT_PULLUP);  
  digitalWrite(pin_joy_GND, LOW);
  digitalWrite(pin_joy_5V, HIGH);
        
  // ESC arming sequence  
  ESC_L.write(0);
  ESC_R.write(0);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(60);  // 100ms is 5 pulses  
  ESC_L.write(180);
  ESC_R.write(180);
  delay(60);  // 100ms is 5 pulses  
  ESC_L.write(90);
  ESC_R.write(90);  
  delay(600);  // 100ms is 5 pulses

  // initialize serial communications at 9600 bps:
  if (debug) Serial.begin(9600);  
  if (debug) Serial.println("Init done.");
}

int limit_value (int value,int _min,int _max) {
  if (value < _min) return _min;
  if (value > _max) return _max;
  return value;
}

int map_pot_to_control (int value, int _min, int dead_min, int dead_max, int _max) {
  if (value < dead_min) return limit_value(map( value, _min, dead_min, -100, 0), -100, 0) ;
  if (value > dead_max) return limit_value(map( value, dead_max, _max, 0, 100), 0, 100);
  return 0;
}


void loop() {
  // loop mechanics = run the loop 10 times per second (it is a boat, nothing special)
  
  // read joystick
  int pot_x = analogRead(pin_joy_RX);  // values read from joystick
  int pot_y = analogRead(pin_joy_RY); 
  bool btn = !digitalRead(pin_joy_btn);

  // read power limiter
  int pot_pwr = 1024;
  
  // map and clean joystick pot values into range -100 to 100
  int control_x = map_pot_to_control( pot_x, pot_x_min, pot_x_dead_min, pot_x_dead_max, pot_x_max);
  int control_y = map_pot_to_control( pot_y, pot_y_min, pot_y_dead_min, pot_y_dead_max, pot_y_max);

  // power limiter 0..100 perc
  int control_pwr = limit_value(map( pot_pwr, pot_pwr_min, pot_pwr_max, 0, 100), 0, 100);

  // now make tank motor formula, in power%
  int power_perc_L = limit_value(control_x + control_y/2, -100, 100) * control_pwr/100;
  int power_perc_R = limit_value(control_x - control_y/2, -100, 100) * control_pwr/100;

  // now map to servo angle 0 to 180
  int ESC_L_value = map ( power_perc_L, -100, 100, 0, 180);
  int ESC_R_value = map ( power_perc_R, -100, 100, 0, 180);

  // reverse detection - to allow the special controll
  bool motor_L_start_reversing = false;
  bool motor_R_start_reversing = false;
  if ( prev_power_perc_L >= 0 && power_perc_L < 0 ) motor_L_start_reversing=true;
  if ( prev_power_perc_R >= 0 && power_perc_R < 0 ) motor_R_start_reversing=true;

  if ( motor_L_start_reversing || motor_R_start_reversing) {
    // kick into reverse // without mid-flight re-arm
    if (debug) if (motor_L_start_reversing) Serial.println("Reverse <<<<< L");
    if (debug) if (motor_R_start_reversing) Serial.println("Reverse R >>>>>");
    if (motor_L_start_reversing) ESC_L.write(100);  // forward
    if (motor_R_start_reversing) ESC_R.write(100);
    delay(60); 
      //allow 3 servo signals of 20 millisec each.
      //servo receives 50 pulses per second, so 20 millisec per pulse
    if (motor_L_start_reversing) ESC_L.write(80); // reverse
    if (motor_R_start_reversing) ESC_R.write(80);
    delay(60); 
    if (motor_L_start_reversing) ESC_L.write(90);  // mid
    if (motor_R_start_reversing) ESC_R.write(90);
    delay(60); // mid for reverse 60ms is okay. for re-arming time 380ms is niet okay, 400ms is okay
    // end of this loop will reverse for real
    
    // Hack to prevent the sweep from positive pos
    if (motor_L_start_reversing) prev_ESC_L_value = 90;
    if (motor_R_start_reversing) prev_ESC_R_value = 90;
    
  }

  // Re-arm detection
  if (btn) {
    if (debug) Serial.println("Re-arming sequence");
    // ESC arming sequence  
    ESC_L.write(0);
    ESC_R.write(0);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(60);  // 100ms is 5 pulses  
    ESC_L.write(180);
    ESC_R.write(180);
    delay(60);  // 100ms is 5 pulses  
    ESC_L.write(90);
    ESC_R.write(90);  
    delay(600);  // 100ms is 5 pulses
    digitalWrite(LED_BUILTIN, LOW);
  }

  // to the motors!
  int step_ESC_L_value = ( ESC_L_value - prev_ESC_L_value ) / 5;
  int step_ESC_R_value = ( ESC_R_value - prev_ESC_R_value ) / 5;
  
  for (int i =0; i <5; i++) {
    ESC_L.write(prev_ESC_L_value + i * step_ESC_L_value);
    ESC_R.write(prev_ESC_R_value + i * step_ESC_R_value);
    delay(20);
  }
  ESC_L.write(ESC_L_value);  // these not needed
  ESC_R.write(ESC_R_value);

  // print the results to the serial monitor:
  if (debug) Serial.println("x= " + String(pot_x) + " y= " + String(pot_y) + " btn= " + String(btn)
    + " control_x= " + String(control_x) + " control_y= " + String(control_y)
    + " prev_power_perc_L= " + String(prev_power_perc_L) + " prev_power_perc_R= " + String(prev_power_perc_R)
    + " power_perc_L= " + String(power_perc_L) + " power_perc_R= " + String(power_perc_R)
    + " prev_ESC_L_value= " + String(prev_ESC_L_value) + " prev_ESC_R_value= " + String(prev_ESC_R_value) 
    + " ESC_L_value= " + String(ESC_L_value) + " ESC_R_value= " + String(ESC_R_value) 
    + " step_ESC_L_value= " + String(step_ESC_L_value) + " step_ESC_R_value= " + String(step_ESC_R_value) );  

  prev_power_perc_L = power_perc_L;
  prev_power_perc_R = power_perc_R;
  prev_ESC_L_value = ESC_L_value;
  prev_ESC_R_value = ESC_R_value;
} // loop


