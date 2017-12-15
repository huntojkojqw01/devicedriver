#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "rsa.h"
//=====COLOR==============
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
//========================
#define BUFF_SIZE 1000
#define ENCODER_MAJOR 50      
#define DECODER_MAJOR 60
#define POSITION     405
//========================

int main(int argc,char* argv[]) {
  FILE *f;
  rsa_params _rsa;
  init(&_rsa);

  printf("Current config:\n");
  printf("  RSA ATTRIBUTES: "RED"n=%d, e=%d, d=%d, u=%d"RESET"\n",_rsa.n, _rsa.e, _rsa.d, _rsa.u);
  printf("  BUFFER SIZE   : "RED"%d"RESET"\n",BUFF_SIZE);
  printf("  DEVICE MAJOR  : "RED"encoder=%d, decoder=%d"RESET"\n",ENCODER_MAJOR, DECODER_MAJOR);  
  
  f=fopen("encoder.c","r+");
  if(!f) {puts("open error");exit(0);}
  fseek( f, POSITION, SEEK_SET );
  fprintf(f, "#define BUFF_SIZE %-10d\n", BUFF_SIZE);
  fprintf(f, "#define ENCODER_MAJOR %-5d\n", ENCODER_MAJOR);
  fprintf(f, "#define N %-10d\n#define E %-10d\n#define D %-10d\n#define U %-10d\n",\
              _rsa.n, _rsa.e, _rsa.d, _rsa.u );
  fclose(f);

  f=fopen("decoder.c","r+");
  if(!f) {puts("open error");exit(0);}
  fseek( f, POSITION, SEEK_SET );
  fprintf(f, "#define BUFF_SIZE %-10d\n", BUFF_SIZE);
  fprintf(f, "#define DECODER_MAJOR %-5d\n", DECODER_MAJOR);
  fprintf(f, "#define N %-10d\n#define E %-10d\n#define D %-10d\n#define U %-10d\n",\
              _rsa.n, _rsa.e, _rsa.d, _rsa.u );
  fclose(f);
  
  return 0;
}