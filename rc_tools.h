
class RC_Output {
  public:
    RC_Output() {};
    virtual void set(int rc_val) = 0;
    virtual void setup(void) = 0;
    virtual void failsafe(void) = 0;
    virtual void update(void) = 0;
};

class RC_Servo: public RC_Output {
  public:
    RC_Servo(int pos_pin) {};
    virtual void set(int rc_val) {};
    virtual void setup(void) {};
    virtual void failsafe(void) {};
    virtual void update(void) {};
};

class RC_StepperSpeedControl: public RC_Output {
  public:
    RC_StepperSpeedControl(int stp_pin, int dir_pin, int en_pin, int max_speed) {};
    virtual void set(int rc_val) {};
    virtual void setup(void) {};
    virtual void failsafe(void) {};
    virtual void update(void) {};
};

class RC_Relay: public RC_Output {
  public:
    RC_Relay(int pin) {};
    virtual void set(int rc_val) {};
    virtual void setup(void) {};
    virtual void failsafe(void) {};
    virtual void update(void) {};
};

class RC_DigitalPotentiometer: public RC_Output {
  public:
    RC_DigitalPotentiometer(int adress, int en_pin) {};
    virtual void set(int rc_val) {};
    virtual void setup(void) {};
    virtual void failsafe(void) {};
    virtual void update(void) {};
};

class RC_Input {
  public:
    RC_Input() {};
    virtual void setup(void) = 0;
    virtual int update(int RC_Values[]) = 0;
};

class RC_PPM: public RC_Input {
  public:
    RC_PPM(int pin, int max_inputs) {};
    virtual void setup(void) {};
    virtual int update(int RC_Values[]) {};
};

class RC_PWM: public RC_Input {
  public:
    RC_PWM(int pin);
    virtual void setup(void) {};
    virtual int update(int RC_Values[]) {};
};

class RC_UART: public RC_Input {
  public:
    RC_UART(int *uart, long speed);
    virtual void setup(void) {};
    virtual int update(int RC_Values[]) {};
};
