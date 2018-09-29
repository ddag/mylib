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

#include "WindowsMutex.h"
#include "Exception.h"

using namespace MyLib;

WindowsMutex::WindowsMutex(const string& mutexName, bool global) : mutex(0), name(mutexName)
{
    if(global)
        mutex = CreateMutex(0, FALSE, name.c_str());
    else
        mutex = CreateMutex(0, FALSE, 0);
    if(mutex == NULL)
        throw MutexException(string("Unable to create mutex ") + name, GetLastError());
}
WindowsMutex::~WindowsMutex()
{
    CloseHandle(mutex);
}
void WindowsMutex::Acquire()
{
    if(WaitForSingleObject(mutex, INFINITE) == WAIT_FAILED)
        throw MutexException(string("Wait on mutex ") + name + string(" failed"), GetLastError());
}
void WindowsMutex::Release()
{
    if(ReleaseMutex(mutex) == 0)
        throw MutexException(string("Release of mutex ") + name + string(" failed"), GetLastError());
}

