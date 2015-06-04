#ifndef SWITCHES_H
#define SWITCHES_H

#include <lm4f120e5qr.h>

void switches() 
{
     
      // switches are connected to PF0 and PF4. See from schemantic.
  // USR_SW1 and USR_SW2
  // And to look switches more from schemantics, we can see that when those are closed they 
  // drive ground / zero to pin.
  // And by default MCU pin is in floating state. There are now to 
  // possibilites. We can use extrenal pull up resistor to drive pine high / one.
  // another is to use MCU internal pull up and that we will use now.
  /* Register 15: GPIO Pull-Up Select (GPIOPUR), offset 0x510
      The GPIOPUR register is the pull-up control register. When a bit is set, 
      a weak pull-up resistor on the corresponding GPIO signal is enabled. Setting a bit in
      GPIOPUR automatically clears the corresponding bit in the GPIO 
      Pull-Down Select (GPIOPDR) register. 
  */
  SYSCTL->RCGCGPIO = (1<<5); //0x20;      // 0b100000 (enable PORTF only)
  GPIOF->LOCK = 0x4C4F434B;
  GPIOF->CR = 0xff;
  GPIOF->DIR = 0x0E;//((1<<1) | (1<<2) | (1<<3)) | ~(1<<0) | ~(1<<4);  //0x0E;            //0b01110 PF0 and PF4 as input, PF1, PF2, PF3 as outputs
  GPIOF->PUR = 0x11; //(1<<0) | (1<<4); //0x11;            //0b10001 pull up PF0 and PF4
  GPIOF->DEN = 0x1F;            //0b11111 enable all needed pins PF0-PF4
 
  while(1)
  {
    switch(GPIOF->DATA & 0x11)
    {
    case 0x00: // both switches are pressed
      GPIOF->DATA = (1<<1); //0b10, turn on red led
      break;
    case 0x01:
      GPIOF->DATA = (1<<2);
      break;
    case 0x10:
      GPIOF->DATA = (1<<3);
      break;
    default:
      GPIOF->DATA &= ((1<<1) | (1<<2) | (1<<3)); // 0b
    }
  }
}

#endif