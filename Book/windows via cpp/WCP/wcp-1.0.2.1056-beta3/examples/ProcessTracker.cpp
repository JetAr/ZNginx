/// Process tracker. WCP library usage example.
///
/// The program is a short example of how efficient and concise programs can be
/// when WCP, STL and Windows API are combined. Note: A project can be compiled
/// in UNICODE as well as ANSI character set.
///
/// A program is a console process tracker. You can create up to 10 trackers.
/// Each tracker waits until a tracked process finishes, after that a chosen
/// tracker action is performed, either alert is shown or command line is run.
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

//
// CRT headers
//
#include <tchar.h>
#include <process.h>

//
// STL headers
//
#include <vector>
#include <algorithm>
#include <map>
#include <iterator>

//
// WCP headers
//
#include "wcp/wcplib.h"
#include "wcp/errh.h"
#include "wcp/stlex.h"
#include "wcp/handles.h"
#include "wcp/stringi.h"
#include "wcp/strtools.h"
#include "wcp/smartptr.h"
#include "wcp/threadpool.h"
#include "wcp/sync.h"
#include "wcp/reference.h"

//
// Windows headers & libraries
//
#include <Shellapi.h>
#pragma comment(lib, "Shell32")

#include <Tlhelp32.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi")

//******************************************************************************
// Global type definitions and variables
//******************************************************************************

// Vector of case-insensitive strings
typedef std::vector<std::tstringi> strings_t;

// Console command error code
enum command_error_t
{
    CommandOk,
    CommandUnknown,
    CommandInvalidArgs
};

// Console command handler
typedef command_error_t(*command_handler_t)(const strings_t& args);

// Console command information
struct command_info_t
{
    const wcp::tchar_t* commandName;
    command_handler_t handler;
    int args_required; // -1 - various argument number
};

// Next process tracker identifier
static wcp::uint_t gNextTrackerId = 0;

// Process tracker action identifier
enum tracker_action_t
{
    TrackerActionShowAlert,
    TrackerActionRunCommand,
};

// Process information smart pointer
typedef wcp::shared_ptr<PROCESSENTRY32> processinfo_ptr_t;

// Process tracker information
struct tracker_info_t
{
    wcp::uint_t id; // Tracker identifier
    wcp::dword_t timeout; // Tracker timeout
    wcp::handles::handle_t hTrackedProcess; // Tracked process handle
    tracker_action_t action; // Tracker action identifier
    std::tstring actionData; // Tracker action data. Either alert message or command line
    processinfo_ptr_t pinfo; // Tracked process information pointer
    bool stop_track; // if true, a tracker has to stop.
};

// Tracker information smart pointer
typedef wcp::shared_ptr<tracker_info_t> tracker_info_ptr_t;

// Tracker infomation list
typedef std::list<tracker_info_ptr_t> tracker_list_t;

// Map of tracker infomation lists per process
typedef std::map<wcp::dword_t, tracker_list_t> tracker_map_t;
// Map of tracker infomation lists per process
static wcp::synchronized<tracker_map_t> gTrackerMap;

// Tracker-process assiciation
typedef std::map<wcp::uint_t, wcp::dword_t> tracker_process_assoc_t;
// Map of tracker-process assiciations
static tracker_process_assoc_t gTrackerAssocs;

// Tracker threadpool 
static wcp::threadpool gTrackerThreadPool;

// Process information map
typedef std::map<wcp::dword_t, wcp::shared_ptr<PROCESSENTRY32> > processe_map_t;
// Process information map
static processe_map_t gProcesseMap;

//******************************************************************************

// Prints out program help screen
static void PrintUsage()
{
    std::tcout <<TEXT("Process Tracking Console. WCP case study. http://wcp.codeplex.com\r\n")
                 TEXT("Copyright (C), ILYNS. 2010. http://www.ilyns.com\r\n\r\n")
                 TEXT("Console commands (command are case-insensitive):\r\n\r\n")
                 TEXT("\thelp|?\r\n\t - Print this screen.\r\n\r\n")
                 TEXT("\tlist|* [by-name|bi-pid]\r\n\t - Print process list and sort by process id or name.\r\n\r\n")
                 TEXT("\ttrack|+ pid timeout [alert|command] {parameters}\r\n\t - Add a new process tracker (max 10 trackers available):\r\n")
                 TEXT("\t\tpid\t- Process id to track;\r\n")
                 TEXT("\t\ttimeout\t- Tracker timeout in milliseconds;\r\n")
                 TEXT("\t\talert\t- Show alert dialog;\r\n")
                 TEXT("\t\tcommand\t- Execute command line.\r\n")
                 TEXT("\t\tparameters\t- Depending on tracker action, either alert message or command line.\r\n\r\n")
                 TEXT("\tstop-track|- tracker_id\r\n\t - Stop a process tracker:\r\n")
                 TEXT("\t\ttracker_id\t- Pracker id to stop.\r\n\r\n")
                 TEXT("\ttrackers|% [pid]\r\n\t - List registered trackers:\r\n")
                 TEXT("\t\tpid\t- Optional. Process id to list trackers for.\r\n\r\n")
                 TEXT("\trun|@ {command_line}\r\n\t - Run command line.\r\n\r\n")
                 TEXT("\texit\r\n\t- exit application.")
                 <<std::endl;
}

//------------------------------------------------------------------------------

// Prints out an error message
static void ReportError(const wcp::tchar_t* pmsg)
{ std::tcout <<"Error: " <<pmsg <<std::endl <<std::endl; }

//------------------------------------------------------------------------------

// Refreshes the global process map
static void RefreshProcessMap()
{
    gProcesseMap.clear();

    // Create ToolHelp handle
    wcp::handles::handle_t hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot == INVALID_HANDLE_VALUE)
        throw wcp::win_exception(GetLastError());

    // Enumerate active processes
    processinfo_ptr_t processInfo(new PROCESSENTRY32);
    processInfo->dwSize = sizeof(PROCESSENTRY32);
    if(Process32First(hSnapshot, processInfo.get()))
    {
        do
        {
            // Add a process information to the global process map
            gProcesseMap[processInfo->th32ProcessID] = processInfo;

            processInfo.reset(new PROCESSENTRY32);
            processInfo->dwSize = sizeof(PROCESSENTRY32);
        }
        while(Process32Next(hSnapshot, processInfo.get()));
    }

    // Note: No CloseHandle needed.
}

//------------------------------------------------------------------------------

// Main tracker procedure
void TrackerProcedure(tracker_list_t::value_type pti)
{
    const wcp::dword_t WaitTimeout = 100;

    tracker_info_t& ti = *pti;
    wcp::thread& thread = wcp::thread::get_current();

#ifdef _DEBUG
    {
        // Set thread name for debug purposes
        std::ostringstream threadName;
        threadName <<"Tracker #" <<ti.id <<" thread";
        wcp::set_thread_name(threadName.str().c_str());
    }
#endif

    // Wait untial a tracked process finishes or tracker stop stop flag is set
    wcp::dword_t waitStatus;
    wcp::dword_t timeout = ti.timeout;
    do
    {
        timeout -= __min(timeout, WaitTimeout);
        waitStatus = WaitForSingleObject(ti.hTrackedProcess, WaitTimeout);
    }
    while(waitStatus == WAIT_TIMEOUT 
        && !ti.stop_track 
        && timeout != 0
        //&& !thread.get_stop_flag()
        );

    // If a tracked process is fihished, perform tracker action
    if(waitStatus == WAIT_OBJECT_0)
    {
        // Implement tracker action function in-place
        struct traker_actions
        {
            // Shows a tracker alert
            static void alert(tracker_list_t::value_type pti)
            {
                std::tstring strid, strpid;
                wcp::obj2str(pti->id, strid);
                wcp::obj2str(pti->pinfo->th32ProcessID, strpid);

                std::tstring message;
                wcp::format_string(message, TEXT("Process tracker #{0} alert.\n")
                    TEXT("Process '{1}' [{2}] has stopped work.\n\nMessage:\n\t{3}"),
                    strid.c_str(), pti->pinfo->szExeFile, strpid.c_str(), 
                    pti->actionData.c_str());
                MessageBox(NULL, message.c_str(), TEXT("Process Tracker"), 
                    MB_ICONINFORMATION | MB_SYSTEMMODAL | MB_NOFOCUS);
            }

            // Executes command line
            static void command(tracker_list_t::value_type pti)
            { _tsystem(pti->actionData.c_str()); }
        };

        // Execute tracker action
        switch(pti->action)
        {
        case TrackerActionShowAlert:
            NONTHROWABLE(traker_actions::alert(pti));
            break;
        case TrackerActionRunCommand:
            NONTHROWABLE(traker_actions::command(pti));
            break;
        }
    }

    // Remove tracker. Note: Operation is synchronous.
    LOCK(gTrackerMap)
    {
        // Remove tracker from tracker map
        wcp::dword_t pid = ti.pinfo->th32ProcessID;
        tracker_map_t::value_type::second_type& list = (*gTrackerMap)[pid];
        for(tracker_list_t::iterator iter = list.begin();
            iter != list.end(); ++iter)
        {
            if((**iter).id == ti.id)
            {
                list.erase(iter);
                break;
            }
        }

        // Remove tracker-process association
        gTrackerAssocs.erase(ti.id);
        ti.id = 0;

        // Reset tracker ID counter is no more trackers left
        if(list.empty())
        {
            (*gTrackerMap).erase(pid);
            if((*gTrackerMap).empty())
                gNextTrackerId = 0;
        }
    }
}

//******************************************************************************
// Command handlers
//******************************************************************************

// 'help' command handler.
static command_error_t oncmd_help(const strings_t&)
{ 
    PrintUsage(); 
    return CommandOk;
}

//------------------------------------------------------------------------------

// 'list' command handler.
// Command syntax:
//  list|* [by-name|by-pid]
//      - by-name   - [optional] Sort processes by name
//      - by-pid    - [optional] Sort processes by PID
static command_error_t OnCommand_List(const strings_t& args)
{ 
    // Process sort order
    enum 
    {
        SortByName,
        SortByPID,

    } sortBy = SortByName;

    // Read optional sort order switch
    switch(args.size())
    {
    case 1:
        break;
    case 2:
        if(args.at(1) == TEXT("by-name"))
            sortBy = SortByName;
        else if(args.at(1) == TEXT("by-pid"))
            sortBy = SortByPID;
        else
            return CommandInvalidArgs;
        break;
    default:
        return CommandInvalidArgs;
    }

    // Implement process list sort predicates in-place
    struct predicates
    {
        // '<' operator for process names
        static bool pe_name_less(const processinfo_ptr_t& lhs,
                                 const processinfo_ptr_t& rhs)
        { return _tcsicmp(lhs->szExeFile, rhs->szExeFile) < 0; }

        // '<' operator for process PIDs
        static bool pe_PID_less(const processinfo_ptr_t& lhs,
                                const processinfo_ptr_t& rhs)
        { return lhs->th32ProcessID < rhs->th32ProcessID; }
    };

    // Refresh processes
    RefreshProcessMap();

    // Fill list of processes
    typedef std::list<processe_map_t::value_type::second_type> process_list_t;
    process_list_t processes;

    for(processe_map_t::const_iterator iter = gProcesseMap.begin(),
        end = gProcesseMap.end(); iter != end; ++iter)
    { 
        processes.push_back((*iter).second);
    }

    // Sort process list by a sort order specified
    bool(*sortPredicate)(const processinfo_ptr_t&, 
                         const processinfo_ptr_t&);
    switch(sortBy)
    {
    case SortByPID:
        sortPredicate = predicates::pe_PID_less;
        break;
    default: // SortByName
        sortPredicate = predicates::pe_name_less;
        break;
    }

    std::stable_sort(processes.begin(), processes.end(), sortPredicate);

    // Print out sorted process list
    std::tcout <<processes.size() <<TEXT(" processes found.") <<std::endl <<std::endl
               <<TEXT("PID\t\tName/Path") <<std::endl
               <<std::tstring(80, TEXT('-')) <<std::endl;
    for(process_list_t::const_iterator iter = processes.begin(), 
        end = processes.end(); iter != end; ++iter)
    {
        std::tcout <<(*iter)->th32ProcessID <<TEXT("\t\t")
                   <<(*iter)->szExeFile <<std::endl;
    }

    return CommandOk;
}

//------------------------------------------------------------------------------

// 'track' command handler.
// Command syntax:
//  track|+ pid timeout action parameters
//      - pid           - [required] Process identifier.
//      - timeout       - [required] Wait timeout in milliseconds. -1 to wait infinitely.
//      - action        - [required] Either 'msg' or 'sys'.
//      - parameters    - [at least 1 required] Either alert text, or command line command.
static command_error_t OnCommand_Track(const strings_t& args)
{ 
    // Check if can add a new tracker
    if(gTrackerThreadPool.number_of_tasks() == gTrackerThreadPool.get_max_threads())
        throw wcp::exception(TEXT("Cannot create more trackers."));

    // At least 4 arguments are required
    if(args.size() < 4)
        return CommandInvalidArgs;

    // Refresh processes
    RefreshProcessMap();

    strings_t::const_iterator parg = args.begin();

    // Parse process identifier
    wcp::dword_t pid;
    if(!wcp::str2obj((*++parg).std_str(), pid))
        return CommandInvalidArgs;

    // Check if a process exists in a process map
    processe_map_t::const_iterator pinfo = gProcesseMap.find(pid);
    if(pinfo == gProcesseMap.end())
        throw wcp::exception(TEXT("A process specified does not exist."));

    // Parse tracker timeout
    wcp::dword_t timeout;
    if(!wcp::str2obj((*++parg).std_str(), timeout))
        return CommandInvalidArgs;

    // Read tracker action
    tracker_action_t action;
    ++parg;
    if(*parg == TEXT("alert"))
        action = TrackerActionShowAlert;
    else if(*parg == TEXT("command"))
        action = TrackerActionRunCommand;
    else
        return CommandInvalidArgs;

    // Add a new tracker. Note: Operation is synchronous.
    ++parg;
    LOCK(gTrackerMap)
    {
        // Compose a tracker action parameters
        std::tostringstream ssparams;
        for(;;)
        {
            ssparams <<*parg;
            if(++parg != args.end())
                ssparams <<TEXT(" ");
            else
                break;
        }

        // Create new tracker information record
        tracker_list_t::value_type trackerInfo(new tracker_info_t());

        // Try to open tracked process
        trackerInfo->hTrackedProcess = OpenProcess(SYNCHRONIZE , FALSE, pid);
        if(!trackerInfo->hTrackedProcess)
            throw wcp::win_exception(GetLastError());

        // Add tracker-process association
        gTrackerAssocs[++gNextTrackerId] = pid;
        try
        {
            // Fill tracker information
            tracker_info_t& ti = *trackerInfo;
            ti.action = action;
            ti.id = gNextTrackerId;
            ti.pinfo = (*pinfo).second;
            ti.actionData = ssparams.str();
            ti.stop_track = false;
            ti.timeout = timeout;
            tracker_map_t::value_type::second_type& list = (*gTrackerMap)[pid];
            list.push_back(trackerInfo);

            // Start tracker thread
            gTrackerThreadPool.enqueue_task(wcp::safe_task(TrackerProcedure, list.back()));

            std::tcout <<TEXT("\tProcess tracker #") <<gNextTrackerId 
                       <<TEXT(" was successfully set for process '")
                       <<ti.pinfo->szExeFile <<TEXT("' [") 
                       <<ti.pinfo->th32ProcessID <<TEXT("].") 
                       <<std::endl;
        }
        catch(...)
        {
            gTrackerAssocs.erase(gNextTrackerId--);
            throw;
        }
    }

    return CommandOk;
}

//------------------------------------------------------------------------------

// 'stop-track' command handler.
// Command syntax:
//
// stop-track|- tracker_id
//
//  - tracker_id    - [required] Tracked identifier.
static command_error_t OnCommand_StopStrack(const strings_t& args)
{ 
    if(args.size() != 2)
        return CommandInvalidArgs;

    // Parse tracker identifier
    const std::tstring& arg = args.at(1).std_str();
    wcp::uint_t trackerId;
    if(!wcp::str2obj(arg, trackerId))
        return CommandInvalidArgs;

    LOCK(gTrackerMap) // Lock tracker map
    {
        // Find tracked process by tracker identifier
        tracker_process_assoc_t::iterator ppid = gTrackerAssocs.find(trackerId);
        if(ppid == gTrackerAssocs.end())
            throw wcp::exception(TEXT("Unknown tracker identifier."));

        // Find a tracker by its idenfier in the list of thrackers
        // for the tracked process. Set stop flag for the tracker.
        tracker_map_t::iterator plist = (*gTrackerMap).find((*ppid).second);
        for(tracker_list_t::iterator iter = (*plist).second.begin(), 
            end = (*plist).second.end(); iter != end; ++iter)
        {
            if((*iter)->id == trackerId)
            {
                (*iter)->stop_track = true;
                break;
            }
        }

        std::tcout <<TEXT("\tTracker #") <<trackerId 
                   <<TEXT(" has been removed.") <<std::endl;
    }

    return CommandOk;
}

//------------------------------------------------------------------------------

// 'list' command handler.
// Command syntax:
//
// trackers|% [pid]
//
//  - pid   - [optional] Process identifier to list trackers for.
static command_error_t OnCommand_ListTrackers(const strings_t& args)
{
    // Parse process identifier if specified.
    wcp::dword_t pid = ~0;
    if(args.size() == 2)
    {
        if(!wcp::str2obj(args.at(1).std_str(), pid))
            return CommandInvalidArgs;
    }

    LOCK(gTrackerMap) // Lock tracker map
    {
        if((*gTrackerMap).empty())
            std::tcout <<TEXT("\tThere is no registered trackers.") <<std::endl;
        else
        {
            // In-place print functions implementations
            struct print
            {
                // Prints out a tracker information
                static void tracker(const tracker_info_ptr_t& pti)
                {
                    const tracker_info_t& ti = *pti;
                    std::tcout <<TEXT("\t#") <<ti.id <<(ti.action == TrackerActionShowAlert 
                        ? TEXT("\t\tAlert: \"") : TEXT("\t\tCommand: \""))
                        <<ti.actionData <<TEXT("\"") <<std::endl;
                }

                // Prints out a list of thrackers
                static void tracker_list(const tracker_list_t& list)
                {
                    std::tcout <<TEXT("\t") <<list.size() << TEXT(" tracker(s):") <<std::endl;
                    std::for_each(list.begin(), list.end(), print::tracker);
                }
            };

            if(pid != ~0) // Print trackers for a process specified
            {
                // Check if a process exists
                tracker_map_t::iterator iter = (*gTrackerMap).find(pid);
                if(iter == (*gTrackerMap).end())
                    throw wcp::exception(TEXT("Unknown process identifier."));
                
                // Print out process information
                const PROCESSENTRY32& pe = *gProcesseMap[(*iter).first];
                std::tcout <<TEXT("Process '") <<pe.szExeFile <<TEXT("' [")
                    <<pe.th32ProcessID <<TEXT("]") <<std::endl;

                // Print out a list of process trackers
                print::tracker_list((*iter).second);
            }
            else // Print out information about all trackers
            {
                // Enumerate trackers
                for(tracker_map_t::iterator iter = (*gTrackerMap).begin(),
                    end = (*gTrackerMap).end(); iter != end; ++iter)
                {
                    // Print out process information
                    const PROCESSENTRY32& pe = *gProcesseMap[(*iter).first];
                    std::tcout <<TEXT("Process '") <<pe.szExeFile <<TEXT("' [")
                        <<pe.th32ProcessID <<TEXT("]") <<std::endl;

                    // Print out a list of process trackers
                    print::tracker_list((*iter).second);
                }
            }
        }
    }

    return CommandOk;
}

//------------------------------------------------------------------------------

// 'run' command handler.
// Command syntax:
//
// run command_line
//
//  - command_line  - [required] Command line to run.
static command_error_t OnCommand_Run(const strings_t& args)
{
    if(args.size() == 1)
        return CommandInvalidArgs;

    // Compose command line
    std::tostringstream cmdl;
    strings_t::const_iterator iter = args.begin(), end = args.end();
    for(++iter; ;)
    {
        cmdl <<*iter;
        if(++iter != end)
            cmdl <<TEXT(" ");
        else
            break;
    }

    // Run command line
    _tsystem(cmdl.str().c_str());

    return CommandOk;
}

//******************************************************************************
// Entry point
//******************************************************************************

// Runs a task and handles all exceptions
bool RunTaskHandleExceptions(wcp::task task)
{
    bool succeded = false;

    try
    {
        task();
        succeded = true;
    }
    catch(const wcp::exception& we)
    { ReportError(we.what().c_str()); }
    catch(const std::exception& e)
    { 
#ifdef UNICODE
        std::wstring ws;
        wcp::mbs2wcs(e.what(), ws);
        ReportError(ws.c_str()); 
#else
        ReportError(e.what()); 
#endif
    }
    catch(...)
    { ReportError(TEXT("Undefined error has occured.")); }

    return succeded;
}

//------------------------------------------------------------------------------

// Console command loop
void CommandLoop(bool& breakLoop)
{
    // Command handlers information
    static const command_info_t commandSet[] =
    {
        { TEXT("help"), oncmd_help, 0 }, 
        { TEXT("?"), oncmd_help, 0 },
        
        { TEXT("list"), OnCommand_List, -1 }, 
        { TEXT("*"), OnCommand_List, -1 },
        
        { TEXT("track"), OnCommand_Track, -1 }, 
        { TEXT("+"), OnCommand_Track, -1 },
        
        { TEXT("stop-track"), OnCommand_StopStrack, 1 }, 
        { TEXT("-"), OnCommand_StopStrack, 1 },
        
        { TEXT("trackers"), OnCommand_ListTrackers, -1 }, 
        { TEXT("%"), OnCommand_ListTrackers, -1 },

        { TEXT("run"), OnCommand_Run, -1 },
        { TEXT("@"), OnCommand_Run, -1 },
    };

    // Print out command prompt
    std::tcout <<TEXT("> ");

    // Input command line
    std::tstring cmdl;
    std::getline(std::tcin, cmdl);
    if(cmdl.empty())
        return;

    // Parse command line
    strings_t args;
    std::tistringstream istm_cmdl(cmdl);
    std::copy(std::istream_iterator<std::tstringi, wcp::tchar_t>(istm_cmdl),
              std::istream_iterator<std::tstringi, wcp::tchar_t>(),
              std::back_inserter(args));
    const std::tstringi& command = args.front();

    // Find and call command handler
    command_error_t commandError = CommandUnknown;
    for(int i = 0; i < ARRAYSIZE(commandSet); ++i)
    {
        const command_info_t& ci = commandSet[i];
        if(command == ci.commandName)
        {
            if(ci.args_required != -1 && ci.args_required != (int)(args.size() - 1))
                commandError = CommandInvalidArgs;
            else
                commandError = ci.handler(args);

            break;
        }
    }

    // Check if 'exit' command is invoked
    if(commandError == CommandUnknown
        && args.size() == 1
        && command == TEXT("exit"))
    {
        breakLoop = true;
        commandError = CommandOk;
    }

    // Print out command error message if error is occured
    std::tstring errorMessage;
    switch(commandError)
    {
    case CommandUnknown:
        wcp::format_string(errorMessage, TEXT("Unknown command name '{0}'."), 
            command.c_str());
        break;
    case CommandInvalidArgs:
        wcp::format_string(errorMessage, TEXT("Invalid arguments in '{0}'."), 
            cmdl.c_str());
        break;
    }

    if(errorMessage.empty())
        std::tcout <<std::endl;
    else
        ReportError(errorMessage.c_str());
}

//------------------------------------------------------------------------------

// Safe program entry. All exceptions will be handled.
void StartProgram(int argc, _TCHAR* argv[])
{
    // Configure tracker threadpool
    gTrackerThreadPool.set_max_threads(10);

    // Print usage information if run with '/?' switch
    if(argc == 2 && std::char_traits<wcp::tchar_t>::compare(argv[1], TEXT("/?"), 2) == 0)
        PrintUsage();
    else
    {
        // Print usage information
        PrintUsage();

        // Console command loop
        for(bool breakLoop = false; !breakLoop;)
        {
            wcp::task loopTask(CommandLoop, wcp::reference<bool>(breakLoop));
            RunTaskHandleExceptions(loopTask);
        }
    }
}

//------------------------------------------------------------------------------

// Entry point.
int _tmain(int argc, _TCHAR* argv[])
{
    int iRet = EXIT_FAILURE;

    // Run safe program entry.
    if(RunTaskHandleExceptions(wcp::task(StartProgram, argc, argv)))
        iRet = EXIT_SUCCESS;

    return iRet;
}
