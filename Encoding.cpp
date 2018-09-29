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

#include "Encoding.h"
#include <stdio.h>

extern "C"
{
#include "base64.h"
#include "urlcode.h"
}

using namespace MyLib;

string Encoding::ToHexString(const char* binaryData, int len)
{
    string tmp;
    for(int i = 0;i < len;i++)
    {
        char hval[2 + 1];
        sprintf(hval, "%02X", static_cast<unsigned char>(binaryData[i]));
        tmp += hval;
    }
    if(len)
        tmp += '\0';
    return tmp;
}
ByteArray Encoding::FromHexString(const string& hexData)
{
    ByteArray a;
    int ascLen = (hexData.length() - 1);
    int binLen = ascLen / 2;
    a.resize(binLen);
    int ch;
    for(int i = 0, j = 0;i < ascLen;i+=2)
    {
        sscanf(&hexData[i], "%02X", &ch);
        a[j++] = ch;
    }
    return a;
}
string Encoding::ToBase64(const char* binaryData, int len)
{
    int newLen = len * 2 + 1;
    char* buf = new char[newLen];
    int n = base64encode(reinterpret_cast<const unsigned char*>(binaryData), len, (unsigned char*) buf, newLen);
    buf[n] = 0;
    string ret(buf);
    delete [] buf;
    return ret;
}
ByteArray Encoding::FromBase64(const string& base64Data)
{
    ByteArray a;
    a.resize(base64Data.length()); // at least
    int n = base64decode(reinterpret_cast<const unsigned char*>(base64Data.c_str()), 
        base64Data.length(), reinterpret_cast<unsigned char*>(&a[0]), base64Data.length());
    a.resize(n);
    return a;
}
string Encoding::URLEncode(const char* data, int len)
{
    char* buf = new char[len * 3 + 1];
    urlencode(data, buf);
    string ret(buf);
    delete [] buf;
    return ret;
}
string Encoding::URLDecode(const char* data, int len)
{
    char* buf = new char[len];
    urldecode(data, buf);
    string ret(buf);
    delete [] buf;
    return ret;
}
