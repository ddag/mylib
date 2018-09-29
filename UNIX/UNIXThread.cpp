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

#include "UNIXThread.h"
#include "Exception.h"
#include <errno.h>
#include <unistd.h> // sleep

using namespace MyLib;

UNIXThread::UNIXThread(const string& tname, Thread::Runnable& r) : name(tname), runnable(r), thread(0)
{
    if(pthread_create(&thread, 0, ThreadFunc, this) != 0)
        throw ThreadException(string("Unable to create thread ") + name, errno);
}
UNIXThread::~UNIXThread()
{
    runnable.Stop();
    pthread_join(thread, 0);
}
void UNIXThread::Sleep(int secs)
{
    ::sleep(secs);
}
void* UNIXThread::ThreadFunc(void* param)
{
    UNIXThread* pthis = static_cast<UNIXThread*>(param);
    pthis->runnable.Run();
    return 0;
}
