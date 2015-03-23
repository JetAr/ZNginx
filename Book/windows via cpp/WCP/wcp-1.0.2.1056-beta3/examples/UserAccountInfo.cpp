/// User account inforamtion. WCP library usage example.
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

#include <Windows.h>
#include <sddl.h>

#pragma comment(lib, "Advapi32.lib")

#include "wcp/stlex.h"
#include "wcp/errh.h"
#include "wcp/handles.h"
#include "wcp/buffer.h"

int _tmain(int argc, TCHAR* argv[])
{
    try
    {
        // Open process handle
        wcp::handles::handle_t hProcess = OpenProcess(PROCESS_ALL_ACCESS, 
            FALSE, GetCurrentProcessId());
        if(!hProcess)
            throw wcp::win_exception(GetLastError());

        // Open process token
        wcp::handles::handle_t hToken;
        OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
        if(!hToken)
            throw wcp::win_exception(GetLastError());

        // Obtain TokenUser record size
        DWORD cbInfo = 0;
        GetTokenInformation(hToken, TokenUser, NULL, 0, &cbInfo);
        if(cbInfo == 0)
            throw wcp::win_exception(GetLastError());

        // Read TokenUser record
        wcp::buffer<TOKEN_USER> userInfo(cbInfo);
        if(!GetTokenInformation(hToken, TokenUser, userInfo, cbInfo, &cbInfo))
            throw wcp::win_exception(GetLastError());

        // Convert user SID to string
        wcp::handles::hlocal_t strSid;
        if(!ConvertSidToStringSid(userInfo->User.Sid, (LPTSTR*)&strSid))
            throw wcp::win_exception(GetLastError());

        // Lookup user account name and domain
        TCHAR accountName[0x1000];
        TCHAR refDomain[0x1000];
        DWORD cchAccountName = ARRAYSIZE(accountName);
        DWORD cchRefDomain = ARRAYSIZE(refDomain);
        SID_NAME_USE sidNameUse;
        if(!LookupAccountSid(NULL, userInfo->User.Sid, 
            accountName, &cchAccountName,
            refDomain, &cchRefDomain, &sidNameUse))
        {
            throw wcp::win_exception(GetLastError());
        }

        // Print out process user info
        std::tcout <<TEXT("Process user information:\n\tAccount name: ") 
            << accountName <<TEXT('@') <<refDomain
            <<TEXT("\n\tSID: ") <<(LPTSTR)(HLOCAL)strSid <<std::endl;

        // No CloseHandle and LocalFree needed!
    }
    catch(const wcp::exception& e)
    {
        std::tcout <<TEXT("Error: ") <<e.what() <<std::endl;
        return EXIT_FAILURE;
    }
    catch(const std::exception& e)
    {
        std::cout <<"Error: " <<e.what() <<std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}