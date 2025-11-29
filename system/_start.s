# To be injected at the end of ScenePrep_Common, 0x801a4ccc
# ---------------------------------------------------------

    .macro backupall
        mflr r0
        stw r0, 0x4(r1)
        stwu r1, -0x100(r1)
        stmw r3, 0x8(r1)
    .endm

    .macro restoreall
        lmw  r3, 0x8(r1)
        lwz r0, 0x104(r1)
        addi r1, r1, 0x100
        mtlr r0
    .endm


    .section .text
    .global _start

_start:
    backupall

    # Call main code
    bl start
    
    restoreall
    
    # Execute replaced instruction
    lwz 0, 0x34(r1)

    # Jump back to original code (0x801a4cd0)
    lis r3, 0x801a
    ori r3, r3, 0x4cd0
    mtctr r3
    bctr
