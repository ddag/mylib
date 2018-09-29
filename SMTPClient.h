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

#ifndef __MYLIB_SMTPCLIENT_H__
#define __MYLIB_SMTPCLIENT_H__

#include <string>
#include <memory>
#include "ClientSocket.h"

using namespace std;

namespace MyLib
{
    // supports AUTH LOGIN only!
    // NOTE: most servers will disconnect your connection after a small
    // period of idle time, so it's best to use this class only when
    // you need to send mail and then destroy it when finished instead
    // of keeping around to send more mail later
    class SMTPClient
    {
    public:
        SMTPClient(const string& name, const string& host, unsigned short port = 25, 
            const char* user = 0, const char* pass = 0);
        ~SMTPClient();
        // separate multiple to: addresses with , -- eg. a@b.com,c@d.com
        void Send(const string& from, const string& to, const string& subject,
            const string& message);
    private:
        string name;
        auto_ptr<ClientSocket> clientSocket;
    private:
        // disable compiler generated code
        SMTPClient(const SMTPClient&);
        SMTPClient& operator=(const SMTPClient&);
    };
}

#endif

