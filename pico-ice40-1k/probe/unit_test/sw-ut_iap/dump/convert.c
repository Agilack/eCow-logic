#include <stdio.h>
#include "convert.h"

int main(void)
{
  int i;
  for (i = 0; i < 2048; i++)
  {
    write(1, &dump[i], 1);
  }
  return(0);
}
