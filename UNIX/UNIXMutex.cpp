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

#include "UNIXMutex.h"
#include "Exception.h"
#include <errno.h>
#include <pthread.h>
#include <sys/mman.h> // mmap
#include <sys/types.h> // open
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close
#include <iostream>

using namespace MyLib;

UNIXMutex::UNIXMutex(const string& mutexName, bool globalMutex) : mutex(0), name(mutexName), global(globalMutex)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    if(global)
    {
        mutex = (pthread_mutex_t*) mmap(0, sizeof(pthread_mutex_t),
            PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        if(mutex == 0)
            throw MutexException(string("Unable to create mmap for mutex ") + name, errno);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    }
    else
        mutex = new pthread_mutex_t();
    if(pthread_mutex_init(mutex, &attr) < 0)
    {
        if(global == false)
            delete mutex;
        throw MutexException(string("Unable to create mutex ") + name, errno);
    }
}
UNIXMutex::~UNIXMutex()
{
    pthread_mutex_destroy(mutex);
    if(global == false)
        delete mutex;
}
void UNIXMutex::Acquire()
{
    pthread_mutex_lock(mutex);
}
void UNIXMutex::Release()
{
    pthread_mutex_unlock(mutex);
}

