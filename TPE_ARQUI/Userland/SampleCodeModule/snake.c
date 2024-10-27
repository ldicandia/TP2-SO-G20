#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <userLibrary.h>
#include <time.h>
#include <snake.h>
#include <string.h>

#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 0xFFFFFFFF

static unsigned int lcg_seed = 1;
static int score = 0;

void lcg_srand() {
    unsigned int seconds = getSeconds();
    unsigned int minutes = getMinutes();
    unsigned int hours = getHours();
    lcg_seed = seconds + minutes * 60 + hours * 3600;
}

unsigned int lcg_rand() {
    lcg_seed = (LCG_A * lcg_seed + LCG_C) & LCG_M;
    return lcg_seed;
}


void initGame(GameState *game) {
    game->gameOver = 0;
    game->snake.length = INITIAL_SNAKE_LENGTH;
    game->snake.dir = RIGHT;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            game->board[y][x] = 0;
        }
    }

    game->food.x = WIDTH / 2;
    game->food.y = HEIGHT / 2;
    game->board[game->food.y][game->food.x] = 2; // Assuming 2 represents food

    drawBoard(game);
}

void drawBoard(GameState *game){
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            //if its the food square draw in color red
            if (game->board[y][x] == 2) {
                drawSquare(x * SQUARE_SIZE, y * SQUARE_SIZE, 0xFF0000);
            }else{
                drawSquare(x * SQUARE_SIZE, y * SQUARE_SIZE, 0x0000FF);
            }
        }
    }
    
}

void redrawBoard(GameState *game, uint32_t colorReciever) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int isSnake = 0;
            for (int i = 0; i < game->snake.length; i++) {
                if (game->snake.body[i].x == x && game->snake.body[i].y == y) {
                    isSnake = 1;
                    break;
                }
            }
            int currentSquare = isSnake ? 1 : (game->food.x == x && game->food.y == y) ? 2 : 0;
            if (game->board[y][x] != currentSquare) {
                int color = currentSquare == 1 ? colorReciever : (currentSquare == 2 ? 0xFF0000 : 0x0000FF);
                drawSquare(x * SQUARE_SIZE, y * SQUARE_SIZE, color);
                game->board[y][x] = currentSquare;
            }
        }
    }
}

void updateGame(GameState *game) {
    Point nextHead = game->snake.body[0];
    switch (game->snake.dir) {
        case UP: nextHead.y--; break;
        case DOWN: nextHead.y++; break;
        case LEFT: nextHead.x--; break;
        case RIGHT: nextHead.x++; break;
    }

    if (nextHead.x < 0 || nextHead.x >= WIDTH || nextHead.y < 0 || nextHead.y >= HEIGHT) {
        game->gameOver = 1;
        return;
    }

    for (int i = 0; i < game->snake.length; i++) {
        if (game->snake.body[i].x == nextHead.x && game->snake.body[i].y == nextHead.y) {
            game->gameOver = 1;
            return;
        }
    }

    for (int i = game->snake.length; i > 0; i--) {
        game->snake.body[i] = game->snake.body[i - 1];
    }
    game->snake.body[0] = nextHead;

    if (nextHead.x == game->food.x && nextHead.y == game->food.y) {
        game->snake.length++;
        game->food.x = lcg_rand() % WIDTH;
        game->food.y = lcg_rand() % HEIGHT;
        score++;
    }
}

void changeDirection(GameState *game, char input) {
    switch (input) {
        case 'w': if (game->snake.dir != DOWN) game->snake.dir = UP; break;
        case 's': if (game->snake.dir != UP) game->snake.dir = DOWN; break;
        case 'a': if (game->snake.dir != RIGHT) game->snake.dir = LEFT; break;
        case 'd': if (game->snake.dir != LEFT) game->snake.dir = RIGHT; break;
    }
}

void changeDirection2(GameState *game, char input) {
    switch (input) {
        case 'i': if (game->snake.dir != DOWN) game->snake.dir = UP; break;
        case 'k': if (game->snake.dir != UP) game->snake.dir = DOWN; break;
        case 'j': if (game->snake.dir != RIGHT) game->snake.dir = LEFT; break;
        case 'l': if (game->snake.dir != LEFT) game->snake.dir = RIGHT; break;
    }
}

void snake(int players) {
    if(players == 1){
        clear();
        lcg_srand(); // Initialize the custom random number generator
        GameState game = {0};
        initGame(&game);
        printStr("                                                                                                                Use WASD to move\n");
        printStr("                                                                                                     Score: ");
        printChar(score + '0');

        while (!game.gameOver) {
            if (getChar() != 0) {
                char input = getChar();
                changeDirection(&game, input);
            }
            updateGame(&game);
            redrawBoard(&game, 0x00FF00);
            sleep(100);
        }

        lcg_srand(); 
        initGame(&game);
        clear();
        increment_size_char();
        increment_size_char();
        printStr("Game Over!\n");
        decrement_size_char();
        decrement_size_char();
        sleep(1500);
        score = 0;
        clear();
    }else if(players == 2){//esta ultra lento esto
        GameState game1 = {0};
        GameState game2 = {0};
        clear();
        lcg_srand(); 
        initGame(&game1);
        initGame(&game2);
        printStr("                                                                                                     Player 1 'W-A-S-D' to move\n");
        printStr("                                                                                                     Player 2 'I-J-K-L' to move\n");


        while (!game1.gameOver && !game2.gameOver) {
            if (getChar() != 0) {
                char input = getChar();
                changeDirection(&game1, input);
                changeDirection2(&game2, input);
            }
            updateGame(&game1);
            updateGame(&game2);
            redrawBoard(&game1, 0x00FF00);
            redrawBoard(&game2, 0xFF00FF);
            sleep(250);
        }

        clear();
        increment_size_char();
        increment_size_char();
        printStr("Game Over!\n");
        decrement_size_char();
        decrement_size_char();
        sleep(1500);
        score = 0;
        clear();

    }else{
        clear();
        printStr("Invalid number of players\n");
        sleep(1000);
        clear();
    }
}