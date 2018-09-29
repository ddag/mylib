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

#include "Logger.h"
#include "Exception.h"
#include <iostream>
#include <time.h>
#include <sys/stat.h>
#ifdef WIN32
#include <stdio.h> // unlink, rename
#else
#include <unistd.h>
#endif

using namespace MyLib;

ofstream Logger::logFile;
string Logger::logFileName;
size_t Logger::currLogSize = 0;
size_t Logger::maxLogSize = Logger::MAX_LOG_SIZE;
bool Logger::exists = false;
bool Logger::consoleOut = true;
bool Logger::debugOn = false;
auto_ptr<Mutex> Logger::lock;
// single instance
static Logger the_log;
Logger& Logger::Log = the_log;

Logger::Logger()
{
    if(exists)
        throw Exception(string("Logger already initialized."), EINIT, string("Initialized"));
    lock.reset(new Mutex("LoggerMutex"));
    exists = true;
}
Logger::~Logger()
{
    Close();
}
void Logger::Close()
{
    Mutex::Lock mlock(*lock);
    InternalClose();
}
void Logger::Open(const string& logFile, bool consoleOutput, int maxSize)
{
    Mutex::Lock mlock(*lock);
    logFileName = logFile;
    maxLogSize = maxSize;
    consoleOut = consoleOutput;
    OpenLogFile();
}
void Logger::Write(const string& val, bool debug)
{
    if(debug && debugOn == false)
        return;
    Mutex::Lock mlock(*lock);
    time_t t = time(NULL);
    struct tm* ts = localtime(&t);
    char tbuf[25];
    strftime(tbuf, 25, "[%Y-%m-%d %H:%M:%S] ", ts);
    if(logFile.is_open())
    {
        if(currLogSize >= maxLogSize)
        {
            BackupLogFile();
            OpenLogFile();
        }
        logFile << tbuf << val << endl;
        currLogSize += strlen(tbuf) + val.length() + 1 /* endl */;
    }
    if(consoleOut)
        cerr << tbuf << val << endl;
}
void Logger::Write(const Dictionary& val, bool debug)
{
    for (Dictionary::const_iterator i = val.begin();
        i != val.end();i++)
    {
        Write((*i).first + "=" + (*i).second, debug);
    }
}
void Logger::SetMaxLogSize(int newSize)
{
    Mutex::Lock mlock(*lock);
    maxLogSize = newSize;
}
void Logger::SetDebug(bool on)
{
    Mutex::Lock mlock(*lock);
    debugOn = on;
}
void Logger::OpenLogFile()
{
    InternalClose();
    logFile.open(logFileName.c_str(), ios_base::app);
    if(!logFile)
    {
        consoleOut = true;
        throw Exception(string("Unable to open log file: ") + logFileName, EFILE, string("File error"));
    }
    struct stat st;
    stat(logFileName.c_str(), &st);
    currLogSize = st.st_size;
}
void Logger::BackupLogFile()
{
    string backupFile(logFileName + ".backup");
    unlink(backupFile.c_str());
    rename(logFileName.c_str(), backupFile.c_str());
    unlink(logFileName.c_str());
}
void Logger::InternalClose()
{
    if(logFile.is_open())
        logFile.close();
}

