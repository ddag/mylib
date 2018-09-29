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

#ifndef __MYLIB_FACTORY_H__
#define __MYLIB_FACTORY_H__

#include "MutexImp.h"
#include "SemaphoreImp.h"
#include "Thread.h" // for Runnable
#include "ThreadImp.h"
#include "ClientSocketImp.h"
#include "ServerSocketImp.h"
#include "DllLoaderImp.h"
#include "MemoryMappedFileImp.h"
#include "ProcessImp.h"
#include "Types.h"

namespace MyLib
{
    // NOT to be used in client code!!!
    class Factory
    {
    public:
        class Mutex
        {
        public:
            static MutexImp* GetInstance(const string& name, bool global);
        };
        class Semaphore
        {
        public:
            static SemaphoreImp* GetInstance(const string& name, bool global);
        };
        class Thread
        {
        public:
            // must use MyLib scope on Runnable since this class is also named Thread
            static ThreadImp* GetInstance(const string& name, MyLib::Thread::Runnable& r);
        };
        class ClientSocket
        {
        public:
            static ClientSocketImp* GetInstance(const string& name, const string& host, unsigned short port, bool secure);
            static ClientSocketImp* GetInstance(const string& name, const string& host, const string& service, bool secure);
        };
        class ServerSocket
        {
        public:
            static ServerSocketImp* GetInstance(const string& name, unsigned short port);
            static ServerSocketImp* GetInstance(const string& name, const string& service);
        };
        class DllLoader
        {
        public:
            static DllLoaderImp* GetInstance(const string& dll);
        };
        class MemoryMappedFile
        {
        public:
            static MemoryMappedFileImp* GetInstance(const string& file, char*& map, unsigned int mapSize);
        };
        class Process
        {
        public:
            static ProcessImp* GetInstance(const string& name, const string& exe, const StringArray& params);
        };
    protected: // instead of private, prevents gcc from complaining
        Factory();
        Factory(const Factory&);
        Factory& operator=(const Factory&);
    };
}

#endif

