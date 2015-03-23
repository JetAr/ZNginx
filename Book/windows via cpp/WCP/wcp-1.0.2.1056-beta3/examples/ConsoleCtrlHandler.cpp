/// Console control handling. WCP library usage example.
///
/// Copyright (c) 2009-2010
/// ILYNS. http://www.ilyns.com
///
/// Copyright (c) 2009-2010
/// Alexander Stoyan
///
/// Follow WCP at:
///      http://wcp.codeplex.com/ - latest releases
///      http://alexander-stoyan.blogspot.com/ - blog about WCP and related stuff

#include <tchar.h>
#include <conio.h>

#include <Windows.h>

#include "wcp/wcplib.h"
#include "wcp/stlex.h"
#include "wcp/functional.h"

// Console handler delegate type definition
typedef wcp::unicast_delegate<bool, wcp::dword_t> console_handler_delegate_t;

// Console handler global delegate
static console_handler_delegate_t gConsoleHandler;

// Native console handler routine
static BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
    // Call console handler delegate if not empty
    return gConsoleHandler ? gConsoleHandler(dwCtrlType) : false;
}





class ConsoleHandler
{
    std::tstring name;
    bool appShutdown;

public:

    // Constructs a named console control handler object
    explicit ConsoleHandler(const wcp::tchar_t* name)
        : name(name) { }

    // Handles console control events
    bool OnCtrlCode(wcp::dword_t code)
    {
        if(appShutdown) // Do not handle console events after application shutdown
        {
            // Console event names
            static const wcp::tchar_t* eventNames[] =
            {
                __TOTSTR(CTRL_C_EVENT),
                __TOTSTR(CTRL_BREAK_EVENT),
                __TOTSTR(CTRL_CLOSE_EVENT),
                TEXT("<reserved #3>"),
                TEXT("<reserved #4>"),
                __TOTSTR(CTRL_LOGOFF_EVENT),
                __TOTSTR(CTRL_SHUTDOWN_EVENT)
            };

            // Print out console event information
            std::tcout <<TEXT("OnCtrlCode[\"") <<name <<TEXT("\"]: ")
                       <<eventNames[code] <<std::endl;

            // Determine if application is being shutted down
            appShutdown =
                !(code == CTRL_C_EVENT
                  || code == CTRL_BREAK_EVENT
                  || code == CTRL_CLOSE_EVENT);
        }

        return true;
    }

    // Some dummy application loop
    void EnterLoop()
    {
        for(appShutdown = true; appShutdown;)
            Sleep(100);
    }

    // Initializes application shutdown process
    void ShutdownApp()
    {
        // Print out some shutting down progress information
        std::tcout <<std::endl;
        const wcp::tchar_t progressSyms[] = TEXT("|/-\\");
        for(int i = 0; i <= 100; ++i)
        {
            wcp::tchar_t progressSym = progressSyms[i % (ARRAYSIZE(progressSyms) - 1)];
            std::tcout <<TEXT("\rShutting down application: ")
                       <<progressSym <<i <<TEXT("% complete...   ");
            Sleep(50);
        }

        std::tcout <<TEXT("\rComplete.                                   ") <<std::endl;
    }
};



int _tmain(int argc, TCHAR* argv[])
{
    // Try to register global console control handler
    if(!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
        std::tcout <<TEXT("Error: Cannot install console control handler.") <<std::endl;
    else
    {
        std::tcout <<TEXT("Console control handler installed ...") <<std::endl;
        std::tcout <<TEXT("[Press Ctrl+C or Ctrl+Break to shutdown application]") <<std::endl <<std::endl;

        // Assigns a console control handler object
        ConsoleHandler handler(TEXT("ConsoleHandler #1"));
        gConsoleHandler = console_handler_delegate_t(handler, &ConsoleHandler::OnCtrlCode);
        handler.EnterLoop();
        handler.ShutdownApp();

        // Unregister global console control handler
        SetConsoleCtrlHandler(HandlerRoutine, FALSE);
    }

    return EXIT_SUCCESS;
}
