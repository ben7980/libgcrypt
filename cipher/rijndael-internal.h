/* Rijndael (AES) for GnuPG
 * Copyright (C) 2000, 2001, 2002, 2003, 2007,
 *               2008, 2011, 2012 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef G10_RIJNDAEL_INTERNAL_H
#define G10_RIJNDAEL_INTERNAL_H

#include "types.h"  /* for byte and u32 typedefs */


#define MAXKC                   (256/32)
#define MAXROUNDS               14
#define BLOCKSIZE               (128/8)


/* Helper macro to force alignment to 16 bytes.  */
#ifdef HAVE_GCC_ATTRIBUTE_ALIGNED
# define ATTR_ALIGNED_16  __attribute__ ((aligned (16)))
#else
# define ATTR_ALIGNED_16
#endif


/* USE_AMD64_ASM indicates whether to use AMD64 assembly code. */
#undef USE_AMD64_ASM
#if defined(__x86_64__) && defined(HAVE_COMPATIBLE_GCC_AMD64_PLATFORM_AS)
# define USE_AMD64_ASM 1
#endif

/* USE_ARM_ASM indicates whether to use ARM assembly code. */
#undef USE_ARM_ASM
#if defined(__ARMEL__)
# ifdef HAVE_COMPATIBLE_GCC_ARM_PLATFORM_AS
#  define USE_ARM_ASM 1
# endif
#endif

/* USE_PADLOCK indicates whether to compile the padlock specific
   code.  */
#undef USE_PADLOCK
#ifdef ENABLE_PADLOCK_SUPPORT
# ifdef HAVE_GCC_ATTRIBUTE_ALIGNED
#  if (defined (__i386__) && SIZEOF_UNSIGNED_LONG == 4) || defined(__x86_64__)
#   define USE_PADLOCK 1
#  endif
# endif
#endif /*ENABLE_PADLOCK_SUPPORT*/

/* USE_AESNI inidicates whether to compile with Intel AES-NI code.  We
   need the vector-size attribute which seems to be available since
   gcc 3.  However, to be on the safe side we require at least gcc 4.  */
#undef USE_AESNI
#ifdef ENABLE_AESNI_SUPPORT
# if ((defined (__i386__) && SIZEOF_UNSIGNED_LONG == 4) || defined(__x86_64__))
#  if __GNUC__ >= 4
#   define USE_AESNI 1
#  endif
# endif
#endif /* ENABLE_AESNI_SUPPORT */


/* Our context object.  */
typedef struct
{
  /* The first fields are the keyschedule arrays.  This is so that
     they are aligned on a 16 byte boundary if using gcc.  This
     alignment is required for the AES-NI code and a good idea in any
     case.  The alignment is guaranteed due to the way cipher.c
     allocates the space for the context.  The PROPERLY_ALIGNED_TYPE
     hack is used to force a minimal alignment if not using gcc of if
     the alignment requirement is higher that 16 bytes.  */
  union
  {
    PROPERLY_ALIGNED_TYPE dummy;
    byte keyschedule[MAXROUNDS+1][4][4];
#ifdef USE_PADLOCK
    /* The key as passed to the padlock engine.  It is only used if
       the padlock engine is used (USE_PADLOCK, below).  */
    unsigned char padlock_key[16] __attribute__ ((aligned (16)));
#endif /*USE_PADLOCK*/
  } u1;
  union
  {
    PROPERLY_ALIGNED_TYPE dummy;
    byte keyschedule[MAXROUNDS+1][4][4];
  } u2;
  int rounds;                /* Key-length-dependent number of rounds.  */
  unsigned int decryption_prepared:1; /* The decryption key schedule is available.  */
#ifdef USE_PADLOCK
  unsigned int use_padlock:1;         /* Padlock shall be used.  */
#endif /*USE_PADLOCK*/
#ifdef USE_AESNI
  unsigned int use_aesni:1;           /* AES-NI shall be used.  */
#endif /*USE_AESNI*/
} RIJNDAEL_context ATTR_ALIGNED_16;

/* Macros defining alias for the keyschedules.  */
#define keyschenc  u1.keyschedule
#define keyschdec  u2.keyschedule
#define padlockkey u1.padlock_key

#endif /* G10_RIJNDAEL_INTERNAL_H */
