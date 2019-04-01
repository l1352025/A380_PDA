; rt_memory.s: template for writing a memory model implementation
;
; Copyright 1999 ARM Limited. All rights reserved.
;
; RCS $Revision: 1.6 $
; Checkin $Date: 2000/11/08 15:43:29 $
; Revising $Author: statham $

; This module gives a framework for writing custom memory models
; in. It actually implements a very simple memory model, in which:
;
;  - the stack is 64Kb and grows down from 0x80000000
;  - the heap is 128Kb and grows up from 0x60000000
;  - a stack overflow is instantly fatal and does not even raise SIGSTAK
;  - no extra memory is available for the heap

        ; This include file defines rt_mm0, rt_mm1, rt_mm2 and
        ; rt_mm3 to be the offsets from the start of the
        ; __user_libspace block of four words of storage available
        ; for use by the memory model.
        GET     rt_mem_h.s

        IMPORT  __user_libspace

        AREA    |x$codeseg|, CODE, READONLY

; --------------------------------------------------

        ; Initialisation function. This routine isn't required to
        ; preserve any registers, although obviously it shouldn't
        ; lose LR because it won't know where to return to. On
        ; exit, SP and SL should describe a usable stack, and
        ; [a1,a2) should be the initial block to use for the heap.
        ;
        ; Returning a1==a2 is allowed.

        EXPORT  __rt_stackheap_init
__rt_stackheap_init

        ; Set up some example values.
        MOV     sp, #0x80000000
        SUB     sl, sp, #0x10000
        MOV     a1, #0x60000000
        ADD     a2, a1, #0x20000

        MOV     pc, lr

; --------------------------------------------------

        ; The stack overflow function. This is called from the
        ; entry sequence of any routine that fails a stack limit
        ; check. If stack-limit checking is not enabled, this
        ; function is not needed at all.

        ; If the memory model supports non-fatal stack overflows,
        ; this function will need to return. It should return with
        ; _all_ registers preserved as they were on entry (except
        ; SP and SL, which it is allowed to modify to preserve
        ; stack validity). It should not try to return to LR, but
        ; should return by branching to __rt_stack_overflow_return.

        ; On entry, IP (r12) contains the value to which the
        ; calling function would like to drop SP.

        IMPORT  __rt_raise
        IMPORT  __rt_exit
        IMPORT  _sys_exit

        EXPORT  __rt_stack_overflow
__rt_stack_overflow

        ; Exit immediately.
        MOV     a1, #100
        B       _sys_exit

; --------------------------------------------------

        ; The heap extend function. This is called using the
        ; ordinary ATPCS calling convention.
        ;
        ; On input: a1 is the minimum size of memory required
        ;              (guaranteed to be a multiple of 4)
        ;           a2 is a pointer to a word of memory, W, in which
        ;              to store the address of the block
        ;
        ; On exit:  a1 is size of returned block
        ;           W  contains base address of returned block
        ;      Or:  a1 is zero
        ;           W  has undefined contents

        EXPORT  __rt_heap_extend
__rt_heap_extend

        ; Refuse all requests for extra heap space.
        MOV     a1, #0

        LDMFD   sp!, {pc}

; --------------------------------------------------

        ; The longjmp cleanup function. This is called using the
        ; ordinary ATPCS calling convention.
        ;
        ; At minimum, this routine is required to set {SL,SP} to
        ; the values in {a1,a2}. It must set them _atomically_
        ; (i.e. in a single LDM instruction) so that an interrupt
        ; cannot occur between loading one and loading the other.
        ;
        ; This routine is not called unless software stack checking
        ; is enabled.

        EXPORT  __rt_stack_postlongjmp
__rt_stack_postlongjmp

        ; Perform only the minimum required processing.
        STMFD   sp!, {a1,a2}
        LDMIA   sp, {sl,sp}             ; notice absence of ! to avoid
                                        ;   undefined behaviour in LDM
        MOV     pc, lr

; --------------------------------------------------

        END
