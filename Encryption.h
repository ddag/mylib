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

#ifndef __MYLIB_ENCRYPTION_H__
#define __MYLIB_ENCRYPTION_H__

// govt certified AES as implemented by Brian Gladman:
// http://fp.gladman.plus.com/cryptography_technology/rijndael/
extern "C"
{
#include "aes.h"
}

#include "Types.h"
#include <string>

using namespace std;

namespace MyLib
{
    class Encryption
    {
    public:
        // key = password/encryption key, MAX of 16 chars!
        Encryption(const string& key);
        // data is encrypted in blocks of 16 characters
        // i.e. the minimum output is 16 bytes
        // set the addPadding to false when encrypting data in pieces (multiples of 16 bytes)
        // set addPadding to true when on final block of data
        // ignore addPadding if encrypting in one shot
        ByteArray Encrypt(const char* data, int len, bool addPadding = true);
        // set checkPadding to false when decrypting data in pieces (multiples of 16 bytes)
        // set checkPadding to true when on final block of data
        // ignore checkPadding if decrypting in one shot
        ByteArray Decrypt(const char* data, int len, bool checkPadding = true);
        void EncryptFile(const string& inFile, const string& outFile);
        void DecryptFile(const string& inFile, const string& outFile);
        // one-way hash functions
        static ByteArray SHA1(const char* data, int len);
        static ByteArray MD5(const char* data, int len);
    private:
        enum Action {ENCRYPT, DECRYPT};
        void EncryptDecryptFile(const string& inFile, const string& outFile, Action a);
    private:
        aes_encrypt_ctx ectx[1];
        aes_decrypt_ctx dctx[1];
    private:
        Encryption(const Encryption&);
        Encryption& operator=(const Encryption&);
    };
}

#endif

