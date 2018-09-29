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

#include "Factory.h"
#ifdef WIN32
// disable include of old Winsock 1.1 header automatically
// included as part of including windows.h -- sigh
#define _WINSOCKAPI_
#include "WindowsMutex.h"
#include "WindowsSemaphore.h"
#include "WindowsThread.h"
#include "WindowsDllLoader.h"
#include "WindowsMemoryMappedFile.h"
#include "WindowsProcess.h"
#else
#include "UNIXMutex.h"
#include "UNIXSemaphore.h"
#include "UNIXThread.h"
#include "UNIXDllLoader.h"
#include "UNIXMemoryMappedFile.h"
#include "UNIXProcess.h"
#endif
#include "BSDClientSocket.h"
#include "BSDServerSocket.h"

using namespace MyLib;

MutexImp* Factory::Mutex::GetInstance(const string& name, bool global)
{
#ifdef WIN32
    return new WindowsMutex(name, global);
#else
    return new UNIXMutex(name, global);
#endif
}
SemaphoreImp* Factory::Semaphore::GetInstance(const string& name, bool global)
{
#ifdef WIN32
    return new WindowsSemaphore(name, global);
#else
    return new UNIXSemaphore(name, global);
#endif
}
ThreadImp* Factory::Thread::GetInstance(const string& name, MyLib::Thread::Runnable& r)
{
#ifdef WIN32
    return new WindowsThread(name, r);
#else
    return new UNIXThread(name, r);
#endif
}
ClientSocketImp* Factory::ClientSocket::GetInstance(const string& name, const string& host, unsigned short port, bool secure)
{
    return new BSDClientSocket(name, host, port, secure);
}
ClientSocketImp* Factory::ClientSocket::GetInstance(const string& name, const string& host, const string& service, bool secure)
{
    return new BSDClientSocket(name, host, service, secure);
}
ServerSocketImp* Factory::ServerSocket::GetInstance(const string& name, unsigned short port)
{
    return new BSDServerSocket(name, port);
}
ServerSocketImp* Factory::ServerSocket::GetInstance(const string& name, const string& service)
{
    return new BSDServerSocket(name, service);
}
DllLoaderImp* Factory::DllLoader::GetInstance(const string& dll)
{
#ifdef WIN32
    return new WindowsDllLoader(dll);
#else
    return new UNIXDllLoader(dll);
#endif
}
MemoryMappedFileImp* Factory::MemoryMappedFile::GetInstance(const string& file, char*& map, unsigned int mapSize)
{
#ifdef WIN32
    return new WindowsMemoryMappedFile(file, map, mapSize);
#else
    return new UNIXMemoryMappedFile(file, map, mapSize);
#endif
}
ProcessImp* Factory::Process::GetInstance(const string& name, const string& exe, const StringArray& params)
{
#ifdef WIN32
    return new WindowsProcess(name, exe, params);
#else
    return new UNIXProcess(name, exe, params);
#endif
}
