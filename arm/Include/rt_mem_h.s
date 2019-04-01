; rt_mem_h.s
;
; Copyright 1999 ARM Limited. All rights reserved.
;
; RCS $Revision: 1.2 $
; Checkin $Date: 2000/11/08 15:43:29 $
; Revising $Author: statham $

; This header defines the offsets, in the __user_libspace block, of the
; words of storage reserved to the memory model implementation.

        MAP     16
rt_mm0  FIELD   4
rt_mm1  FIELD   4
rt_mm2  FIELD   4
rt_mm3  FIELD   4

        END
