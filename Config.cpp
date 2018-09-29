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

#include "Config.h"
#include "Logger.h"
#include "Exception.h"
#include <fstream>
#include <sstream>

using namespace MyLib;

ifstream Config::cfgFile;
bool Config::exists = false;
auto_ptr<Mutex> Config::lock;
SectionDictionary Config::values;
// single instance
static Config cfg;
Config& Config::Cfg = cfg;

Config::Config()
{
    if(exists)
        throw Exception(string("Config already initialized."), EINIT, string("Initialized"));
    auto_ptr<Mutex> tmp(new Mutex("ConfigMutex"));
    lock = tmp;
    exists = true;
}
Config::~Config()
{
    CloseCfgFile();
}
void Config::Load(const string& file)
{
    Mutex::Lock mlock(*lock);
    CloseCfgFile();
    cfgFile.open(file.c_str(), ios_base::in);
    if(!cfgFile)
        throw Exception(string("Unable to open config file: ") + file, EFILE, string("File error"));
    string line;
    string section;
    string key;
    string value;
    char c;
    bool comment = false;
    bool isSection = false;
    bool isKey = false;
    while(cfgFile.get(c))
    {
        if(line.length() == 0 && (c == '\r' || c == '\n'))
            continue;
        line += c;
        if(comment)
        {
            if(c == '\n')
            {
                comment = false;
                line.clear();
            }
            continue;
        }
        if(c == '#' && line.length() == 1)
        {
            comment = true;
            continue;
        }
        if(isSection)
        {
            if(c == ']')
            {
                // insert empty map
                map<string, string> m;
                values[section] = m;
            }
            else
            if(c == '\n')
            {
                isSection = false;
                line.clear();
            }
            else
            if(c != '\r')
                section += c;
            continue;
        }
        if(c == '[' && line.length() == 1)
        {
            isSection = true;
            section.clear();
            continue;
        }
        if(isKey)
        {
            if(c == '=')
                isKey = false;
            else
            if(c != '\r' && c != '\n')
                key += c;
            continue;
        }
        if((c != '\r' && c != '\n') && line.length() == 1)
        {
            isKey = true;
            key += c;
            continue;
        }
        // must be value here
        if(c == '\r')
            continue;
        else
        if(c != '\n')
            value += c;
        else
        {
            values[section][key] = value;
            key.clear();
            value.clear();
            line.clear();
        }
    }
    cfgFile.close();
}
string Config::GetValue(const string& key)
{
    Mutex::Lock mlock(*lock);
    SectionDictionary::iterator i = values.find(string(""));
    if(i != values.end())
    {
        Dictionary& m = (*i).second;
        Dictionary::iterator j = m.find(key);
        if(j != m.end())
            return m[key];
    }
    // try environment
    char* val = getenv(key.c_str());
    if(val == 0)
        throw Exception(string("Config value for key ") + key + string(" not found"), EKEY, string("Key not found"));
    return string(val);
}
int Config::GetIntValue(const string& key)
{
    string s = GetValue(key);
    int i = 0;
    istringstream is(s);
    is >> i;
    return i;
}
double Config::GetDoubleValue(const string& key)
{
    string s = GetValue(key);
    double d = 0;
    istringstream is(s);
    is >> d;
    return d;
}
bool Config::GetBoolValue(const string& key)
{
    string s = GetValue(key);
    if(s == "true" || s == "1")
        return true;
    return false;
}
string Config::GetValue(const string& section, const string& key)
{
    Mutex::Lock mlock(*lock);
    SectionDictionary::iterator i = values.find(section);
    if(i != values.end())
    {
        Dictionary& m = (*i).second;
        Dictionary::iterator j = m.find(key);
        if(j != m.end())
            return m[key];
    }
    throw Exception(string("Config value for section ") + section + string(" and key ") + key + string(" not found"), ESKEY, string("Section key not found"));
}
int Config::GetIntValue(const string& section, const string& key)
{
    string s = GetValue(section, key);
    int i = 0;
    istringstream is(s);
    is >> i;
    return i;
}
double Config::GetDoubleValue(const string& section, const string& key)
{
    string s = GetValue(section, key);
    double d = 0;
    istringstream is(s);
    is >> d;
    return d;
}
bool Config::GetBoolValue(const string& section, const string& key)
{
    string s = GetValue(section, key);
    if(s == "true" || s == "1")
        return true;
    return false;
}
void Config::CloseCfgFile()
{
    if(cfgFile.is_open())
        cfgFile.close();
}

