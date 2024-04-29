## Comments
The code works on the testbenches we made (confirmed with Prof. Kluter) but we didn't manage to make it work on the FPGA. For some reason we couldn't find, starting a DMA transfer would freeze the CPU. After many hours, we couldn't trace back the problem to ramDmaCI or singleCore.v and thus decided to submit this partially working version.

You should be able to run the testbenches we made with the following commands and replacing _ by the number of the exercise tested: 
iverilog -s dmaEx_ dualSSRAM.v ramDmaCI.v ramDmaEx_Tb.v

./a.out

gtkwave ramEx_.vcd

## Modified files
- virtual_prototype/modules/ramDma/verilog/dualSSRAM.v (new, given in the course)
- virtual_prototype/modules/ramDma/verilog/ramDmaCI.v (new)
- virtual_prototype/modules/ramDma/verilog/ramDmaEx1Tb.v (new)
- virtual_prototype/modules/ramDma/verilog/ramDmaEx2Tb.v (new)
- virtual_prototype/modules/ramDma/verilog/ramDmaEx3Tb.v (new)
- virtual_prototype/systems/singleCore/verilog/or1420SingleCore.v (modified)
- virtual_prototype/systems/singleCore/config/project.files (modified)
- virtual_prototype/programms/helloWorld/src/hello.c (modified, used for testing final implementation on the gecko)
