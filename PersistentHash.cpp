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

#include "PersistentHash.h"
#include "Exception.h"

using namespace MyLib;

PersistentHash::PersistentHash(const string& dir) : dbDir(dir)
{
    dbHandle = cropen(dbDir.c_str(), CR_OWRITER|CR_OREADER|CR_OCREAT, -1, -1);
    if(dbHandle == 0)
        throw PersistentHashException(string("Unable to create persistent hash in dir ") + dbDir, dpecode, dperrmsg(dpecode));
}
PersistentHash::~PersistentHash()
{
    crclose(dbHandle);
}
void PersistentHash::Store(const string& key, const string& value)
{
    if(crput(dbHandle, key.data(), key.length(), value.data(), value.length(), CR_DOVER) == 0)
        throw PersistentHashException(string("store of key (") + key + 
            string(") and value (") + value + 
            string(") into persisent hash ") +
            dbDir + string(" failed"), 
            dpecode, dperrmsg(dpecode));
}
bool PersistentHash::Retrieve(const string& key, string& value)
{
    bool ret = false;
    char* val;
    int valueSize;
    if((val = crget(dbHandle, key.data(), key.length(), 0, -1, &valueSize)))
    {
        value.clear();
        value.append(val, valueSize);
        ret = true;
    }
    return ret;
}
bool PersistentHash::Delete(const string& key)
{
    bool ret = false;
    if(crout(dbHandle, key.data(), key.length()))
    {
        // keep hash optimized
        // deletes are not so common
        // as stores/retrieves
        Compact();
        ret = true;
    }
    return ret;
}
void PersistentHash::DeleteHash(const string& dbDir)
{
    crremove(dbDir.c_str());
}
void PersistentHash::Compact()
{
    croptimize(dbHandle, 0);
}

