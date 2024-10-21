#include "sys_calls.h"
#include <stdio.h>


#define STDIN 0
#define STDOUT 1
#define STDERR 2

const Color BLACK = {0, 0, 0};
const Color WHITE = {255, 255, 255};
const Color RED = {0, 0, 255};
const Color LIGHT_BLUE = {173, 216, 230};
const Color BLUE = {255, 0, 0};
const Color ORANGE = {16, 160, 255};
const Color YELLOW = {30, 224, 255};
const Color PURPLE = {255, 32, 160};
const Color PINK = {100, 0, 244};
const Color GREEN = {0, 255, 0};
const Color LIGHT_RED = {0, 255, 255};
const Color LIGHT_PURPLE = {255, 0, 255};
const Color LIGHT_ORANGE = {0, 160, 255};
const Color LIGHT_YELLOW = {0, 224, 255};
const Color LIGHT_PINK = {0, 100, 244};
const Color LIGHT_GREEN = {0, 255, 0};

static char buffer[64] = { '0' };


void printChar(char c){
	u_sys_write(STDOUT, c);
}

char getChar(){
	char c;
	u_sys_read(STDIN, &c);
	return c;
}

void clear(){
	u_sys_clear();
}

void increment_size_char(){
	u_sys_increment_size();
}

void decrement_size_char(){
	u_sys_decrement_size();
}

void printCharColor(char c, Color fnt){
	u_sys_write_color(STDOUT, c, fnt);
}

void printStr(char * str){
	for(int i=0; str[i]; i++){
		u_sys_write(STDOUT, str[i]);
	}
}

int isChar(char c){
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		return 1;
	}
	return 0;
}

int isDigit(char c){
		if (c >= '0' && c <= '9'){
		return 1;
	}
	return 0;
}

void exc_invopcode(){
	u_exc_invopcode();
}

void exc_zerodiv(){
	u_exc_zerodiv();
}