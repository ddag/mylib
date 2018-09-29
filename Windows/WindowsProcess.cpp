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

#include "WindowsProcess.h"
#include "Exception.h"

using namespace MyLib;

WindowsProcess::WindowsProcess(const string& pname, const string& exe, const StringArray& params) : name(pname)
{
    SECURITY_ATTRIBUTES saAttr;
    
    // Set the bInheritHandle flag so pipe handles are inherited. 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    // create pipe for child process stdin
    HANDLE wr;
    if(CreatePipe(&childRd, &wr, &saAttr, 0) == 0)
        throw ProcessException(string("Unable to create stdin pipe for process ") + name, GetLastError());

    // duplicate the write handle to the pipe in order to
    // make sure it's not inherited and close it
    if(DuplicateHandle(GetCurrentProcess(), wr, 
        GetCurrentProcess(), &parentWr, 0, 
        FALSE,                  // not inherited 
        DUPLICATE_SAME_ACCESS) == FALSE)
        throw ProcessException(string("Unable to duplicate stdin write pipe for process ") + name, GetLastError());
    CloseHandle(wr); 

    // create pipe for child process stdout
    HANDLE rd;
    if(CreatePipe(&rd, &childWr, &saAttr, 0) == 0)
        throw ProcessException(string("Unable to create stdout pipe for process ") + name, GetLastError());

    // duplicate the read handle to the pipe in order to
    // make sure it's not inherited and close it
    if(DuplicateHandle(GetCurrentProcess(), rd, 
        GetCurrentProcess(), &parentRd, 0, 
        FALSE,                  // not inherited 
        DUPLICATE_SAME_ACCESS) == FALSE)
        throw ProcessException(string("Unable to duplicate stdout read pipe for process ") + name, GetLastError());
    CloseHandle(rd); 

    // Set up members of the PROCESS_INFORMATION structure. 
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

    siStartInfo.cb = sizeof(STARTUPINFO); 
    siStartInfo.hStdError = childWr;
    siStartInfo.hStdOutput = childWr;
    siStartInfo.hStdInput = childRd;
    siStartInfo.dwFlags = STARTF_USESTDHANDLES;

    // create command line
    string cmdLine = exe;
    for(int i = 0;i < static_cast<int>(params.size());i++)
        cmdLine += string(" ") + params[i];

    // create child process
    BOOL ret = CreateProcess(NULL, 
        reinterpret_cast<LPSTR>(const_cast<char*>(cmdLine.c_str())),    // cmd line
        NULL,                       // process security attributes 
        NULL,                       // primary thread security attributes 
        TRUE,                       // handles are inherited 
        0,                          // creation flags 
        NULL,                       // use parent's environment 
        NULL,                       // use parent's current directory 
        &siStartInfo,               // STARTUPINFO pointer 
        &piProcInfo);               // receives PROCESS_INFORMATION 

    if(ret == FALSE)
        throw ProcessException(string("Failed to create child process ") + name, GetLastError());

    // we need to close our end to receive output
    CloseHandle(childRd);
    childRd = 0;
    CloseHandle(childWr);
    childWr = 0;
}
WindowsProcess::~WindowsProcess()
{
    // in case there was an exception
    if(childRd)
        CloseHandle(childRd);
    if(childWr)
        CloseHandle(childWr);
    // close others
    CloseHandle(parentRd);
    CloseHandle(parentWr);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
}
void WindowsProcess::WaitForCompletion(int timeout)
{
    if(timeout)
    {
        if(WaitForSingleObject(piProcInfo.hProcess, timeout * 1000) == WAIT_TIMEOUT)
            TerminateProcess(piProcInfo.hProcess, 0);
    }
    else
        WaitForSingleObject(piProcInfo.hProcess, INFINITE);
}
bool WindowsProcess::ReadChar(char& c)
{
    DWORD dwRead = 0;
    c = 0;
    bool ret = true;
    if(ReadFile(parentRd, reinterpret_cast<LPVOID>(&c), 1, &dwRead, NULL) == FALSE)
    {
        DWORD ec = GetLastError();
        if(ec != ERROR_BROKEN_PIPE)
            throw ProcessException(string("Read failed from process ") + name, ec);
        else
            ret = false;
    }
    else
    if(dwRead != 1)
        ret = false;
    return ret;
}
bool WindowsProcess::Read(string& data, int nBytes)
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
bool WindowsProcess::ReadLine(string& data)
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
bool WindowsProcess::Write(const string& data)
{
    DWORD dwWritten = 0;
    bool ret = true;
    if(WriteFile(parentWr, data.data(), data.length(), &dwWritten, NULL) == FALSE)
    {
        DWORD ec = GetLastError();
        if(ec != ERROR_BROKEN_PIPE)
            throw ProcessException(string("Write failed to process ") + name, ec);
        else
            ret = false;
    }
    else
    if(dwWritten == 0)
        ret = false;
    return ret;
}

