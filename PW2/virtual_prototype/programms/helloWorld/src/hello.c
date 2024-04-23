#include <stdio.h>
#include <vga.h>
#include <ov7670.h>
#include <swap.h>

void read(uint32_t valueA);
void write(uint32_t valueA, uint32_t valueB);
uint32_t input(uint32_t code, uint32_t addr);
void polling();

int main () {
  vga_clear();
  printf("Hello World!\n" );
  volatile uint32_t memoryArray[64];

  uint32_t busAddress = (uint32_t) &memoryArray[0];
  uint32_t extracted_word;

  printf("DEBUGGING DMA\n");

  printf("WRITE TEST\n");
  write(input(1, 0), 12);
  printf("READ TEST, should print 12\n");
  read(0);

  printf("BusStartAddress TEST (%d)\n", busAddress);
  write(input(0b0011, 0), busAddress);
  read(input(0b0010, 0));
  printf("MemoryStartAddress TEST (0)\n");
  write(input(0b0101, 0), 0);
  read(input(0b0100, 0));
  printf("BlockSize TEST (17)\n");
  write(input(0b0111, 0), 17);
  read(input(0b0110, 0));
  printf("BurstSize TEST (4)\n");
  write(input(0b1001, 0), 4);
  read(input(0b1000, 0));

  for(int i = 0; i < 64; i++) {
    memoryArray[i] = i;
  }

  printf("After init\n");
  write(input(0b1011, 0), 1);
  printf("After start\n");
  polling();
  for(int i = 0; i < 18; i++) {
    read(i);
  }
  printf("THE END\n");
  return 0;
}

void polling() {
  uint32_t res = 1;
  uint32_t valueA = input(0b1010, 0);
  uint32_t valueB = 0;
  while((res & 0b1) == 0b1) {
    asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0xE" : [out1] "=r"(res) : [in1] "r"(valueA), [in2] "r"(valueB));
    printf("%d\n", res);
  }
}

void read(uint32_t valueA) {
  uint32_t valueB = 0; //osef
  uint32_t res = 666;
  asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0xE" : [out1] "=r"(res) : [in1] "r"(valueA), [in2] "r"(valueB));
  printf("result after read %d\n", res);
}

void write(uint32_t valueA, uint32_t valueB) {
  uint32_t res = 0;
  asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0xE" : [out1] "=r"(res) : [in1] "r"(valueA), [in2] "r"(valueB));
  printf("sortie de write\n");
}

uint32_t input(uint32_t code, uint32_t addr) {
  return (code << 9) | addr;
}
