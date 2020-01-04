#include <Servo.h>

#define NUM_MOTORS 5
#define TIMEOUT 1000  //in ms
#define MIN_RC_VAL 1000
#define MAX_RC_VAL 2000 //only ued for rejecting commands out of range. 
//center is always considered to be 1500, and range +-500.
//i.e. 2100 is more than max speed for steppers...

#define MICROSTEPS 16 //microsteps per step
#define STEPS 200     //steps per revs (1.8deg per step)
#define STEPPER_MAX_SPEED 1*MICROSTEPS*STEPS //in revs per second

#define X_STEP_PIN    54
#define X_DIR_PIN     55
#define X_ENABLE_PIN  38

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62

#define SERVO0_PIN 11
#define SERVO1_PIN 12

#define MY_SERIAL Serial
#define DEBUG

class Motor {
  public:
    Motor() {};
    virtual void set(int rc_val) = 0;
    virtual void setup(void) = 0;
    virtual void failsafe(void) = 0;
    virtual void update(void) = 0;
};

class RcMotor: public Motor {
    const int pos_pin;
    int enabled = false;
    Servo myServo;
  public:
    RcMotor(int _pos_pin): pos_pin(_pos_pin) {}
    void setup(void) {
      MY_SERIAL.println("rc_servo: pos_pin " + String(pos_pin) );
    }
    void set(int rc_val) {
      if (!enabled) myServo.attach(pos_pin);
      myServo.writeMicroseconds(rc_val);
    }
    void update(void) {}
    void failsafe(void) {
      if (myServo.attached())
      {
        myServo.detach();
      }
    }
};

class StepperMotor: public Motor {
    const int step_pin, enable_pin, direction_pin;
    const unsigned long int max_speed;
    unsigned long int duration = 0, t0 = 0;
    int pin = HIGH;
  public:
    StepperMotor(int _step_pin, int _enable_pin, int _direction_pin, unsigned long int _max_speed):
      step_pin(_step_pin), enable_pin(_enable_pin), direction_pin(_direction_pin), max_speed(_max_speed) {
      pinMode(step_pin, OUTPUT); pinMode(enable_pin, OUTPUT); pinMode(direction_pin, OUTPUT);
      digitalWrite(enable_pin, HIGH);
    };
    void setup(void)
    {
      MY_SERIAL.println("stepper: step_pin " + String(step_pin) + ", enable_pin " + String(enable_pin) + ", direction_pin " + String(direction_pin) + ", max_speed " + String(max_speed ) + " microsteps/sec");
    }
    void set(int rc_val) {
      digitalWrite(enable_pin, LOW); //enabled
      if (rc_val != 1500) duration = 1000000 * 500 / (2 * max_speed * abs(rc_val - 1500) );
      else duration = 0;
      digitalWrite(direction_pin, (rc_val > 1500) ? HIGH : LOW);
    }
    void update(void) {
      if (duration == 0) return;
      unsigned long int tf = micros(), t = tf - t0;
      if (t > duration) {
        digitalWrite(step_pin, pin = !pin);
        t0 = tf;
      }
    }
    void failsafe(void) {
      digitalWrite(enable_pin, HIGH);//disabled
    }
};

class Parser {
    char msg[1000];
    int l = 0;
    int val = 0;
    int vals[NUM_MOTORS];
    int i = 0;
    unsigned long int timer = 0;
  public:
    void setup(long rate) {
      MY_SERIAL.begin(rate);
      MY_SERIAL.println(String(NUM_MOTORS) + " motors, range: " + String(MIN_RC_VAL) + " " + String(MAX_RC_VAL));
      MY_SERIAL.println("Only digits, SPACE and newline (\\n) are considered");
    };
    int update(int *_vals) {
      if (MY_SERIAL.available() < 1) return -1;
      const char c = MY_SERIAL.read();
      msg[l++] = c;
      if ( isDigit(c) ) {
        val *= 10;
        val += c - '0';
      }
      if ( (c == ' ') || (c == '\n') ) {
        if ( (val >= MIN_RC_VAL) && (val <= MAX_RC_VAL) ) vals[i++] = val;
        l = 0;
        val = 0;
        if (c == '\n') {
          int j;
          for (j = 0; j < i; j++) _vals[j] = vals[j];
          i = 0;
          if (j == NUM_MOTORS)  return j;
        }
      }
      return -1;
    }
} myParser;


Motor * const myMotors[] =
{
  new StepperMotor(X_STEP_PIN, X_ENABLE_PIN, X_DIR_PIN, STEPPER_MAX_SPEED),
  new StepperMotor(Y_STEP_PIN, Y_ENABLE_PIN, Y_DIR_PIN, STEPPER_MAX_SPEED),
  new StepperMotor(Z_STEP_PIN, Z_ENABLE_PIN, Z_DIR_PIN, STEPPER_MAX_SPEED),
  new RcMotor(SERVO0_PIN),
  new RcMotor(SERVO1_PIN)
};

unsigned long int timer = 0;
int failsafe = 1;

void setup() {
  myParser.setup(115200); //Open communication

  for (int i = 0; i < NUM_MOTORS; i++) myMotors[i]->setup(); 

  MY_SERIAL.println("timeout is " + String(TIMEOUT) + "ms");
  MY_SERIAL.println("Ready");
}

void loop() {
  int vals[NUM_MOTORS];

  if ( (millis() - timer > TIMEOUT) && !failsafe )
  {
    failsafe = true;
    for (int i = 0; i < NUM_MOTORS; i++) myMotors[i]->failsafe();
#ifdef DEBUG
    MY_SERIAL.println("failsafe");
#endif
  }

  if (myParser.update(vals) == NUM_MOTORS)
  {
#ifdef DEBUG
    for (int i = 0; i < NUM_MOTORS; i++) {
      MY_SERIAL.print(vals[i]);
      MY_SERIAL.print(" ");
    }
    MY_SERIAL.println("READY");
#endif
    timer = millis();
    failsafe = false;
    for (int i = 0; i < NUM_MOTORS; i++) myMotors[i]->set(vals[i]);
  }

  for (int i = 0; i < NUM_MOTORS; i++) myMotors[i]->update();
}
