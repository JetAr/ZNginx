/***********************************************************************
tcping.exe -- A tcp probe utility
Copyright (C) 2005 Eli Fulkerson

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

----------------------------------------------------------------------

Other license terms may be negotiable.  Contact the author if you would
like a copy that is licensed differently.

Contact information (as well as this program) lives at http://www.elifulkerson.com

----------------------------------------------------------------------

This application includes public domain code from the Winsock Programmer's FAQ:
  http://www.tangentsoft.net/wskfaq/
... and a big thank you to the maintainers and contributers therein.

***********************************************************************/

#include "ws-util.h"

#include <winsock.h>

#include <iostream>
#include <string.h>
#include <time.h>

#include <sys/timeb.h> // for ftime

#define NUM_PROBES 4

using namespace std;

////////////////////////////////////////////////////////////////////////
// Prototypes

u_long LookupAddress(const char* pcHost);
SOCKET EstablishConnection(u_long nRemoteAddr, u_short nPort);

LARGE_INTEGER cpu_frequency;
LARGE_INTEGER response_timer1;
LARGE_INTEGER response_timer2;


const int BufferSize = 1024;

bool SendHttp(SOCKET sd, char* server, char* document)
{

    char message[1024];
    sprintf( message, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nUser-Agent: httping.exe (www.elifulkerson.com)\r\n\r\n", document, server);
    const int messageLen = strlen(message);

    //cout << endl << message << endl;

    // Send the string to the server
    if (send(sd, message, messageLen, 0) != SOCKET_ERROR)
    {
        return true;
    }
    else
    {
        return false;
    }
}


//// ReadReply /////////////////////////////////////////////////////////
// Read the reply packet and check it for sanity.  Returns -1 on
// error, 0 on connection closed, > 0 on success.

int ReadReply(SOCKET sd, int &bytes_received, int &http_status)
{
    // Read reply from server
    char acReadBuffer[BufferSize];
    char acTrashBuffer[BufferSize];
    int nTotalBytes = 0;
    int nNewBytes = 0;
    while (1)
    {
        //while (1) {

        if (nTotalBytes < BufferSize)
        {
            //cout << "receiving x";
            nNewBytes = recv(sd, acReadBuffer + nTotalBytes, BufferSize - nTotalBytes, 0);
            //cout << "got " << nNewBytes << " new bytes x!" << endl;
        }
        else
        {
            //cout << "receiving y";
            nNewBytes = recv(sd, acTrashBuffer, BufferSize, 0);
            //cout << "got " << nNewBytes << " new bytes y!" << endl;
            //cout << acTrashBuffer;
        }

        if (nNewBytes == SOCKET_ERROR)
        {
            return -1;
        }
        else if (nNewBytes == 0)
        {
            //cerr << "Connection closed by peer." << endl;
            //return 0;
            break;
        }

        //cout << nNewBytes << endl;
        nTotalBytes += nNewBytes;
    }

    //cout << endl << endl << acReadBuffer << endl;
    bytes_received =  nTotalBytes;

    //parse out the http status from the first line of the response
    char* statusptr;
    char* tmpptr;
    // hop over the initial "HTTP/1.1"
    statusptr = strchr(acReadBuffer, ' ');
    ++statusptr;

    tmpptr = strchr(statusptr, ' ');
    // should be at the " " past the error code now
    *tmpptr = '\0';

    http_status = atoi(statusptr);
    return 0;
}


//// DoWinsock /////////////////////////////////////////////////////////
// The module's driver function -- we just call other functions and
// interpret their results.

int DoWinsock(char* pcHost, int nPort, int times_to_ping, int ping_interval, int include_timestamp, int include_url, char* document)
{

    double response_time;

    int start_time = 0;
    int end_time = 0;
    int start_milltime = 0;
    int end_milltime = 0;
    int total_ms = 0;			// total number of milliseconds per pass

    int number_of_pings = 0;	// total number of tcpings issued
    int running_total_ms = 0;	// running total of values of pings... divide by number_of_pings for avg
    int lowest_ping = 50000;		// lowest ping in series ... starts high so it will drop
    int max_ping = 0;			// highest ping in series

    int success_counter = 0;
    int failure_counter = 0;


    struct _timeb timebuffer;

    int loopcounter = 0;			// number of probes to send

    time_t rawtime;
    struct tm * timeinfo;
    char dateStr [11];
    char timeStr [9];

    int bytes_received = 0;
    int http_status = 0;

    float bps = 0;

    cout << endl;

    // Find the server's address
    u_long nRemoteAddress = LookupAddress(pcHost);
    if (nRemoteAddress == INADDR_NONE)
    {
        cerr << endl << "Could not find host " << pcHost << ", aborting." << endl;
        return 3;
    }
    in_addr Address;
    memcpy(&Address, &nRemoteAddress, sizeof(u_long));

    while (loopcounter < times_to_ping || times_to_ping == -1)
    {

        if (include_timestamp == 1)
        {
            //_strdate( dateStr);
            _strtime( timeStr );
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            strftime(dateStr, 11, "%Y:%m:%d",timeinfo);
            cout << dateStr << " " << timeStr << " ";
        }


        // apparently... QueryPerformanceCounter isn't thread safe unless we do this
        SetThreadAffinityMask(GetCurrentThread(),1);

        // start the timer right before we do the connection
        //QueryPerformanceFrequency((LARGE_INTEGER *)&cpu_frequency);
        //QueryPerformanceCounter((LARGE_INTEGER *) &response_timer1);



        // Connect to the server
        SOCKET sd = EstablishConnection(nRemoteAddress, htons(nPort));

        if (sd == INVALID_SOCKET)
        {
            cerr <<  WSAGetLastErrorMessage("") << " ";
            failure_counter++;

        }
        else
        {
            //cout << "Port is open";
            success_counter++;

            SendHttp(sd,pcHost,document);

            QueryPerformanceFrequency((LARGE_INTEGER *)&cpu_frequency);
            QueryPerformanceCounter((LARGE_INTEGER *) &response_timer1);

            ReadReply(sd, bytes_received, http_status);

            // Shut connection down
            if (ShutdownConnection(sd))
            {
                // room here for connection shutdown success check...
            }
            else
            {
                // room here for connection shutdown failure check...
            }
        }

        // end the clock
        QueryPerformanceCounter((LARGE_INTEGER *) &response_timer2);


        response_time=( (double) ( (response_timer2.QuadPart - response_timer1.QuadPart) * (double) 1000.0 / (double) cpu_frequency.QuadPart) );

        cout.precision(3);
        cout.setf(ios::showpoint);
        cout.setf(ios::fixed);

        cout << "time=" << response_time << "ms" << " ";


        if (include_url == 1)
        {
            cout << "page:" << "http://" << pcHost << "/" << document << " ";
        }

        cout << "status=" << http_status << " ";

        cout << "bytes=" << bytes_received << " ";

        bps = bytes_received * 1000 / response_time;
        bps = bps * 8 / 1000;

        cout << "kbit/s=~" << bps << " ";


        cout << endl;


        // Calculate the statistics...
        number_of_pings++;

        total_ms = response_time;

        if (sd != INVALID_SOCKET)
        {
            running_total_ms += total_ms;

            if (total_ms < lowest_ping)
            {
                lowest_ping = total_ms;
            }

            if (total_ms > max_ping)
            {
                max_ping = total_ms;
            }
        }

        if (total_ms < ping_interval * 1000)
        {
            Sleep((ping_interval * 1000) - total_ms);
        }

        loopcounter++;
    }

    cout << endl << "Ping statistics for " << inet_ntoa(Address) << ":" << nPort << endl;
    cout             << "     " << number_of_pings << " probes sent. "<< endl;
    cout	           << "     " << success_counter << " successful, " << failure_counter << " failed." << endl;
    if (success_counter > 0)
    {
        if ( failure_counter > 0)
        {
            cout     << "Approximate trip times in milli-seconds (successful connections only):" << endl;
        }
        else
        {
            cout             << "Approximate trip times in milli-seconds:" << endl;
        }
        cout             << "     Minimum = " << lowest_ping << "ms, Maximum = " << max_ping << "ms, Average = " << running_total_ms/number_of_pings << "ms" <<endl;
    }
    else
    {
        cout             << "Was unable to connect, cannot provide trip statistics." << endl;
    }

    return 0;
}


//// LookupAddress /////////////////////////////////////////////////////
// Given an address string, determine if it's a dotted-quad IP address
// or a domain address.  If the latter, ask DNS to resolve it.  In
// either case, return resolved IP address.  If we fail, we return
// INADDR_NONE.

u_long LookupAddress(const char* pcHost)
{
    u_long nRemoteAddr = inet_addr(pcHost);
    if (nRemoteAddr == INADDR_NONE)
    {
        // pcHost isn't a dotted IP, so resolve it through DNS
        hostent* pHE = gethostbyname(pcHost);
        if (pHE == 0)
        {
            return INADDR_NONE;
        }
        nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
    }

    return nRemoteAddr;
}


//// EstablishConnection ///////////////////////////////////////////////
// Connects to a given address, on a given port, both of which must be
// in network byte order.  Returns newly-connected socket if we succeed,
// or INVALID_SOCKET if we fail.

SOCKET EstablishConnection(u_long nRemoteAddr, u_short nPort)
{

    // Create a stream socket
    SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);

    // would be nice to limit the huge timeout in cases where the tcp connection times out (as opposed
    // to bouncing off a closed port, but this stuff doesn't work for some reason.
    /*int timeout = 10;
    int tosize = sizeof(timeout);
    setsockopt(sd, SOL_SOCKET, SO_SNDTIMEO , (char*)&timeout, (int)&tosize);
    setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO , (char*)&timeout, (int)&tosize);
    */

    if (sd != INVALID_SOCKET)
    {
        sockaddr_in sinRemote;
        sinRemote.sin_family = AF_INET;
        sinRemote.sin_addr.s_addr = nRemoteAddr;
        sinRemote.sin_port = nPort;
        if (connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) ==
                SOCKET_ERROR)
        {
            sd = INVALID_SOCKET;
        }
    }

    return sd;
}

