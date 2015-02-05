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

#include <winsock.h>

#include <stdlib.h>
#include <iostream>

using namespace std;


//// Prototypes ////////////////////////////////////////////////////////

extern int DoWinsock(char* pcHost, int nPort, int times_to_ping, int ping_interval, int include_timestamp, int include_url, char* docptr);


//// Constants /////////////////////////////////////////////////////////

// Default port to connect to on the server
const int kDefaultServerPort = 80;


//// main //////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    // Do we have enough command line arguments?
    if (argc < 2)
    {
        cerr << "--------------------------------------------------------------" << endl;
        cerr << "httping.exe by Eli Fulkerson " << endl;
        cerr << "Please see http://www.elifulkerson.com/projects/ for updates. " << endl;
        cerr << "--------------------------------------------------------------" << endl;
        cerr << "Usage: " << argv[0] << " [-t] [-d] [-i interval] [-n times] [-u] url " <<
             "[server-port]" << endl << endl;
        cerr << "\t -t   : ping continuously until stopped via control-c" << endl;
        cerr << "\t -n 5 : for instance, send 5 pings" << endl;
        cerr << "\t -i 5 : for instance, ping every 5 seconds" << endl;
        cerr << "\t -d   : include date and time on each line" << endl;
        cerr << "\t -u   : include target URL on each line" << endl;
        cerr << endl << "\tIf you don't pass server-port, it defaults to " <<
             kDefaultServerPort << "." << endl;

        return 1;
    }

    int times_to_ping = 4;
    int offset = 0;  // because I don't feel like writing a whole command line parsing thing, I just want to accept an optional -t
    int ping_interval = 2;
    int include_timestamp = 0;
    int include_url = 0;


    for (int x=0; x < argc; x++)
    {
        // ping continuously
        if (!strcmp(argv[x], "-t"))
        {
            times_to_ping = -1;
            offset = x;
            cout << endl << "** Pinging continuously.  Press control-c to stop **" << endl;
        }

        // Number of times to ping
        if (!strcmp(argv[x], "-n"))
        {
            times_to_ping = atoi(argv[x+1]);
            offset = x+1;
        }

        // interval between pings
        if (!strcmp(argv[x], "-i"))
        {
            ping_interval = atoi(argv[x+1]);
            offset = x+1;
        }

        // optional datetimestamp output
        if (!strcmp(argv[x], "-d"))
        {
            include_timestamp = 1;
            offset = x;
        }

        // optional datetimestamp output
        if (!strcmp(argv[x], "-u"))
        {
            include_url = 1;
            offset = x;
        }


        // dump version and quit
        if (!strcmp(argv[x], "-v"))
        {
            cout << "httping.exe 0.1, June 27 2009" << endl;
            return 1;
        }
    }

    // Get host and (optionally) port from the command line
    const char* pcHost = argv[1 + offset];
    int nPort = kDefaultServerPort;
    if (argc >= 3 + offset)
    {
        nPort = atoi(argv[2 + offset]);
    }

    // Do a little sanity checking because we're anal.
    int nNumArgsIgnored = (argc - 3 - offset);
    if (nNumArgsIgnored > 0)
    {
        cerr << nNumArgsIgnored << " extra argument" <<
             (nNumArgsIgnored == 1 ? "" : "s") <<
             " ignored.  FYI." << endl;
    }

    // parse out our server and document
    const char *serverptr;
    char *docptr;

    char server[2048];
    char document[2048];

    serverptr = strchr(pcHost, ':');
    if (serverptr != NULL)
    {
        ++serverptr;
        ++serverptr;
        ++serverptr;
    }
    else
    {
        serverptr = pcHost;
    }
    //cout << serverptr << endl;

    docptr = strchr(serverptr, '/');
    if (docptr != NULL)
    {
        *docptr = '\0';
        ++docptr;

        strcpy( server, static_cast<const char*>(serverptr) );
        strcpy( document, static_cast<const char*>(docptr) );
    }
    else
    {
        //cout << "found nothing, no document";
        //strcpy( document, static_cast<const char*>('/') );
        strcpy( server, static_cast<const char*>(serverptr) );
        //document[0] = '/';
        //document[1] = '\0';
        document[0] = '\0';
    }


    cout << "** Requesting \"" << document << "\" from " << server << ":" << endl;
    cout << "(for various reasons, kbit/s is an approximation)" << endl;


    // Start Winsock up
    WSAData wsaData;
    int nCode;
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0)
    {
        cerr << "WSAStartup() returned error code " << nCode << "." << endl;
        return 255;
    }

    // Call the main example routine.
    //int retval = DoWinsock(pcHost, nPort, times_to_ping, ping_interval, include_timestamp, serverptr, docptr);
    //int retval = DoWinsock(serverptr, nPort, times_to_ping, ping_interval, include_timestamp, docptr);
    int retval = DoWinsock(server, nPort, times_to_ping, ping_interval, include_timestamp, include_url, document);


    // Shut Winsock back down and take off.
    WSACleanup();
    return retval;
}

