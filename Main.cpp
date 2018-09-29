#include "Mutex.h"
#include "Semaphore.h"
#include "Thread.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Logger.h"
#include "Config.h"
#include "FindFile.h"
#include "DllLoader.h"
#include "MemoryMappedFile.h"
#include "PersistentHash.h"
#include "PersistentQueue.h"
#include "SQLDatabase.h"
#include "ToString.h"
#include "Exception.h"
#include "Encryption.h"
#include "Encoding.h"
#include "StringTokenizer.h"
#include "HTTPClient.h"
#include "Process.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace MyLib;

class MyWorker : public Thread::Runnable, public ClientSocket::SocketIO
{
public:
    MyWorker(Mutex& mtx, Semaphore &s) : stop(false), name("MyWorker"), mutex(mtx), sem(s)
    {
    }
    virtual ~MyWorker() {}
    void Run()
    {
        try
        {
            ServerSocket server("MyServerSocket", 9999);
            while(stop == false)
            {
                Logger::Log.Write(string("Waiting for connection..."));
                {
                    auto_ptr<ClientSocket> s(server.Accept());
                    Logger::Log.Write(string("Connection from ") + s->Name());
                    char c;
                    string str;
                    str.resize(4);
                    unsigned short sh;
                    unsigned long l;
                    *s >> c >> str >> sh >> l >> *this;
                    Logger::Log.Write(string("Received ") + c + 
                        string(",") + str + string(",") + 
                        sh + string(",") + l + 
                        string(",") + name);
                }
                Mutex::Lock lock(mutex);
                Logger::Log.Write("Got lock!");
            }
            Logger::Log.Write(string("Waiting on semaphore..."));
            sem.Wait();
            Logger::Log.Write(string("Released from semaphore."));
        }
        catch(Exception& e)
        {
            Logger::Log.Write(e.what());
        }
    }
    void Stop()
    {
        stop = true;
        ClientSocket c("MyClientSocket", "localhost", 9999);
        unsigned short s(1);
        unsigned long l(2);
        c << 'x' << "quit" << s << l << *this;
        Logger::Log.Write(string("Signalling semaphore..."));
        sem.Signal();
    }
    void ToSocket(ClientSocket& c) const 
    {
        c << "MxWorker";
    }
    void FromSocket(ClientSocket& c)
    {
        c >> name;
    }
private:
    bool stop;
    string name;
    Mutex& mutex;
    Semaphore& sem;
};

class MyWorker2 : public Thread::Runnable, public ClientSocket::SocketIO
{
public:
    MyWorker2(Mutex& mtx) : name("MyWorker2"), mutex(mtx)
    {
    }
    virtual ~MyWorker2() {}
    void Run()
    {
        try
        {
            ClientSocket c("MyClientSocket2", "localhost", 9999);
            unsigned short s(3);
            unsigned long l(4);
            c << 'y' << "xxxx" << s << l << *this;
            Mutex::Lock lock(mutex);
            Logger::Log.Write(string("Got Lock! Exiting..."));
        }
        catch(Exception& e)
        {
            Logger::Log.Write(e.what());
        }
    }
    void Stop()
    {
    }
    void ToSocket(ClientSocket& c) const 
    {
        c << "MzWorker";
    }
    void FromSocket(ClientSocket& c)
    {
        c >> name;
    }
private:
    string name;
    Mutex& mutex;
};

class MyFindFileHandler : public FindFileHandler
{
public:
    MyFindFileHandler() : i(0) {}
    bool FileFound(const string& file, bool dir)
    {
        Logger::Log.Write(file);
        if(++i == 3)
            return true; // stop
        return false; // dont stop
    }
private:
    int i;
};

int main()
{
    try
    {
        // log to file and console
        Logger::Log.Open("logfile.txt", true);
        // non-debug line
        Logger::Log.Write(string("Debug not turned on"));
        // this wont show up
        Logger::Log.Write(string("This won't show"), true);
        // turn on debug
        Logger::Log.SetDebug(true);
        Logger::Log.Write(string("This will show"), true);
        {
            StringArray args;
            args.push_back("-al");
            args.push_back(".");
            Process p("MyProcess", "ls", args);
            string line;
            while(p.ReadLine(line))
                Logger::Log.Write(line);
            p.WaitForCompletion(0);
        }
        {
            string data("going to urlencode this string");
            Logger::Log.Write(data);
            string result = Encoding::URLEncode(data.c_str(), data.length());
            Logger::Log.Write(string("encoded: ") + result);
            result = Encoding::URLDecode(result.c_str(), result.length());
            Logger::Log.Write(string("decoded: ") + result);
        }
        {
            StringTokenizer s("1;2;3;4;5",";");
            for(int i = 0;i < s.NumTokens();i++)
                Logger::Log.Write(s[i]);
        }
        {
            Encryption e(string("encryption key"));
            e.EncryptFile("Main.cpp", "Main.cpp.E");
            e.DecryptFile("Main.cpp.E", "Main.cpp.D");
            char data[64] = "1234567890123456789012345678901";
            int len = int(strlen(data));
            ByteArray b = e.Encrypt(data, len);
            memset(data, 0, len); // clear out data buffer
            Logger::Log.Write(string("Encrypted (Hex-encoded): ") + Encoding::ToHexString(&b[0], int(b.size())));
            Logger::Log.Write(string("Encrypted (Base64-encoded): ") + Encoding::ToBase64(&b[0], int(b.size())));
            b = e.Decrypt(&b[0], int(b.size()));
            memcpy(data, &b[0], int(b.size())); // put decrypted data in
            Logger::Log.Write(string("Decrypted: ") + data);
            // test one-way hashes
            strcpy(data, "this is my password");
            len = strlen(data);
            b = Encryption::SHA1(data, len);
            Logger::Log.Write(string("SHA1 (Hex-encoded): ") + Encoding::ToHexString(&b[0], int(b.size())));
            b = Encryption::MD5(data, len);
            Logger::Log.Write(string("MD5 (Hex-encoded): ") + Encoding::ToHexString(&b[0], int(b.size())));
        }
        // SQL test
        {
            SQLDatabase::DeleteDatabase("test.db");
            SQLDatabase db("test.db");
            // create 2 tables and indexes
            db.Update(string("create table table1 (int)"));
            db.Update(string("create unique index table1_int_idx on table1(int);"));
            db.Update(string("create table table2 (int, double, string, date)"));
            db.Update(string("create index table2_int_idx on table2(int)"));
            // SQLite is typeless -- all data are strings!
            db.Update(string("insert into table1 values ('1')"), false);
            db.Update(string("insert into table1 values ('2')"), false);
            // dates are really stored as strings of form
            // YYYYMMDDHH24MISS
            db.Update(string("insert into table2 values ('1', '3.2', 'string4', '20031223081533')"), false);
            db.Update(string("insert into table2 values ('1', '3.3', 'string5', '20031223081534')"), false);
            db.Update(string("insert into table2 values ('2', '3.4', 'string6', '20031223081535')"), true);
            // join 2 tables on key field
            SQLDatabase::ResultSet rs = db.Query(string("select table1.int, table2.double, table2.string, table2.date ") +
                string("from table1, table2 where table1.int = table2.int;"));
            // print out result of join
            while(rs.Next())
            {
                Logger::Log.Write(rs.GetInt(string("table1.int")) + string(",") +
                    rs.GetDouble(string("table2.double")) + string(",") + rs.GetString(string("table2.string")) +
                    string(",") + rs.GetDate(string("table2.date")));
            }
        }
        // callback search
        // find all files matching *Sock* recursively
        MyFindFileHandler fh;
        FindFile(".", "*Sock*", fh, true);
        // same search but synchronous approach
        FileList fl;
        FindFile(".", "*Sock*", fl, true);
        for(FileList::iterator i = fl.begin();
            i != fl.end();i++)
        {
            Logger::Log.Write(*i);
        }
        // load a dll
        {
    #ifdef WIN32
            DllLoader loader("msvcrt.dll");
    #else
            // see which libc is in your /lib dir !!! :)
            DllLoader loader("/lib/libc.so");
    #endif
            int (*ptr_to_atoi) (const char*) = (int (*) (const char*)) loader.GetAddressOf("atoi");
            Logger::Log.Write(string("atoi(\"1234\") = ") + ptr_to_atoi("1234"));
        } // dll automatically unloaded
        {
            // map a 1k file into mem -- if map file doesnt exist
            // a default 1k (1024 bytes) map file is created
            char* map;
            MemoryMappedFile mf("mmfile", map);
            // fill data with 'X's
            for(int i = 0;i < 1024;i++)
                map[i] = 'X';
        } // check file named mmfile -- it should have all 'X's :)
        {
            // remove any existing db
            PersistentHash::DeleteHash("phash");
            // create
            PersistentHash phash("phash");
            // put in store
            string key("123-456-7890");
            phash.Store(key, "Don Gobin");
            // get from store
            string val;
            phash.Retrieve(key, val);
            Logger::Log.Write(string("Query persistent hash for key(") + key + string(") = ") + val);
        }
        {
            // remove any existing queue
            PersistentQueue::DeleteQueue("pqueue");
            // create
            PersistentQueue pqueue("pqueue");
            // put in queue
            pqueue.Enqueue("5", "fifth", 1);
            pqueue.Enqueue("4", "fourth", 3);
            pqueue.Enqueue("3", "third", 3);
            pqueue.Enqueue("2", "second", 2);
            pqueue.Enqueue("1", "first", 1);
            // dequeue
            string val;
            pqueue.Dequeue(val);
            Logger::Log.Write(string("Dequeued ") + val);
            pqueue.Dequeue(val);
            Logger::Log.Write(string("Dequeued ") + val);
            pqueue.Dequeue(val);
            Logger::Log.Write(string("Dequeued ") + val);
            pqueue.Dequeue(val);
            Logger::Log.Write(string("Dequeued ") + val);
            pqueue.Dequeue(val);
            Logger::Log.Write(string("Dequeued ") + val);
        }
        // read a configuration file
        Config::Cfg.Load("test.cfg");
        Logger::Log.Write(string("[key1]=") + Config::Cfg.GetValue("key1"));
        Logger::Log.Write(string("[section 1][key1]=") + Config::Cfg.GetValue("section 1", "key1"));
        Logger::Log.Write(string("[key3]=") + Config::Cfg.GetIntValue("key3"));
        Logger::Log.Write(string("[key4]=") + Config::Cfg.GetDoubleValue("key4"));
        Logger::Log.Write(string("[key5]=") + Config::Cfg.GetBoolValue("key5"));
        {
            ClientSocket c("client", "www.delorie.com", "http");
            c << "GET /\r\n";
            string reply;
            c >> reply;
            Logger::Log.Write(reply + string(" : ") + int(reply.length()));
        }
        // OR using the HTTPClient api
        /*{
            {
                string reply;
                HTTPClient client("MyGetClient", "http://www.delorie.com/web/headers.html");
                client.Get(reply);
                Logger::Log.Write(reply + string(" : ") + int(reply.length()));
                // get another page
                reply.resize(0);
                client.SetPath("/web/lynxview.html");
                client.Get(reply);
                Logger::Log.Write(reply + string(" : ") + int(reply.length()));
            }
            {
                string reply;
                // fill out html form entries
                HTTPClient client("MyPostClient", 
                    "http://www.delorie.com/web/headers.cgi");
                client.SetHeader("Content-Type", "application/x-www-form-urlencoded");
                client.SetData("url=http%3A%2F%2Fwww.yahoo.com");
                client.Post(reply);
                Logger::Log.Write(reply + string(" : ") + int(reply.length()));
            }
            // how about a SSL connection ?
            {
                string reply;
                HTTPClient client("MySSLClient", "https://www.wachovia.com");
                client.Get(reply);
                Logger::Log.Write(reply + string(" : ") + int(reply.length()));
            }
        }*/
        Mutex mutex("MyMutex", true);
        Semaphore sem("MySemaphore", true);
        MyWorker w(mutex, sem);
        // wait for listener to come up
        Thread::Sleep(2);
        Thread thread("MyThread", w);
        MyWorker2 w2(mutex);
        Thread thread2("MyThread2", w2);
        char c;
        cin >> c;
    }
    catch(Exception& e)
    {
        Logger::Log.Write(e.what());
    }
    catch(exception& e)
    {
        Logger::Log.Write(e.what());
    }
    catch(...)
    {
        Logger::Log.Write(string("Hrmmm. What happened ?"));
    }
    return 0;
}
