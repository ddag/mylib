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

#ifndef __MYLIB_SQLDATABASE_H__
#define __MYLIB_SQLDATABASE_H__

extern "C"
{
#include "sqlite.h"
}
#include <string>

using namespace std;

namespace MyLib
{
    // Instance of each SQLDatabase MUST NOT be shared!
    // Have each thread open it's own session to the db instead!
    class SQLDatabase
    {
    public:
        // SQLite is typeless, so our own simple! date wrapper
        // format of all dates = YYYYMMDDHH24MISS
        class Date
        {
        public:
            // YYYYMMDDHH24MISS
            Date(const string& dstr);
            Date(int year, int month, int day,
                int hour = 0, int minute = 0, int second = 0);
            int GetYear();
            int GetMonth();
            int GetDay();
            int GetHour();
            int GetMinute();
            int GetSecond();
            // YYYYMMDDHH24MISS
            string ToString();
        private:
            string date;
            int year;
            int month;
            int day;
            int hour;
            int minute;
            int second;
        };
    public:
        class ResultSet
        {
        public:
            // these are here as a convenience
            // for the following syntax:
            // ResultSet r = db.Query(...);
            // do NOT copy ResultSets otherwise
            ResultSet(const ResultSet&);
            ResultSet& operator=(const ResultSet&);
            ~ResultSet();
        public:
            bool Next();
            int GetInt(const string& col);
            string GetString(const string& col);
            double GetDouble(const string& col);
            Date GetDate(const string& col);
        private:
            ResultSet(sqlite_vm* vm);
            int FindCol(const string& col);
        private:
            sqlite_vm* vm;
            bool destroy;
            int numCols;
            const char** colData;
            const char** colInfo;
            friend class SQLDatabase;
        };
    public:
        SQLDatabase(const string& dbName);
        ~SQLDatabase();
        // if you want to batch updates, just omit the
        // commit = false on the last update you submit
        void Update(const string& sql, bool commit = true);
        // scroll through select result
        ResultSet Query(const string& sql);
        // compact
        void Compact();
        // delete db
        static void DeleteDatabase(const string& dbName);
    private:
        void ExecuteSQL(const char* sql);
    private:
        string dbName;
        sqlite* db;
    private:
        SQLDatabase(const SQLDatabase&);
        SQLDatabase& operator=(const SQLDatabase&);
    };
}

// conversions
string operator+(MyLib::SQLDatabase::Date lhs, string rhs);
string operator+(string lhs, MyLib::SQLDatabase::Date rhs);

#endif

