/// Console keyboard handling. WCP library usage example.
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

#include "wcp/stlex.h"
#include "wcp/functional.h"
#include "wcp/singleton.h"


// ConsoleManager is responsible for reading and firing console keyboard events,
// i.e. event source.
class ConsoleManager : public wcp::singleton<ConsoleManager>
{
    friend class wcp::singleton<ConsoleManager>;

    ConsoleManager() throw() { }

public:

    // Keyboard event type definition.
    // Note: Event delegate return wcp::event_result which means that
    // event can be canceled. Any other return value type would mean
    // that event cannot be canceled.
    typedef wcp::multicast_event<wcp::event_result, int> kbd_event_t;

    // Keyboard event object
    kbd_event_t KeyboardEvent;

    // Enters keyboard reading loop
    void EnterKeyoardLoop()
    {
        for(;;)
        {
            // Read a character from keyboard
            int ch = _getch();

            std::tcout <<TEXT("Fire KeyboardEvent with character code 0x") <<std::hex <<ch <<std::endl;

            // Fire event and check if event is canceled
            if(!KeyboardEvent(ch))
                std::tcout <<TEXT("Event has been canceled.") <<std::endl;
        }
    }
};




// ConsoleKbdHandler is a named console event consumer.
class ConsoleKbdHandler
{
    // Handler name
    std::tstring name;

public:

    // Constructs a handler object
    explicit ConsoleKbdHandler(const wcp::tchar_t* name)
        : name(name)
    {
        // Subscribe OnChar method to receiving console keyboard events
        ConsoleManager::instance().KeyboardEvent += ConsoleManager::kbd_event_t
                ::delegate_t(*this, &ConsoleKbdHandler::OnChar);
    }

    ~ConsoleKbdHandler()
    {
        // Unsubscribe OnChar method from receiving console keyboard events
        ConsoleManager::instance().KeyboardEvent -= ConsoleManager::kbd_event_t
                ::delegate_t(*this, &ConsoleKbdHandler::OnChar);
    }

    // Receives console keyboard events
    wcp::event_result OnChar(int ch)
    {
        // Default event result means that all previously subscribed delegates
        // will get invoked until one returns wcp::event_result_canceled.
        wcp::event_result result = wcp::event_result_default;

        std::tcout <<TEXT("\t") <<name <<TEXT(": ");

        if(ch != VK_ESCAPE)
            std::tcout <<TEXT("OnChar( 0x") <<std::hex <<ch <<(" )");
        else
        {
            std::tcout <<TEXT("Escape button click event propagation is canceled.");
            result = wcp::event_result_canceled;
        }

        std::tcout <<std::endl;

        return result;
    }
};

int _tmain(int argc, TCHAR* argv[])
{
    std::tcout <<TEXT("Press any button to test multicast event handlings.\r\n")
               TEXT("Press Escape to test event cancelation.\r\n")
               TEXT("Press Ctrl+Break to exit application.") <<std::endl;

    // Create 3 named console keyboard event handlers
    ConsoleKbdHandler handler1(TEXT("event subscriber #1"));
    ConsoleKbdHandler handler2(TEXT("event subscriber #2"));
    ConsoleKbdHandler handler3(TEXT("event subscriber #3"));

    // Enter keyboard event loop
    ConsoleManager::instance().EnterKeyoardLoop();

    return EXIT_SUCCESS;
}
