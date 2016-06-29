/*memory communication practice*/
/*goal: write to different parts of memory using assembly and print data to console*/

// Compile with:  
 // gcc -lprussdrv
#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define PRU_NUM 0

static void *pru0DataMemory;
static unsigned int *pru0DataMemory_int;
static void *ddr_memory;
static unsigned int *ddr_memory_int;

int main(int argc, char** argv) {
   if(getuid()!=0){
      printf("You must run this program as root. Exiting.\n");
      exit(EXIT_FAILURE);
   } 
   
   
   // Initialize structure used by prussdrv_pruintc_intc
   // PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h
   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
   
   // Allocate and initialize memory
   prussdrv_init ();
   
   //Initilize Interrupts
   prussdrv_open (PRU_EVTOUT_0);
   
   // Map PRU's interrupts
   prussdrv_pruintc_init(&pruss_intc_initdata);
   
   // Map PRU memory to a pointer
   prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0DataMemory);
   pru0DataMemory_int = (unsigned int *) pru0DataMemory;
   // Map ddr memory
   prussdrv_map_extmem((void **) &ddr_memory);
   ddr_memory_int = (unsigned int *) ddr_memory;
   unsigned int shared_ddr_len = prussdrv_extmem_size();
   unsigned int physical_address = prussdrv_get_phys_addr((void *) ddr_memory);
   
   // Use first 8 bytes of PRU memory to tell where the share memory is
   pru0DataMemory_int[0] = physical_address;
   pru0DataMemory_int[1] = shared_ddr_len;
   
   printf("%u bytes of shared DDR available.\n Physical (PRU-side) address:%x\n",  shared_ddr_len, physical_address);  
   printf("Virtual (linux-side) address: %p\n\n", ddr_memory);  
   
   
   // Load and execute the PRU program on the PRU
   prussdrv_exec_program (PRU_NUM, "./memory.bin");
   
   // Wait for event completion from PRU, returns the PRU_EVTOUT_0 number
   int n = prussdrv_pru_wait_event (PRU_EVTOUT_0);
   printf("EBB PRU program completed, event number %d.\n", n);
   printf("The number read from PRU0 memory is: %d\n", *pru0DataMemory_int);
   
   

   printf("%d\n", ddr_memory_int[199999]);
 
   
   // Disable PRU and close memory mappings
   prussdrv_pru_disable(PRU_NUM);
   prussdrv_exit ();
   return EXIT_SUCCESS;

}