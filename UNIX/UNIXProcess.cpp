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

#include "UNIXProcess.h"
#include "Exception.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

using namespace MyLib;

UNIXProcess::UNIXProcess(const string& pname, const string& exe, const StringArray& params) : name(pname), args(0)
{
    // create read/write pipes
    if(pipe(readPipe) < 0)
        throw ProcessException(string("Failed to create read pipe ") + name, errno);
    if(pipe(writePipe) < 0)
        throw ProcessException(string("Failed to create write pipe ") + name, errno);
    // create child process
    pid = fork();
    if(pid < 0)
        throw ProcessException(string("Failed to create child process ") + name, errno);
    if(pid)
    {
        // parent running here
        // close pipe ends we're not using
        close(readPipe[0]);
        close(writePipe[1]);
        parentRd = writePipe[0];
        parentWr = readPipe[1];
    }
    else
    {
        // child running here
        // replace stdin/stdout with pipes and close stdin/stdout in the process
        if(dup2(readPipe[0], 0) < 0)
            throw ProcessException(string("Failed to duplicate stdin ") + name, errno);
        if(dup2(writePipe[1], 1) < 0)
            throw ProcessException(string("Failed to duplicate stdout ") + name, errno);
        // run specified image
        // create command line
        if(params.size())
        {
            args = new char*[params.size() + 1];
            for(int i = 0;i < params.size();i++)
                args[i] = &params[i].c_str();
            // last param must be null
            args[params.size()] = 0;
        }
        if(execvp(exe.c_str(), args) < 0)
            throw ProcessException(string("Failed to run executable ") + name, errno);
        if(args)
        {
            delete [] args;
            args = 0;
        }
    }
}
UNIXProcess::~UNIXProcess()
{
    // in case of exception
    if(args)
        delete [] args;
    // close descriptors
    close(readPipe[0]);
    close(readPipe[1]);
    close(writePipe[0]);
    close(writePipe[1]);
}
void UNIXProcess::WaitForCompletion(int timeout)
{
    if(timeout)
    {
        if(waitpid(pid, 0, WNOHANG) == 0)
        {
            // wait
            sleep(timeout);
            // try again
            if(waitpid(pid, 0, WNOHANG) == 0)
                kill(pid, SIGTERM);
        }
    }
    else
        waitpid(pid, 0, 0);
}
bool UNIXProcess::ReadChar(char& c)
{
    int nRead = 0;
    c = 0;
    bool ret = true;
    if((nRead = read(parentRd, reinterpret_cast<void*>(&c), 1)) < 0)
        throw ProcessException(string("Read failed from process ") + name, errno);
    else
    if(nRead == 0)
        ret = false; // eof
    return ret;
}
bool UNIXProcess::Read(string& data, int nBytes)
{
    char c;
    bool ret = true;
    data.resize(0);
    for(int i = 0;i < nBytes;i++)
    {
        if(ret = ReadChar(c))
            data += c;
        else
            break;
    }
    return ret;
}
bool UNIXProcess::ReadLine(string& data)
{
    char c;
    data.resize(0);
    bool ret = true;
    do
    {
        if((ret = ReadChar(c)) && c != '\r' && c != '\n')
            data += c;
        else
            break;
    }
    while(c != '\n');
    return ret;
}
bool UNIXProcess::Write(const string& data)
{
    int nWritten = 0;
    bool ret = true;
    if((nWritten = write(parentWr, data.data(), data.length())) < 0)
        throw ProcessException(string("Write failed to process ") + name, errno);
    else
    if(nWritten == 0)
        ret = false;
    return ret;
}

