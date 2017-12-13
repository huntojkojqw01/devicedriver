#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "rsa.h"

int main(int argc,char* argv[]) { 

  unsigned char dummy;
  char *write_str,*read_str;

  FILE * MEMORY;
  rsa_params _rsa;
  init(&_rsa);

  char src[1000],des[1000]; 
  if(argc>=2){    
    strcpy(src,argv[1]);
    ma_hoa2(_rsa,src,des);
    puts(src);puts(des);
    strcpy(src,des);
    giai_ma2(_rsa,src,des);
    puts(src);puts(des);
  }
  if(argc<2){
    puts("Argument not found.");
    return -1;
  }  

  /* Opening the device memory */

  MEMORY=fopen("/dev/memory","w+");
  if(!MEMORY){
    puts("IO ERROR!!! (Device not found).");
    return -1;
  }

  /* We remove the buffer from the file i/o */
  setvbuf(MEMORY,&dummy,_IONBF,BUFF_SIZE);  

  write_str=(char*)malloc(sizeof(char)*BUFF_SIZE);
  memset(write_str,0,BUFF_SIZE);
  strcpy(write_str,argv[1]);

  printf("Write string is %s\n",write_str);
  fwrite(write_str,1,strlen(write_str),MEMORY);

  sleep(1);

  read_str=(char*)malloc(sizeof(char)*BUFF_SIZE);
  memset(read_str,0,BUFF_SIZE);

  fread(read_str,sizeof(char),BUFF_SIZE,MEMORY);
  printf("Read string is %s\n",read_str);

  fclose(MEMORY);
  return 0;
}