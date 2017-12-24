#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "rsa.h"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define BUFF_SIZE 1000

int main(int argc,char* argv[]) { 

  unsigned char dummy, i;
  char *write_str, *read_str, src[1000], des[1000];;

  FILE *ENCODER, *DECODER;
  rsa_params _rsa;  
  if(argc >= 2){
    if(strcmp(argv[1], "-i") == 0){
      init(&_rsa);
      printf("Init RSA attributes: "RED"n=%d, e=%d, d=%d, u=%d"RESET"\n",_rsa.n, _rsa.e, _rsa.d, _rsa.u);
      return 1;
    }
    if(strcmp(argv[1], "-t") == 0){
      init(&_rsa);
      puts("Test RSA algorithms...");
      if(argc>=3)    strcpy(src, argv[2]);
      else           strcpy(src,"default string");
      ma_hoa2(_rsa, src, des);
      printf("   Encode: "GREEN"\"%s\""RESET" ----> "RED"\"%s\""RESET" \n",src,des);
      strcpy(src,des);
      giai_ma2(_rsa, src, des);
      printf("   Decode: "GREEN"\"%s\""RESET" <---- "RED"\"%s\""RESET" \n",des,src);
      return 1;
    }  

    puts("Test encoder device and decoder device...");
    /* Opening the device memory */

    ENCODER=fopen("/dev/encoder", "w+");
    DECODER=fopen("/dev/decoder", "w+");
    if(!ENCODER || !DECODER){
      puts(RED"IO ERROR!!!"RESET" (Device not found).");
      return -1;
    }

    /* We remove the buffer from the file i/o */
    setvbuf(ENCODER, &dummy, _IONBF,BUFF_SIZE);
    setvbuf(DECODER, &dummy, _IONBF,BUFF_SIZE);

    // Prepare buffer:
    write_str=(char*)malloc(sizeof(char)*BUFF_SIZE);
    memset(write_str, 0, BUFF_SIZE);
    read_str=(char*)malloc(sizeof(char)*BUFF_SIZE);
    memset(read_str, 0, BUFF_SIZE);

    //USE ENCODER:
    strcpy(write_str,argv[1]);

    printf("   Writing "GREEN"\"%s\""RESET" to encoder device... \n",write_str);
    fwrite(write_str, 1, strlen(write_str),ENCODER);

    puts("   ..."); sleep(1);  

    fread(read_str,sizeof(char),BUFF_SIZE,ENCODER);
    printf("   Read from encoder device, get "RED"\"%s\""RESET"\n",read_str);

    for(i = 0; i<3; i++){
      puts("   ..."); sleep(1); 
    }
    // USE DECODER:
    strcpy(write_str,read_str);

    printf("   Writing "RED"\"%s\""RESET" to decoder device... \n",write_str);
    fwrite(write_str, 1, strlen(write_str),DECODER);

    puts("   ..."); sleep(1);  

    fread(read_str,sizeof(char),BUFF_SIZE,DECODER);
    printf("   Read from decoder device, get "GREEN"\"%s\""RESET"\n",read_str);

    fclose(ENCODER);
    fclose(DECODER);
    return 1;
  }
  else{
    puts("Argument not found.");
    puts("./test -i #init RSA attributes");
    puts("./test -t [string] #test RSA with input string");
    puts("./test string #test encoder device and decoder device ");
    return -1;
  }  
  return 0;
}