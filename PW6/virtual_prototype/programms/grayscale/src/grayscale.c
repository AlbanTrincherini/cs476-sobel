#include <stdio.h>
#include <ov7670.h>
#include <swap.h>
#include <vga.h>

#define __WITH_CI

void polling() {
  int res = 1;
  while(res == 1) {
    asm volatile ("l.nios_rrr %[out1],%[in1],r0,0x14":[out1]"=r"(res):[in1]"r"(0b1010 << 9));
  }
}

void set_bus_start(uint32_t* start) {
  uint32_t addr = (uint32_t) start;
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0011 << 9),[in2]"r"(addr));
}

void set_mem_start(uint32_t start) {
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0101 << 9),[in2]"r"(start));
}

void start_read() {
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1011 << 9),[in2]"r"(0b1));
}

void start_write() {
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1011 << 9),[in2]"r"(0b10));
}

uint32_t read(uint32_t addr) {
  uint32_t result = 0;
  asm volatile ("l.nios_rrr %[out1],%[in1],r0,0x14":[out1]"=r"(result):[in1]"r"(addr));
  return result;
}

void write(uint32_t addr, uint32_t value) {
  addr = (1 << 9) | addr;
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(addr),[in2]"r"(value));
}

int main () {
  volatile uint16_t rgb565[640*480];
  volatile uint8_t grayscale[640*480];
  volatile uint32_t result, cycles,stall,idle;
  volatile uint32_t current_buffer = 0;
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
  uint32_t grayPixels;
  vga[2] = swap_u32(2);
  vga[3] = swap_u32((uint32_t) &grayscale[0]);
  while(1) {
    takeSingleImageBlocking((uint32_t) &rgb565[0]);
    asm volatile ("l.nios_rrr r0,r0,%[in2],0xC"::[in2]"r"(7));

      uint32_t * rgb = (uint32_t *) &rgb565[0];
      uint32_t * gray = (uint32_t *) &grayscale[0];
      /*for (int pixel = 0; pixel < ((camParams.nrOfLinesPerImage*camParams.nrOfPixelsPerLine) >> 1); pixel +=2) {
        uint32_t pixel1 = rgb[pixel];
        uint32_t pixel2 = rgb[pixel+1];
        asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0x9":[out1]"=r"(grayPixels):[in1]"r"(pixel1),[in2]"r"(pixel2));
        gray[0] = grayPixels;
        gray++;
      }*/

    asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0111 << 9),[in2]"r"(256));
    asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1001 << 9),[in2]"r"(31));
    set_bus_start(rgb);
    set_mem_start(current_buffer);
    start_read();

    polling();
    //Step 2: transfer next 512 pixels to second buffer and calculate grayscale of the pixels in first buffer.
    // Then check DMA transfer to buffer 2 is done. Then transfer the results of buffer 1 into grayscale screen buffer with DMA
    // Repeat this for 599 iterations. Get pixel in one buffer while calculating previous pixels in the other, alternate
    for(int i = 0; i < 600; i++) {
      uint32_t start_addr = current_buffer;
      if(i != 599) {
        //Transfer to second buffer
        asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0111 << 9),[in2]"r"(256));
        asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1001 << 9),[in2]"r"(31));
        current_buffer = (current_buffer == 0) ? 256 : 0;
        set_mem_start(current_buffer);
        rgb += 256;
        set_bus_start(rgb);
        start_read();
      }

      //Grayscale computation
      for(int j = 0; j < 256; j+=2) {
        uint32_t pixel1 = read(start_addr + j);
        uint32_t pixel2 = read(start_addr + j + 1);

        asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0x9":[out1]"=r"(grayPixels):[in1]"r"(pixel1),[in2]"r"(pixel2));
        write(start_addr + (j/2), grayPixels);
      }


      //check other transfer finished
      polling();

      //Transfer from dma to gray buffer
      asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0111 << 9),[in2]"r"(128));
      asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1001 << 9),[in2]"r"(15));
      set_mem_start(start_addr);
      set_bus_start(gray);
      gray += 128;
      start_write();
      polling();
    }

    //Profling
    asm volatile ("l.nios_rrr %[out1],r0,%[in2],0xC":[out1]"=r"(cycles):[in2]"r"(1<<8|7<<4));
    asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(stall):[in1]"r"(1),[in2]"r"(1<<9));
    asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(idle):[in1]"r"(2),[in2]"r"(1<<10));
    printf("nrOfCycles: %d %d %d\n", cycles, stall, idle);
  }
}
