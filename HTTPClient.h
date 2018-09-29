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

#ifndef __MYLIB_HTTPCLIENT_H__
#define __MYLIB_HTTPCLIENT_H__

#include "Types.h"
#include "ClientSocket.h"

namespace MyLib
{
    // fyi: does not support chunked encoding
    class HTTPClient
    {
    public:
        // url is of the form http://host[:port][/path] or
        // https://host[:port][/path] where port and path
        // are optional. the default port for http is 80
        // 443 for https. the default path is /.
        HTTPClient(const string& name, const string& url);
        // set a header value to send to the server
        // for ex. User-Agent = "My HTTP Client"
        void SetHeader (const string& header, const string& value);
        void SetData(const string& data);
        void SetPath(const string& newPath);
        // fetches the page specified by the url on construction
        // or the specified page given in urlPath
        void Get(string& result);
        // write to a file
        void GetToFile(const string& filename);
        // posts data to the page given in the constructor
        // or to the specified page
        void Post(string& result);
        // parses a url and fills the values for the host, port, and path
        // throws an exception if parsing fails
        static void ParseURL(const string& url, string& host, unsigned short& port, string& path, bool& secure);
    private:
        enum Operation
        {
            HTTP_GET,
            HTTP_POST
        };
        string GetHeaders();
        void PerformOp(Operation op, string& result);
    private:
        string name;
        string host;
        string path;
        string body;
        Dictionary headers;
        string headerData;
        auto_ptr<ClientSocket> clientSocket;
    private:
        HTTPClient(const HTTPClient&);
        HTTPClient& operator=(const HTTPClient&);
    };
}

#endif

