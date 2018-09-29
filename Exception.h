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

#ifndef __MYLIB_EXCEPTION_H__
#define __MYLIB_EXCEPTION_H__

#include <string>
#include <stdexcept>

using namespace std;

namespace MyLib
{
    class Exception : public runtime_error
    {
    public:
        // no error code
        Exception(const string& msg);
        // for system exceptions
        Exception(const string& msg, unsigned long ec);
        // for user-defined/lib exceptions
        Exception(const string& msg, int ec, const string& emsg);
        Exception(const Exception& rhs);
        ~Exception() throw();
        Exception& operator=(const Exception& rhs);
        const char* what() const throw();
        unsigned long ErrorCode() const;
        const char* ErrorStr() const;
    private:
        void Copy(const Exception& rhs);
        void CreateMsg();
    private:
        string message;
        unsigned long errorCode;
        string errorStr;
    };
    // catch by type!
    class SocketException : public Exception
    {
    public:
        SocketException(const string& msg);
        SocketException(const string& msg, unsigned long ec);
    };
    class DllLoaderException : public Exception
    {
    public:
        DllLoaderException(const string& msg, unsigned long ec);
    };
    class MemoryMappedFileException : public Exception
    {
    public:
        MemoryMappedFileException(const string& msg, unsigned long ec);
    };
    class MutexException : public Exception
    {
    public:
        MutexException(const string& msg, unsigned long ec);
    };
    class SemaphoreException : public Exception
    {
    public:
        SemaphoreException(const string& msg, unsigned long ec);
    };
    class ThreadException : public Exception
    {
    public:
        ThreadException(const string& msg, unsigned long ec);
    };
    class PersistentHashException : public Exception
    {
    public:
        PersistentHashException(const string& msg);
        PersistentHashException(const string& msg, unsigned long ec);
        PersistentHashException(const string& msg, int ec, const string& emsg);
    };
    class PersistentQueueException : public Exception
    {
    public:
        PersistentQueueException(const string& msg);
        PersistentQueueException(const string& msg, unsigned long ec);
        PersistentQueueException(const string& msg, int ec, const string& emsg);
    };
    class SQLDatabaseException : public Exception
    {
    public:
        SQLDatabaseException(const string& msg);
        SQLDatabaseException(const string& msg, unsigned long ec);
        SQLDatabaseException(const string& msg, int ec, const string& emsg);
    };
    class EncryptionException : public Exception
    {
    public:
        EncryptionException(const string& msg);
    };
    class StringTokenizerException : public Exception
    {
    public:
        StringTokenizerException(const string& msg);
    };
    class ProcessException : public Exception
    {
    public:
        ProcessException(const string& msg, unsigned long ec);
    };
}

#endif

