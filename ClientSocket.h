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

#ifndef __MYLIB_CLIENTSOCKET_H__
#define __MYLIB_CLIENTSOCKET_H__

#include <string>
#include <memory>

using namespace std;

namespace MyLib
{
    class ClientSocketImp;
    class ClientSocket
    {
    public:
        class SocketIO
        {
        public:
            virtual void ToSocket(ClientSocket&) const = 0;
            virtual void FromSocket(ClientSocket&) = 0;
        };
    public:
        ClientSocket(const string& name, const string& host, 
            unsigned short port, bool secure = false);
        ClientSocket(const string& name, const string& host, 
            const string& service, bool secure = false);
        ~ClientSocket();
        // write
        ClientSocket& operator<<(char c);
        ClientSocket& operator<<(unsigned short s);
        ClientSocket& operator<<(unsigned long l);
        ClientSocket& operator<<(const string& s);
        ClientSocket& operator<<(const SocketIO& io);
        void Write(const unsigned char* buf, int len);
        void WriteLine(const string& line);
        // read
        ClientSocket& operator>>(char& c);
        ClientSocket& operator>>(unsigned short& s);
        ClientSocket& operator>>(unsigned long& l);
        // call string::resize() to set length of string to read
        ClientSocket& operator>>(string& s);
        ClientSocket& operator>>(SocketIO& io);
        void Read(unsigned char* buf, int len);
        void ReadLine(string& line);
        bool Connected();
        void Reconnect();
        void Close();
        // peer name/ip or user-given name
        const string& Name();
    private:
        auto_ptr<ClientSocketImp> imp;
    private:
        // called by ServerSocket to create
        friend class ServerSocket;
        ClientSocket(ClientSocketImp* imp);
        // disable compiler code
        ClientSocket(const ClientSocket&);
        ClientSocket& operator=(const ClientSocket&);
    };
}

#endif

