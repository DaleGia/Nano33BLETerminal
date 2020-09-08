#include <Arduino.h>
#include "Nano33BLETerminal.h"

#define LED_PIN     22
void ledToggle(void);

void setup() 
{
  
  /* Serial must be started before Terminal! */
  Serial.begin(115200);
  while(!Serial);
 
  pinMode(LED1, OUTPUT);

  Terminal.registerCommand("led-toggle", ledToggle);
  Terminal.begin();
}

void loop() 
{
}

void ledToggle(void)
{
  static bool state = false;
  state = !state;
  digitalWrite(LED_PIN, state);
}