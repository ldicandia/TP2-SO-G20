
//#include <usr_stdlib.h>
//#include "userLibrary.h"
#include "shell.h"
#include <stdlib.h>
#define MAX_BUFFER 254
#define MAX_COMMANDS 10

static void printLine(char c);
int linePos = 0;
char lastc;
int lastEnter = 0;
char line[MAX_BUFFER+1] = {0};

char command[MAX_BUFFER+1] = {0};
char parameter[MAX_BUFFER+1] = {0};

const char *commands[MAX_COMMANDS] = {"undefined", "help", "time", "clear", "snake", "inforeg", "zerodiv", "invopcode", "sizeplus", "sizeminus"};



void shell(){
    printStr("Bienvenido al shell\n");
    printStr("Ingrese un caracter\n");
    printStr(">");
    char c;

    while(1){
		c = getChar();
		if(lastc != c){
			printLine(c);
		}
	}
    
}

static void printLine(char c){
	if(c == '\b'){
		if(lastEnter == 0){
			//solo actualiza sin borrar nada
			lastc = c;
			return;
		}else{
			//caso en que hubieron caracteres, lo descuenta
			lastEnter--;
		}
	}
	if(c != '\n'){
		//todos los caracteres suman uno a la distancia al ultimo enter
		lastEnter++;
	}else{
		//en enter vuelve a empezar el contador
		lastEnter = 0;
	}
	//salvo el caso de querer borrar despues de un enter, todos los caracteres se imprimen
	printChar(c);
	lastc = c;
}


//make the newline function


void commandMenu(){
	printStr("\n--------------------| MANUAL |--------------------");
	printStr("\n time 						shows actual time");
	printStr("\n clear 						clears the screen");
	printStr("\n snake [cant jugadores]		starts de Snake game");
	printStr("\n inforeg 					prints the regsiter's values");
	printStr("\n zerodiv					division by cero exception");
	printStr("\n invopcode					tests the invalid operation code");
	printStr("\n sizeplus					increase letter size");
	printStr("\n sizeminus					decrease letter size");
	printStr("\n---------------------------------------------------");
	printChar('\n');
}

/*


// funcion para leer comando de parametro
// static void readCommand(){
// 	int i;
// 	int j=0;
// 	int k = 0;
	
// 	for(j=0; j < linePos && line[j] != ' '; j++){
// 		command[j] = line[j];
// 	}

// 	if(j < linePos){
// 		j++;
// 		while(j < linePos){
// 			parameter[k++] = line[j++];
// 		}
// 	}

// 	for(i = 1; i < MAX_COMMANDS; i++){
// 		if(strcmp( command, commands[i]) == 0){
// 			void (*commands[i])();
// 		}
// 		notFound();

// 	}

// }


// static void notFound(){
// 	printStr("\n command not found. Try using 'help'\n");
// 	return;
// }
	

// static void help(){
// 	printStr("\n---HELP---\n");
// 	commandMenu();

// }

// static void time(){
// 	display_time();
// 
//}

static void clear(){
	clear_screen();
}//
static void cmd_inforeg(){
	inforeg();
}



*/



