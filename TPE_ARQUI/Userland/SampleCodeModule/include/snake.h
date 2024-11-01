#ifndef SNAKE_H
#define SNAKE_H

#include <snake.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <userLibrary.h>

#define WIDTH 19
#define HEIGHT 19
#define INITIAL_SNAKE_LENGTH 3
#define SQUARE_SIZE 40
#define MAX_SNAKE_LENGTH WIDTH *HEIGHT - 1

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
  int x;
  int y;
} Point;

typedef struct {
  Point body[MAX_SNAKE_LENGTH];
  int length;
  Direction dir;
} Snake;

typedef struct {
  Point food;
  Snake snake;
  int gameOver;
  int board[HEIGHT][WIDTH];
} GameState;

void snake(int players);
void drawBoard(GameState *game);
void redrawBoard(GameState *game, uint32_t colorReciever);
void drawSquare(int x, int y, uint32_t color);

#endif