/* `sysconf', `pathconf', and `confstr' NAME values.  Generic version.
Copyright (C) 1993 Free Software Foundation, Inc.
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

/* Values for the NAME argument to `pathconf' and `fpathconf'.  */
#define _PC_LINK_MAX		0
#define _PC_MAX_CANON		1
#define _PC_MAX_INPUT		2
#define _PC_NAME_MAX		3
#define _PC_PATH_MAX		4
#define _PC_PIPE_BUF		5
#define _PC_CHOWN_RESTRICTED	6
#define _PC_NO_TRUNC		7
#define _PC_VDISABLE		8

/* Values for the argument to `sysconf'.  */
#define _SC_ARG_MAX		0
#define _SC_CHILD_MAX		1
#define _SC_CLK_TCK		2
#define _SC_NGROUPS_MAX		3
#define _SC_OPEN_MAX		4
#define _SC_JOB_CONTROL		5
#define _SC_SAVED_IDS		6
#define _SC_VERSION		7
#define _SC_PASS_MAX		8
#define _SC_XOPEN_VERSION	9

#ifdef __USE_POSIX2
/* Values for the NAME argument to `confstr'.  */
enum
  {
    _CS_PATH			/* The default search path.  */
  };
#endif
