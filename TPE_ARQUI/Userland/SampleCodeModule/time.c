#include <sys_calls.h>
#include <userLibrary.h>
#include <user_time.h>
#include <stdlib.h>
#include <stdio.h>

int getHours()
{
  return u_sys_getHours(); // Assuming this returns an integer
}

int getMinutes()
{
  return u_sys_getMinutes(); // Assuming this returns an integer
}

int getSeconds()
{
  return u_sys_getSeconds(); // Assuming this returns an integer
}

void user_time()
{
  int hours, minutes, seconds;

  hours = getHours();
  minutes = getMinutes();
  seconds = getSeconds();

  printChar('\n');

  // Printing hours
  if (hours < 10)
    printChar('0');
  printChar(hours + '0');

  printChar(':');

  // Printing minutes
  if (minutes < 10)
    printChar('0');
  printChar(minutes + '0');

  printChar(':');

  // Printing seconds
  if (seconds < 10)
    printChar('0');
  printChar(seconds + '0');
}