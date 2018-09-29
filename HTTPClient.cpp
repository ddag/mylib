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

#include "HTTPClient.h"
#include "Encoding.h"
#include "Exception.h"
#include "ToString.h"
#include "StringTokenizer.h"
#include <fstream>

using namespace MyLib;

HTTPClient::HTTPClient(const string& cname, const string& url)
{
    unsigned short port;
    bool secure;
    ParseURL(url, host, port, path, secure);
    clientSocket.reset(new ClientSocket(name, host, port, secure));
}
void HTTPClient::SetHeader(const string& header, const string& value)
{
    headers[header] = value;
    headerData.resize(0);
    for (Dictionary::const_iterator i = headers.begin();
        i != headers.end();i++)
    {
        headerData += (*i).first;
        headerData += ": ";
        headerData += (*i).second;
        headerData += "\r\n";
    }
}
void HTTPClient::SetData(const string& data)
{
    body = data;
}
void HTTPClient::SetPath(const string& newPath)
{
    path = newPath;
}
void HTTPClient::Get(string& result)
{
    result.resize(0);
    PerformOp(HTTP_GET, result);
}
void HTTPClient::GetToFile(const string& result)
{
    string file(result);
    PerformOp(HTTP_GET, file);
}
void HTTPClient::Post(string& result)
{
    PerformOp(HTTP_POST, result);
}
void HTTPClient::ParseURL(const string& url, string& host, unsigned short& port, string& path, bool& secure)
{
    // all http urls have the following format:
    // http://<host>:<port>/<location>?<searchpart>

    // validate and strip off scheme if present
    secure = false;
    string urlData;
    int pos = url.find("://");
    if(pos < 0)
        // no scheme, maybe just host/location or host:port/location
        urlData = url;
    else
    {
        // make sure scheme = http
        string scheme(url.substr(0, pos));
        if(stricmp(scheme.c_str(), "http") == 0)
            port = 80;
        else
        if(stricmp(scheme.c_str(), "https") == 0)
        {
            secure = true;
            port = 443;
        }
        else
            throw Exception("Unsupported scheme: " + scheme);
        // get scheme-specific-part after these 3 chars "://"
        if(pos + 3 > static_cast<int>(url.length() - 1))
            throw Exception("Invalid url: " + url);
        // we have at least "http://x"
        urlData = url.substr(pos + 3);
    }
    // search for port
    pos = urlData.find(':');
    if(pos < 0)
    {
        // no port, search for location
        pos = urlData.find('/');
        if(pos < 0)
        {
            host = urlData;
            path = "/";
        }
        else
        {
            // make sure it's not "/"
            if(pos == 0 || pos == static_cast<int>(urlData.length() - 1))
                throw Exception("Invalid url: " + url);
            // we have at least "x/", get x as host
            host = urlData.substr(0, pos);
            // remaining is path even if just "/"
            path = urlData.substr(pos);
        }
    }
    else
    {
        // make sure it's not just ":"
        if(pos == 0 || pos == static_cast<int>(urlData.length() - 1))
            throw Exception("Invalid url: " + url);
        // we have at least "x:", get x
        host = urlData.substr(0, pos);
        // make sure it's not just "x:"
        if(pos + 1 >= static_cast<int>(urlData.length()))
            throw Exception("Invalid url: " + url);
        // we have "x:y", get y
        string portAndPath(urlData.substr(pos + 1));
        pos = portAndPath.find('/');
        if(pos < 0)
        {
            // no location, just port
            port = atoi(portAndPath.c_str());
            if(port == 0)
                throw Exception("Invalid port: " + portAndPath);
            path = "/";
        }
        else
        {
            // make sure it's not just "x:/"
            if(pos == 0 || pos == static_cast<int>(portAndPath.length() - 1))
                throw Exception("Invalid url: " + url);
            // we have "x:y/"
            string portStr(portAndPath.substr(0, pos));
            port = atoi(portStr.c_str());
            if(port == 0)
                throw Exception("Invalid port: " + portStr);
            // rest of url is path
            path = portAndPath.substr(pos);
        }
    }
}
void HTTPClient::PerformOp(Operation op, string& result)
{
    // construct op
    string data;
    if(op == HTTP_GET)
        data += "GET ";
    else
        data += "POST ";
    data += path;
    if(op == HTTP_GET)
    {
        if(body.length() > 0)
            data += "?";
        // add parameters if any
        data += body;
        data += " HTTP/1.1\r\n";
        data += string("Host: ") + host + string("\r\n");
        data += headerData;
        data += "\r\n";
    }
    else
    {
        data += " HTTP/1.1\r\n";
        data += string("Host: ") + host + string("\r\n");
        data += string("Content-Length: ") + body.length() + string("\r\n");
        data += headerData; // additional user headers
        data += "\r\n";
        data += body;
        data += "\r\n";
    }
    if(clientSocket->Connected() == false)
        clientSocket->Reconnect();
    *clientSocket << data;
    // read size of reply
    string line;
    int len = 0;
    int pos = -1;
    bool closeConnection = false;
    while(clientSocket->Connected())
    {
        clientSocket->ReadLine(line);
        if(line.length() == 0)
            break;
        pos = line.find("Content-Length");
        if(pos < 0)
            pos = line.find("content-length");
        if(pos >= 0)
        {
            StringTokenizer tk(line, ':');
            len = atoi(tk[1].c_str());
        }
        pos = line.find("Connection");
        if(pos < 0)
            pos = line.find("connection");
        if(pos >= 0)
        {
            StringTokenizer tk(line, ':');
            if(strstr(tk[1].c_str(), "Close") || strstr(tk[1].c_str(),"close"))
                closeConnection = true;
        }
    }
    // store to file ?
    if(result.length() > 0)
    {
        ofstream strm(result.c_str(), 
            ios_base::out | ios_base::trunc | ios_base::binary);
        string buf;
        buf.resize(len);
        *clientSocket >> buf;
        strm << buf;
    }
    else
    {
        result.resize(len);
        *clientSocket >> result;
    }
    if(closeConnection)
        clientSocket->Close();
}
