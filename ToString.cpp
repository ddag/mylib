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

#include "ToString.h"
#include <sstream>

string operator+(string lhs, bool rhs)
{
    if(rhs == false)
        return lhs + "false";
    return lhs + "true";
}
string operator+(bool lhs, string rhs)
{
    if(lhs == false)
        return "false" + rhs;
    return "true" + rhs;
}
string operator+(string lhs, char rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(char lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, unsigned char rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(unsigned char lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, short rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(short lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, unsigned short rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(unsigned short lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, int rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(int lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, unsigned int rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(unsigned int lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, long rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(long lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, unsigned long rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(unsigned long lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, float rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(float lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(string lhs, double rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}
string operator+(double lhs, string rhs)
{
    ostringstream s;
    s << lhs << rhs;
    return s.str();
}

