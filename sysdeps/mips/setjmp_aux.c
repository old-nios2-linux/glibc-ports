/* Copyright (C) 1996, 1997, 2000, 2002, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Brendan Kehoe (brendan@zen.org).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <setjmp.h>
#include <sys/asm.h>

#define STRINGXP(X) __STRING(X)
#define REGS STRINGXP(REG_S)
#define PTRS STRINGXP(PTR_S)

/* This function is only called via the assembly language routine
   __sigsetjmp, which arranges to pass in the stack pointer and the frame
   pointer.  We do things this way because it's difficult to reliably
   access them in C.  */

int
__sigsetjmp_aux (jmp_buf env, int savemask, int sp, int fp)
{
  /* Store the floating point callee-saved registers...  */
#if _MIPS_SIM == _MIPS_SIM_ABI32
  asm volatile ("s.d $f20, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[0]));
  asm volatile ("s.d $f22, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[1]));
  asm volatile ("s.d $f24, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[2]));
  asm volatile ("s.d $f26, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[3]));
  asm volatile ("s.d $f28, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[4]));
  asm volatile ("s.d $f30, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[5]));
#else
  asm volatile ("s.d $f24, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[0]));
  asm volatile ("s.d $f25, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[1]));
  asm volatile ("s.d $f26, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[2]));
  asm volatile ("s.d $f27, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[3]));
  asm volatile ("s.d $f28, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[4]));
  asm volatile ("s.d $f29, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[5]));
  asm volatile ("s.d $f30, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[6]));
  asm volatile ("s.d $f31, %0" : : "m" (env[0].__jmpbuf[0].__fpregs[7]));
#endif  

  /* .. and the PC;  */
  asm volatile (PTRS " $31, %0" : : "m" (env[0].__jmpbuf[0].__pc));

  /* .. and the stack pointer;  */
  env[0].__jmpbuf[0].__sp = (void *) sp;

  /* .. and the FP; it'll be in s8. */
  env[0].__jmpbuf[0].__fp = (void *) fp;

  /* .. and the GP; */
  asm volatile (PTRS " $gp, %0" : : "m" (env[0].__jmpbuf[0].__gp));

  /* .. and the callee-saved registers; */
  asm volatile (REGS " $16, %0" : : "m" (env[0].__jmpbuf[0].__regs[0]));
  asm volatile (REGS " $17, %0" : : "m" (env[0].__jmpbuf[0].__regs[1]));
  asm volatile (REGS " $18, %0" : : "m" (env[0].__jmpbuf[0].__regs[2]));
  asm volatile (REGS " $19, %0" : : "m" (env[0].__jmpbuf[0].__regs[3]));
  asm volatile (REGS " $20, %0" : : "m" (env[0].__jmpbuf[0].__regs[4]));
  asm volatile (REGS " $21, %0" : : "m" (env[0].__jmpbuf[0].__regs[5]));
  asm volatile (REGS " $22, %0" : : "m" (env[0].__jmpbuf[0].__regs[6]));
  asm volatile (REGS " $23, %0" : : "m" (env[0].__jmpbuf[0].__regs[7]));

  /* .. and finally get and reconstruct the floating point csr.  */
  asm ("cfc1 %0, $31" : "=r" (env[0].__jmpbuf[0].__fpc_csr));

  /* Save the signal mask if requested.  */
  return __sigjmp_save (env, savemask);
}
