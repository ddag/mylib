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

#ifndef __MYLIB_BSDSOCKETCONSTS_H__
#define __MYLIB_BSDSOCKETCONSTS_H__

#ifndef WIN32
#ifndef SOCKET
typedef int SOCKET;
#endif
#ifndef SOCKADDR_IN
typedef struct sockaddr_in SOCKADDR_IN;
#endif
#ifndef LPSOCKADDR
typedef struct sockaddr* LPSOCKADDR;
#endif
#ifndef INVALID_SOCKET
const int INVALID_SOCKET = -1;
#endif
#ifndef INADDR_NONE
const unsigned long INADDR_NONE = 0xffffffff;
#endif
#ifndef SOCKET_ERROR
const int SOCKET_ERROR = -1;
#endif
#ifndef SD_BOTH
const int SD_BOTH = 2;
#endif
#ifndef socklen_t
#define socklen_t int
#endif
#define closesocket close
#include <errno.h>
#define SocketLastError() errno
#else
#include <winsock2.h>
#define SocketLastError() WSAGetLastError()
#endif

#endif

