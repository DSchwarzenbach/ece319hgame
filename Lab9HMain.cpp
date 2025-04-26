// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Dominic Schwarzenbach and Saketh Chinamuthevi
// Last Modified: 12/26/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"
#include "SnakeGame.h"
typedef enum { STATE_START, STATE_PLAY, STATE_END } GameState_t;
static GameState_t gameState = STATE_START;
static Difficulty_t selectedDiff = DIFF_MEDIUM;  // default

//we want to call sound killed and sound shoot

// add our two languages
typedef enum { LANG_EN, LANG_ES } GameLang_t;
static GameLang_t gameLang = LANG_EN;


// semaphore for 30 Hz tick
static volatile bool TickFlag = false;

#define LED1 (1 << 12)
#define LED2 (1 << 16)
#define LED3 (1 << 17)
#define LED4 (1 << 19)
int Speed_Setting = 10;

extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
extern bool Snake_IsStarted(void);


// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

SlidePot Sensor(1749,122); // copy calibration from Lab 7

// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){
  if((TIMG12->CPU_INT.IIDX) == 1){      // acknowledge interrupt
    // read switches exactly once
    uint8_t sw      = Switch_In();
    static uint8_t lastSw = 0;
    // bits that just went from 0→1
    uint8_t changes = sw & ~lastSw;
    lastSw = sw;

    if (!Snake_IsGameOver() && !gameStarted && changes) {
      Snake_StartGame();  // start the snake
      return;             // skip rest of this tick to avoid immediate movement
    }

    int slide_input = Sensor.In();
    Speed_Setting = ((slide_input*(30)) / 4095);

    // enqueue each newly pressed direction exactly once
    if(changes & 0x01) Snake_EnqueueDirection(UP);
    if(changes & 0x02) Snake_EnqueueDirection(DOWN);
    if(changes & 0x04) Snake_EnqueueDirection(LEFT);
    if(changes & 0x08) Snake_EnqueueDirection(RIGHT);

    // tell main loop it’s time to step/draw
    TickFlag = true;

    // (your existing debug‐toggles, etc)
    GPIOB->DOUTTGL31_0 = GREEN;
    GPIOB->DOUTTGL31_0 = GREEN;
  }
}


uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};
// use main1 to observe special characters
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0x0000);            // set screen to black
  for(int myPhrase=0; myPhrase<= 2; myPhrase++){
    for(int myL=0; myL<= 3; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString((char *)"GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString((char *)"Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString((char *)"Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}


// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  int light = 0;
      
  
  while(1){
    LED_Off(LED1 | LED2 | LED3 | LED4);
    light = Switch_In();
    // write code to test switches and LEDs
    // Map each switch to a LED
    if (light & 0x01) LED_On(LED1);  // UP
    if (light & 0x02) LED_On(LED2);  // DOWN
    if (light & 0x04) LED_On(LED3);  // LEFT -> LED that doesnt work
    if (light & 0x08) LED_On(LED4);  // RIGHT
    Clock_Delay(100);
  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
 // TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
    last = now;
  }
}
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init();               // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(ST7735_BLACK);

  // initialize peripherals BEFORE language screen
  Sensor.Init();            // slide pot
  Switch_Init();            // buttons
  LED_Init();               // LEDs
  Sound_Init();             // sounds
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26);
  TimerG12_IntArm(2666667,1); // 30 Hz
  __enable_irq();

  // —— LANGUAGE SELECTION SCREEN ——
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_SetCursor(1,2);
  ST7735_OutString((char*)"Select Language:");
  ST7735_SetCursor(1,4);
  ST7735_OutString((char*)"UP = English");
  ST7735_SetCursor(1,6);
  ST7735_OutString((char*)"DOWN = Espa\xA4ol");
  // wait for UP or DOWN
  ST7735_DrawBitmap(20, 140, AppleBig, 22, 22);
  while(1){
    uint8_t sw = Switch_In();
    if(sw & 0x01){ gameLang = LANG_EN; break; }
    if(sw & 0x02){ gameLang = LANG_ES; break; }
  }
  Clock_Delay1ms(500);
  Sound_Fastinvader4();
  static uint16_t tickCounter;

MENU:
  // ─── STATE_START ───────────────────────────────────────────────
  ST7735_FillScreen(ST7735_BLACK);
  if(gameLang == LANG_EN){
    ST7735_SetCursor(1,2); ST7735_OutString((char*)"SNAKE GAME");
    ST7735_SetCursor(1,5); ST7735_OutString((char*)"PRESS RIGHT");
    ST7735_SetCursor(1,6); ST7735_OutString((char*)"TO START");
  } else {
    ST7735_SetCursor(1,2); ST7735_OutString((char*)"JUEGO SNAKE");
    ST7735_SetCursor(1,5); ST7735_OutString((char*)"PULSAR DERECHA");
    ST7735_SetCursor(1,6); ST7735_OutString((char*)"PARA INICIAR");
  }
  ST7735_DrawBitmap(20, 150, AppleBig, 22, 22);

  // speed‐selection readout until start pressed
  {
    int last_spd = -1, ctr = 0;
    while(!(Switch_In() & 0x08)){ // wait for RIGHT
      if(++ctr >= 10){
        ctr = 0;
        int v = Sensor.In();
        Speed_Setting = (v*30)/4095;
        int disp = 30 - Speed_Setting;
        if(disp != last_spd){
          ST7735_SetCursor(1,8);
          // first clear any leftover digits
          if(gameLang==LANG_EN){
            ST7735_SetCursor(1,8); ST7735_OutString("SPEED (0-30):     ");
            ST7735_SetCursor(1,8); ST7735_OutString("SPEED (0-30):");
            ST7735_OutUDec(disp);

          } else {
            ST7735_SetCursor(1,8); ST7735_OutString("VELOCIDAD(0-30):  ");
            ST7735_SetCursor(1,8); ST7735_OutString("VELOCIDAD(0-30):");
            ST7735_OutUDec(disp);

          }
          last_spd = disp;
        }
      }
    }
  }

START:
  // initialize and draw first frame
  Snake_SetDifficulty(selectedDiff);
  Snake_Init();
  Snake_Draw();
  tickCounter = 0;
  gameState = STATE_PLAY;
  Sound_Fastinvader4();
  // ─── STATE_PLAY ────────────────────────────────────────────────
  while(gameState == STATE_PLAY){
    if(TickFlag){
      TickFlag = false;
      if(++tickCounter >= Speed_Setting){
        tickCounter = 0;
        Snake_Update();
        Snake_Draw();
        ST7735_SetCursor(0,0);
        if(gameLang==LANG_EN){
          ST7735_OutString((char*)"S:");
        } else {
          ST7735_OutString((char*)"P:");
        }
        ST7735_OutUDec(Snake_GetScore());
      }
    }
    if(Snake_IsGameOver()){
      gameState = STATE_END;
    }
  }

  // ─── STATE_END ─────────────────────────────────────────────────
  ST7735_FillScreen(ST7735_BLACK);
  if(gameLang==LANG_EN){
    ST7735_SetCursor(2,5); ST7735_OutString((char*)"GAME OVER");
    ST7735_SetCursor(2,6); ST7735_OutString((char*)"SCORE:");
    ST7735_OutUDec(Snake_GetScore());
    ST7735_SetCursor(2,7); ST7735_OutString((char*)"HIGH:");
    ST7735_OutUDec(Snake_GetHighScore());
    ST7735_SetCursor(2,9); ST7735_OutString((char*)"RIGHT=RESTART");
    ST7735_SetCursor(2,10);ST7735_OutString((char*)"LEFT=MAIN MENU");
  } else {
    ST7735_SetCursor(2,5); ST7735_OutString((char*)"FIN DEL JUEGO");
    ST7735_SetCursor(2,6); ST7735_OutString((char*)"PUNTAJE:");
    ST7735_OutUDec(Snake_GetScore());
    ST7735_SetCursor(2,7); ST7735_OutString((char*)"MEJOR:");
    ST7735_OutUDec(Snake_GetHighScore());
    ST7735_SetCursor(2,9); ST7735_OutString((char*)"DERECHA=REINICIAR");
    ST7735_SetCursor(2,10);ST7735_OutString((char*)"IZQUIERDA=MENU");
  }

  // wait for RESTART or back to MENU
  while(1){
    uint8_t in = Switch_In();
    if(in & 0x08) goto START;
    if(in & 0x04) goto MENU;
  }
}


bool Snake_IsStarted(void) {
  return gameStarted;
}