/* Machine-dependent definitions for profiling support.  ARM version.
   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.
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
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* GCC for the ARM cannot compile __builtin_return_address(N) for N != 0, 
   so we must use an assembly stub.  */

#include <sysdep.h>
#ifndef NO_UNDERSCORES
/* The asm symbols for C functions are `_function'.
   The canonical name for the counter function is `mcount', no _.  */
void _mcount (void) asm ("mcount");
#else
/* The canonical name for the function is `_mcount' in both C and asm,
   but some old asm code might assume it's `mcount'.  */
void _mcount (void);
weak_alias (_mcount, mcount)
#endif

static void mcount_internal (u_long frompc, u_long selfpc);

#define _MCOUNT_DECL(frompc, selfpc) \
static void mcount_internal (u_long frompc, u_long selfpc)

/* This macro/func MUST save r0, r1 because the compiler inserts
	blind calls to _mount(), ignoring the fact that _mcount may
	clobber registers; therefore, _mcount may NOT clobber registers */
/* if (this_fp!=0) {
	r0 = this_lr
	r1 = this_fp
  	r1 = [r1-4] which is caller's fp
	if (r1!=0) 
		r1 = caller's lr
	call mcount_internal(this_lr, caller's_lr)
   }
*/  
#define MCOUNT								\
void _mcount (void)							\
{									\
  __asm__("stmdb	sp!, {r0, r1, r2, r3};"				\
	  "movs		fp, fp;"				      	\
	  "moveq	r0, #0;"					\
	  "ldrne	r0, [fp, $-4];"					\
	  "ldrne	r1, [fp, $-12];"				\
	  "movnes	r1, r1;"					\
	  "ldrne	r1, [r1, $-4];"					\
	  "movs		r1, r1;"					\
	  "blne		mcount_internal;"				\
	  "ldmia	sp!, {r0, r1, r2, r3}");			\
}
