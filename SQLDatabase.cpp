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

#include "SQLDatabase.h"
#include "Exception.h"
#include "Thread.h"
#include "ToString.h"
#include <stdio.h>
#include <string.h> // stricmp here
#ifndef WIN32
#include <strings.h> // strcasecmp here
#endif

extern "C" 
{
#include "sqliteInt.h"
int sqliteOsDelete(const char*);
}

using namespace MyLib;

SQLDatabase::Date::Date(const string& dstr)
{
    year = atoi(dstr.substr(0, 4).c_str());
    month = atoi(dstr.substr(4, 2).c_str());
    day = atoi(dstr.substr(6, 2).c_str());
    hour = atoi(dstr.substr(8, 2).c_str());
    minute = atoi(dstr.substr(10, 2).c_str());
    second = atoi(dstr.substr(12, 2).c_str());
    date = dstr;
}
SQLDatabase::Date::Date(int year, int month, int day,
    int hour, int minute, int second)
{
    this->year = year;
    this->month = month;
    this->day = day;
    this->hour = hour;
    this->minute = minute;
    this->second = second;
    // string representation
    char dbuf[14+1];
    sprintf(dbuf, "%d%02d%02d%02d%02d%02d", year, month,
        day, hour, minute, second);
    date = dbuf;
}
int SQLDatabase::Date::GetYear()
{
    return year;
}
int SQLDatabase::Date::GetMonth()
{
    return month;
}
int SQLDatabase::Date::GetDay()
{
    return day;
}
int SQLDatabase::Date::GetHour()
{
    return hour;
}
int SQLDatabase::Date::GetMinute()
{
    return minute;
}
int SQLDatabase::Date::GetSecond()
{
    return second;
}
string SQLDatabase::Date::ToString()
{
    return date;
}
SQLDatabase::ResultSet::ResultSet(const SQLDatabase::ResultSet& r) : vm(r.vm),
    destroy(true), numCols(0), colData(0), colInfo(0)
{
}
SQLDatabase::ResultSet& SQLDatabase::ResultSet::operator=(const SQLDatabase::ResultSet& r)
{
    if(&r == this)
        return *this;
    vm = r.vm;
    return *this;
}
SQLDatabase::ResultSet::~ResultSet()
{
    if(destroy && vm)
    {
        char *errMsg = 0;
        int errCode = sqlite_finalize(vm, &errMsg);
        if(errCode != 0)
        {
            string err(errMsg);
            sqlite_freemem(errMsg);
            throw SQLDatabaseException(string("VM finalize failed"), 
                errCode, errMsg);
        }
    }
}
bool SQLDatabase::ResultSet::Next()
{
    int errCode;
    while((errCode = sqlite_step(vm, &numCols, &colData, &colInfo)) == SQLITE_BUSY)
        Thread::Sleep(1);
    if(errCode == SQLITE_ROW)
        return true;
    return false;
}
int SQLDatabase::ResultSet::GetInt(const string& col)
{
    return atoi(colData[FindCol(col)]);
}
double SQLDatabase::ResultSet::GetDouble(const string& col)
{
    return atof(colData[FindCol(col)]);
}
string SQLDatabase::ResultSet::GetString(const string& col)
{
    return string(colData[FindCol(col)]);
}
SQLDatabase::Date SQLDatabase::ResultSet::GetDate(const string& col)
{
    return SQLDatabase::Date(GetString(col));
}
SQLDatabase::ResultSet::ResultSet(sqlite_vm* pvm) : vm(pvm), 
    destroy(true), numCols(0), colData(0), colInfo(0)
{
}
int SQLDatabase::ResultSet::FindCol(const string& col)
{
    for(int i = 0;i < numCols;i++)
    {
        // case insensitive compare
#ifdef WIN32
        if(stricmp(string(colInfo[i]).c_str(), col.c_str()) == 0)
#else
        if(strcasecmp(string(colInfo[i]).c_str(), col.c_str()) == 0)
#endif
            return i;
    }
    throw SQLDatabaseException(string("Column ") + col + string(" not found!"));
}
SQLDatabase::SQLDatabase(const string& name) : dbName(name)
{
    char *errMsg = 0;
    if((db = sqlite_open(dbName.c_str(), 0 /* not used */, &errMsg)) == 0)
    {
        string err(errMsg);
        sqlite_freemem(errMsg);
        throw SQLDatabaseException(string("Unable to open database ") + dbName + 
            string(". Error: ") + err);
    }
}
SQLDatabase::~SQLDatabase()
{
    sqlite_close(db);
}
void SQLDatabase::Update(const string& sql, bool commit)
{
    if(commit == false && !(db->flags & SQLITE_InTrans))
        ExecuteSQL("BEGIN");
    // execute statement
    ExecuteSQL(sql.c_str());
    // commit work ?
    if(commit == true && (db->flags & SQLITE_InTrans))
        ExecuteSQL("COMMIT");
}
SQLDatabase::ResultSet SQLDatabase::Query(const string& sql)
{
    char *errMsg = 0;
    // compile the statement
    const char *next = 0;
    sqlite_vm* vm = 0;
    int errCode = sqlite_compile(db, sql.c_str(), &next, &vm, &errMsg);
    if(errCode != 0)
    {
        string err(errMsg);
        sqlite_freemem(errMsg);
        throw SQLDatabaseException(string("Compile of SQL string ") + sql +
            string( " failed on db ") + dbName, 
            errCode, errMsg);
    }
    SQLDatabase::ResultSet rs(vm);
    // don't destroy the vm when this goes out of scope
    rs.destroy = false;
    return rs;
}
void SQLDatabase::Compact()
{
    ExecuteSQL("VACUUM;");
}
void SQLDatabase::DeleteDatabase(const string& dbName)
{
    // all this does is delete the file
    sqliteOsDelete(dbName.c_str());
}
void SQLDatabase::ExecuteSQL(const char* sql)
{
    char *errMsg = 0;
    int errCode = sqlite_exec(db, sql, 0, 0, &errMsg);
    if(errCode != 0)
    {
        string err(errMsg);
        sqlite_freemem(errMsg);
        throw SQLDatabaseException(string("Update <") + sql + 
            string("> failed on db ") + dbName, 
            errCode, err);
    }
}

string operator+(MyLib::SQLDatabase::Date lhs, string rhs)
{
    return lhs.ToString() + rhs;
}
string operator+(string lhs, MyLib::SQLDatabase::Date rhs)
{
    return lhs + rhs.ToString();
}
