/* Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
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

#include <sys/types.h>
#include <errno.h>
#include <endian.h>
#include <unistd.h>

#include <sysdep.h>
#include <sys/syscall.h>

#include "kernel-features.h"

#ifdef __NR_ftruncate64
#ifndef __ASSUME_TRUNCATE64_SYSCALL
/* The variable is shared between all wrappers around *truncate64 calls.  */
extern int __have_no_truncate64;
#endif

/* The order of hight, low depends on endianness.  */
extern int __syscall_ftruncate64 (int fd, int dummy, int high_length,
				  int low_length);


/* Truncate the file FD refers to to LENGTH bytes.  */
int
ftruncate64 (int fd, off64_t length)
{
#ifndef __ASSUME_TRUNCATE64_SYSCALL
  if (! __have_no_truncate64)
#endif
    {
      unsigned int low = length & 0xffffffff;
      unsigned int high = length >> 32;
#ifndef __ASSUME_TRUNCATE64_SYSCALL
      int saved_errno = errno;
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
      /* Use a fill argument to pass low, high in an aligned pair of
         arguments (here 2/3).  */
      int result = INLINE_SYSCALL (ftruncate64, 3, fd, 0, low, high);
#elif __BYTE_ORDER == __BIG_ENDIAN
      int result = INLINE_SYSCALL (ftruncate64, 3, fd, 0, high, low);
#endif
#ifndef __ASSUME_TRUNCATE64_SYSCALL
      if (result != -1 || errno != ENOSYS)
#endif
	return result;

#ifndef __ASSUME_TRUNCATE64_SYSCALL
      __set_errno (saved_errno);
      __have_no_truncate64 = 1;
#endif
    }

#ifndef __ASSUME_TRUNCATE64_SYSCALL
  if ((off_t) length != length)
    {
      __set_errno (EINVAL);
      return -1;
    }
  return __ftruncate (fd, (off_t) length);
#endif
}

#else
/* Use the generic implementation.  */
# include <sysdeps/generic/ftruncate64.c>
#endif
