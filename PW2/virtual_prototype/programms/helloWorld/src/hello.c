#include <stdio.h>
#include <vga.h>
#include <ov7670.h>
#include <swap.h>

int main () {
  vga_clear();
  printf("Hello World!\n" );

  volatile uint32_t memoryArray[512];
  uint32_t busAddress = (uint32_t) &memoryArray[0];
  uint32_t extracted_word;

  printf("DEBUGING DMA\n");
  uint32_t address = 0x00000211;
  uint32_t res = 0;
  uint32_t writeValue = 45;
  asm volatile("l.nios_rrr %[out1],%[in1],%[in2],0x2" : [out1] "=r"(res) : [in1] "r"(address), [in2] "r"(writeValue));
  printf("result after read %d\n", res);

  printf("END DEBUG DMA\n");
  return 0;
}
