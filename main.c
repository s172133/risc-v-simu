
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(){
    printf("********** Starting **********\n");

    uint32_t pc = 0; // Program counter
    uint32_t reg[32]; // Register

    malloc( 10000 * sizeof(uint32_t) ); // Allocate binary code

    // OPEN FILE
    FILE *fp = fopen("binary.txt", "r");
    if(fp == NULL){
        perror("Unable to open file!");
        exit(1);
    }


    uint32_t opcode;
    uint32_t rd;
    uint32_t rs1;
    uint32_t imm;

    char str[40];
     while(pc != 10) {
         pc += 1;


         switch (opcode) {

             case 0x13:
                 reg[rd] = reg[rs1] + imm;
                 break;
             default:
                 cout << "Opcode " << opcode << " not yet implemented" << endl;
                 break;
         }
     }
    fclose(fp);
    return 0;
}