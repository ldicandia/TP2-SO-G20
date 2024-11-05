#include <stdio.h>
#include <stdlib.h>
#include <sys_calls.h>
#include <time.h>
#include <userLibrary.h>

int getHours() {
  return (u_sys_getHours() + 21) % 24;  // transform to -3 GMT
}

int getMinutes() {
  return u_sys_getMinutes(); 
}

int getSeconds() {
  return u_sys_getSeconds(); 
}

void sleep(int ms) { u_sys_sleep(ms); }

static void print_time(int time) {
  if (time < 10) {
    printChar('0');
    printChar(time + '0');
  } else {
    printChar(time / 10 + '0');
    printChar(time % 10 + '0');
  }
}

void user_time() {
  int hours, minutes, seconds;

  hours = getHours();
  minutes = getMinutes();
  seconds = getSeconds();

  printChar('\n');

  print_time(hours);
  printChar(':');

  print_time(minutes);
  printChar(':');

  print_time(seconds);
}
