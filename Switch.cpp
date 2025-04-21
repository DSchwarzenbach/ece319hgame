/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    // write this
  IOMUX->SECCFG.PINCM[PB13INDEX] = 0x00050081; // UP - input,  no pull
  IOMUX->SECCFG.PINCM[PA28INDEX] = 0x00050081; // LEFT - input, no pull
  IOMUX->SECCFG.PINCM[PA16INDEX] = 0x00050081; // DOWN - input, no pull
  IOMUX->SECCFG.PINCM[PA17INDEX] = 0x00050081; // RIGHT - input, no pull
}
// return current state of switches
uint32_t Switch_In(void){
  uint32_t result = 0;
  
  // UP on PB13 (bit 13 of GPIOB)
  if (GPIOB->DIN31_0 & (1 << 13)) result |= 0x01; // bit 0
  int debug = GPIOA->DIN31_0;

  // DOWN on PA16 (bit 16)
  if (GPIOA->DIN31_0 & (1 << 16)) result |= 0x02; // bit 1

  // LEFT on PA28 (bit 28)
  if (GPIOA->DIN31_0 & (1 << 28)) result |= 0x04; // bit 3

  // RIGHT on PA17 (bit 17)
  if (GPIOA->DIN31_0 & (1 << 17)) result |= 0x08; // bit 2

  return result; // return 0; //replace this your code
}
