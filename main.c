
#include <stdio.h>
#include <stdlib.h>

#ifdef linux

#include <stdint-gcc.h>

#elif _WIN32
#include <stdint.h>
#endif

void readFile(uint32_t* memory);


int main() {

    printf("********** Starting **********\n");

    uint32_t pc = 0; // Program counter
    uint32_t reg[32]; // Register
    uint32_t *memory = malloc(10000 * sizeof(uint8_t)); // Allocate binary code

    readFile(memory);

    for (int i = 0; i < 10; i++) {
        printf("%x\n", memory[i*4]);
    }

    return 0;
}

void readFile(uint32_t* memory){
    // OPEN FILE
    FILE *fp = fopen("hello.bin", "rb");
    if (fp == NULL) {
        perror("Unable to open file!");
        exit(1);
    }



    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    //printf("size of program is %d\n", size);

    uint32_t word = getw(fp);

    int i = 0;
    while (word != EOF) {
        //printf("%x\n", word);
        memory[i] = word;
        i += 4;
        word = getw(fp);
    }

    fclose(fp);

}