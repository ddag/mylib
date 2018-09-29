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

#ifndef __MYLIB_TOSTRING_H__
#define __MYLIB_TOSTRING_H__

#include <string>

using namespace std;

// convert to string!
string operator+(string lhs, bool rhs);
string operator+(bool lhs, string rhs);
string operator+(string lhs, char rhs);
string operator+(char lhs, string rhs);
string operator+(string lhs, unsigned char rhs);
string operator+(unsigned char lhs, string rhs);
string operator+(string lhs, short rhs);
string operator+(short lhs, string rhs);
string operator+(string lhs, unsigned short rhs);
string operator+(unsigned short lhs, string rhs);
string operator+(string lhs, int rhs);
string operator+(int lhs, string rhs);
string operator+(string lhs, unsigned int rhs);
string operator+(unsigned int lhs, string rhs);
string operator+(string lhs, long rhs);
string operator+(long lhs, string rhs);
string operator+(string lhs, unsigned long rhs);
string operator+(unsigned long lhs, string rhs);
string operator+(string lhs, float rhs);
string operator+(float lhs, string rhs);
string operator+(string lhs, double rhs);
string operator+(double lhs, string rhs);
string ToHexString(const char* binaryData, int len);

#endif

