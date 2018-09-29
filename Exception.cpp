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

#include "Exception.h"
#ifdef WIN32
#include <windows.h>
#endif
#include "ToString.h"

using namespace MyLib;

Exception::Exception(const string& msg) : runtime_error(msg),
    message(msg)
{
}
Exception::Exception(const string& msg, unsigned long ec) : runtime_error(msg), 
    message(msg), errorCode(ec)
{
#ifdef WIN32
    LPVOID lpMsgBuf;
    if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, 
        0, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf, 0, NULL))
    {
        errorStr = (LPCTSTR) lpMsgBuf;
        LocalFree(lpMsgBuf);
    }
    else
        errorStr = "N/A";
#else
    errorStr = strerror(errorCode);
#endif
    CreateMsg();
}
Exception::Exception(const string& msg, int ec, const string& emsg) : runtime_error(msg),
    message(msg), errorCode(ec), errorStr(emsg)
{
    CreateMsg();
}
Exception::Exception(const Exception& rhs) : runtime_error(rhs.message)
{
    Copy(rhs);
}
Exception::~Exception() throw()
{
}
Exception& Exception::operator=(const Exception& rhs)
{
    Copy(rhs);
    return *this;
}
const char* Exception::what() const throw()
{
    return message.c_str();
}
unsigned long Exception::ErrorCode() const
{
    return errorCode;
}
const char* Exception::ErrorStr() const 
{
    return errorStr.c_str();
}
void Exception::Copy(const Exception& rhs)
{
    message = rhs.message;
    errorCode = rhs.errorCode;
    errorStr = rhs.errorStr;
}
void Exception::CreateMsg()
{
    message += string(" | [More Info] Error code: ") + errorCode + string(", ") + errorStr;
}
// types
SocketException::SocketException(const string& msg) : Exception(msg) 
{
}
SocketException::SocketException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
DllLoaderException::DllLoaderException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
MemoryMappedFileException::MemoryMappedFileException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
MutexException::MutexException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
SemaphoreException::SemaphoreException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
ThreadException::ThreadException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
PersistentHashException::PersistentHashException(const string& msg) : Exception(msg) 
{
}
PersistentHashException::PersistentHashException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
PersistentHashException::PersistentHashException(const string& msg, int ec, const string& emsg) : Exception(msg, ec, emsg)
{
}
PersistentQueueException::PersistentQueueException(const string& msg) : Exception(msg) 
{
}
PersistentQueueException::PersistentQueueException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
PersistentQueueException::PersistentQueueException(const string& msg, int ec, const string& emsg) : Exception(msg, ec, emsg)
{
}
SQLDatabaseException::SQLDatabaseException(const string& msg) : Exception(msg) 
{
}
SQLDatabaseException::SQLDatabaseException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}
SQLDatabaseException::SQLDatabaseException(const string& msg, int ec, const string& emsg) : Exception(msg, ec, emsg)
{
}
EncryptionException::EncryptionException(const string& msg) : Exception(msg) 
{
}
StringTokenizerException::StringTokenizerException(const string& msg) : Exception(msg) 
{
}
ProcessException::ProcessException(const string& msg, unsigned long ec) : Exception(msg, ec) 
{
}

