#include <sys_calls.h>
#include <userLibrary.h>
#include <user_time.h>
#include <stdlib.h>
#include <stdio.h>



int getHours()
{
  return u_sys_getHours()-3; // returns u_int64_t, -3 because of the timezone argentina
}

int getMinutes()
{
  return u_sys_getMinutes(); // returns u_int64_t
}

int getSeconds()
{
  return u_sys_getSeconds(); // returns u_int64_t
}


static void print_time(int time)
{
  if (time < 10){
    printChar('0');
    printChar(time + '0');
  }
  else{
    printChar(time/10 + '0');
    printChar(time%10 + '0');
  }
}


void user_time()
{
  int hours, minutes, seconds;

  hours = getHours();
  minutes = getMinutes();
  seconds = getSeconds();

  printChar('\n');

  // Printing hours
  print_time(hours);
  printChar(':');
  // Printing minutes
  print_time(minutes);
  printChar(':');
  // Printing seconds
  print_time(seconds);


}



