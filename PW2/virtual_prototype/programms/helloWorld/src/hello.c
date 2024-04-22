#include <stdio.h>
#include <vga.h>
#include <ov7670.h>
#include <swap.h>

void read(uint32_t valueA);
void write(uint32_t valueA, uint32_t valueB);
uint32_t input(uint32_t code, uint32_t addr);
void read_write_ctrl(uint32_t code, uint32_t value);
void polling();

int main () {
  vga_clear();
  printf("Hello World!\n" );
  volatile uint32_t memoryArray[64];

  uint32_t busAddress = (uint32_t) &memoryArray[0];
  uint32_t extracted_word;

  printf("DEBUGING DMA\n");
  /* uint32_t address = 0x00000uint32_t input(uint32_t code, uint32_t addr) {
211;
  uint32_t res = 0;
  uint32_t writeValue = 45;
  asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0x2" : [out1] "=r"(res) : [in1] "r"(address), [in2] "r"(writeValue));
  printf("result after read %d\n", res); */

  printf("WRITE TEST\n");
  write(input(1, 0), 12);
  printf("READ TEST, should print 12\n");
  read(0);

  printf("BusStartAddress TEST (%d)\n", busAddress);
  read_write_ctrl(0b0010, busAddress);
  printf("MemoryStartAddress TEST (0)\n");
  read_write_ctrl(0b0100, 0);
  printf("BlockSize TEST (17)\n");
  read_write_ctrl(0b0110, 17);
  printf("BurstSize TEST (4)\n");
  read_write_ctrl(0b1000, 4);

  /*for(int i = 0; i < 64; i++) {
    memoryArray[i] = i;
  }*/

  printf("After init\n");
  read_write_ctrl(0b1010, 1);
  printf("After start\n");
  polling();
  for(int i = 0; i < 17; i++) {
    read(i);
  }
  printf("END DEBUG DMA\n");
  return 0;
}

void polling() {
  uint32_t res = 1;
  uint32_t valueA = input(0b1010, 0);
  uint32_t valueB = 0;
  while(res == 1) {
    asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0xE" : [out1] "=r"(res) : [in1] "r"(valueA), [in2] "r"(valueB));
    printf("Woooo\n");
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
  if (res != 0) {
    printf("PROBLEMEEEEEEEE");
  }
}

uint32_t input(uint32_t code, uint32_t addr) {
  return code << 9 | addr;
}

void read_write_ctrl(uint32_t code, uint32_t value){
  uint32_t ctrl = input(code, 0);
  write(ctrl | 1 << 9, value);
  read(ctrl);
}
