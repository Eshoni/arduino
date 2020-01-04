
#include "rc_tools.h"

RC_Output  * const myRC_Outputs[] = {
  new RC_Servo(11),
  new RC_Servo(12),
  new RC_DigitalPotentiometer(1, 13),
  new RC_DigitalPotentiometer(2, 13),
  new RC_Relay(14)
};

#define NUM_RC_OUTPUTS 5
#define MAX_RC_INPUTS 32
RC_Input * myRC_Input = new RC_PPM(15, MAX_RC_INPUTS);

unsigned long int timer;
int failsafe = 1;
#define TIMEOUT 1000

void setup() {
  myRC_Input->setup();
  for (int i = 0; i < NUM_RC_OUTPUTS; i++) myRC_Outputs[i]->setup();
  timer = millis();
}

void loop() {
  int vals[MAX_RC_INPUTS];

  if ( (millis() - timer > TIMEOUT) && !failsafe )
  {
    failsafe = true;
    for (int i = 0; i < NUM_RC_OUTPUTS; i++) myRC_Outputs[i]->failsafe();
  }

  if (myRC_Input->update(vals) >= NUM_RC_OUTPUTS)
  {

    timer = millis();
    failsafe = false;
    for (int i = 0; i < NUM_RC_OUTPUTS; i++) myRC_Outputs[i]->set(vals[i]);
  }

  for (int i = 0; i < NUM_RC_OUTPUTS; i++) myRC_Outputs[i]->update();
}
