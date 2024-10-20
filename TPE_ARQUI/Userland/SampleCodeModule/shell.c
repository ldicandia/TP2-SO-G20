#include "userLibrary.h"
#include "shell.h"
#include <stdlib.h>
#define MAX_BUFFER 254

static void printLine(char c);
int linePos = 0;
char lastc;
char line[MAX_BUFFER+1] = {0};

void shell(){
    printStr("Bienvenido al shell\n");
    printStr("Ingrese un caracter\n");
    printStr(">");
    char c;

    while(1){
		  c = getChar();
      printLine(c);
	  }
    
}

static void printLine(char c){
	if (linePos < MAX_BUFFER && c != lastc){
		if (isChar(c) || c == ' ' ||isDigit(c)){
			line[linePos++] = c;
			printChar(c);
		} else if (c == '\b' && linePos > 0){
			printChar(c);
			line[--linePos] = 0;
		} else if (c == '\n'){
			//newLine();
		}
	}
	lastc = c;
}

