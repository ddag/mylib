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

#include "ClientSocket.h"
#include "ClientSocketImp.h"
#include "Factory.h"

using namespace MyLib;

ClientSocket::ClientSocket(const string& name, const string& host, unsigned short port, bool secure) :
    imp(Factory::ClientSocket::GetInstance(name, host, port, secure))
{        
}
ClientSocket::ClientSocket(const string& name, const string& host, const string& service, bool secure) :
    imp(Factory::ClientSocket::GetInstance(name, host, service, secure))
{        
}
ClientSocket::ClientSocket(ClientSocketImp *i) : imp(i)
{
}
ClientSocket::~ClientSocket()
{
}
// write
ClientSocket& ClientSocket::operator<<(char c)
{
    imp->operator <<(c);
    return *this;
}
ClientSocket& ClientSocket::operator<<(unsigned short s)
{
    imp->operator <<(s);
    return *this;
}
ClientSocket& ClientSocket::operator<<(unsigned long l)
{
    imp->operator <<(l);
    return *this;
}
ClientSocket& ClientSocket::operator<<(const string& s)
{
    imp->operator <<(s);
    return *this;
}
ClientSocket& ClientSocket::operator<<(const SocketIO& io)
{
    io.ToSocket(*this);
    return *this;
}
void ClientSocket::Write(const unsigned char* buf, int len)
{
    imp->Write(buf, len);
}
void ClientSocket::WriteLine(const string& line)
{
    imp->WriteLine(line);
}
// read
ClientSocket& ClientSocket::operator>>(char& c)
{
    imp->operator >>(c);
    return *this;
}
ClientSocket& ClientSocket::operator>>(unsigned short& s)
{
    imp->operator >>(s);
    return *this;
}
ClientSocket& ClientSocket::operator>>(unsigned long& l)
{
    imp->operator >>(l);
    return *this;
}
ClientSocket& ClientSocket::operator>>(string& s)
{
    imp->operator >>(s);
    return *this;
}
ClientSocket& ClientSocket::operator>>(SocketIO& io)
{
    io.FromSocket(*this);
    return *this;
}
void ClientSocket::Read(unsigned char* buf, int len)
{
    imp->Read(buf, len);
}
void ClientSocket::ReadLine(string& line)
{
    imp->ReadLine(line);
}
bool ClientSocket::Connected()
{
    return imp->Connected();
}
void ClientSocket::Reconnect()
{
    return imp->Reconnect();
}
void ClientSocket::Close()
{
    return imp->Close();
}
const string& ClientSocket::Name()
{
    return imp->Name();
}

