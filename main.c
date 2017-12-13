#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "rsa.h"

int main(int argc,char* argv[]) { 

  unsigned char dummy;
  char *write_str,*read_str;

  FILE *ENCODER, *DECODER;
  rsa_params _rsa;
  init(&_rsa);

  // char src[1000],des[1000]; 
  // if(argc>=2){    
  //   strcpy(src,argv[1]);
  //   ma_hoa2(_rsa,src,des);
  //   puts(src);puts(des);
  //   strcpy(src,des);
  //   giai_ma2(_rsa,src,des);
  //   puts(src);puts(des);
  // }

  if(argc<2){
    puts("Argument not found.");
    return -1;
  }  

  /* Opening the device memory */

  ENCODER=fopen("/dev/encoder","w+");
  DECODER=fopen("/dev/decoder","w+");
  if(!ENCODER || !DECODER){
    puts("IO ERROR!!! (Device not found).");
    return -1;
  }

  /* We remove the buffer from the file i/o */
  setvbuf(ENCODER,&dummy,_IONBF,BUFF_SIZE);
  setvbuf(DECODER,&dummy,_IONBF,BUFF_SIZE);

  // Prepare buffer:
  write_str=(char*)malloc(sizeof(char)*BUFF_SIZE);
  memset(write_str,0,BUFF_SIZE);
  read_str=(char*)malloc(sizeof(char)*BUFF_SIZE);
  memset(read_str,0,BUFF_SIZE);

  //USE ENCODER:
  strcpy(write_str,argv[1]);

  printf("Writing \"%s\" to encoder device... \n",write_str);
  fwrite(write_str,1,strlen(write_str),ENCODER);

  sleep(1);  

  fread(read_str,sizeof(char),BUFF_SIZE,ENCODER);
  printf("Read from encoder device, get \"%s\"\n",read_str);

  // USE DECODER:
  strcpy(write_str,read_str);

  printf("Writing \"%s\" to decoder device... \n",write_str);
  fwrite(write_str,1,strlen(write_str),DECODER);

  sleep(1);  

  fread(read_str,sizeof(char),BUFF_SIZE,DECODER);
  printf("Read from decoder device, get \"%s\"\n",read_str);

  fclose(ENCODER);
  fclose(DECODER);
  return 0;
}