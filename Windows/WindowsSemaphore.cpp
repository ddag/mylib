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

#include "WindowsSemaphore.h"
#include "Exception.h"

using namespace MyLib;

WindowsSemaphore::WindowsSemaphore(const string& semaphoreName, bool global) : semaphore(0), name(semaphoreName)
{
    if(global)
        semaphore = CreateSemaphore(0, 0, LONG_MAX, name.c_str());
    else
        semaphore = CreateSemaphore(0, 0, LONG_MAX, 0);
    if(semaphore == NULL)
        throw SemaphoreException(string("Unable to create semaphore ") + name, GetLastError());
}
WindowsSemaphore::~WindowsSemaphore()
{
    CloseHandle(semaphore);
}
void WindowsSemaphore::Wait()
{
    if(WaitForSingleObject(semaphore, INFINITE) == WAIT_FAILED)
        throw SemaphoreException(string("Wait on semaphore ") + name + string(" failed"), GetLastError());
}
void WindowsSemaphore::Signal()
{
    if(ReleaseSemaphore(semaphore, 1, 0) == 0 && GetLastError() != ERROR_TOO_MANY_POSTS)
        throw SemaphoreException(string("Signal of semaphore ") + name + string(" failed"), GetLastError());
}
