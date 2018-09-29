/**************************************************************************
Copyright (c) 2003 Donald Gobin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**************************************************************************/

#ifndef __MYLIB_FXR_TYPES_H__
#define __MYLIB_FXR_TYPES_H__

#include <stddef.h>

typedef unsigned int u_int;
typedef unsigned char u_int8_t;
typedef unsigned long u_int32_t;
typedef unsigned long long u_int64_t;
typedef char* caddr_t;

void bcopy(const void *src, void *dst, size_t len);
void bzero(void *b, size_t len);

#define LITTLE_ENDIAN   1234 /* byte 0 is least significant (i386) */
#define BIG_ENDIAN      4321 /* byte 0 is most significant (mc68k) */

#if   defined( __alpha__ ) || defined( __alpha ) || defined( i386 )       || \
      defined( __i386__ )  || defined( _M_I86 )  || defined( _M_IX86 )    || \
      defined( __OS2__ )   || defined( sun386 )  || defined( __TURBOC__ ) || \
      defined( vax )       || defined( vms )     || defined( VMS )        || \
      defined( __VMS )
#  define BYTE_ORDER LITTLE_ENDIAN

#elif defined( AMIGA )    || defined( applec )  || defined( __AS400__ )  || \
      defined( _CRAY )    || defined( __hppa )  || defined( __hp9000 )   || \
      defined( ibm370 )   || defined( mc68000 ) || defined( m68k )       || \
      defined( __MRC__ )  || defined( __MVS__ ) || defined( __MWERKS__ ) || \
      defined( sparc )    || defined( __sparc)  || defined( SYMANTEC_C ) || \
      defined( __TANDEM ) || defined( THINK_C ) || defined( __VMCMS__ )
#  define BYTE_ORDER BIG_ENDIAN
#endif

#endif

