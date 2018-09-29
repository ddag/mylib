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

#include "BSDServerSocket.h"
#include "BSDClientSocket.h"
#include "ClientSocket.h"
#include "Exception.h"
#include <cstdio> // sprintf
#ifdef WIN32
#include <ws2tcpip.h> // getaddrinfo
#else
#include <sys/types.h>
#include <sys/socket.h> // socket
#include <netdb.h> // getaddrinfo
#include <netinet/in.h> // inet_addr
#include <arpa/inet.h>
#include <unistd.h> // close()
#endif
#include <memory>

using namespace MyLib;

BSDServerSocket::BSDServerSocket(const string& sname, unsigned short port) : name(sname), sock(INVALID_SOCKET)
{
    char portBuf[5+1];
    sprintf(portBuf, "%d", port);
    Init(string(portBuf));
}
BSDServerSocket::BSDServerSocket(const string& sname, const string& service) : name(sname), sock(INVALID_SOCKET)
{
    Init(service);
}
BSDServerSocket::~BSDServerSocket()
{
    shutdown(sock, SD_BOTH);
    closesocket(sock);
}
ClientSocketImp* BSDServerSocket::Accept()
{
    SOCKADDR_IN clientAddr;
    memset(&clientAddr, 0, sizeof(SOCKADDR_IN));
    socklen_t addrLen = sizeof(clientAddr);
    SOCKET clientSock = accept(sock, reinterpret_cast<LPSOCKADDR>(&clientAddr), &addrLen);
    if(clientSock == INVALID_SOCKET)
        throw SocketException(string("Accept failed on server socket ") + name, SocketLastError());
    string peer(name + " client");
    memset(&clientAddr, 0, sizeof(SOCKADDR_IN));
    addrLen = sizeof(clientAddr);
    if(getpeername(clientSock, reinterpret_cast<LPSOCKADDR>(&clientAddr), &addrLen) == 0)
        peer = inet_ntoa(clientAddr.sin_addr);
    return new BSDClientSocket(peer, clientSock);
}
void BSDServerSocket::Init(const string& service)
{
    struct addrinfo hints, *serverAddr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    if(getaddrinfo(0, service.c_str(), &hints, &serverAddr) != 0)
        throw SocketException(string("Server socket ") + name + string(" cannot resolve host on service ") + service, SocketLastError());
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        freeaddrinfo(serverAddr);
        throw SocketException(string("Unable to create server socket ") + name, SocketLastError());
    }
    if(bind(sock, serverAddr->ai_addr, static_cast<int>(serverAddr->ai_addrlen)) == SOCKET_ERROR)
    {
        freeaddrinfo(serverAddr);
        closesocket(sock);
        throw SocketException(string("Unable to bind server socket ") + name, SocketLastError());
    }
    if(listen(sock, SOMAXCONN) == SOCKET_ERROR)
    {
        freeaddrinfo(serverAddr);
        closesocket(sock);
        throw SocketException(string("Unable to listen on server socket ") + name, SocketLastError());
    }
    freeaddrinfo(serverAddr);
}

