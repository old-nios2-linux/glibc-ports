/* Copyright (C) 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <setjmp.h>

#ifndef __GNUC__
#error This file uses GNU C extensions; you must compile with GCC.
#endif

register long int
  r9 asm ("$9"), r10 asm ("$10"), r11 asm ("$11"), r12 asm ("$12"),
  r13 asm ("$13"), r14 asm ("$14");

register long int *fp asm ("$15"), *sp asm ("$30"), *retpc asm ("$26");

#if 1				/* XXX */
register double
  f2 asm ("$f2"), f3 asm ("$f3"), f4 asm ("$f4"), f5 asm ("$f5"),
  f6 asm ("$f6"), f7 asm ("$f7"), f8 asm ("$f8"), f9 asm ("$f9");
#endif

/* Save the current program position in ENV and return 0.  */
int
__setjmp (jmp_buf env)
{
  /* Save the integer registers.  */
  env[0].__9 = r9;
  env[0].__10 = r10;
  env[0].__11 = r11;
  env[0].__12 = r12;
  env[0].__13 = r13;
  env[0].__14 = r14;

  /* Save the return address of our caller, where longjmp will jump to.  */
  env[0].__pc = retpc;

  /* We lose if the compiler uses the FP in __setjmp.  XXX */
  env[0].__fp = fp;

  env[0].__sp = sp;

  return 0;
}
