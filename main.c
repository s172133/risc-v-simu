
#include <stdio.h>
#include <stdlib.h>
#ifdef linux
#include <stdint-gcc.h>
#elif _WIN32
#include <stdint.h>
#else

#endif

int main(){

    printf("********** Starting **********\n");

    uint32_t pc = 0; // Program counter
    uint32_t reg[32]; // Register

    uint32_t
    memory = malloc( 42 * sizeof(uint32_t) ); // Allocate binary code

    // OPEN FILE
    FILE *fp = fopen("output.bin", "r");
    if(fp == NULL){
        perror("Unable to open file!");
        exit(1);
    }


    int opcode;
    char str[40];
     while(pc != 10) {
         pc += 1;

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    printf("size of program is %d\n", size);


    uint32_t *memory = malloc( 10000 * sizeof(uint32_t) ); // Allocate binary code

    uint32_t word = getw(fp);

    int i,j = 0;
    printf("test\n");
    //printf("test");
    while(word != EOF){
        if(j < 1264) i=0;
        memory[i] = word;
        i+=4;
        j+=4;
        word = getw(fp);
    }

    printf("%u\n", memory[0]);
    printf("%u\n", memory[4]);

        /* switch (opcode) {

             case 0x13:
                 reg[rd] = reg[rs1] + imm;
                 break;
             default:
                // cout << "Opcode " << opcode << " not yet implemented" << endl;
                 break;
         }*/
     }
    fclose(fp);
    return 0;
}