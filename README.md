<HTML>
    <BODY>
        <P><STRONG><FONT size="5">How to use this library</FONT><STRONG></P>
        <P><STRONG>Mutexes</STRONG></P>
        <P>To create a mutex:<BR>
            <BR>
            #include "Mutex.h"<BR>
            using namespace MyLib;<BR>
            ...<BR>
            // for process-wide mutex -- available ONLY to threads within the current 
            process<BR>
            Mutex mutex("MyMutex");<BR>
            // for a mutex shared by other processes (child processes ONLY on *NIX 
            platforms)<BR>
            Mutex mutex("MyMutex", true);<BR>
            <BR>
            To lock a mutex:<BR>
            <BR>
            Mutex::Lock lock(mutex);<BR>
            <BR>
            Unlocking is automatic via the lock object.</P>
        <P><STRONG>Semaphores</STRONG></P>
        <P>The create a semaphore:</P>
        <P>#include "Semaphore.h"<BR>
            using namespace MyLib;<BR>
            ...<BR>
            // for process-wide&nbsp;semaphore-- available ONLY to threads within the 
            current process<BR>
            Semaphore sem("MySemaphore");<BR>
            // for a&nbsp;semaphore shared by other processes (Solaris and Windows ONLY)<BR>
            Semaphore sem("MySemaphore", true);<BR>
            <BR>
            To wait on a semaphore:<BR>
            <BR>
            sem.Wait();<BR>
            <BR>
            To signal a semaphore (i.e. release a waiter):</P>
        <P>sem.Signal();</P>
        <P><STRONG>Threads</STRONG></P>
        <P>In order to create a thread, you need to specify an instance of a class derived 
            from MyLib::Thread::Runnable. This interface has two pure virtual functions: 
            Run() and Stop() called when the thread is started and when it is stopping 
            respectively.<BR>
            <BR>
            #include "Thread.h"<BR>
            using namespace MyLib;<BR>
            ...<BR>
            // derive some class that needs to perform background processing from 
            MyLib::Thread::Runnable<BR>
            class SomeBackgroundProcessor : public Thread::Runnable<BR>
            {<BR>
            ...<BR>
            void Run()<BR>
            {<BR>
            // put code to do background work here<BR>
            }<BR>
            void Stop()<BR>
            {<BR>
            // put code to stop background work here<BR>
            }<BR>
            };<BR>
            ...<BR>
            // create thread specifying background processing class<BR>
            SomeBackgroundProcessor p;<BR>
            MyLib::Thread thread("MyThread", p);<BR>
            <BR>
            SomeBackgroundProcessor::Stop() will automatically be called when thread goes 
            out of scope (or is deleted if it's allocated on the heap.)</P>
        <P>To put a thread to sleep including the main() thread, simply call the static 
            function MyLib::Thread::Sleep(secs).</P>
        <P><STRONG>Sockets</STRONG></P>
        <P>To create a client socket:</P>
        <P>#include "ClientSocket.h"<BR>
            using namespace MyLib;<BR>
            ...<BR>
            // connect to yahoo web server<BR>
            ClientSocket client("MyClientSocket", "<A href="http://www.yahoo.com">www.yahoo.com</A>", 
            80);<BR>
            // OR<BR>
            //ClientSocket client("MyClientSocket", "<A href="http://www.yahoo.com">www.yahoo.com</A>", 
            "http");<BR>
            <BR>
            To write to a socket:<BR>
            <BR>
            client &lt;&lt; "GET /\n";<BR>
            <BR>
            There&nbsp;are also&nbsp;Write and WriteLine methods for those of you that want 
            this type of functionality.<BR>
            <BR>
            To read from a socket:<BR>
            <BR>
            string reply;<BR>
            client &gt;&gt; reply; // read till the connection is closed and put the result 
            in reply<BR>
            cout &lt;&lt; reply;<BR>
            <BR>
            or</P>
        <P>string reply;<BR>
            reply.resize(1000); // read 1000 bytes and put the result in reply<BR>
            client &gt;&gt; reply;<BR>
            cout &lt;&lt; reply;<BR>
            <BR>
            There&nbsp;are also Read and ReadLine methods for those of you that want this 
            type of functionality.<BR>
            <BR>
            To create a server socket:<BR>
            <BR>
            #include "ServerSocket.h"<BR>
            using namespace MyLib;<BR>
            ...<BR>
            // listen server on port 4321<BR>
            ServerSocket server("MyServerSocket", 4321);<BR>
            // OR<BR>
            //ServerSocket server("MyServerSocket", "myservice");<BR>
            <BR>
            To accept a connection:<BR>
            <BR>
            auto_ptr&lt;ClientSocket&gt; client(server.Accept());<BR>
            cout &lt;&lt; client-&gt;Name() &lt;&lt; endl;<BR>
            // now use client socket as described above</P>
        <P><STRONG>Logging</STRONG></P>
        <P>The default maximum size for a log file is 100k. A backup is created if the 
            limit is exceeded. So, by default, the total space used by log files = 200k. Of 
            course, this limit can be changed by calling Logger::SetMaxLogSize(). Backups 
            are ALWAYS created -- no use reaching a limit and then forgeting the data just 
            sent to the log!</P>
        <P>To use the logger:</P>
        <P>#include "Logger.h"<BR>
            using namespace MyLib;</P>
        <P>// logging to a file<BR>
            Logger::Log.Open("logfile"); // to write to file named "logfile"<BR>
            or<BR>
            Logger::Log.Open("logfile", true); // write to file and console (cerr)<BR>
            or<BR>
            Logger::Log.Open("logfile", true, 200000); // same as above but set logfile max 
            size at 200k bytes<BR>
            <BR>
            If you don't want to log to a file -- i.e. console output only -- then don't do 
            any of the above and all your log statements will automatically go to cerr.</P>
        <P>To log something:</P>
        <P>// '+' is used to concatenate variables for output<BR>
            Logger::Log.Write(string("test 123") + 4 /* int */ + 5.5 /* float */ + 
            string("another string"));</P>
        <P><STRONG>Configuration</STRONG></P>
        <P>A configuration file is similar in format to a Windows .ini file. Comments are 
            denoted by setting the first character of a line to '#'. Sections are bracketed 
            -- for instance, [section1] creates a section named 'section1'. Values are tied 
            to keys in the form key=value. Take a look at test.cfg as a sample 
            configuration file.</P>
        <P>To load a configuration file:</P>
        <P>#include "Config.h"<BR>
            using namespace MyLib;</P>
        <P>Config::Cfg.Load("configfile");</P>
        <P>To access configuration values:</P>
        <P>// get string value of global key 'key'<BR>
            string value = Config::Cfg.GetValue("key");<BR>
            // or integer value<BR>
            int value = Config::Cfg.GetIntValue("key");<BR>
            // or double value<BR>
            double&nbsp;value = Config::Cfg.GetDoubleValue("key");<BR>
            // or boolean value<BR>
            bool value = Config::Cfg.GetBoolValue("key");</P>
        <P><BR>
            // get string value of key 'key' in section 'section'<BR>
            string value = Config::Cfg.GetValue("section","key");<BR>
            // or integer value<BR>
            int value = Config::Cfg.GetIntValue("section","key");<BR>
            // or double value<BR>
            double&nbsp;value = Config::Cfg.GetDoubleValue("section","key");<BR>
            // or boolean value<BR>
            bool value = Config::Cfg.GetBoolValue("section","key");</P>
        <P><STRONG>Finding Files</STRONG></P>
        <P>The C runtime library provides pretty much everything for dealing with files -- 
            can read, write, delete, rename, etc. However, there is one piece that is 
            missing -- finding files! The api implementation on *NIX and Windows are a bit 
            different. As a result, we have our own wrapper for running through a directory 
            (even recursively) and returning files that match a wildcard pattern.</P>
        <P>There are 2 implementations.&nbsp;The first&nbsp;implementation relies on the 
            "hollywood principle" -- i.e. we'll call you or in other words you give a 
            callback and it'll be called for each file matching the wildcard pattern.</P>
        <P>To create a callback:</P>
        <P>#include "FindFile.h"<BR>
            using namespace MyLib;</P>
        <P>// our callback simply&nbsp;writes the found file to the log<BR>
            class MyFindFileHandler : public FindFileHandler
            <BR>
            {
            <BR>
            public:
            <BR>
            bool FileFound(const char* file, bool dir)&nbsp;<BR>
            {
            <BR>
            // file is full path to file, if dir = true, then file is a dir<BR>
            Logger::Log.Write(file);<BR>
            return false; // keep searching
            <BR>
            }
            <BR>
            };</P>
        <P>To find some matching files:</P>
        <P>// create our callback<BR>
            MyFindFileHandler fh;
            <BR>
            // find all files matching *Sock* recursively from the current dir<BR>
            FindFile(".", "*Sock*", fh, true);</P>
        <P>The second implementation does not rely on callback and lets you provide a 
            container for the results and it will search for all matching files, fill the 
            container with the matches and return.</P>
        <P>// create our container -- it's&nbsp;a vector of strings<BR>
            FileList fl;<BR>
            // same as above, but get back results in the FileList<BR>
            FindFile(".", "*Sock*", fl, true);</P>
        <P><STRONG>DLLs</STRONG></P>
        <P>Ok, so we only want to load what we use. The old days of the big monolithic app 
            are gone and we're smarter and have partitioned our app into modules. These 
            modules are called DLLs in the Windows world and Shared Objects (or Dynamic 
            Shared Objects) in the *NIX world. The apis for handling dlls are a bit 
            different, as usual,&nbsp;between the platforms. And, of course, we have our 
            little platform independent wrapper.</P>
        <P>To load a DLL:</P>
        <P>#include "DllLoader.h"<BR>
            using namespace MyLib;</P>
        <P>DllLoader loader("msvcrt.dll");<BR>
            or<BR>
            DllLoader loader("/lib/libc-2.2.5.so");</P>
        <P>To get the address of a symbol:</P>
        <P>// get a symbol from the dll, in this case address of atoi<BR>
            int (*ptr_to_atoi) (const char*) = (int (*) (const char*)) 
            loader.GetAddressOf("atoi");<BR>
            // call atoi via address from dll<BR>
            Logger::Log.Write(string("atoi(\"1234\") = ") + ptr_to_atoi("1234"));
        </P>
        <P>The dll/so is automatically unloaded when the loader object goes out of scope.</P>
        <P><STRONG>Creating DLLs</STRONG></P>
        <P>Creating dlls on *NIX and Windows&nbsp;pretty much works the same way. You need 
            to create your dll interface in a header file and the implementation in a cpp 
            file. The one thing to remember is that you need to disable name mangling on 
            the exported symbols as we're using a C++ compiler which will&nbsp;do that if 
            we don't say otherwise.</P>
        <P>Here's the header and&nbsp;implementation of a simple dll:</P>
        <P>dll.h :</P>
        <P>extern "C"&nbsp;// prevent name mangling so "foo" is really "foo"<BR>
            {<BR>
            int foo();<BR>
            }</P>
        <P>dll.cpp :</P>
        <P>#include "dll.h"</P>
        <P>int foo()<BR>
            {<BR>
            return 1;<BR>
            }</P>
        <P>So, there is the source for our dll on BOTH Windows and *NIX. We now need to 
            create the binary and this is where things are obviously different :)</P>
        <P>On&nbsp;Windows, we NEED to export our foo function. To do this, we use a DEF 
            file:</P>
        <P>dll.def:</P>
        <P>; name of&nbsp;the dll without the extension .dll<BR>
            LIBRARY dll<BR>
            ; we're exporting foo, place each export on a separate line<BR>
            EXPORTS<BR>
            foo</P>
        <P>To compile on Windows:</P>
        <P>cl /MD /LD dll.def dll.cpp</P>
        <P>The /MD says link with the dynamic runtime library. The /LD says to create dll.</P>
        <P>On *NIX, all symbols are exported so all that is needed is the header file for 
            the dll and you're all set.</P>
        <P>To compile on *NIX:</P>
        <P>g++ -fPIC -shared -o dll.so&nbsp;dll.cpp</P>
        <P>The -fPIC says to create "Position Indepedent Code" which can be loaded anywhere 
            in the address space of the process. The -shared says to create a dll/so. The 
            -o names the output file.</P>
        <P><STRONG>Persistent Shared Memory (or Memory Mapped Files)</STRONG></P>
        <P>A neat feature that is present in both *NIX and Windows is the ability to map 
            the contents of a file into memory thereby allowing you to go through 
            its&nbsp;data via a memory pointer. This is a lot more niftier than using than 
            using the C library read/write calls, not to mention that it's quite a bit 
            faster as we're rumaging through memory. Both implementations rely on the 
            operating system to flush "dirty" pages back to disk thereby keeping memory and 
            disk data in sync -- one of the routine jobs the OS performs. (There is also an 
            API on both platforms for "forcing" the flushing of dirty pages on an opened 
            mapping -- when a mapping is closed, a flush automatically takes place.)</P>
        <P>To create a mapping:</P>
        <P>#include "MemoryMappedFile.h"<BR>
            using namespace MyLib;</P>
        <P>// pointer we will use to address the file as a chunk of memory<BR>
            char* map;<BR>
            // create a mapping using filename "mmfile" of size 1024<BR>
            MemoryMappedFile mf("mmfile", map, 1024);<BR>
            // fill data with 'X's
            <BR>
            for(int i = 0;i &lt; 1024;i++)<BR>
            map[i] = 'X';
        </P>
        <P>The mapping is automatically closed and the data flushed when the object 
            instance goes out of scope (or is deleted if heap created.)</P>
        <P>Sharing between threads and processes is automatic! If you create additional 
            threads and/or processes that would like to update the mapping, have each of 
            them open the same map file. The operating system will ensure that the pages 
            are shared amongst the threads/processes (actually, this is all hidden in the 
            implementation -- it's optional, but since this is MY implementation, I've 
            decided on sharing as the default!) Of course, as with any shared region, you 
            should use a mutex to control updates if needed.</P>
        <P>Memory-mapped files have a practical limitation of 2GB in size. But if you're 
            using memory-mapped files to keep a database of some sort, I suggest you take a 
            look at QDBM. It is an open-source fast, scalable, and persistent hash (similar 
            to Berkeley DB a.k.a. SleepyCat except it's FREE for commercial use!) The QDBM 
            website is here:</P>
        <P><A href="http://qdbm.sourceforge.net/">http://qdbm.sourceforge.net/</A></P>
        <P><STRONG>Persistent Hash (QDBM Interface)</STRONG></P>
        <P>The last section mentioned a persistent hash called QDBM (Quick DataBase 
            Manager). It is&nbsp;sometimes necessary to keep a persistent hash and we 
            usually end up choosing some SQL relational database implementation to do the 
            job. Most often, this is overkill. What we really want is a simple, fast, and 
            scalable data store WITHOUT all the baggage that comes with making a RDBMS work 
            for us. Just give us a header file and a lib for pete's sake! :) I have chosen 
            QDBM for my needs. The maximum database size "should be" 1TB -- more than we 
            should ever need :) It is licensed under the LGPL and the source can be 
            integrated freely into your application.&nbsp;As a result, I've created my own 
            wrapper class for QDBM.</P>
        <P>To open a hash database:</P>
        <P>#include "PersistentHash.h"<BR>
            using namespace MyLib;</P>
        <P>// the QDBM implementation stores its files in a dir<BR>
            PersistentHash phash("phashdir");
        </P>
        <P>To store data:</P>
        <P>
            // key and&nbsp;value can be non-null terminated strings -- i.e. binary<BR>
            phash.Store(key /* string value holding key */, value /* string value holding 
            value */);</P>
        <P>To retrieve data:</P>
        <P>
            //&nbsp;key and&nbsp;value can be non-null terminated strings -- i.e. binary<BR>
            phash.Retrieve(key /* string value holding key */, value /* value to hold 
            result&nbsp;*/);<BR>
            <BR>
            To delete:</P>
        <P>
            phash.Delete(key /* string */);</P>
        <P><STRONG>Persistent Priority Queue (QDBM Interface)</STRONG></P>
        <P>Another way to use the QDBM library is to persist records in a particular order. 
            For instance, a priority queue stores records in an order that is dependent on 
            some user-defined priority number. A FIFO queue implementation would set the 
            priority of all items to be the same. Again, the maximum database size "should 
            be" 1TB.</P>
        <P>To open a queue database:</P>
        <P>#include "PersistentQueue.h"<BR>
            using namespace MyLib;</P>
        <P>PersistentHash phash("pqueuefile");
        </P>
        <P>To enqueue data:</P>
        <P>pqueue.Enqueue(key /* string value holding key */, value /* string value 
            holding&nbsp;value */, priority /* int */);<BR>
            <BR>
            To dequeue data:</P>
        <P>phash.Dequeue(value /* string value&nbsp;to hold data&nbsp;*/);<BR>
            <BR>
            To remove:</P>
        <P>phash.Remove(key /* string value holding key */);</P>
        <P>For a priority FIFO, the keys MUST be incremental as records are sorted by 
            priority, key. For example, storing 2 records as follows [key=1, pri=1] then 
            [key=0, pri=1] will cause the the second record to be retrieved before the 
            first as they both have the same priority (in which case the code uses the keys 
            to decide which one gets out first.)</P>
        <P><STRONG>Small, Fast embedded SQL DB (SQLite Interface)</STRONG></P>
        <P>So, we want a small, fast, embedded SQL DB WITHOUT the headaches of installing, 
            setting up, and managing a separate db server. This is where SQLite comes in: <A href="http://www.hwaci.com/sw/sqlite/">
                http://www.hwaci.com/sw/sqlite/</A> It is a 100% FREE implementation of a 
            relational database that you can embed in your applications. The interface, as 
            with QDBM, is C-based. To automatically take care of session and memory 
            management as well as providing an object-oriented interface to this library is 
            what we'll do here. The maximum database size "should" be 2TB.</P>
        <P>To create a SQL database:</P>
        <P>#include "SQLDatabase.h"<BR>
            using namespace MyLib;</P>
        <P>SQLDatabase db("test.db");
        </P>
        <P>Create a couple of tables and indexes:<BR>
            <BR>
            // SQLite is typeless!&nbsp;-- column types are optional :)
            <BR>
            db.Update(string("create table table1 (int)"));
            <BR>
            db.Update(string("create unique index table1_int_idx on table1(int);"));
            <BR>
            db.Update(string("create table table2 (int, double, string, date)"));
            <BR>
            db.Update(string("create index table2_int_idx on table2(int)"));
        </P>
        <P>Insert some values:</P>
        <P>// SQLite is typeless -- all data are strings!
            <BR>
            db.Update(string("insert into table1 values ('1')"), false);
            <BR>
            db.Update(string("insert into table1 values ('2')"), false);
            <BR>
            // dates are really stored as strings of form
            <BR>
            // YYYYMMDDHH24MISS
            <BR>
            db.Update(string("insert into table2 values ('1', '3.2', 'string4', 
            '20031223081533')"), false);
            <BR>
            db.Update(string("insert into table2 values ('1', '3.3', 'string5', 
            '20031223081534')"), false);
            <BR>
            // true at end of this statement = COMMIT<BR>
            db.Update(string("insert into table2 values ('2', '3.4', 'string6', 
            '20031223081535')"), true);
        </P>
        <P>Query the database:</P>
        <P>// join 2 tables on key field
            <BR>
            SQLDatabase::ResultSet rs = db.Query(string("select table1.int, table2.double, 
            table2.string, table2.date from table1, table2 where table1.int = 
            table2.int;"));
            <BR>
            // print out result of join
            <BR>
            while(rs.Next())
            <BR>
            {
            <BR>
            Logger::Log.Write(rs.GetInt(string("table1.int")) + string(",") + 
            rs.GetDouble(string("table2.double")) + string(",") + 
            rs.GetString(string("table2.string")) + string(",") + 
            rs.GetDate(string("table2.date")));
            <BR>
            }
        </P>
        <P><STRONG>Encryption</STRONG></P>
        <P>Being that this library is about storing and communicating data for the most 
            part, it is a good idea to be able to protect this data. After some looking 
            around, I have found that AES (Advanced Encryption Standard) is the preferred 
            means&nbsp;of encryption today. There are several implementations out there but 
            I have decided to use Brian Gladman's source (<A href="http://fp.gladman.plus.com/cryptography_technology/rijndael/">http://fp.gladman.plus.com/cryptography_technology/rijndael/</A>) 
            as it seems the most flexible and easiest to wrap (it also used by several 
            commercial products such as WinZip.)</P>
        <P>As usual, we want things to be as simple as possible. Encrypting any data should 
            simply mean providing a key/password and calling a function to encrypt and 
            doing likewise for decrypting.</P>
        <P>To encrypt:</P>
        <P>#include "Encryption.h"<BR>
            using namespace MyLib;</P>
        <P>Encryption e(string("encryption key"));<BR>
            char data[] = "encrypt this";<BR>
            // MyLib::ByteArray = vector&lt;char&gt;<BR>
            ByteArray a = e.Encrypt(data, strlen(data));<BR>
            // a contains a sequence of bytes representing the encrypted data</P>
        <P><STRONG>Note: </STRONG>the encrypted data could be MORE than the input data due 
            the nature of AES's implementation. (AES falls into a category of encryption 
            algorithms that outputs encrypted data in blocks of a certain size. In the case 
            of AES, the standard block size is 16 bytes. So, the minimum output from an 
            encryption run is 16 bytes even if the data is just 1 byte.)</P>
        <P>To decrypt:</P>
        <P>// continuing from above<BR>
            ByteArray b = e.Decrypt(&amp;a[0], a.size());<BR>
            // b now contains "encrypt this" -- the original string we encrypted</P>
        <P>As mentioned in the note above, AES is a block-based cipher scheme. As a result, 
            data is outputted in blocks of 16 byte chunks. Since the last chunk might be 
            LESS THAN 16 bytes, the data needs to be "padded" to bring it up to 16 bytes. 
            The decrypting side needs to be aware of this "padding" and remove it while 
            decrypting. This is all taken cared of in this implementation.</P>
        <P>This class can also be used to create SHA1 and MD5 digests.</P>
        <P><STRONG>Encoding</STRONG></P>
        <P>Since we deal with binary data quite frequently, it is often handy to encode 
            this in a form that can be handled by ASCII-based programs/utilities/protocols. 
            There are many schemes for encoding binary data into ASCII. The de-facto 
            standard for the web is base64 encoding. A more direct and easier approach is 
            hex encoding -- converting each binary character to its 2 character hexadecimal 
            equivalent. (The drawback to this approach of course is that it doubles the 
            size of the original data.) Base64 is more compact but a bit more involved to 
            implement. Anyway, we have implemented both schemes :)</P>
        <P>To hex encode data:</P>
        <P>#include "Encoding.h"<BR>
            using namespace MyLib;</P>
        <P>string str = Encoding::ToHexString(data /* const char*/, len /* int */);</P>
        <P>To decode:</P>
        <P>ByteArray bin = Encoding::FromHexString(hexString /* string */);</P>
        <P>Base64 encode:</P>
        <P>string str = Encoding::ToBase64(data /* const char*/, len /* int */);</P>
        <P>Base64 decode:</P>
        <P>ByteArray bin = Encoding::FromHexString(base64String /* string */);</P>
        <P>This class can also be used to perform URL encoding and decoding.</P>
        <P><STRONG>String Tokenizer</STRONG></P>
        <P>We often need to split apart a string, usually representing parts of a record, 
            into its constituent parts. For example, say we're reading an ASCII file of 
            records where each line represents a record and has the format: 
            field1;field2;field3;field4;field5. In order to properly process the preceding 
            record format, we need extract the individual fields within the record. 
            Unfortunately, there is no splitter or string tokenizer class that is part of 
            the standard C++ library :( As a result, we've created our own aptly named 
            StringTokenizer that handles this functionality for us.</P>
        <P>To tokenize a string:</P>
        <P>#include "StringTokenizer.h"<BR>
            using namespace MyLib;</P>
        <P>StringTokenizer tokenizer(string("1;2;3;4;5"), ';');<BR>
            for(int i = 0;i &lt; tokenizer.NumTokens();i++)<BR>
            {<BR>
            // access tokenizer[i] or tokenizer.GetToken(i)<BR>
            }</P>
        <P>Note that an empty string has 0 tokens; a string with a single token, returns 
            that token (eg. "1" = "1"); otherwise the number of tokens = number of 
            delimiters + 1 (eg. "1;" = 2 tokens "1" and "", ";" = 2 tokens of 2 empty 
            strings.)</P>
        <P><STRONG>Processes</STRONG></P>
        <P>See Process.h. Allows you to run any process, pass it input, and read its ouput 
            -- lots of existing progs out there we'd just like to reuse :) and sometimes 
            the prog is web server, so we have the stuff below for that...</P>
        <P><STRONG>HTTP Client (with SSL support)</STRONG></P>
        <P>See HTTPClient.h. Allows you create HTTP connections to GET/POST data.</P>
        <P><STRONG>SMTP Client</STRONG></P>
        <P>Coming up next...</P>
        <P><STRONG>POP3 Client</STRONG></P>
        <P>Coming after SMTP client...</P>
        <P><STRONG>Regular Expressions</STRONG></P>
        <P>Coming after POP3 client...</P>
        <P><STRONG>Notes</STRONG></P>
        <P>Using a separate namespace ("MyLib") prevents any potential conflicts with the 
            global namespace. It is also recommended for library writers to 
            use&nbsp;namespaces.&nbsp;If the syntax is a little too verbose, then a "using 
            namespace MyLib" directive can always be used (prior to a sequence of calls to 
            methods of MyLib classes) but be wary of conflicts.</P>
        <P>Note that every object above takes a string parameter as the first parameter in 
            the constructor, this helps with two things: 1) when debugging or tracing, the 
            object's name can serve as an easy identifier, and 2) it helps document the 
            code by giving meaningful names to resources.</P>
        <P>Also, note that in all the code above there is NEVER a call to operator delete 
            NOR any call to a Close() or other destructor function! This is because we 
            abide by the rule "resource acquisition is initialization" -- see Stroustrup's 
            page about this here: <A href="http://www.research.att.com/~bs/bs_faq2.html#finally">
                http://www.research.att.com/~bs/bs_faq2.html#finally</A>. If you must 
            create any of the above items on the heap, please use the standard C++ library 
            auto_ptr template class.</P>
        <P>As far as exceptions, our Exception class derives from runtime_error and you can 
            ALWAYS catch this or the standard C++ exception which is the base class for all 
            exceptions. However, you might want to use Exception since it encapsulates the 
            message, the error code, and the system error message. The C++ exception 
            classes do not have a placeholder for a error code since error codes are really 
            application-defined. One could argue that the existing string component of 
            exception is ok as a message is really not tied any application implementation. 
            Anyway, if you want an error code, catch Exception; if you don't care and are 
            used to exception and runtime_error, then you catch these (the message will 
            include the error code and system message as well.)</P>
        <P>&nbsp;</P>
    </BODY>
</HTML>
