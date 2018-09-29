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

#include "UNIXMemoryMappedFile.h"
#include "Exception.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>

using namespace MyLib;

UNIXMemoryMappedFile::UNIXMemoryMappedFile(const string& file, char*& mapView, unsigned int mapSize)
{
    fileHandle = open(file.c_str(), O_CREAT|O_RDWR);
    if(fileHandle < 0)
        throw MemoryMappedFileException(string("Unable to create map file ") + file, errno);
    else
        // allow us to access it again
        chmod(file.c_str(), S_IRWXU);
    // get file info
    fstat(fileHandle, &fileStat);
    // if initial creation, then grow file
    if(fileStat.st_size == 0)
    {
        // initialize file/memory with nulls
        char byte = 0;
        for(unsigned int i = 0;i < mapSize;i++)
            write(fileHandle, &byte, 1);
        fileStat.st_size = mapSize;
    }
    map = (char *) mmap(0, fileStat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, 
        fileHandle, 0);
    if(map < 0)
    {
        close(fileHandle);
        throw MemoryMappedFileException(string("Unable to create file mapping for file ") + file, errno);
    }
    mapView = map;
}
UNIXMemoryMappedFile::~UNIXMemoryMappedFile()
{
    munmap(map, fileStat.st_size);
    close(fileHandle);
}
void UNIXMemoryMappedFile::FlushMap()
{
    msync(map, fileStat.st_size, MS_SYNC);
}

