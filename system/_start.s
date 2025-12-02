# To be injected into jump to main from __start, 0x80005338
# ---------------------------------------------------------

    .section .text
    .global _start

# ---------------------------------------------------------

_start:

    bl _r3_r4_backup          # back up r3 and r4 before running hooks
    mflr r16
    stwu r3, 0(r16)
    stwu r4, 4(r16)

    bl _relocate_got2_table   # relocate .got2 table

    bl _patch                 # call patches

    bl _r3_r4_backup          # restore r3 and r4 after running hooks
    mflr r16
    lwz r3, 0(r16)
    lwz r4, 4(r16)

    lis r16, MeleeMain@ha
    addi r16, r16, MeleeMain@l
    mtctr r16
    bctrl # call main
 
_r3_r4_backup:
    blrl
    .long 0x00000003 # r3 backup
    .long 0x00000004 # r4 backup

# ---------------------------------------------------------

    .globl _breakpoint_helper

# ---------------------------------------------------------

_breakpoint_helper:
    nop    
    blr

# ---------------------------------------------------------

    .globl _relocate_got2_table

# ---------------------------------------------------------

_relocate_got2_table:

    mflr r27
    bl _relocate_got2_table_context

_relocate_got2_table_context:

    mflr r22
    lis  r21, _relocate_got2_table_context@ha
    addi r21, r21, _relocate_got2_table_context@l

    subf r22, r21, r22

    lis  r23, __rei_wolf_got2_start@ha
    addi r23, r23, __rei_wolf_got2_start@l
    add  r23, r23, r22

    lis  r24, __rei_wolf_got2_end@ha
    addi r24, r24, __rei_wolf_got2_end@l
    add  r24, r24, r22

_relocate_got2_table_got2_loop:

    cmpw  r23, r24
    bge   _relocate_got2_table_got2_loop_done

    lwz   r25, 0(r23)

    # skip melee static pointers @ 0x8000xxxx
    lis   r26, 0x8000
    and.  r26, r26, r25
    bne   _relocate_got2_table_got2_next

    add   r25, r25, r22 # relocate entry
    stw   r25, 0(r23)

_relocate_got2_table_got2_next:
    addi  r23, r23, 4
    b     _relocate_got2_table_got2_loop

_relocate_got2_table_got2_loop_done:
    mtlr r27
    blr
