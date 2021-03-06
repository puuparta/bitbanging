#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "include/lm4f/lm4f120e5qr.h"

#define INT_TIMER_FLAG_NONE 0x00000000
#define INT_TOGGLE_LED 0x00000001

volatile uint32_t t_flags = INT_TIMER_FLAG_NONE;

// pasihe: timer interrupt handler set in cstartup_M.c
// we set flag on that timer0a interrupt called
// no any time consuming logic allowed in interrupt, slows down MCU.
// minimum processing as possible.
void Timer0A_Handler(void)
{  
  t_flags = INT_TOGGLE_LED;
  // If the GPTM Timer A Time-Out Raw Interrupt bit (TATORIS) is 1 then
  // we have Timer A timeout.
  // Clear TATORIS by writing a 1 to the TATOCINT bit in the 
  // GPTMICR register. So counter restarts counting.
  TIMER0->ICR |=(1<<0);
}

void interrupt() 
{
  /*
  Programmable timers can be used to count or time external events that drive 
  the Timer input pins.
  
  Timer modes and initialization and configuration is described in
  datasheet starting at page 683.
  
  To use a GPTM, the appropriate TIMERn bit must be set in the RCGCTIMER or 
  RCGCWTIMER register (see page 313 and page 330). If using any CCP pins, 
  the clock to the appropriate GPIO module must be enabled via the RCGCGPIO 
  register (see page 315). To find out which GPIO port to enable, refer to 
  Table 21-4 on page 1138. Configure the PMCn fields in the GPIOPCTL register 
  to assign the CCP signals to the appropriate pins (see page 649 and 
  Table 21-5 on page 1142).
  */
  
  // set GPIOF blue LED (PF2)
  SYSCTL->RCGCGPIO |= (1<<5);   // enable clock on PORT F
  GPIOF->AFSEL &= ~(1<<2);      // disable alternate function on PF2 for sure
  GPIOF->DIR = (1<<2);          // PF2 as output
  GPIOF->DEN = (1<<2);          // enable PF2 
  GPIOF->DATA |= (1<<2);        // set PF1 high (led on)
  
  
  // 11.4 Initialization and Configuration of timer, lets choose TIMER0
  SYSCTL->RCGCTIMER = (1<<0);    // bit 0 to 1
  
  /* 11.4.1 One-Shot/Periodic Timer Mode */
  
  // 1. Ensure the timer is disabled (the TnEN bit in the GPTMCTL register 
  // is cleared) before making any changes to timer.
  TIMER0->CTL &= ~(1<<0);       // clear bit 0
  
  // 2. Write the GPTM Configuration Register (GPTMCFG) with a value of 0x0000.0000.
  TIMER0->CFG = 0x00000000;
  
  // 3. Configure the TnMR field in the GPTM Timer n Mode Register (GPTMTnMR)
  TIMER0->TAMR = (0x2<<0);      // We use periodic mode, LSB is 0.
  
  // 4. Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits in the GPTMTnMR register
  // to select whether to capture the value of the free-running timer at time-out, use an external
  // trigger to start counting, configure an additional trigger or interrupt, and count up or down.
  TIMER0->TAMR &= ~(1<<4);      // GPTM Timer A Count Direction, set 0 to count down.
  
  // 5. Load the start value into the GPTM Timer n Interval Load Register (GPTMTnILR).
  // 16M correspondence to clock speed of MCU.
  // From RCC2 registry, the default value of bit OSCSRC2 is 0x1
  // which points to PIOSC.
  TIMER0->TAILR = 0x00F42400;   // 16M
  // So counter takes 16M clock cycles to zero and because that is same than
  // default clock speed (16MHz) of this MCU the time it takes is 1 second.
  /* 
  5.2.5.3 Precision Internal Oscillator Operation (PIOSC) 
  The microcontroller powers up with the PIOSC running. If another clock 
  source is desired, the PIOSC must remain enabled as it is used for internal 
  functions. The PIOSC generates a 16-MHz clock with a �1% accuracy at room 
  temperatures. Across the extended temperature range, the accuracy is �3%.
  */
  
  // 6. If interrupts are required, set the appropriate bits in the GPTM 
  // Interrupt Mask Register (GPTMIMR).
  // GPTM Timer A Time-Out Interrupt Mask TATOIM bit
  TIMER0->IMR |= (1<<0);
  NVIC->ISER[0] |= (1<<19); // enable bit 19 which is our vector table entry
  // pasihe: another way to enable is to use core_cm4.h file: NVIC_EnableIRQ
  // NVIC_EnableIRQ(TIMER0A_IRQn);
  // surely advance of using function is that we don't have to know EN registry
  // index at all since it is calculated in function.
  // anyway function and direct ISER-manipulation is the same.
  
  // 7. Set the TnEN bit in the GPTMCTL register to enable the timer and 
  // start counting.
  TIMER0->CTL |=(1<<0);
  
  /*
  8. Poll the GPTMRIS register or wait for the interrupt to be generated 
  (if enabled). In both cases, the status flags are cleared by writing a 1 to 
  the appropriate bit of the GPTM Interrupt Clear Register (GPTMICR).
  */
  while(1)
  {
    // pasihe: NOT needed to poll here, we just copy implementation 
    // to Timer interrupt handler
    // instead of polling, we check is interrupt flag set
    if(t_flags == INT_TOGGLE_LED) 
    {
      GPIOF->DATA ^=(1<<2); // blink the led
      t_flags = INT_TIMER_FLAG_NONE; //reset flag
    }
  }
}

#endif