// SnakeGame.cpp
#include "SnakeGame.h"
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "Sound.h"
#include "images/images.h"

#define INPUT_QUEUE_SIZE 3
static Direction_t dirQueue[INPUT_QUEUE_SIZE];
static uint8_t qHead = 0, qTail = 0, qCount = 0;

// new globals for score & difficulty  
static uint16_t score = 0;

static uint16_t highScore = 0;  
static Difficulty_t difficulty = DIFF_MEDIUM;  
extern uint32_t Random(uint32_t n);

typedef struct { uint8_t x, y; } Cell_t;

static Cell_t snake[ SNAKE_COLS * SNAKE_ROWS ];
static uint16_t snakeLen;
static Cell_t food;
static Direction_t dir;
static bool gameOver;
static bool Snake_DequeueDirection(Direction_t *out);

bool gameStarted = false;

// draw one cell in color
static void DrawCell(uint8_t col, uint8_t row, uint16_t color){
  ST7735_FillRect(col*CELL_SIZE, row*CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

void Snake_Init(void){
  gameStarted = false;
  qHead = qTail = qCount = 0;
  snakeLen = 3;
  score = 0;
  dir = RIGHT;
  gameOver = false;

  // Snake start position
  snake[0] = (Cell_t){SNAKE_COLS/2, SNAKE_ROWS/2};
  snake[1] = (Cell_t){SNAKE_COLS/2 - 1, SNAKE_ROWS/2};
  snake[2] = (Cell_t){SNAKE_COLS/2 - 2, SNAKE_ROWS/2};
    ST7735_FillScreen(ST7735_BLACK);
  // Draw walls (color = BLUE)
    for (int x = 0; x < SNAKE_COLS; x++) {
    DrawCell(x, 0, ST7735_BLUE); // Top wall
    DrawCell(x, SNAKE_ROWS - 1, ST7735_BLUE); // Bottom wall
    }
    for (int y = 1; y < SNAKE_ROWS - 1; y++) {
    DrawCell(0, y, ST7735_BLUE); // Left wall
    DrawCell(SNAKE_COLS - 1, y, ST7735_BLUE); // Right wall
    }

  // Place food safely
  bool valid;
  do {
    food.x = WALL_LEFT + Random(WALL_RIGHT - WALL_LEFT + 1);
    food.y = WALL_TOP  + Random(WALL_BOTTOM - WALL_TOP + 1);
    valid = true;
    for(int i = 0; i < snakeLen; i++){
      if(snake[i].x == food.x && snake[i].y == food.y){
        valid = false;
        break;
      }
    }
  } while(!valid);

  ST7735_DrawBitmap(food.x * CELL_SIZE, food.y * CELL_SIZE + 7 ,apple, CELL_SIZE, CELL_SIZE); // draw new food

  // Draw initial snake
  for (int i = 0; i < snakeLen; i++) {
    DrawCell(snake[i].x, snake[i].y, ST7735_GREEN);
  }
}


void Snake_SetDirection(Direction_t d){
  // prevent 180° reversal
  if ((d==UP && dir!=DOWN) ||
      (d==DOWN && dir!=UP) ||
      (d==LEFT && dir!=RIGHT) ||
      (d==RIGHT && dir!=LEFT)) {
    dir = d;
  }
}

bool Snake_IsGameOver(void){ return gameOver; }

void Snake_Update(void){

  if(gameOver || !gameStarted) return;

  // 1) Pull one pending direction, if any
  Direction_t nd;
  if(Snake_DequeueDirection(&nd)){
    if((nd == UP    && dir != DOWN ) ||
       (nd == DOWN  && dir != UP   ) ||
       (nd == LEFT  && dir != RIGHT) ||
       (nd == RIGHT && dir != LEFT  )) {
      dir = nd;
    }
  }

  // 2) Compute new head position
  Cell_t newHead = snake[0];
  switch(dir){
    case UP:    newHead.y--; break;
    case DOWN:  newHead.y++; break;
    case LEFT:  newHead.x--; break;
    case RIGHT: newHead.x++; break;
  }

  // 3) Check if food is eaten
  bool ateFood = (newHead.x == food.x && newHead.y == food.y);

  // 4) Shift body and insert new head
  for(int i = snakeLen; i > 0; i--){
    snake[i] = snake[i-1];
  }
  snake[0] = newHead;

  // 5) Handle growth
  if(ateFood){
    Snake_Grow();
    Sound_Shoot();

    // Respawn food safely
    bool valid;
    do {
      food.x = WALL_LEFT + Random(WALL_RIGHT - WALL_LEFT + 1);
      food.y = WALL_TOP  + Random(WALL_BOTTOM - WALL_TOP + 1);
      valid = true;
      for(int i = 0; i < snakeLen; i++){
        if(snake[i].x == food.x && snake[i].y == food.y){
          valid = false;
          break;
        }
      }
    } while(!valid);

    ST7735_DrawBitmap(food.x * CELL_SIZE, food.y * CELL_SIZE + 7,apple, CELL_SIZE, CELL_SIZE); // draw new food

  } else {
    // Clear tail if not growing
    DrawCell(snake[snakeLen].x, snake[snakeLen].y, ST7735_BLACK);
  }

  // 6) Check wall collision
  if(newHead.x < WALL_LEFT || newHead.x > WALL_RIGHT ||
     newHead.y < WALL_TOP  || newHead.y > WALL_BOTTOM){
    gameOver = true;
    Sound_Killed();

    // Flash snake red to indicate death
    for (int flash = 0; flash < 2; flash++) {
    for (int i = 0; i < snakeLen; i++) {
        DrawCell(snake[i].x, snake[i].y, (flash % 2 == 0) ? ST7735_RED : ST7735_GREEN);
    }
    Clock_Delay1ms(400); // 200ms delay
}

return;

  }

  // 7) Check self collision (ignore head at i=0)
  for(int i = 1; i < snakeLen; i++){
    if(snake[i].x == newHead.x && snake[i].y == newHead.y){
      gameOver = true;
        Sound_Killed();

        // Flash snake red to indicate death
        for (int flash = 0; flash < 2; flash++) {
        for (int i = 0; i < snakeLen; i++) {
            DrawCell(snake[i].x, snake[i].y, (flash % 2 == 0) ? ST7735_RED : ST7735_GREEN);
        }
        Clock_Delay1ms(400); // 200ms delay
}

return;

    }
  }

  // 8) Draw new head
  DrawCell(newHead.x, newHead.y, ST7735_GREEN);
}



void Snake_Grow(void){
  if(snakeLen < SNAKE_COLS*SNAKE_ROWS - 1){
    snakeLen++;
    score += (60/Speed_Setting) ;
    if(score > highScore) highScore = score;
  }
}

void Snake_Draw(void){
  // clear board

  // draw food
  ST7735_DrawBitmap(food.x * CELL_SIZE, food.y * CELL_SIZE + 7,apple, CELL_SIZE, CELL_SIZE); // draw new food
  // draw snake
  for (int i = 0; i < snakeLen; i++){
    DrawCell(snake[i].x, snake[i].y, ST7735_GREEN);
    if (i == 0) {
      if(dir == UP) {
       ST7735_DrawBitmap(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE + 7,head_up, 8, 8); // draw new food
      }
      if(dir == DOWN) {
       ST7735_DrawBitmap(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE + 7,head_down, 8, 8); // draw new food
      }
      if(dir == LEFT) {
       ST7735_DrawBitmap(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE + 7,head_left, 8, 8); // draw new food
      }
      if(dir == RIGHT) {
       ST7735_DrawBitmap(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE + 7,head_right, 8, 8); // draw new food
      }
    }
  }
  if (gameOver) {
    ST7735_SetCursor(3,10);
    ST7735_OutString("GAME OVER");
  }
}

void Snake_SetDifficulty(Difficulty_t d){
  difficulty = d;
}

uint16_t Snake_GetScore(void){
  return score;
}
uint16_t Snake_GetHighScore(void){
  return highScore;
}


void Snake_EnqueueDirection(Direction_t d){
  // if queue not empty, look at the last thing we enqueued:
  if(qCount > 0){
    uint8_t lastIdx = (qTail + INPUT_QUEUE_SIZE - 1) % INPUT_QUEUE_SIZE;
    if(dirQueue[lastIdx] == d) return; // skip duplicates
  }
  if(qCount < INPUT_QUEUE_SIZE){
    dirQueue[qTail] = d;
    qTail = (qTail + 1) % INPUT_QUEUE_SIZE;
    qCount++;
  }
}

static bool Snake_DequeueDirection(Direction_t *out){
  if(qCount == 0) return false;
  *out = dirQueue[qHead];
  qHead = (qHead + 1) % INPUT_QUEUE_SIZE;
  qCount--;
  return true;
}


void Snake_StartGame(void) {
  gameStarted = true;
}