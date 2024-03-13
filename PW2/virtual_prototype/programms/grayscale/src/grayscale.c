#include <stdio.h>
#include <ov7670.h>
#include <swap.h>
#include <vga.h>

void init_counters();
void reset_print_counters();

int main () {
  volatile uint16_t rgb565[640*480];
  volatile uint8_t grayscale[640*480];
  volatile uint32_t result, cycles,stall,idle;
  volatile unsigned int *vga = (unsigned int *) 0X50000020;
  camParameters camParams;
  vga_clear();
  
  printf("Initialising camera (this takes up to 3 seconds)!\n" );
  camParams = initOv7670(VGA);
  printf("Done!\n" );
  printf("NrOfPixels : %d\n", camParams.nrOfPixelsPerLine );
  result = (camParams.nrOfPixelsPerLine <= 320) ? camParams.nrOfPixelsPerLine | 0x80000000 : camParams.nrOfPixelsPerLine;
  vga[0] = swap_u32(result);
  printf("NrOfLines  : %d\n", camParams.nrOfLinesPerImage );
  result =  (camParams.nrOfLinesPerImage <= 240) ? camParams.nrOfLinesPerImage | 0x80000000 : camParams.nrOfLinesPerImage;
  vga[1] = swap_u32(result);
  printf("PCLK (kHz) : %d\n", camParams.pixelClockInkHz );
  printf("FPS        : %d\n", camParams.framesPerSecond );
  uint32_t * rgb = (uint32_t *) &rgb565[0];
  uint32_t grayPixels;
  vga[2] = swap_u32(2);
  vga[3] = swap_u32((uint32_t) &grayscale[0]);
  init_counters();
  while(1) {
    uint32_t * gray = (uint32_t *) &grayscale[0];
    takeSingleImageBlocking((uint32_t) &rgb565[0]);
    for (int line = 0; line < camParams.nrOfLinesPerImage; line++) {
      for (int pixel = 0; pixel < camParams.nrOfPixelsPerLine; pixel++) {
        uint16_t rgb = swap_u16(rgb565[line*camParams.nrOfPixelsPerLine+pixel]);
        uint32_t red1 = ((rgb >> 11) & 0x1F) << 3;
        uint32_t green1 = ((rgb >> 5) & 0x3F) << 2;
        uint32_t blue1 = (rgb & 0x1F) << 3;
        uint32_t gray = ((red1*54+green1*183+blue1*19) >> 8)&0xFF;
        grayscale[line*camParams.nrOfPixelsPerLine+pixel] = gray;
      }
    }
    reset_print_counters();
  }
}

void init_counters() {
  uint32_t control = 0b111;
  asm volatile ("l.nios_rrr r0, r0, %[in2], 0x9"::[in2]"r"(control)); // PROBLEME
}

void reset_print_counters() {
  uint32_t result, counterid = 0;
  uint32_t control = 0b1 << 8;
  asm volatile ("l.nios_rrr %[out1], %[in1], %[in2], 0x9": [out1]"=r"(result):
                                                        [in1]"r"(counterid),
                                                        [in2]"r"(control));
  printf("#execution cycles: %d\n", result);
  
  counterid++;
  control <<= 1;
  asm volatile ("l.nios_rrr %[out1], %[in1], %[in2], 0x9": [out1]"=r"(result):
                                                        [in1]"r"(counterid),
                                                        [in2]"r"(control));
  printf("#stall cycles: %d\n", result);  
  
  counterid++;
  control <<= 1;
  asm volatile ("l.nios_rrr %[out1], %[in1], %[in2], 0x9": [out1]"=r"(result):
                                                        [in1]"r"(counterid),
                                                        [in2]"r"(control));
  printf("#bus-idle cycles: %d\n", result);         
  printf("\n");                                          
}
