#include "Local.h"
#include <csignal>
#include <iostream>

#ifndef _DEBUG
#include <execinfo.h>

// Return a string with a backtrace
std::string GetTrace (void)
{
    void *array[15];
    size_t size;
    char **strings;
    std::string StringTrace;

    size = backtrace (array, 15);
    strings = backtrace_symbols (array, size);
     
    for (size_t i = 0; i < size; i++)
    {
       StringTrace += strings[i];
       StringTrace += "\n";
    }

    free (strings);
    return StringTrace;
}
#endif

// The following is a gdb command that should be able 
// to allow the handler to reach this function
// handle <signal> nostop nopass
// it shouldn't be needed if the debugger is attached
// since it already handles signals

void Handler (int param)
{
    ServerPrintf("Signal triggered (%i)\n", param);


#ifdef _DEBUG
    raise(SIGTRAP);
#else

    ServerPrintf("CleanCode has encountered a problem. Printing trace\n"
    		 "If possible, send a report of this issue at http://code.google.com/p/cleancodequake2/\n"
		 "detailing server conditions, reproduction of the issue and additional data you think\n"
		 "might be useful.\n");
    std::string Trace = GetTrace();

    ServerPrintf("Backtrace:\n"
    		 "%s", Trace.c_str());
    
    exit(1);
#endif
}

class sighandle_assign
{
    public:
    	// Constructor hack.
	// Unix has no 'APIENTRY DllMain' so instead we assign signal handlers
	// with an object constructor, called when game<arch>.so is loaded.
        sighandle_assign()
	{
		// Signals INT and TERM
		// shouldn't be needed since these
		// are handled with no issues by the server
		signal(SIGSEGV, Handler);
		signal(SIGILL , Handler);
		signal(SIGFPE , Handler);
		signal(SIGABRT, Handler);
	}
}sighan_autoasobj;
