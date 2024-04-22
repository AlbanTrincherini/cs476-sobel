#include <stdio.h>
#include <vga.h>
#include <ov7670.h>
#include <swap.h>

volatile uint32_t memoryArray[512];

int main () {
  vga_clear();
  printf("Hello World!\n" );

  uint32_t busAddress = (uint32_t) &memoryArray[0];
  uint32_t extracted_word;

  printf("DEBUGING DMA\n");
  /* uint32_t address = 0x00000211;
  uint32_t res = 0;
  uint32_t writeValue = 45;
  asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0x2" : [out1] "=r"(res) : [in1] "r"(address), [in2] "r"(writeValue));
  printf("result after read %d\n", res); */

  printf("WRITE TEST\n");
  write(0, 12);
  printf("READ TEST, should print 12\n");
  read(input(1, 0));

  printf("BusStartAddress TEST (%d)\n", busAddress);
  read_write_ctrl(0b0010, busAddress);
  printf("MemoryStartAddress TEST (0)\n");
  read_write_ctrl(0b0100, 0);
  printf("BlockSize TEST (17)\n");
  read_write_ctrl(0b0110, 17);
  printf("BurstSize TEST (4)\n");
  read_write_ctrl(0b1000, 4);

  printf("END DEBUG DMA\n");
  return 0;
}

void read(uint32_t valueA) {
  uint32_t valueB = 0; //osef
  uint32_t res = 0;
  asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0x2" : [out1] "=r"(res) : [in1] "r"(valueA), [in2] "r"(valueB));
  printf("result after read %d\n", res);
}

void write(uint32_t valueA, uint32_t valueB) {
  uint32_t res = 0;
  asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0x2" : [out1] "=r"(res) : [in1] "r"(valueA), [in2] "r"(valueB));
}

uint32_t input(uint32_t code, uint32_t addr) {
  return code << 9 | addr;
}

read_write_ctrl(uint32_t code, uint32_t value){
  uint32_t ctrl = input(code, 0);
  write(ctrl | 1 << 9, value);
  read(ctrl);
}
