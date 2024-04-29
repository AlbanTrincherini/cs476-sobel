## Modified files: 

- virtual_prototype/modules/rgb565grayscale/verilog/rgb565grayscaleIse.v (new)
- virtual_prototype/modules/rgb565grayscale/verilog/rgb565grayscaleIse_tb.v (new)
- (To compile the testbench: iverilog -s rgb565grayscaleIseTestbench -o testbench rgb565grayscaleIse.v rgb565grayscaleIse_tb.v )
- virtual_prototype/programms/grayscale/src/grayscale.c
- virtual_prototype/systems/singleCore/verilog/or1420SingleCore.v 
- virtual_prototype/systems/singleCore/config/project.files


## Observed results

- The number of cycles for the software implementation are : 31247857 execution cycles with 19312121 stall cycles = 11935736 "working cycles (we measured the cycles for one full execution of the for loop).
- The number of cycles for the hardware implementation are : 25362131 execution cycles with 18680495 stall cycles = 6681636 "working" cycles
- This corresponds to an execution time decrease of approximately 44%.