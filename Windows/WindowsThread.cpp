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

#include "WindowsThread.h"
#include "Logger.h"
#include "Exception.h"

using namespace MyLib;

WindowsThread::WindowsThread(const string& tname, Thread::Runnable& r) : name(tname), runnable(r), thread(0)
{
    DWORD threadId;
    thread = CreateThread(0, 0, ThreadFunc, this, 0, &threadId);
    if(thread == 0)
        throw ThreadException(string("Unable to create thread ") + name, GetLastError());
}
WindowsThread::~WindowsThread()
{
    runnable.Stop();
    if(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0)
        CloseHandle(thread);
    else
        Logger::Log.Write(string("Waiting for thread ") + name + string(" to finish failed."));
}
void WindowsThread::Sleep(int secs)
{
    ::Sleep(secs * 1000); // windows Sleep() is in millisecs
}
DWORD WINAPI WindowsThread::ThreadFunc(LPVOID param)
{
    WindowsThread* pthis = reinterpret_cast<WindowsThread*>(param);
    pthis->runnable.Run();
    return 0;
}
