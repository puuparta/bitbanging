#ifndef LEDS_H
#define LEDS_H


#include <lm4f120e5qr.h>

void wait(void);

void leds()
{
 // main datasheet: http://www.mouser.com/ds/2/405/lm4f120h5qr-124014.pdf
  /* Register 51: General-Purpose Input/Output Run Mode 
     Clock Gating Control (SCGCGPIO), offset 0x608.
     The RCGCGPIO register provides software the capability to enable and disable GPIO modules in
     Run mode. When enabled, a module is provided a clock and accesses to module registers are
     allowed. When disabled, the clock is disabled to save power and accesses to module registers
     generate a bus fault.
  
     This register should be used to control the clocking for the GPIO modules
  */
  
  // from schemantics we see that RGB leds are connected to PF1, PF2, PF3
  // http://www.ti.com/lit/ug/spmu289c/spmu289c.pdf
  // so we enable PORTF
  SYSCTL->RCGCGPIO = 0x20; // 0b100000 (enable PORTF only)
  
  // Register 2: GPIO Direction (GPIODIR), offset 0x400
  // set PF1, PF2, PF3 as output and enable pins
  GPIOF->DIR = 0xE; //0b1110; 
  GPIOF->DEN = 0xE;
  
  while(1)
  {
    GPIOF->DATA = 0x02; // 0b0010 red on 
    wait();
    GPIOF->DATA = 0x04; // 0b0100 blue on
    wait();
    GPIOF->DATA = 0x08; // 0b1000 green on
    wait();
    
  }
}


void wait() 
{
  int c = 0;
  while(c++<1000000);
}
#endif