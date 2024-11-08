#include "ceit/ceit.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    //allocate a memory chunk
    Memchunk *chunk = memc_init("joyc",1024*1024);
    memc_dbg(1,chunk); //debug the memory chunk
    //ask for memory from the chunk
    Memory *mem = memory_alloc(chunk, 10,"SJOY_1");
    mem_dbg(1,mem); //debug the memory block
    //write data to the memory block if size is 0 it automatcially increases size as per needed else it will use the size provided and if the data doesnt fit , itt trims it 
    if (memory_write(mem,"HI0099",0) == 0) {
        printf("Write success\n");
    } else {
        printf("Write failed\n");
    }
    //read data from the memory block , we are reading the entire 10 bytes
    char buffer[10];
    if (memory_read(mem,buffer,10) == 0) {
        printf("Read success: %s\n",buffer);
    } else {
        printf("Read failed\n");
    }
    mem_clr();
}
