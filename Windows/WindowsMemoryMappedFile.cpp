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

#include "WindowsMemoryMappedFile.h"
#include "Exception.h"

using namespace MyLib;

WindowsMemoryMappedFile::WindowsMemoryMappedFile(const string& file, char*& mapView, unsigned int mapSize)
{
    mapHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, file.c_str());
	if(mapHandle == 0)
	{
		// create file
        fileHandle = CreateFile(file.c_str(), GENERIC_READ|GENERIC_WRITE, 
			FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 0);
        if(fileHandle == INVALID_HANDLE_VALUE)
            throw MemoryMappedFileException(string("Unable to create map file ") + file, GetLastError());
		mapHandle = CreateFileMapping(fileHandle, 0, 
            PAGE_READWRITE, 0, mapSize, file.c_str());
        if(mapHandle == 0)
        {
            CloseHandle(fileHandle);
            throw MemoryMappedFileException(string("Unable to create file mapping for file ") + file, GetLastError());
        }
    }
	// write initial size
	map = (char *) MapViewOfFile(mapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if(map == 0)
    {
		CloseHandle(mapHandle);
		CloseHandle(fileHandle);
        throw MemoryMappedFileException(string("Unable to map view of file ") + file, GetLastError());
    }
    mapView = map;
}
WindowsMemoryMappedFile::~WindowsMemoryMappedFile()
{
	UnmapViewOfFile(map);
	CloseHandle(mapHandle);
	CloseHandle(fileHandle);
}
void WindowsMemoryMappedFile::FlushMap()
{
    FlushViewOfFile(map, 0);
}

