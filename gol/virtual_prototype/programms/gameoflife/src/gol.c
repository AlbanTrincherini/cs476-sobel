#include <stdio.h>
#include <swap.h>
#include <ov7670.h>
#include <vga.h>

volatile uint32_t memBuffer[512];

enum { WIDTH = 40, HEIGHT = 30};
enum { SEED_W = 40, SEED_H = 30};
#define SIZE 1200

int const CELLSIDE = 16;
static uint8_t seed[SEED_H][SEED_W] = 
                             {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0}, 
                              {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};


int fateOfTheCell(int cell, int liveNeighbors) {
  if (cell == 1) {
    if (liveNeighbors == 2 || liveNeighbors == 3) {
      return 1;
    } else {return 0;}
  } else {
    if (liveNeighbors == 3) {
      return 1;
    } else {return 0;}
  }
}

void polling() {
  int res = 1;
  while(res == 1) {
    asm volatile ("l.nios_rrr %[out1],%[in1],r0,0x14":[out1]"=r"(res):[in1]"r"(0b1010 << 12));
  }
}

void set_bus_start(uint32_t* start) {
  uint32_t addr = (uint32_t) start;
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0011 << 12),[in2]"r"(addr));
}

void set_mem_start(uint32_t start) {
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0101 << 12),[in2]"r"(start));
}

void start_read() {
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1011 << 12),[in2]"r"(0b1));
}

void start_write() {
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1011 << 12),[in2]"r"(0b10));
}

uint32_t read(uint32_t addr) {
  uint32_t result = 0;
  asm volatile ("l.nios_rrr %[out1],%[in1],r0,0x14":[out1]"=r"(result):[in1]"r"(addr));
  return result;
}

void write(uint32_t addr, uint32_t value) {
  addr = (1 << 12) | addr;
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(addr),[in2]"r"(value));
}

uint32_t toAddr(int x, int y) {
  int res = x*WIDTH + y;
  return res;
}

int main() {
  
  uint8_t array[HEIGHT][WIDTH];
  uint8_t nextArray[HEIGHT][WIDTH];
  uint16_t frameBuffer[640*480];
  volatile uint32_t current_buffer = 0;


  volatile int result;
  volatile unsigned int *vga = (unsigned int *) 0X50000020;
  camParameters camParams;
  vga_clear();

  volatile uint32_t cycles,stall,idle;
  
  uint32_t * fb = (uint32_t *) &frameBuffer[0];

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
  vga[2] = swap_u32(1); // 1 for rgb, 2 for grayscale (changes bit length too)
  vga[3] = swap_u32((uint32_t) &frameBuffer[0]);

  
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b0111 << 12),[in2]"r"(640*480/2));
  uint32_t res = 0;
  asm volatile ("l.nios_rrr r0,%[in1],%[in2],0x14"::[in1]"r"(0b1001 << 12),[in2]"r"(31));
  set_bus_start(fb);

  // INIT
  for (int i = 0 ; i < HEIGHT ; i++) {
    for (int j = 0 ; j < WIDTH ; j++) {
      write(toAddr(i, j), seed[i][j]);
      //printf("%d %d |", seed[i][j], read(toAddr(i,j)));
      write(SIZE + toAddr(i, j), 0);
      //array[i][j] = seed[i][j];
      //nextArray[i][j] = 0;
    }
     //printf("\n");
  }

  // Init profiling counters
  asm volatile ("l.nios_rrr r0,r0,%[in2],0xC"::[in2]"r"(7));

  // GAME
  while(1) {
    
    // writing current array to frame buffer (doing it here because of init)
    /*for (int line = 0; line < camParams.nrOfLinesPerImage; line++) { // x
      int arrayLine = line/CELLSIDE; // integer division
      for (int pixel = 0; pixel < camParams.nrOfPixelsPerLine; pixel++) { // y          
        int arrayCol = pixel/CELLSIDE;
        uint16_t value = 0xee6b * array[arrayLine][arrayCol];
        frameBuffer[line*camParams.nrOfPixelsPerLine+pixel] = value; // test
      }
    }*/

    // //Profling
    // asm volatile ("l.nios_rrr %[out1],r0,%[in2],0xC":[out1]"=r"(cycles):[in2]"r"(1<<8|7<<4));
    // asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(stall):[in1]"r"(1),[in2]"r"(1<<9));
    // asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(idle):[in1]"r"(2),[in2]"r"(1<<10));
    // printf("write current array: %d %d %d\n", cycles, stall, idle);
    // asm volatile ("l.nios_rrr r0,r0,%[in2],0xC"::[in2]"r"(7));

    set_mem_start(current_buffer);
    start_write();
break;
    uint32_t source = current_buffer;
    uint32_t dest = current_buffer == 0 ? SIZE : 0;

    // computing nextArray from array
    for (int x = 0 ; x < HEIGHT ; x++) {
      for (int y = 0 ; y < WIDTH ; y++) {
        int theCell = array[x][y];
        int xMinus1 = (x-1 + HEIGHT) % HEIGHT;
        int xPlus1 = (x+1) % HEIGHT;
        int yMinus1 = (y-1 + WIDTH) % WIDTH;
        int yPlus1 = (y+1) % WIDTH;
        int liveNeighbors = read(source + toAddr(xMinus1, yMinus1)) 
              + read(source + toAddr(xMinus1, y))
              + read(source + toAddr(xMinus1, yPlus1)) 
              + read(source + toAddr(x, yMinus1)) 
              + read(source + toAddr(x, yPlus1)) 
              + read(source + toAddr(xPlus1, yMinus1))
              + read(source + toAddr(xPlus1, y))
              + read(source + toAddr(xPlus1, yPlus1));
        int fate = fateOfTheCell(theCell, liveNeighbors);
        write(dest + toAddr(x, y), fate);
      }
    }

    polling();

    //Profling
    asm volatile ("l.nios_rrr %[out1],r0,%[in2],0xC":[out1]"=r"(cycles):[in2]"r"(1<<8|7<<4));
    asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(stall):[in1]"r"(1),[in2]"r"(1<<9));
    asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(idle):[in1]"r"(2),[in2]"r"(1<<10));
    printf("compute next array: %d %d %d\n", cycles, stall, idle);
    asm volatile ("l.nios_rrr r0,r0,%[in2],0xC"::[in2]"r"(7));

    // updating array
    for (int i = 0 ; i < HEIGHT ; i++) {
      for (int j = 0 ; j < WIDTH ; j++) {
        array[i][j] = nextArray[i][j];
      }
    }

    // //Profling
    // asm volatile ("l.nios_rrr %[out1],r0,%[in2],0xC":[out1]"=r"(cycles):[in2]"r"(1<<8|7<<4));
    // asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(stall):[in1]"r"(1),[in2]"r"(1<<9));
    // asm volatile ("l.nios_rrr %[out1],%[in1],%[in2],0xC":[out1]"=r"(idle):[in1]"r"(2),[in2]"r"(1<<10));
    // printf("update array from next array: %d %d %d\n", cycles, stall, idle);
    // asm volatile ("l.nios_rrr r0,r0,%[in2],0xC"::[in2]"r"(7));

    // printf("Done\n");

    current_buffer = current_buffer == 0 ? SIZE : 0;

  } 

  return 0;
}