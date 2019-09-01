#include <stdio.h>
#include <fmodex/fmod.h>

int main() 
{
  char ver[100];
  sprintf(ver,"%x",FMOD_VERSION);
  
  char maj[10],min[10],dev[10];
  maj[0] = ver[0];
  maj[1] = '\0';

  min[0] = ver[1];
  min[1] = ver[2];
  min[2] = '\0';
  
  dev[0] = ver[3];
  dev[1] = ver[4];
  dev[2] = '\0';
  
  printf("%s.%s.%s",maj,min,dev);
  
}