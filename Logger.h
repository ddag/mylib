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

#ifndef __MYLIB_LOGGER_H__
#define __MYLIB_LOGGER_H__

#include <string>
#include <fstream>
#include <memory>
#include "Mutex.h"
#include "Types.h"

using namespace std;

namespace MyLib
{
    class Logger
    {
    public:
        // error codes
        enum {EINIT = 1, EFILE};
    public:
        Logger();
        ~Logger();
        static const int MAX_LOG_SIZE = 100000; // ~100k, max space used ~ 200k (current + backup)
        void Open(const string& logFile, bool consoleOut = false, int maxLogSize = MAX_LOG_SIZE);
        void Close();
        void Write(const string& val, bool debug = false);
        void Write(const Dictionary& val, bool debug = false);
        void SetMaxLogSize(int newSize);
        void SetDebug(bool on);
    public:
        static Logger& Log; // public accessor to single instance
    private:
        void OpenLogFile();
        void BackupLogFile();
        void InternalClose();
    private:
        static ofstream logFile;
        static string logFileName;
        static size_t currLogSize;
        static size_t maxLogSize;
        static bool exists;
        static bool consoleOut;
        static bool debugOn;
        static auto_ptr<Mutex> lock;
    private:
        Logger(const Logger&);
        Logger& operator=(const Logger&);
    };
}

#endif

