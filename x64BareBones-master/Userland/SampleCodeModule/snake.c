#include "snake.h"
#include <usr_stdlib.h>
#include <colores.h>
#include "sys_calls.h"

static unsigned long int next = 1;

int rand() {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}


#define WIDTH 40
#define HEIGHT 40
#define MAXDIM 100

static Color BLACK = {0,0,0};
static Color WHITE = {255,255,255};
static Color RED = {0,0,255};
//static Color LIGHT_BLUE = {255,255,255};
//static Color BLUE = {255,0,0};
static Color ORANGE = {16,160,255};
//static Color YELLOW = {30,224,255};
static Color PURPLE = {255,32,160};
//static Color PINK = {100,0,244};

#define PIXELWIDTH (get_scrWidht() / WIDTH)
#define PIXELHEIGHT (get_scrHeight() / HEIGHT)
#define MAX_BUFFER 254

#define PLAYER1_UP 'w'
#define PLAYER1_DOWN 's'
#define PLAYER1_LEFT 'a'
#define PLAYER1_RIGHT 'd'

#define PLAYER2_UP 'i'
#define PLAYER2_DOWN 'k'
#define PLAYER2_LEFT 'j'
#define PLAYER2_RIGHT 'l'


int gameover;
int foodX, foodY;

struct Position {
    int i;
    int j;
};

struct Player {
    int actualX;
    int actualY;
    int directionToGo;
    int alive;
    char symbol;
    Color playerColor;
    int length;
    struct Position vecPos[MAXDIM];
};

void generateFood(char game[HEIGHT][WIDTH], int *foodX, int *foodY) {
    do {
        *foodX = rand() % (WIDTH - 2) + 1;
        *foodY = rand() % (HEIGHT - 2) + 1;
    } while (game[*foodY][*foodX] != ' ');

    game[*foodY][*foodX] = '*';
}

void clearCell(char game[HEIGHT][WIDTH], int i, int j) {
    game[i][j] = ' ';
}

void drawBoard(char game[HEIGHT][WIDTH], struct Player *player) {
    Color currentColor;
    int i, j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (game[i][j] == ' ') {
                currentColor = WHITE;
            } else if (game[i][j] == player->symbol) {
                currentColor = player->playerColor;
            } else if (game[i][j] == '*') {
                currentColor = RED;
            }
            fill_rect(j * PIXELWIDTH, i * PIXELHEIGHT, PIXELWIDTH - 1, PIXELHEIGHT - 1, currentColor);
        }
    }
}

void startGame(char game[HEIGHT][WIDTH], struct Player *player) {
    player->actualX = WIDTH / 2;
    player->actualY = HEIGHT / 2;
    player->directionToGo = PLAYER1_RIGHT;
    player->alive = 1;
    player->symbol = '#';
    player->playerColor = ORANGE;
    player->length = 2;

    game[player->actualY][player->actualX] = player->symbol;

    int i, j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            game[i][j] = ' ';
        }
    }

    generateFood(game, &foodX, &foodY);

}

void input(struct Player *player,char s1, char s2, char s3, char s4) {
    char ch;
    ch = getChar();

    if (ch == s1 && player->directionToGo != s2) {
        player->directionToGo = s1;
    } else if (ch == s2 && player->directionToGo != s1) {
        player->directionToGo = s2;
    } else if (ch == s3 && player->directionToGo != s4) {
        player->directionToGo = s3;
    } else if (ch == s4 && player->directionToGo != s3) {
        player->directionToGo = s4;
    }
}

void updateSnakeLength(char game[HEIGHT][WIDTH], struct Player *player) {
    if (player->length >= 1) {
        clearCell(game, player->vecPos[player->length - 1].i, player->vecPos[player->length - 1].j);
        for (int i = player->length - 1; i > 0; i--) {
            player->vecPos[i] = player->vecPos[i - 1];
        }
    }
}



void inLogic(char game[HEIGHT][WIDTH], struct Player * player, char s1, char s2, char s3, char s4){
    // int prevX = player->actualX;
    // int prevY = player->actualY;
    //up down left right
    if (player->directionToGo == s1) {
        player->actualY--;
    } else if (player->directionToGo == s2) {
        player->actualY++;
    } else if (player->directionToGo == s3) {
        player->actualX--;
    } else if (player->directionToGo == s4) {
        player->actualX++;
    }


    // Verificar si la serpiente se come a sí misma
    for (int i = 0; i < player->length; i++) {
        if (player->actualX == player->vecPos[i].j && player->actualY == player->vecPos[i].i) {
            player->alive = 0;
            break;
        }
    }
    

    //verifica si la serpiente choco con el borde de la pantalla
    if (player->actualX < 0 || player->actualX >= WIDTH  || player->actualY < 0 || player->actualY >= HEIGHT ) {
        player->alive = 0;
    }
    

    if (!player->alive) {
        gameover = 1;
        player->playerColor = BLACK;
    }

    if (player->actualX == foodX && player->actualY == foodY) {
        player->length++;
        generateFood(game, &foodX, &foodY);
        sys_playSound(1500);
        sys_mute();
    }

    if (player->alive) {
        game[player->actualY][player->actualX] = player->symbol;
        player->vecPos[0].i = player->actualY;
        player->vecPos[0].j = player->actualX;
    }
}

void logic(char game[HEIGHT][WIDTH], struct Player *player, char s1, char s2, char s3, char s4) {
    inLogic(game,player,s1,s2,s3,s4);

    drawBoard(game, player);
}

void snakeGame() {
    char game[HEIGHT][WIDTH];
    struct Player player;
    startGame(game, &player);
    gameover = 0;

    while (!gameover) {
        input(&player,PLAYER1_UP,PLAYER1_DOWN,PLAYER1_LEFT,PLAYER1_RIGHT);
        updateSnakeLength(game, &player);
        logic(game, &player,PLAYER1_UP,PLAYER1_DOWN,PLAYER1_LEFT,PLAYER1_RIGHT);
        wait(100);
    }
    fill_rect(0, 0, get_scrWidht() / 2, get_scrHeight() / 8, BLACK);
    prints("\nGame Over. Press space to exit\n", MAX_BUFFER);
    while (getChar() != ' ') {
        continue;
    }

    clear_scr();
}



/////////////////////////////////////////////////////// MODO 2 JUGADORES //////////////////////////////////////////////////////


struct Player player1;
struct Player player2;

void startGame2Players(char game[HEIGHT][WIDTH], struct Player *player1, struct Player *player2) {
    player1->actualX = WIDTH / 4;
    player1->actualY = HEIGHT / 2;
    player1->directionToGo = PLAYER1_RIGHT;
    player1->alive = 1;
    player1->symbol = '#';
    player1->playerColor = ORANGE;
    player1->length = 2;

    player2->actualX = 3 * WIDTH / 4;
    player2->actualY = HEIGHT / 2;
    player2->directionToGo = PLAYER2_RIGHT;
    player2->alive = 1;
    player2->symbol = '@';
    player2->playerColor = PURPLE;
    player2->length = 2;

    game[player1->actualY][player1->actualX] = player1->symbol;
    game[player2->actualY][player2->actualX] = player2->symbol;

    // Inicializa el tablero y la comida, asegurándote de que las serpientes no se superpongan con la comida inicialmente
    int i, j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            game[i][j] = ' ';
        }
    }

    // game[player1->actualY][player1->actualX] = player1->symbol;
    // game[player2->actualY][player2->actualX] = player2->symbol;

    generateFood(game, &foodX, &foodY);
}

void logic2(char game[HEIGHT][WIDTH], struct Player *player,char s1,char s2,char s3,char s4) {
    inLogic(game,player,s1,s2,s3,s4);

    //drawBoard(game, player);
}



void drawBoard2(char game[HEIGHT][WIDTH], struct Player *player1, struct Player *player2) {
    Color currentColor;
    int i, j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (game[i][j] == ' ') {
                currentColor = WHITE;
            } else if (game[i][j] == player1->symbol) {
                currentColor = player1->playerColor;
            } else if (game[i][j] == player2->symbol) {
                currentColor = player2->playerColor;
            } else if (game[i][j] == '*') {
                currentColor = RED;
            }
            fill_rect(j * PIXELWIDTH, i * PIXELHEIGHT, PIXELWIDTH - 1, PIXELHEIGHT - 1, currentColor);
        }
    }
}




// void input2Players(struct Player *player1, struct Player *player2) {
//     char ch1, ch2;
//     ch1 = getChar();
//     ch2 = getChar();

//     // Actualiza las direcciones de ambos jugadores
//     if (ch1 == PLAYER1_UP && player1->directionToGo != DOWN) {
//         player1->directionToGo = UP;
//     } else if (ch1 == PLAYER1_DOWN && player1->directionToGo != UP) {
//         player1->directionToGo = DOWN;
//     } else if (ch1 == PLAYER1_LEFT && player1->directionToGo != RIGHT) {
//         player1->directionToGo = LEFT;
//     } else if (ch1 == PLAYER1_RIGHT && player1->directionToGo != LEFT) {
//         player1->directionToGo = RIGHT;
//     }

//     if (ch2 == PLAYER2_UP && player2->directionToGo != DOWN) {
//         player2->directionToGo = UP;
//     } else if (ch2 == PLAYER2_DOWN && player2->directionToGo != UP) {
//         player2->directionToGo = DOWN;
//     } else if (ch2 == PLAYER2_LEFT && player2->directionToGo != RIGHT) {
//         player2->directionToGo = LEFT;
//     } else if (ch2 == PLAYER2_RIGHT && player2->directionToGo != LEFT) {
//         player2->directionToGo = RIGHT;
//     }
// }

// void logic2Players(char game[HEIGHT][WIDTH], struct Player *player1, struct Player *player2) {
//     // int prevX1 = player1->actualX;
//     // int prevY1 = player1->actualY;

//     // int prevX2 = player2->actualX;
//     // int prevY2 = player2->actualY;

//     // Actualiza la lógica de movimiento y colisión para ambos jugadores
//     // Verifica si ambos jugadores chocan con las paredes o consigo mismos
//     // Verifica si ambos jugadores comen la comida y aumenta sus longitudes

//     if (player1->directionToGo == UP) {
//         player1->actualY--;
//     } else if (player1->directionToGo == DOWN) {
//         player1->actualY++;
//     } else if (player1->directionToGo == LEFT) {
//         player1->actualX--;
//     } else if (player1->directionToGo == RIGHT) {
//         player1->actualX++;
//     }

//     if (player2->directionToGo == UP) {
//         player2->actualY--;
//     } else if (player2->directionToGo == DOWN) {
//         player2->actualY++;
//     } else if (player2->directionToGo == LEFT) {
//         player2->actualX--;
//     } else if (player2->directionToGo == RIGHT) {
//         player2->actualX++;
//     }

//     // Verifica las colisiones con las paredes y consigo mismos para ambos jugadores
//     if (player1->actualX < 0 || player1->actualX >= WIDTH || player1->actualY < 0 || player1->actualY >= HEIGHT) {
//         player1->alive = 0;
//     }
    
//     if (player2->actualX < 0 || player2->actualX >= WIDTH || player2->actualY < 0 || player2->actualY >= HEIGHT) {
//         player2->alive = 0;
//     }

//     // Verifica si ambos jugadores chocan consigo mismos
//     for (int i = 0; i < player1->length; i++) {
//         if (player1->actualX == player1->vecPos[i].j && player1->actualY == player1->vecPos[i].i) {
//             player1->alive = 0;
//             break;
//         }
//     }
    
//     for (int i = 0; i < player2->length; i++) {
//         if (player2->actualX == player2->vecPos[i].j && player2->actualY == player2->vecPos[i].i) {
//             player2->alive = 0;
//             break;
//         }
//     }

//     // Verifica si ambos jugadores comen la comida y aumenta sus longitudes
//     if (player1->actualX == foodX && player1->actualY == foodY) {
//         player1->length++;
//         generateFood(game, &foodX, &foodY);
//     }
    
//     if (player2->actualX == foodX && player2->actualY == foodY) {
//         player2->length++;
//         generateFood(game, &foodX, &foodY);
//     }

//     if (!player1->alive && !player2->alive) {
//         gameover = 1;
//     }

//     // Dibuja el tablero para ambos jugadores
//     drawBoard2(game, player1, player2);
// }

void snakeGame2Players() {
    char game[HEIGHT][WIDTH];
    startGame2Players(game, &player1, &player2);
    gameover = 0;

    while (!gameover) {
        //input2Players(&player1, &player2);
        // input(&player1,UP,DOWN,LEFT,RIGHT);
        // updateSnakeLength(game, &player1);
        // input(&player2,PLAYER2_UP,PLAYER2_DOWN,PLAYER2_LEFT,PLAYER2_RIGHT);
        // updateSnakeLength(game, &player2);
        // //logic2Players(game, &player1, &player2);
        // logic2(game,&player1,UP,DOWN,LEFT,RIGHT);
        // logic2(game,&player2,PLAYER2_UP,PLAYER2_DOWN,PLAYER2_LEFT,PLAYER2_RIGHT);
        // drawBoard2(game,&player1,&player2);
        // wait(100);

        input(&player1, PLAYER1_UP, PLAYER1_DOWN, PLAYER1_LEFT, PLAYER1_RIGHT);
        updateSnakeLength(game, &player1);
        logic2(game, &player1, PLAYER1_UP, PLAYER1_DOWN, PLAYER1_LEFT, PLAYER1_RIGHT);

        input(&player2, PLAYER2_UP, PLAYER2_DOWN, PLAYER2_LEFT, PLAYER2_RIGHT);
        updateSnakeLength(game, &player2);
        logic2(game, &player2, PLAYER2_UP, PLAYER2_DOWN, PLAYER2_LEFT, PLAYER2_RIGHT);

        drawBoard2(game, &player1, &player2);
        wait(100);


    }
    fill_rect(0, 0, get_scrWidht() / 2, get_scrHeight() / 8, BLACK);
    prints("\nGame Over. Press space to exit\n", MAX_BUFFER);
    while (getChar() != ' ') {
        continue;
    }

    clear_scr();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int startSnake(int option) {
    clear_scr();

    if (option == 1) {
        prints("\nModo 1 jugador\n", MAX_BUFFER);
        snakeGame();
    } else if (option == 2) {
        prints("\nModo 2 jugadores\n", MAX_BUFFER);
        snakeGame2Players();
    } else {
        return 0;
    }

    return 1;
}