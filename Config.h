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

#ifndef __MYLIB_CONFIG_H__
#define __MYLIB_CONFIG_H__

#include <memory>
#include <string>
#include <map>
#include "Mutex.h"
#include "Types.h"

using namespace std;

namespace MyLib
{
    typedef map<string, Dictionary> SectionDictionary;

    class Config
    {
    public:
        // error codes
        enum {EINIT = 1, EFILE, EKEY, ESKEY};
    public:
        Config();
        ~Config();
        void Load(const string& cfgFile);
        string GetValue(const string& key);
        int GetIntValue(const string& key);
        double GetDoubleValue(const string& key);
        bool GetBoolValue(const string& key);
        string GetValue(const string& section, const string& key);
        int GetIntValue(const string& section, const string& key);
        double GetDoubleValue(const string& section, const string& key);
        bool GetBoolValue(const string& section, const string& key);
    public:
        static Config& Cfg; // public accessor to single instance
    private:
        void CloseCfgFile();
    private:
        static ifstream cfgFile;
        static bool exists;
        static auto_ptr<Mutex> lock;
        // section -> (key, val)
        static SectionDictionary values;
    private:
        Config(const Config&);
        Config& operator=(const Config&);
    };
}

#endif

