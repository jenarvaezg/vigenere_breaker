#include <stdio.h>
#include <stdlib.h>
#define KEY_LENGTH 7 

int main(){
  unsigned char ch;
  FILE *fpIn, *fpOut;
  int i;
  unsigned char key[KEY_LENGTH] = {0xBA, 0x1F, 0x91, 0xB2, 0x53, 0xCD, 0x3E};

  fpIn = fopen("ptext.txt", "r");
  fpOut = fopen("ctext.txt", "w");

  i=0;
  while (fscanf(fpIn, "%c", &ch) != EOF) {
    if (ch!='\n') {
      fprintf(fpOut, "%02X", ch ^ key[i % KEY_LENGTH]);    
      i++;
      }
    }
 
  fclose(fpIn);
  fclose(fpOut);
  exit(EXIT_SUCCESS);
} 