#include <stdio.h>
#include <stdlib.h>
#include <stdint-gcc.h>

int main(){
    printf("********** Starting **********\n");

    uint32_t pc;
    uint32_t reg[32];

    FILE *fp = fopen("binary.txt", "r");

    if(fp == NULL){
        perror("Unable to open file!");
        exit(1);
    }

    char str[40];
 
     while(fgets(str, sizeof(str), fp) != NULL) {
         fputs(str, stdout);
         
     }
    fclose(fp);
    return 0;
}