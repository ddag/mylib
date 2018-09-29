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

#include "Encryption.h"
#include "Exception.h"
#include "ToString.h"
#include "Encoding.h"
#include <fstream>

extern "C"
{
#include "sha1.h"
#include "md5.h"
}

using namespace MyLib;

Encryption::Encryption(const string& key)
{
    string tkey(key);
    if(key.length() > AES_BLOCK_SIZE)
        throw EncryptionException(string("Encryption key is too long. 16 characters max!"));
    else
    if(key.length() < AES_BLOCK_SIZE)
    {
        // pad with 'X's
        for(int i = static_cast<int>(tkey.length());i < AES_BLOCK_SIZE;i++)
            tkey += 'X';
    }
    string hkey = Encoding::ToHexString(tkey.c_str(), static_cast<int>(tkey.length()));
    aes_encrypt_key256(reinterpret_cast<const unsigned char*>(hkey.c_str()), ectx);
    aes_decrypt_key256(reinterpret_cast<const unsigned char*>(hkey.c_str()), dctx);
}
ByteArray Encryption::Encrypt(const char* data, int len, bool addPadding)
{
    ByteArray tmp;
    int blocks = len / AES_BLOCK_SIZE;
    if(len % AES_BLOCK_SIZE != 0)
        blocks++; // add padding block
    int partialLen = 0;
    int paddingLen = 0;
    for(int i = 0;i < blocks;i++)
    {
        unsigned char blk[AES_BLOCK_SIZE];
        if((AES_BLOCK_SIZE * i) + AES_BLOCK_SIZE > len)
        {
            // finish up remainder of data
            unsigned char b2[AES_BLOCK_SIZE];
            memset(b2, 0, AES_BLOCK_SIZE);
            partialLen = len - (AES_BLOCK_SIZE * i);
            paddingLen = AES_BLOCK_SIZE - partialLen;
            memcpy(b2, &data[AES_BLOCK_SIZE * i], partialLen);
            aes_encrypt(b2, blk, ectx);
        }
        else
            aes_encrypt(reinterpret_cast<const unsigned char*>(&data[AES_BLOCK_SIZE * i]), blk, ectx);
        for(int i = 0;i < AES_BLOCK_SIZE;i++)
            tmp.push_back(blk[i]);
    }
    if(addPadding)
    {
        // pad using NIST Special Publication 800-38A scheme
        unsigned char pb[AES_BLOCK_SIZE];
        unsigned char pbout[AES_BLOCK_SIZE];
        for(int i = 0;i < AES_BLOCK_SIZE;i++)
            pb[i] = paddingLen;
        aes_encrypt(pb, pbout, ectx);
        for(int i = 0;i < AES_BLOCK_SIZE;i++)
            tmp.push_back(pbout[i]);
    }
    return tmp;
}
ByteArray Encryption::Decrypt(const char* data, int len, bool checkPadding)
{
    ByteArray tmp;
    int blocks = len / AES_BLOCK_SIZE;
    if(checkPadding)
        blocks--; // last block is padding indicator block
    // last block is always padding indicator
    for(int i = 0;i < blocks;i++)
    {
        unsigned char blk[AES_BLOCK_SIZE];
        memset(blk, 0, AES_BLOCK_SIZE);
        int padding = 0;
        // check for padding
        if(checkPadding && i + 1 == blocks)
        {
            // decrypt last block
            aes_decrypt(reinterpret_cast<const unsigned char*>(&data[AES_BLOCK_SIZE * (i + 1)]), blk, dctx);
            // check first byte for number of padding chars
            padding = (unsigned char) blk[i];
        }
        memset(blk, 0, AES_BLOCK_SIZE);
        aes_decrypt(reinterpret_cast<const unsigned char*>(&data[AES_BLOCK_SIZE * i]), blk, dctx);
        for(int i = 0;i < AES_BLOCK_SIZE - padding;i++)
            tmp.push_back(blk[i]);
    }
    return tmp;
}
void Encryption::EncryptFile(const string& inFile, const string& outFile)
{
    EncryptDecryptFile(inFile, outFile, ENCRYPT);
}
void Encryption::DecryptFile(const string& inFile, const string& outFile)
{
    EncryptDecryptFile(inFile, outFile, DECRYPT);
}
void Encryption::EncryptDecryptFile(const string& inFile, const string& outFile, Action a)
{
    ifstream in;
    in.open(inFile.c_str(), ios_base::in | ios_base::binary);
    if(!in)
        throw EncryptionException(string("Input file: ") + inFile + string(" does not exist."));
    ofstream out;
    out.open(outFile.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);
    if(!out)
    {
        in.close();
        throw EncryptionException(string("Unable to open output file: ") + outFile);
    }
    // read, encrypt/decrypt, write
    while(in)
    {
        char blk[AES_BLOCK_SIZE * 2];
        memset(blk, 0, AES_BLOCK_SIZE * 2);
        in.read(blk, AES_BLOCK_SIZE * 2);
        int nread = in.gcount();
        ByteArray b;
        if(a == ENCRYPT)
        {
            if(in.peek() == char_traits<char>::eof())
                b = Encrypt(blk, nread);
            else
                b = Encrypt(blk, nread, false);
        }
        else
        {
            // if we're at end, last block contains number
            // of pad chars
            if(in.peek() == char_traits<char>::eof())
                b = Decrypt(blk, nread);
            else
                b = Decrypt(blk, nread, false);
        }
        out.write(&b[0], static_cast<int>(b.size()));
    }
    in.close();
    out.close();
}
ByteArray Encryption::SHA1(const char* data, int len)
{
    SHA1_CTX ctx;
    char result[20];
    ByteArray tmp;
    SHA1Init(&ctx);
    SHA1Update(&ctx, reinterpret_cast<const unsigned char*>(data), len);
    SHA1Final(result, &ctx);
    for(int i = 0;i < 20;i++)
        tmp.push_back(static_cast<unsigned char>(result[i]));
    return tmp;
}
ByteArray Encryption::MD5(const char* data, int len)
{
    MD5_CTX ctx;
    u_int8_t result[16];
    ByteArray tmp;
    MD5Init(&ctx);
    MD5Update(&ctx, reinterpret_cast<unsigned char*>(const_cast<char*>(data)), len);
    MD5Final(result, &ctx);
    for(int i = 0;i < 16;i++)
        tmp.push_back(static_cast<unsigned char>(result[i]));
    return tmp;
}

