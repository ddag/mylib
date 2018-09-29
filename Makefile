# the NICE thing about this makefile is that it DOES keep track
# of dependencies AND i DONT have to manually add file
# dependencies (i.e. specify source and object files) as it is 
# all automatic! -- add the source file to the directory 
# (or any matching the wildcard for the allCppMakes var) 
# and we're done!

# Configuration section -------------------------------------------------------
outputName := MyLibTest # Filename to give to final executable
cppExtension := .cpp         # File extension of source files
cExtension := .c         # File extension of source files
CPPFLAGS :=                  # List flags to pass to C/C++ preprocessor
CXXFLAGS :=                  # List flags to pass to C++ compiler
LDFLAGS :=                   # List flags to pass to linker
CC := gcc -DTHREADSAFE=1 -DMYPTHREAD -I./QDBM -I./SQLite -I./Encryption -I./Encoding
# solaris needs libsocket
#LDLIBS := -lpthread -lsocket -lnsl -lrt -ldl                # List additional system libraries to link with
# linux doesn't have libsocket
LDLIBS := -lpthread -lnsl -lrt -ldl               # List additional system libraries to link with
INC := -I. -I.. -I./SocketImp -I./UNIX -I./QDBM -I./SQLite -I./Encryption -I./Encoding

default : incremental

# Generate intermediates variables and files ----------------------------------
CXXFLAGS := ${INC} -Wall -O0 -MMD # update the makefiles on every compilation

# Create a lists of object and .d files to create - one for each source file in the directory
allCppMakes := ${patsubst %${strip ${cppExtension}},%.d,${wildcard *${cppExtension} SocketImp/*${cppExtension} UNIX/*${cppExtension}}}
allCppObjects := ${allCppMakes:.d=.o} 

allCMakes := ${patsubst %${strip ${cExtension}},%.d,${wildcard QDBM/*${cExtension} SQLite/*${cExtension} Encryption/*${cExtension} Encoding/*${cExtension}}}
allCObjects := ${allCMakes:.d=.o} 

# make any non-existant make files (using the g++ preprocessor)
${allCppMakes} : %.d : %${cppExtension}
	${CXX} ${INC} ${CPPFLAGS} -MM $< > $@ 

${allCMakes} : %.d : %${cExtension}
	${CXX} ${INC} ${CPPFLAGS} -MM $< > $@ 

ifneq (${MAKECMDGOALS},clean)
include ${allCMakes} # include the generated make files, which make the object files
include ${allCppMakes} # include the generated make files, which make the object files
endif

# Final targets ---------------------------------------------------------------
.PHONY : clean incremental full

full : clean incremental

incremental : ${outputName}

clean :
	rm -f *.o *~ *.d
	rm -f UNIX/*.o UNIX/*~ UNIX/*.d
	rm -f SocketImp/*.o SocketImp/*~ SocketImp/*.d
	rm -f QDBM/*.o QDBM/*~ QDBM/*.d
	rm -f SQLite/*.o SQLite/*~ SQLite/*.d
	rm -f Encryption/*.o Encryption/*~ Encryption/*.d
	rm -f Encoding/*.o Encoding/*~ Encoding/*.d
	rm ${outputName} core

${outputName} : ${allCObjects} ${allCppObjects}
	g++ ${INC} -o $@ $^ ${LDFLAGS} ${LDLIBS}


