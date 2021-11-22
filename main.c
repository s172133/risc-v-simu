
#include <stdio.h>
#include <stdlib.h>

#ifdef linux

#include <stdint-gcc.h>

#elif _WIN32
#include <stdint.h>
#endif

void readFile(uint32_t *memory);

void load(uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t *memory, uint32_t funct3, uint32_t *reg);

void store(uint32_t funct3, uint32_t funct7, uint32_t imm, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *memory,
           uint32_t *reg);

void intergerOp(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *reg);

void immediate(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *reg);

int ecall(uint32_t print, int printCounter, int printOffset, uint32_t *memory, uint32_t *reg);

int main() {

    printf("********** Starting **********\n");

    uint32_t pc = 0; // Program counter
    uint32_t reg[32] = {0}; // Register
    uint32_t *memory = malloc(0xFFFFFFFF * sizeof(uint32_t)); // Allocate binary code

    readFile(memory);

    while (1) {
        pc += 4;
        uint32_t instr = memory[pc];
        uint32_t opcode = instr & 0x7f;
        uint32_t funct3 = (instr >> 12) & 0x3;
        uint32_t funct7 = (instr >> 25) & 0x7F;
        uint32_t rd = (instr >> 7) & 0x01f;
        uint32_t rs1 = (instr >> 15) & 0x01f;
        uint32_t rs2 = (instr >> 20) & 0x01f;
        uint32_t imm12 = (instr >> 20);
        uint32_t imm20 = (instr >> 12);
        uint32_t part1 = (instr >> 12) & 0xFF;
        uint32_t part2 = (instr >> 20) & 0x3FF;
        uint32_t part3 = (instr >> 31);
        uint32_t imm;
        uint32_t jimm = part3 << 18 | part1 << 10 | part2; //This might be wrong!!
        uint32_t print;
        int printCounter = 0;
        int printOffset = 0;
        int ecallVal = 0;
        reg[0] = 0;
        //printf("\nOpcode: %x\n", opcode);


        switch (opcode) {
            case 0x3: //load
                load(imm12, rd, rs1, memory, funct3, reg);
                break;
            case 0x13: //Immediate
                immediate(funct3, imm12, rd, rs1, rs2, reg);
                break;
            case 0x17: //auipc
                if ((imm20 >> 19) == 1) {
                    imm20 += 0xFFF00000;
                }
                //printf("rd: %x\n", rd);
                reg[rd] = pc + imm20;
                pc = pc + imm20;
                //printf("Stored value: %x\n", reg[rd]);
                break;
            case 0x23: //store
                store(funct3, funct7, imm, rd, rs1, rs2, memory, reg);
                break;
            case 0x33: //interger operations
                intergerOp(funct3, imm12, rd, rs1, rs2, reg);
                break;
            case 0x37: //LUI
                reg[rd] = imm20 << 12; //Think this is right
                break;
            case 0x63: //Branch
                //todo
                break;
            case 0x67: //jalr
                reg[rd] = reg[rs1] - 4 + imm12;
                pc = reg[rd];
                break;
            case 0x6F: //jal
                reg[rd] = pc + 4;
                pc += jimm - 4;
                break;
            case 0x73: //ecall
                ecallVal = ecall(print, printCounter, printOffset, memory, reg);
                if (ecallVal == 10) return 0;
                else if (ecallVal == 17) return reg[10];
                break;

            default:
                printf("Opcode %x not yet implemented\n", opcode);
        }
        /*
        printf("pc: %x, \tx1: %x,\tx2: %x,\tx3: %x,\tx4: %x,\tx5: %x, \tx6: %x,\tx7: %x,\n",
            pc, reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);
        printf("x8: %x, \tx9: %x,\tx10: %x,\tx11: %x,\tx12: %x,\tx13: %x, \tx14: %x,\tx15: %x,\n",
            reg[8], reg[9], reg[10], reg[11], reg[12], reg[13], reg[14], reg[15]);
        printf("x16: %x, \tx17: %x,\tx18: %x,\tx19: %x,\tx20: %x,\tx21: %x, \tx22 %x,\tx23: %x,\n",
            reg[16], reg[17], reg[18], reg[19], reg[20], reg[21], reg[22], reg[23]);
        printf("x24: %x, \tx25: %x,\tx26: %x,\tx27: %x,\tx28: %x,\tx29: %x, \tx30: %x,\tx31: %x,\n",
            reg[24], reg[25], reg[26], reg[27], reg[28], reg[29], reg[30], reg[31]);
        */
    }
}

void readFile(uint32_t *memory) {
    // OPEN FILE
    FILE *fp = fopen("hello.bin", "rb");
    if (fp == NULL) {
        perror("Unable to open file!");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

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

void load(uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t *memory, uint32_t funct3, uint32_t *reg) {
    switch (funct3) {
        case 0x0: //LB Untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = memory[reg[rs1] + imm12] & 0x000000FF;
            if ((reg[rd] >> 7) == 1) {
                reg[rd] += 0xFFFFFF00;
            }
            break;
        case 0x1: //LH Untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = memory[reg[rs1] + imm12] & 0x0000FFFF;
            if ((reg[rd] >> 7) == 1) {
                reg[rd] += 0xFFFF0000;
            }
            break;

        case 0x2: //LW
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = memory[reg[rs1] + imm12];
            break;
        case 0x4: //LBU Untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = memory[reg[rs1] + imm12] & 0x000000FF;
            break;
        case 0x5: //LHU Untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = memory[reg[rs1] + imm12] & 0x0000FFFF;
            break;
        default:
            printf("Invalid funct3");
    }
}

void store(uint32_t funct3, uint32_t funct7, uint32_t imm, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *memory,
           uint32_t *reg) {
    switch (funct3) {
        case 0x0: //SB Untested
            if (funct7 >> 6 == 1) {
                funct7 += 0xFFFFFF80;
            }
            imm = (funct7 << 5) + rd;

            memory[reg[rs1] + imm] = (reg[rs2] & 0x000000FF);
            break;
        case 0x1: //SH Untested
            if (funct7 >> 6 == 1) {
                funct7 += 0xFFFFFF80;
            }
            imm = (funct7 << 5) + rd;

            memory[reg[rs1] + imm] = (reg[rs2] & 0x0000FFFF);
            break;
        case 0x2: //SW
            if (funct7 >> 6 == 1) {
                funct7 += 0xFFFFFF80;
            }

            imm = (funct7 << 5) + rd;

            memory[reg[rs1] + imm] = reg[rs2];
            //printf("Value saved: %x, at %x\n", memory[reg[rs1] + imm], reg[rs1] + imm);
            break;
        default:
            printf("Invalid funct3");
    }
}

void immediate(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *reg) {
    switch (funct3) {
        case 0: //addi
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] + imm12;
            break;
        case 2: //SLTI
            if (reg[rs1] >> 4 == 0 && imm12 >> 11 == 0) { // (rs = 5 bit) (imm12 = 12 bit)
                if (reg[rs1] < imm12) {
                    reg[rd] = 1;
                } else {
                    reg[rd] = 0;
                }
            }

            break;
        case 3: //SLTIU
            if (reg[rs1] >> 4 == 1 && imm12 >> 11 == 1) // (rs = 5 bit) (imm12 = 12 bit)
                if (reg[rs1] < imm12) {
                    reg[rd] = 1;
                } else {
                    reg[rd] = 0;
                }
            break;
        case 4: //XORI
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] ^ imm12;
            break;
        case 6: //ORI
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] | imm12;
            break;
        case 7: //ANDI
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] & imm12;
            break;
        default:
            printf("Invalid funct3");
    }
}

void intergerOp(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *reg) {
    switch (funct3) {
        case 0: //add/sub
            if (imm12 >> 10 & 0x1) { //sub
                reg[rd] = reg[rs1] - reg[rs2];
            } else { //add
                reg[rd] = reg[rs1] + reg[rs2];
            }
            break;
        case 1: //SLL UNtested
            reg[rd] = reg[rs1] << (reg[rs2] & 0x1F);
            break;
        case 2: //SLT Untested

            if ((int) reg[rs1] < (int) reg[rs2]) {
                reg[rd] = 1;
            } else {
                reg[rd] = 0;
            }
            break;

        case 3: //SLTU Untested
            if (reg[rs1] < reg[rs2]) {
                reg[rd] = 1;
            } else {
                reg[rd] = 0;
            }
            break;
        case 4: //XOR
            reg[rd] = reg[rs1] ^ reg[rs2]; //Not tested
            break;
        case 5: //SRL/SRA
            if (imm12 >> 10 & 0x1) { //SRA   Arithmetic right shift
                reg[rd] = (reg[rs1] >> reg[rs2]) & reg[rs1]; // Might not be right
            } else { //SRL      Logical right shift
                reg[rd] = reg[rs1] >> reg[rs2];
            }
            break;
        case 6: //OR
            reg[rd] = reg[rs1] | reg[rs2]; //Not tested
            break;
        case 7: //AND
            reg[rd] = reg[rs1] & reg[rs2]; //Not tested
            break;
        default:
            printf("Invalid funct3");
    }
}

int ecall(uint32_t print, int printCounter, int printOffset, uint32_t *memory, uint32_t *reg) {
    switch (reg[17]) {
        case 1: //printInt
            printf("%d\n", memory[reg[10]]); //This doesn't work
            break;
        case 4: //printString
            print = memory[reg[10]];
            while (print & 0xFF) {
                printf("%c", print);
                print = print >> 8;
                printCounter++;
                if (printCounter > 3) {
                    printOffset++;
                    printCounter = 0;
                    print = memory[reg[10] + printOffset * 4];
                }
            }
            printf("\n");
            break;
        case 10: //exits program
            break;
        case 11: //printChar
            printf("%c", reg[10]);
            break;
        case 17: //Exit with exit code
            break;
        default:
            break;
    }
    return reg[17];
}
