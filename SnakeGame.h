// SnakeGame.h
#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <stdint.h>

// Board size (fits 128×160 display in 8×8 cells)
#define SNAKE_COLS 16
#define SNAKE_ROWS 20
#define CELL_SIZE 8

extern int Speed_Setting;
extern bool gameStarted;
void Snake_StartGame(void);

typedef enum { UP, DOWN, LEFT, RIGHT } Direction_t;

void Snake_Init(void);
void Snake_Update(void);    // advance one step
void Snake_Draw(void);      // render on ST7735
bool Snake_IsGameOver(void);
void Snake_Grow(void);      // call when food eaten
void Snake_SetDirection(Direction_t dir);
bool Snake_IsStarted(void);

// ─── Wall boundaries ─────────────────────────
#define WALL_LEFT    1
#define WALL_RIGHT   (SNAKE_COLS-2)
#define WALL_TOP     1
#define WALL_BOTTOM  (SNAKE_ROWS-2)

// ─── Difficulty levels ──────────────────────
typedef enum { DIFF_EASY, DIFF_MEDIUM, DIFF_HARD } Difficulty_t;

// ─── New API ───────────────────────────────
void Snake_SetDifficulty(Difficulty_t d);
uint16_t Snake_GetScore(void);
uint16_t Snake_GetHighScore(void);
void Snake_EnqueueDirection(Direction_t d);
#endif // SNAKE_GAME_H