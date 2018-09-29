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

#include "PersistentQueue.h"
#include "Exception.h"

using namespace MyLib;

PersistentQueue::PersistentQueue(const string& qDir) : queueDir(qDir)
{
    queueHandle = vlopen(queueDir.c_str(), VL_OWRITER|VL_OREADER|VL_OCREAT, VL_CMPLEX);
    if(queueHandle == 0)
        throw PersistentQueueException(string("Unable to create persistent queue in dir ") + queueDir, dpecode, dperrmsg(dpecode));
    vlsettuning(queueHandle, 111, 192, 1024, 128);
}
PersistentQueue::~PersistentQueue()
{
    vlclose(queueHandle);
}
void PersistentQueue::Enqueue(const string& key, const string& value, int priority)
{
    // create key, priority|key
    char* k = (char*) malloc(key.length() + sizeof(int));
    if(k == 0)
        throw PersistentQueueException(string("store of item (") + value + 
            string(") into persisent queue ") + queueDir + 
            string(" failed -- Memory allocation failure!"));
    memcpy(k, &priority, sizeof(int));
    memcpy(&k[sizeof(int)], key.data(), key.length());
    if(vlput(queueHandle, k, key.length() + sizeof(int), value.data(), 
        value.length(), VL_DDUP) == 0)
    {
        free(k);
        throw PersistentQueueException(string("store of item (") + value + 
            string(") into persisent queue ") + queueDir + 
            string(" failed"), 
            dpecode, dperrmsg(dpecode));
    }
    free(k);
}
bool PersistentQueue::Dequeue(string& value)
{
    char *vbuf;
    char *kbuf;
    int ksiz, vsiz;
    // move to first
    if(!vlcurfirst(queueHandle) || 
        !(vbuf = vlcurval(queueHandle, &vsiz)))
        return false;
    // get key for first
    if(!(kbuf = vlcurkey(queueHandle, &ksiz)))
    {
        free(vbuf);
        return false;
    }
    // remove from queue using key for first
    if(!vlout(queueHandle, kbuf, ksiz))
    {
        free(kbuf);
        free(vbuf);
        return false;
    }
    // store data
    value.clear();
    value.append(vbuf, vsiz);
    // free data
    free(kbuf);
    free(vbuf);
    int nrecs = vlrnum(queueHandle);
    if(nrecs == 0)
        // if we're not busy, optimize file
        Compact();
    return true;
}
bool PersistentQueue::Remove(const string& key, int priority)
{
    // create key, priority|key
    char* k = (char*) malloc(key.length() + sizeof(int));
    if(k == 0)
        throw PersistentQueueException(string("remove of item (") + key + 
            string(") from persisent queue ") + queueDir + 
            string(" failed -- Memory allocation failure!"));
    memcpy(k, &priority, sizeof(int));
    memcpy(&k[sizeof(int)], key.data(), key.length());
    if(vlout(queueHandle, k, key.length() + sizeof(int)))
    {
        free(k);
        return true;
    }
    free(k);
    return false;
}
void PersistentQueue::DeleteQueue(const string& queueDir)
{
    vlremove(queueDir.c_str());
}
void PersistentQueue::Compact()
{
    vloptimize(queueHandle);
}

