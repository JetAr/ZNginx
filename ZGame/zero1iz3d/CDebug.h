// CDebug.h -



#ifndef __CDEBUGGER_INCLUDED
#define __CDEBUGGER_INCLUDED


#include <string>
#include <vector>
#include <fstream>

using namespace std;

#define __DEBUG_FILE__  "debug.txt"
#define DTHROW(err) (throw CDebug( __FILE__, __LINE__, err ))


class CDebug
{

private:
    int		  line;
    string    msg;
    string    file;
    ofstream  odbg;
    bool	  bdbg_file;

public:

    CDebug();
    CDebug( string debug_file );
    ~CDebug();

    // throw erros
    void Throw( string error_msg );
    void Throw( string error_file, int error_line, string error_msg );
    //void Fatal();

    void CloseStream();

    // get last error info
    string getError()
    {
        return msg;
    };
    string getFile()
    {
        return file;
    };
    int	   getLine()
    {
        return line;
    };

};


#endif
