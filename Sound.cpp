// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

// Sound playback globals
static const uint8_t* SoundPt;     // Pointer to current sound array
static uint32_t SoundCount;        // Number of samples to play
static uint32_t SoundIndex;        // Current index into sound array
static bool SoundActive = false;   // Whether sound is playing


void SysTick_IntArm(uint32_t period, uint32_t priority){
  // write this
  SysTick->CTRL = 0x00;      // disable SysTick during setup
  SysTick->LOAD = period-1;  // reload value
  SCB->SHP[1] = (SCB->SHP[1]&(~0xC0000000))|(priority<<30); // priority 2
  SysTick->VAL = 0;          // any write to VAL clears COUNT and sets VAL equal to LOAD
  SysTick->CTRL = 0x07;      // enable SysTick with 80 MHz bus clock and interrupts

}

// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
// write this
  // write this
  DAC5_Init();                            // Init 5-bit DAC on PB0–PB4
  SysTick_IntArm(7272, 2);                // 80 MHz / 7272 ≈ 11 kHz
  SoundActive = false;                    // no sound playing yet

}
extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
    // output one value to DAC if a sound is active
      // write this
    // output one value to DAC5
 //   DAC5_Out(SoundArray[CurrentSound][Index]);    // output one value each interrupt
 //   Index = ((Index+1)& 0x01F)%(FrequencyArray[CurrentSound]);     // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3,... 

    if (SoundActive) {
    DAC5_Out(SoundPt[SoundIndex]);        // output next sample
    SoundIndex++;
    if (SoundIndex >= SoundCount) {
      SoundActive = false;                // stop when done
      DAC5_Out(0);                        // silence
    }
  }

}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){

  if (pt == nullptr || count == 0) return;

  SoundPt = pt;
  SoundCount = count;
  SoundIndex = 0;
  SoundActive = true;
  
}

void Sound_Shoot(void){
// write this
  Sound_Start(explosion, 2000);
}
void Sound_Killed(void){
// write this
  Sound_Start(fastinvader2, 1042);
}
void Sound_Explosion(void){
// write this
  Sound_Start(fastinvader3, 1054);
}

void Sound_Fastinvader1(void){
  Sound_Start(shoot, 4080);

}
void Sound_Fastinvader2(void){

}
void Sound_Fastinvader3(void){

}
void Sound_Fastinvader4(void){

}
void Sound_Highpitch(void){

}
