#include <stdio.h>
#include <swap.h>
#include <ov7670.h>
#include <vga.h>

volatile uint32_t memBuffer[512];

enum { WIDTH = 20, HEIGHT = 15 };

int const CELLSIDE = 32;
// int const WIDTH = 20;
// int const HEIGHT = 15;
static int seed[HEIGHT][WIDTH] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, };


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

// int getLiveNeighbors(int x, int y) {
//   return array[x-1][y-1] + array[x-1][y] + array[x-1][y+1] + array[x][y-1] + array[x][y+1] + array[x+1][y-1] + array[x+1][y] + array[x+1][y+1];
// }

int main() {
  
  int array[HEIGHT][WIDTH];
  int nextArray[HEIGHT][WIDTH];
  uint16_t frameBuffer[640*480];
  

  volatile int result;
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
  vga[2] = swap_u32(1); // 1 for rgb, 2 for grayscale (changes bit length too)
  vga[3] = swap_u32((uint32_t) &frameBuffer[0]);

  // INIT
  for (int i = 0 ; i < HEIGHT ; i++) {
    for (int j = 0 ; j < WIDTH ; j++) {
      array[i][j] = seed[i][j];
      nextArray[i][j] = 0;
    }
  }

  // GAME
  while(1) {
    // writing current array to frame buffer (doing it here because of init)
    for (int line = 0; line < camParams.nrOfLinesPerImage; line++) { // x
      int arrayLine = line/CELLSIDE; // integer division
      for (int pixel = 0; pixel < camParams.nrOfPixelsPerLine; pixel++) { // y          
        int arrayCol = pixel/CELLSIDE;
        uint16_t value = 0xee6b * array[arrayLine][arrayCol];
        frameBuffer[line*camParams.nrOfPixelsPerLine+pixel] = value; // test
      }
    }

    // computing nextArray from array
    for (int x = 0 ; x < HEIGHT ; x++) {
      for (int y = 0 ; y < WIDTH ; y++) {
        int theCell = array[x][y];
        int liveNeighbors = array[x-1][y-1] + array[x-1][y] + array[x-1][y+1] + array[x][y-1] + array[x][y+1] + array[x+1][y-1] + array[x+1][y] + array[x+1][y+1];
        nextArray[x][y] = fateOfTheCell(theCell, liveNeighbors);
      }
    }

    // updating array
    for (int i = 0 ; i < HEIGHT ; i++) {
      for (int j = 0 ; j < WIDTH ; j++) {
        array[i][j] = nextArray[i][j];
      }
    }
    printf("Done\n");
  } 

  return 0;
}