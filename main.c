
#include <stdio.h>
#include <stdlib.h>

#ifdef linux

#include <stdint-gcc.h>

#elif _WIN32
#include <stdint.h>
#endif

void readFile(uint32_t *memory);

void load(uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t *memory, uint32_t funct3, uint32_t *reg);

void store(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *memory,
           uint32_t *reg);

int branch(uint32_t funct3, uint32_t offset, uint32_t rs1, uint32_t rs2, uint32_t *reg, int pc);

void intergerOp(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *reg);

void immediate(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t shamt, uint32_t *reg);

uint32_t ecall(uint32_t print, int printCounter, int printOffset, uint32_t *memory, uint32_t *reg);

int main() {

    printf("********** Starting **********\n");

    int pc = 0; // Program counter
    uint32_t reg[32] = {0}; // Register
    uint32_t *memory = malloc(0xFFFFFFFF * sizeof(uint32_t)); // Allocate binary code

    readFile(memory);

    while (1) {

        uint32_t instr = memory[pc];
        uint32_t opcode = instr & 0x7f;
        uint32_t funct3 = (instr >> 12) & 0x7;
        uint32_t funct7 = (instr >> 25) & 0x7F;
        uint32_t rd = (instr >> 7) & 0x01f;
        uint32_t rs1 = (instr >> 15) & 0x01f;
        uint32_t rs2 = (instr >> 20) & 0x01f;
        uint32_t imm12 = (instr >> 20);
        uint32_t imm20 = (instr >> 12);
        uint32_t part1 = (instr >> 21) & 0x3FF;
        uint32_t part2 = (instr >> 20) & 0x1;
        uint32_t part3 = (instr >> 12) & 0xFF;
        uint32_t part4 = (instr >> 31) & 0x1;
        uint32_t jimm = (part1 << 1) | (part2 << 11) | (part3 << 12) | (part4 << 20); //This might be wrong!!
        uint32_t imm4_1 = (rd >> 1) & 0xF;
        uint32_t imm10_5 = (funct7) & 0x3F;
        uint32_t imm11_ = rd & 0x1;
        uint32_t imm12_ = (funct7 >> 6) & 0x1;
        uint32_t offset = (imm4_1 << 1) | (imm10_5 << 5) | (imm11_ << 11) | (imm12_ << 12);
        if ((offset >> 11) & 0x1) {
            offset += 0xFFFFE000;
        }
        if((jimm >> 20) & 0x1){
            jimm += 0xFFE00000;
        }
        uint32_t print;
        int printCounter = 0;
        int printOffset = 0;
        uint32_t ecallVal;
        reg[0] = 0;
        printf("\nOpcode: %x\n", opcode);


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
                store(funct3, funct7, rd, rs1, rs2, memory, reg);
                break;
            case 0x33: //interger operations
                intergerOp(funct3, imm12, rd, rs1, rs2, reg);
                break;
            case 0x37: //LUI
                //printf("imm20: %d,", imm20);
                if ((imm20 >> 19) == 1) {
                    imm20 += 0xFFF00000;
                }
                reg[rd] = imm20 << 12; //Think this is right
                break;
            case 0x63: //Branch
                pc = branch(funct3, offset, rs1, rs2, reg, pc);
                break;
            case 0x67: //jalr
                reg[rd] = reg[rs1] - 4 + imm12;
                pc = reg[rd];
                break;
            case 0x6F: //jal
                printf("jimm: %x\n", jimm);
                reg[rd] = pc + 4;
                pc += jimm - 4;
                break;
            case 0x73: //ecall
                ecallVal = ecall(print, printCounter, printOffset, memory, reg);
                if (ecallVal == 10) return 0;
                else if (ecallVal == 17) return (int) reg[10];
                break;

            default:
                printf("Opcode %x not yet implemented\n", opcode);
        }

        printf("pc: %x, \tx1: %x,\tx2: %x,\tx3: %x,\tx4: %x,\tx5: %x, \tx6: %x,\tx7: %x,\n",
               pc, reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);
        printf("x8: %x, \tx9: %x,\tx10: %x,\tx11: %x,\tx12: %x,\tx13: %x, \tx14: %x,\tx15: %x,\n",
               reg[8], reg[9], reg[10], reg[11], reg[12], reg[13], reg[14], reg[15]);
        printf("x16: %x, \tx17: %x,\tx18: %x,\tx19: %x,\tx20: %x,\tx21: %x, \tx22 %x,\tx23: %x,\n",
               reg[16], reg[17], reg[18], reg[19], reg[20], reg[21], reg[22], reg[23]);
        printf("x24: %x, \tx25: %x,\tx26: %x,\tx27: %x,\tx28: %x,\tx29: %x, \tx30: %x,\tx31: %x,\n",
               reg[24], reg[25], reg[26], reg[27], reg[28], reg[29], reg[30], reg[31]);
        pc += 4;
    }
}

void readFile(uint32_t *memory) {
    // OPEN FILE
    char str[] = "t14.bin";
    printf("\nRunning simulator with %s\n" ,str);
    FILE *fp = fopen(str, "rb");
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
        printf("adress: %x: \t %x\n", i, memory[i]);
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
            switch (reg[rs1]%4) { //which byte to target
                case 0:
                    printf("Load byte: %x\n", reg[rs1+1]);
                    printf("Value: %x\n", memory[reg[rs1]]);
                    reg[rd] = memory[reg[rs1] + imm12] & 0x000000FF;
                    break;
                case 1:
                    reg[rd] = (memory[reg[rs1] - 1 + imm12] & 0x0000FF00) >> 0x8;
                    break;
                case 2:
                    reg[rd] = (memory[reg[rs1] - 2 + imm12] & 0x00FF0000) >> 0x10;
                    break;
                case 3:
                    reg[rd] = (memory[reg[rs1] - 3 + imm12] & 0xFF000000) >> 0x18;
                    break;
                default:
                    printf("shouldn't happen");
            }

            if ((reg[rd] >> 7) == 1) {
                reg[rd] += 0xFFFFFF00;
            }
            break;
        case 0x1: //LH Untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            switch (reg[rs1]%2) { //which byte to target
                case 0:
                    reg[rd] = memory[reg[rs1] + imm12] & 0x0000FFFF;
                    break;
                case 1:
                    reg[rd] = (memory[reg[rs1] - 1 + imm12] & 0xFFFF0000) >> 0x10;
                    printf("test %x\n", reg[rs1] - 1 + imm12);
                    break;

                default:
                    printf("shouldn't happen");
            }
            reg[rd] = memory[reg[rs1] + imm12] & 0x0000FFFF;
            if ((reg[rd] >> 15) == 1) {
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
            switch (reg[rs1]%4) { //which byte to target
                case 0:
                    reg[rd] = (memory[reg[rs1] + imm12] & 0x000000FF) & 0x000000FF;
                    break;
                case 1:
                    reg[rd] = ((memory[reg[rs1] - 1 + imm12] & 0x0000FF00) >> 0x8) & 0x000000FF;
                    break;
                case 2:
                    reg[rd] = ((memory[reg[rs1] - 2 + imm12] & 0x00FF0000) >> 0x10) & 0x000000FF;
                    break;
                case 3:
                    reg[rd] = ((memory[reg[rs1] - 3 + imm12] & 0xFF000000) >> 0x18) & 0x000000FF;
                    break;
                default:
                    printf("shouldn't happen");
            }

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

void store(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *memory,
           uint32_t *reg) {
    uint32_t imm;
    switch (funct3) {
        case 0x0: //SB Untested
            imm = (funct7 << 5) + rd;
            if(imm >> 11 & 0x1){
                imm += 0xFFFFF000;
            }

            //printf("stored value: %x\n", (reg[rs2] & 0x000000FF));
            //memory[reg[rs1] + imm] = (reg[rs2] & 0x000000FF);

            switch (reg[rs1]%4) { //which byte to target
                case 0:
                    memory[reg[rs1] + imm] += (reg[rs2] & 0x000000FF);
                    break;
                case 1:
                    memory[reg[rs1] - 1 + imm] += (reg[rs2] & 0x000000FF) << 0x8;
                    break;
                case 2:
                    memory[reg[rs1] - 2 + imm] += (reg[rs2] & 0x000000FF) << 0x10;
                    break;
                case 3:
                    memory[reg[rs1] - 3 + imm] += (reg[rs2] & 0x000000FF) << 0x18;
                    break;
                default:
                    printf("shouldn't happen");
            }


            break;
        case 0x1: //SH Untested
            imm = (funct7 << 5) + rd;
            if(imm >> 11 & 0x1){
                imm += 0xFFFFF000;
            }
            switch (reg[rs1]%2) { //which byte to target
                case 0:
                    memory[reg[rs1] + imm] += (reg[rs2] & 0x0000FFFF);
                    //reg[rd] = memory[reg[rs1] + imm12] & 0x000000FF;
                    break;
                case 1:
                    memory[reg[rs1] + imm] += (reg[rs2] & 0x0000FFFF) << 0x18;
                    //reg[rd] = (memory[reg[rs1] - 1 + imm12] & 0x0000FF00) >> 0x8;
                    break;
                default:
                    printf("shouldn't happen");
            }
            memory[reg[rs1] + imm] = (reg[rs2] & 0x0000FFFF);
            break;
        case 0x2: //SW
            imm = (funct7 << 5) + rd;
            if(imm >> 11 & 0x1){
                imm += 0xFFFFF000;
            }

            memory[reg[rs1] + imm] = reg[rs2];
            //printf("Value saved: %x, at %x\n", memory[reg[rs1] + imm], reg[rs1] + imm);
            break;
        default:
            printf("Invalid funct3");
    }
}


int branch(uint32_t funct3, uint32_t offset, uint32_t rs1, uint32_t rs2, uint32_t *reg, int pc) {
    switch (funct3) {
        case 0: // BEQ
            if (reg[rs1] == reg[rs2]) {
                pc += offset - 4;
            }
            break;
        case 1: // BNE
            if (reg[rs1] != reg[rs2]) {
                pc += offset - 4;
            }
            break;
        case 4: // BLT
            if ((int) reg[rs1] < (int) reg[rs2]) {
                pc += offset - 4;
            }
            break;
        case 5: // BGE
            if ((int) reg[rs1] >= (int) reg[rs2]) {
                pc += offset - 4;
            }
            break;
        case 6: // BLTU
            if (reg[rs1] < reg[rs2]) {
                pc += offset - 4;
            }
            break;
        case 7: // BGEU
            if (reg[rs1] >= reg[rs2]) {
                pc += offset - 4;
            }
            break;
        default:
            printf("bad branch\n");
            break;
    }
    return pc;
}

void immediate(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t shamt, uint32_t *reg) {
    switch (funct3) {
        case 0: //addi
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] + imm12;
            break;
        case 1: //SLLI
            reg[rd] = (reg[rs1] << (shamt & 0x1F));
            break;
        case 2: //SLTI untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            if ((int) reg[rs1] < (int) imm12) {
                reg[rd] = 1;
            } else {
                reg[rd] = 0;
            }
            break;
        case 3: //SLTIU untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            if (reg[rs1] < imm12) {
                reg[rd] = 1;
            } else {
                reg[rd] = 0;
            }
            break;
        case 4: //XORI untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] ^ imm12;
            break;
        case 5: //SRLI/SRAI
            if (imm12 >> 10 & 0x1) {//SRAI
                int s = -(reg[rs1] >> 31);
                reg[rd] = (s ^ reg[rs1]) >> (shamt & 0x1F) ^ s;
            } else { //SRLI
                reg[rd] = (reg[rs1] >> (shamt & 0x1F));
            }
            break;

        case 6: //ORI untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] | imm12;
            break;
        case 7: //ANDI untested
            if ((imm12 >> 11) == 1) {
                imm12 += 0xFFFFF000;
            }
            reg[rd] = reg[rs1] & imm12;
            break;
        default:
            printf("Invalid funct3\n");
    }
}

void intergerOp(uint32_t funct3, uint32_t imm12, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t *reg) {
    //printf("funct3: %d\n", funct3);
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
            printf("reg[rs1]: %b, \t reg[rs2]: %b\n", reg[rs1], reg[rs2]);
            break;
        case 5: //SRL/SRA
            if (imm12 >> 10 & 0x1) { //SRA   Arithmetic right shift
                reg[rd] =
                        (reg[rs1] >> (reg[rs2] & 0x1F)) | (reg[rs1] << (32 - (reg[rs2] & 0x1F))); // Might not be right
            } else { //SRL      Logical right shift
                reg[rd] = reg[rs1] >> (reg[rs2] & 0x1F);
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

uint32_t ecall(uint32_t print, int printCounter, int printOffset, uint32_t *memory, uint32_t *reg) {
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
            //case 17: //Exit with exit code doesn't need case since check is performed elsewhere
        default:
            break;
    }
    return reg[17];
}
