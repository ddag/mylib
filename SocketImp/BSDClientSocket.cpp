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

#include "BSDClientSocket.h"
#include "Exception.h"
#include <cstdio> // sprintf
#ifdef WIN32
#include <ws2tcpip.h> // getaddrinfo
#else
#include <sys/types.h>
#include <sys/socket.h> // socket
#include <netdb.h> // getaddrinfo
#include <netinet/in.h> // inet_addr
#include <netinet/tcp.h> // TCP_NODELAY
#include <arpa/inet.h>
#include <unistd.h> // close()
#endif
#include <memory>

using namespace MyLib;

BSDClientSocket::BSDClientSocket(const string& cname, const string& phost, unsigned short pport, bool sec) : name(cname), connected(false), secure(sec), host(phost), port(pport), ssl(0), ctx(0)
{
    Init();
}
BSDClientSocket::BSDClientSocket(const string& cname, const string& phost, const string& pservice, bool sec) : name(cname), connected(false), secure(sec), host(phost), service(pservice), ssl(0), ctx(0)
{
    Init();
}
BSDClientSocket::BSDClientSocket(const string& cname, SOCKET s) : name(cname), sock(s), secure(false)
{
    SetNoDelay();
}
BSDClientSocket::~BSDClientSocket()
{
    Close();
}
void BSDClientSocket::SetNoDelay()
{
    char nodelay = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
}
int BSDClientSocket::ReadChar(char& c)
{
    int ret;
    if(secure)
    {   
        if((ret = SSL_read(ssl, &c, sizeof(char))) < 0)
            throw SocketException(string("Receive on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if((ret = recv(sock, &c, sizeof(char), 0)) == SOCKET_ERROR)
        throw SocketException(string("Receive on socket ") + name + string(" failed"), SocketLastError());
    if(ret == 0)
        connected = false;
    return ret;
}
void BSDClientSocket::Init()
{
    char portBuf[5+1];
    if(service.length() == 0)
    {
        sprintf(portBuf, "%d", port);
        service = portBuf;
    }
    struct addrinfo hints, *serverAddr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if(getaddrinfo(host.c_str(), service.c_str(), &hints, &serverAddr) != 0)
        throw SocketException(string("Client socket ") + name + string(" cannot resolve host ") + host, SocketLastError());
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        freeaddrinfo(serverAddr);
        throw SocketException(string("Unable to create client socket ") + name, SocketLastError());
    }
    SetNoDelay();
    if(connect(sock, serverAddr->ai_addr, static_cast<int>(serverAddr->ai_addrlen)) == SOCKET_ERROR)
    {
        freeaddrinfo(serverAddr);
        closesocket(sock);
        throw SocketException(string("Client socket ") + name + string(" unable to connect to host ") + host, SocketLastError());
    }
    freeaddrinfo(serverAddr);
    // initialize SSL ?
    if(secure)
    {
        SSL_METHOD* method;
        OpenSSL_add_all_algorithms();   /* load & register cryptos */
        SSL_load_error_strings();       /* load all error messages */
        method = SSLv23_client_method(); /* create client instance */
        ctx = SSL_CTX_new(method);      /* create context */
        ssl = SSL_new(ctx);             /* create new SSL connection state */
        SSL_set_fd(ssl, sock);          /* attach the socket descriptor */
        int ret = 0;
        if((ret = SSL_connect(ssl)) < 0)
            throw SocketException(string("SSL connect on ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    connected = true;
}
// write
void BSDClientSocket::operator<<(char c)
{
    int ret;
    if(secure)
    {
        if((ret = SSL_write(ssl, &c, sizeof(char))) < 0)
            throw SocketException(string("Send on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if(send(sock, &c, sizeof(char), 0) == SOCKET_ERROR)
        throw SocketException(string("Send on socket ") + name + string(" failed"), SocketLastError());
}
void BSDClientSocket::operator<<(unsigned short s)
{
    int ret;
    s = htons(s);
    if(secure)
    {
        if((ret = SSL_write(ssl, &s, sizeof(unsigned short))) < 0)
            throw SocketException(string("Send on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if(send(sock, reinterpret_cast<const char*>(&s), sizeof(unsigned short), 0) == SOCKET_ERROR)
        throw SocketException(string("Send on socket ") + name + string(" failed"), SocketLastError());
}
void BSDClientSocket::operator<<(unsigned long l)
{
    int ret;
    l = htonl(l);
    if(secure)
    {
        if((ret = SSL_write(ssl, &l, sizeof(unsigned long))) < 0)
            throw SocketException(string("Send on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if(send(sock, reinterpret_cast<const char*>(&l), sizeof(unsigned long), 0) == SOCKET_ERROR)
        throw SocketException(string("Send on socket ") + name + string(" failed"), SocketLastError());
}
void BSDClientSocket::operator<<(const string& s)
{
    if(s.length() == 0)
        return; // ignore 0 length strings
    Write((const unsigned char*) s.data(), s.length());
}
void BSDClientSocket::Write(const unsigned char* buf, int len)
{
    int ret;
    if(secure)
    {
        if((ret = SSL_write(ssl, buf, len)) < 0)
            throw SocketException(string("Send on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if(send(sock, reinterpret_cast<const char*>(buf), len, 0) == SOCKET_ERROR)
        throw SocketException(string("Send on socket ") + name + string(" failed"), SocketLastError());
}
void BSDClientSocket::WriteLine(const string& line)
{
    if(line.length() == 0)
        return; // ignore 0 length strings
    Write(reinterpret_cast<const unsigned char*>(line.data()), line.length());
    operator<<('n');
}
// read
void BSDClientSocket::operator>>(char& c)
{
    ReadChar(c);
}
void BSDClientSocket::operator>>(unsigned short& s)
{
    int ret;
    if(secure)
    {
        if((ret = SSL_read(ssl, &s, sizeof(unsigned short))) < 0)
            throw SocketException(string("Receive on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if((ret = recv(sock, reinterpret_cast<char*>(&s), sizeof(unsigned short), 0)) == SOCKET_ERROR)
        throw SocketException(string("Receive on socket ") + name + string(" failed"), SocketLastError());
    s = ntohs(s);
    if(ret == 0)
        connected = false;
}
void BSDClientSocket::operator>>(unsigned long& l)
{
    int ret;
    if(secure)
    {
        if((ret = SSL_read(ssl, &l, sizeof(unsigned long))) < 0)
            throw SocketException(string("Receive on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if((ret = recv(sock, reinterpret_cast<char*>(&l), sizeof(unsigned long), 0)) == SOCKET_ERROR)
        throw SocketException(string("Receive on socket ") + name + string(" failed"), SocketLastError());
    l = ntohl(l);
    if(ret == 0)
        connected = false;
}
void BSDClientSocket::operator>>(string& s)
{
    int len = s.length();
    if(len)
        Read(reinterpret_cast<unsigned char*>(const_cast<char*>(s.data())), len);
    else
    {
        char c = 0;
        s.clear();
        while(ReadChar(c))
            s.append(sizeof(char), c);
    }
}
void BSDClientSocket::Read(unsigned char* buf, int len)
{
    int ret;
    if(secure)
    {
        if((ret = SSL_read(ssl, buf, len)) < 0)
            throw SocketException(string("Receive on socket ") + name + string(" failed"), SSL_get_error(ssl, ret));
    }
    else
    if((ret = recv(sock, reinterpret_cast<char*>(buf), len, 0)) == SOCKET_ERROR)
        throw SocketException(string("Receive on socket ") + name + string(" failed"), SocketLastError());
    if(ret == 0)
        connected = false;
}
void BSDClientSocket::ReadLine(string& line)
{
    char c = 0;
    line.clear();
    while(ReadChar(c) && c != '\n')
    {
        if(c != '\r') // ignore CR
            line.append(sizeof(char), c);
    }
}
bool BSDClientSocket::Connected()
{
    return connected;
}
void BSDClientSocket::Reconnect()
{
    if(connected == false)
        Init();
}
void BSDClientSocket::Close()
{
    if(sock != INVALID_SOCKET)
    {
        if(secure)
        {
            if(ssl)
            {
                SSL_shutdown(ssl);         /* send SSL/TLS close_notify */
                SSL_free(ssl);              /* release SSL state */
            }
            if(ctx)
                SSL_CTX_free(ctx);
        }
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        sock = INVALID_SOCKET;
        connected = false;
    }
}
const string& BSDClientSocket::Name()
{
    return name;
}
