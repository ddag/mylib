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

#ifndef __MYLIB_WINDOWSPROCESS_H__
#define __MYLIB_WINDOWSPROCESS_H__

#include "ProcessImp.h"
#include "Types.h"
#include <windows.h>

namespace MyLib
{
    class WindowsProcess : public ProcessImp
    {
    public:
        WindowsProcess(const string& name, const string& exe, const StringArray& params);
        ~WindowsProcess();
        void WaitForCompletion(int timeout);
        // read a single character from process stdout
        bool ReadChar(char& c);
        // read nBytes from process stdout
        bool Read(string& data, int nBytes);
        // read from process stdout
        bool ReadLine(string& data);
        // write to process stdin
        bool Write(const string& data);
    private:
        string name;
        HANDLE childRd;
        HANDLE childWr;
        HANDLE parentRd;
        HANDLE parentWr;
        PROCESS_INFORMATION piProcInfo; 
    };
}

#endif

