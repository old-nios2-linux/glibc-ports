/* Machine-dependent ELF dynamic relocation inline functions.  m68k version.
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
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef dl_machine_h
#define dl_machine_h

#define ELF_MACHINE_NAME "m68k"

#include <sys/param.h>

#include <assert.h>

/* Return nonzero iff E_MACHINE is compatible with the running host.  */
static inline int
elf_machine_matches_host (Elf32_Half e_machine)
{
  switch (e_machine)
    {
    case EM_68K:
      return 1;
    default:
      return 0;
    }
}


/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT.  This must be inlined in a function which
   uses global data.  */
static inline Elf32_Addr
elf_machine_dynamic (void)
{
  register Elf32_Addr *got asm ("%a5");
  return *got;
}


/* Return the run-time load address of the shared object.  */
static inline Elf32_Addr
elf_machine_load_address (void)
{
  Elf32_Addr addr;
  asm ("lea _dl_start(%%pc), %0\n\t"
       "sub.l _dl_start@GOT.w(%%a5), %0"
       : "=a" (addr));
  return addr;
}


/* Set up the loaded object described by L so its unrelocated PLT
   entries will jump to the on-demand fixup code in dl-runtime.c.  */

static inline int
elf_machine_runtime_setup (struct link_map *l, int lazy, int profile)
{
  Elf32_Addr *got;
  extern void _dl_runtime_resolve (Elf32_Word);
  extern void _dl_runtime_profile (Elf32_Word);

  if (l->l_info[DT_JMPREL] && lazy)
    {
      /* The GOT entries for functions in the PLT have not yet been
	 filled in.  Their initial contents will arrange when called
	 to push an offset into the .rela.plt section, push
	 _GLOBAL_OFFSET_TABLE_[1], and then jump to
	 _GLOBAL_OFFSET_TABLE_[2].  */
      got = (Elf32_Addr *) (l->l_addr + l->l_info[DT_PLTGOT]->d_un.d_ptr);
      got[1] = (Elf32_Addr) l;	/* Identify this shared object.  */

      /* The got[2] entry contains the address of a function which gets
	 called to get the address of a so far unresolved function and
	 jump to it.  The profiling extension of the dynamic linker allows
	 to intercept the calls to collect information.  In this case we
	 don't store the address in the GOT so that all future calls also
	 end in this function.  */
      if (profile)
	{
	  got[2] = (Elf32_Addr) &_dl_runtime_profile;

	  if (_dl_name_match_p (_dl_profile, l))
	    {
	      /* This is the object we are looking for.  Say that we really
		 want profiling and the timers are started.  */
	      _dl_profile_map = l;
	    }
	}
      else
	/* This function will get called to fix up the GOT entry indicated by
	   the offset on the stack, and then jump to the resolved address.  */
	got[2] = (Elf32_Addr) &_dl_runtime_resolve;
    }

  return lazy;
}

/* This code is used in dl-runtime.c to call the `fixup' function
   and then redirect to the address it returns.  */
#define TRAMPOLINE_TEMPLATE(tramp_name, fixup_name) \
"| Trampoline for " #fixup_name "
	.globl " #tramp_name "
	.type " #tramp_name ", @function
" #tramp_name ":
	| Save %a0 (struct return address) and %a1.
	move.l %a0, -(%sp)
	move.l %a1, -(%sp)
	| Call the real address resolver.
	jbsr " #fixup_name "
	| Restore register %a0 and %a1.
	move.l (%sp)+, %a1
	move.l (%sp)+, %a0
	| Pop parameters
	addq.l #8, %sp
	| Call real function.
	jmp (%d0)
	.size " #tramp_name ", . - " #tramp_name "\n"
#ifndef PROF
#define ELF_MACHINE_RUNTIME_TRAMPOLINE \
asm (TRAMPOLINE_TEMPLATE (_dl_runtime_resolve, fixup) \
     TRAMPOLINE_TEMPLATE (_dl_runtime_profile, profile_fixup));
#else
#define ELF_MACHINE_RUNTIME_TRAMPOLINE \
asm (TRAMPOLINE_TEMPLATE (_dl_runtime_resolve, fixup) \
     ".globl _dl_runtime_profile\n" \
     ".set _dl_runtime_profile, _dl_runtime_resolve");
#endif
#define ELF_MACHINE_RUNTIME_FIXUP_ARGS long int save_a0, long int save_a1


/* Mask identifying addresses reserved for the user program,
   where the dynamic linker should not map anything.  */
#define ELF_MACHINE_USER_ADDRESS_MASK	0x80000000UL

/* Initial entry point code for the dynamic linker.
   The C function `_dl_start' is the real entry point;
   its return value is the user program's entry point.  */

#define RTLD_START asm ("\
	.text
	.globl _start
	.type _start,@function
_start:
	move.l %sp, -(%sp)
	jbsr _dl_start
	addq.l #4, %sp

	.globl _dl_start_user
	.type _dl_start_user,@function
_dl_start_user:
	| Save the user entry point address in %a4.
	move.l %d0, %a4
	| Point %a5 at the GOT.
	lea _GLOBAL_OFFSET_TABLE_@GOTPC(%pc), %a5
	| Remember the highest stack address.
	move.l %sp, ([__libc_stack_end@GOT.w, %a5])
	| See if we were run as a command with the executable file
	| name as an extra leading argument.
	move.l ([_dl_skip_args@GOT.w, %a5]), %d0
	jeq 0f
	| Pop the original argument count
	move.l (%sp)+, %d1
	| Subtract _dl_skip_args from it.
	sub.l %d0, %d1
	| Adjust the stack pointer to skip _dl_skip_args words.
	lea (%sp, %d0*4), %sp
	| Push back the modified argument count.
	move.l %d1, -(%sp)
0:	| Push the searchlist of the main object as argument in
	| the _dl_init_next call below.
	move.l ([_dl_main_searchlist@GOT.w, %a5]), %d2
0:	move.l %d2, -(%sp)
	| Call _dl_init_next to return the address of an initializer
	| function to run.
	bsr.l _dl_init_next@PLTPC
	add.l #4, %sp | Pop argument.
	| Check for zero return, when out of initializers.
	tst.l %d0
	jeq 1f
	| Call the shared object initializer function.
	| NOTE: We depend only on the registers (%d2, %a4 and %a5)
	| and the return address pushed by this call;
	| the initializer is called with the stack just
	| as it appears on entry, and it is free to move
	| the stack around, as long as it winds up jumping to
	| the return address on the top of the stack.
	move.l %d0, %a0
	jsr (%a0)
	| Loop to call _dl_init_next for the next initializer.
	jra 0b
1:	| Clear the startup flag.
	clr.l ([_dl_starting_up@GOT.w, %a5])
	| Pass our finalizer function to the user in %a1.
	move.l _dl_fini@GOT.w(%a5), %a1
	| Initialize %fp with the stack pointer.
	move.l %sp, %fp
	| Jump to the user's entry point.
	jmp (%a4)
	.size _dl_start_user, . - _dl_start_user
	.previous");

/* Nonzero iff TYPE describes a relocation that should
   skip the executable when looking up the symbol value.  */
#define elf_machine_lookup_noexec_p(type) ((type) == R_68K_COPY)

/* Nonzero iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.  */
#define elf_machine_lookup_noplt_p(type) ((type) == R_68K_JMP_SLOT)

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_MACHINE_JMP_SLOT	R_68K_JMP_SLOT

/* The m68k never uses Elf32_Rel relocations.  */
#define ELF_MACHINE_NO_REL 1

static inline void
elf_machine_fixup_plt (struct link_map *map, const Elf32_Rela *reloc,
		       Elf32_Addr *reloc_addr, Elf32_Addr value)
{
  *reloc_addr = value;
}

/* Return the final value of a plt relocation.  On the m68k the JMP_SLOT
   relocation ignores the addend.  */
static inline Elf32_Addr
elf_machine_plt_value (struct link_map *map, const Elf32_Rela *reloc,
		       Elf32_Addr value)
{
  return value;
}

#endif /* !dl_machine_h */

#ifdef RESOLVE

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */

static inline void
elf_machine_rela (struct link_map *map, const Elf32_Rela *reloc,
		  const Elf32_Sym *sym, const struct r_found_version *version,
		  Elf32_Addr *const reloc_addr)
{
  if (ELF32_R_TYPE (reloc->r_info) == R_68K_RELATIVE)
    *reloc_addr = map->l_addr + reloc->r_addend;
  else
    {
      const Elf32_Sym *const refsym = sym;
      Elf32_Addr value = RESOLVE (&sym, version, ELF32_R_TYPE (reloc->r_info));
      if (sym)
	value += sym->st_value;

      switch (ELF32_R_TYPE (reloc->r_info))
	{
	case R_68K_COPY:
	  if (sym == NULL)
	    /* This can happen in trace mode if an object could not be
	       found.  */
	    break;
	  if (sym->st_size > refsym->st_size
	      || (_dl_verbose && sym->st_size < refsym->st_size))
	    {
	      extern char **_dl_argv;
	      const char *strtab;

	      strtab = ((void *) map->l_addr
			+ map->l_info[DT_STRTAB]->d_un.d_ptr);
	      _dl_sysdep_error (_dl_argv[0] ?: "<program name unknown>",
				": Symbol `", strtab + refsym->st_name,
				"' has different size in shared object, "
				"consider re-linking\n", NULL);
	    }
	  memcpy (reloc_addr, (void *) value, MIN (sym->st_size,
						   refsym->st_size));
	  break;
	case R_68K_GLOB_DAT:
	case R_68K_JMP_SLOT:
	  *reloc_addr = value;
	  break;
	case R_68K_8:
	  *(char *) reloc_addr = value + reloc->r_addend;
	  break;
	case R_68K_16:
	  *(short *) reloc_addr = value + reloc->r_addend;
	  break;
	case R_68K_32:
	  *reloc_addr = value + reloc->r_addend;
	  break;
	case R_68K_PC8:
	  *(char *) reloc_addr
	    = value + reloc->r_addend - (Elf32_Addr) reloc_addr;
	  break;
	case R_68K_PC16:
	  *(short *) reloc_addr
	    = value + reloc->r_addend - (Elf32_Addr) reloc_addr;
	  break;
	case R_68K_PC32:
	  *reloc_addr = value + reloc->r_addend - (Elf32_Addr) reloc_addr;
	  break;
	case R_68K_NONE:		/* Alright, Wilbur.  */
	  break;
	default:
	  assert (! "unexpected dynamic reloc type");
	  break;
	}
    }
}

static inline void
elf_machine_lazy_rel (Elf32_Addr l_addr, const Elf32_Rela *reloc)
{
  Elf32_Addr *const reloc_addr = (void *) (l_addr + reloc->r_offset);
  assert (ELF32_R_TYPE (reloc->r_info) == R_68K_JMP_SLOT);
  *reloc_addr += l_addr;
}

#endif /* RESOLVE */
