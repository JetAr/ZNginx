// CDebug.cpp -


#include "main.h"



CDebug::CDebug()
{

    odbg.open( __DEBUG_FILE__ , ios::out );

    if ( odbg.fail() )
        bdbg_file = false;
    else
    {
        odbg << "Debug Opened..";
        bdbg_file = true;
    }
}


CDebug::CDebug( string debug_file )
{

    odbg.open( debug_file.c_str() , ios::out );

    if ( odbg.fail() )
        bdbg_file = false;
    else
    {
        odbg << "Debug Opened..";
        bdbg_file = true;
    }

}

CDebug::~CDebug()
{
    this->CloseStream();
}


void CDebug::CloseStream()
{
    if ( bdbg_file )
    {
        odbg.close();
        bdbg_file = false;
    }
}


void CDebug::Throw( string error_msg )
{
    line = 0;
    msg = error_msg;
    file = "Unknown Module";

    if ( bdbg_file )
        odbg << endl << file << " at line " << line << " :: " << msg;
}


void CDebug::Throw( string error_file, int error_line, string error_msg )
{
    line = error_line;
    msg = error_msg;
    file = error_file;

    if ( bdbg_file )
        odbg << endl << file << " at line " << line << " : " << msg;

}

