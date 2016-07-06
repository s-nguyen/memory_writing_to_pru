//Compiled with "pasm -b memory.p"

.origin 0 // start of program in PRU memory
.entrypoint START // program entry point 

#define PRU0_R31_VEC_VALID  32;
#define PRU_EVTOUT_0	    3
#define PRU_EVTOUT_1	    4
#define CONST_PRUCFG	     C4
#define CONST_PRUDRAM        C24
#define CONST_PRUSHAREDRAM   C28
#define CONST_DDR            C31
#define PRU0_CTRL            0x22000
#define PRU1_CTRL            0x24000
#define CTPPR0               0x28
#define OWN_RAM              0x00000000
#define OTHER_RAM            0x020
#define SHARED_RAM           0x100

#define DDR r29
#define DDR_SIZE r28

START:
    //Enable OCP master port
    lbco r0, CONST_PRUCFG, 4, 4
    clr r0, r0, 4 //clear syscfg[standby_init] to enable ocp master port
    sbco r0, CONST_PRUCFG, 4, 4
    
    // Grab address of shared DDR segment for data ram
    mov r0, 0x0
    lbbo r29, r0, 0, 4
    lbbo r28, r0, 4, 4
    
    //set up 200k worth of int
    mov r12, 0
    mov r14, 200000
    
    //Let's start the loop
    mov r10, DDR
LOOP0:
    sbbo r12, r10, 0, 4
    add r10, r10, 4
    add r12, r12, 1
    qbne LOOP0, r12, r14

DONE:
    
    mov r1, 423
    mov r0, 0x00000000
    sbbo r1, r0, 0, 4


    
END:                             // notify the calling app that finished
	MOV	R31.b0, PRU0_R31_VEC_VALID | PRU_EVTOUT_0
	HALT                     // halt the pru program