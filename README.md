# risc-v-simu
Project 02155

When compiling C program to work in the simulator the following code is needed to print hello world

asm("li sp, 0x100000"); // SP set to 1 MB
asm("jal main");        // call main
asm("li a7, 10");       // prepare ecall exit
asm("ecall");           // now your simulator should stop
void prints(volatile char* ptr);

void main() {
  char* str = "Hello world";
  prints(str);
}

void prints(volatile char* ptr){ // ptr is passed through register a0
  asm("li a7, x"); //You must decide the value of x
  asm("ecall");
}


To compile it run:

riscv64-linux-gnu-gcc -nostartfiles -nostdlib -march=rv32i -mabi=ilp32 -T $HOME/linker.ld hello.c -o hello.out

and finally run the following command to get a .bin file which the simulator can work with.

riscv64-linux-gnu-objcopy -O binary hello.out hello.bin


