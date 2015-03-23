#ifdef ___DEBUG_H_CYCLE__
#error Cyclic dependency discovered: debug.h
#endif

#ifndef __DEBUG_H__


#ifndef DOXYGEN

#define __DEBUG_H__
#define __DEBUG_H_CYCLE__

/////////////////////////////////////////////////////////////////////////////////
///
///           ----------- WCP (Windows via C++) v.1.0.2.1056 ---------- 
///                                     Beta 3.
/// 
/// 
///                       Microsoft Public License (Ms-PL)
/// 
/// 
/// This license governs use of the accompanying software. If you use the software,
///   you accept this license. If you do not accept the license, do not use the 
///                                     software.
/// 
/// Definitions
/// 
/// The  terms  "reproduce,"  "reproduction," "derivative works," and "distribution"
/// have  the same meaning here as under U.S. copyright law. A "contribution" is the
/// original  software, or any additions or changes to the software. A "contributor"
/// is  any  person  that distributes its contribution under this license. "Licensed
/// patents"   are   a  contributor`s  patent  claims  that  read  directly  on  its
/// contribution.
/// 
/// Grant of Rights
/// 
/// (A) Copyright Grant- Subject to the terms of this license, including the license
/// conditions  and  limitations  in  section  3,  each  contributor  grants  you  a
/// non-exclusive,  worldwide,  royalty-free  copyright  license  to  reproduce  its
/// contribution,  prepare  derivative works of its contribution, and distribute its
/// contribution  or any derivative works that you create. 
/// (B) Patent Grant- Subject
/// to  the  terms of this license, including the license conditions and limitations
/// in   section   3,  each  contributor  grants  you  a  non-exclusive,  worldwide,
/// royalty-free  license  under its licensed patents to make, have made, use, sell,
/// offer  for  sale,  import,  and/or  otherwise dispose of its contribution in the
/// software or derivative works of the contribution in the software.
/// 
/// Conditions and Limitations
/// 
/// (A)  No  Trademark  License-  This  license does not grant you rights to use any
/// contributors' name, logo, or trademarks. 
/// (B) If you bring a patent claim against
/// any  contributor over patents that you claim are infringed by the software, your
/// patent  license from such contributor to the software ends automatically. 
/// (C) If
/// you  distribute  any  portion  of  the  software, you must retain all copyright,
/// patent, trademark, and attribution notices that are present in the software. 
/// (D)
/// If you distribute any portion of the software in source code form, you may do so
/// only  under  this license by including a complete copy of this license with your
/// distribution.  If  you  distribute  any  portion  of the software in compiled or
/// object  code  form,  you  may only do so under a license that complies with this
/// license.  
/// (E)  The  software is licensed "as-is." You bear the risk of using it.
/// The  contributors give no express warranties, guarantees, or conditions. You may
/// have  additional consumer rights under your local laws which this license cannot
/// change.  To the extent permitted under your local laws, the contributors exclude
/// the  implied warranties of merchantability, fitness for a particular purpose and
/// non-infringement.
/// 
/// 
/// 
/// 
/// This   source  code  was compiled and tested in Microsoft Visual Studio 2008. If
/// you found any bug in this source code, please e-mail me to admin@ilyns.com.
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
///
/////////////////////////////////////////////////////////////////////////////////
///
///  Revision history:
///      10/16/2009 - Initial release.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains debug tools.
*/


#include <Windows.h>

#include <sstream>
#include <iomanip>


#include "_wcpdef.h"
#include "macros.h"



namespace WCP_NS 
{

//-----------------------------------------------------------------------------
// Debug messages.
//-----------------------------------------------------------------------------

#if defined(_DEBUG) || defined(WCP_FORCE_DBG)

/// <summary>
/// Indicates that debug tools are enabled.
/// </summary>
#define WCP_DEBUG

#ifdef DOXYGEN

/// <summary>
/// Forces WCP to print out debug information in XML format so that you can save
/// debug output as an XML file and apply an XSLT transformation to it to get a formatted
/// HTML report. By default is not defined.
/// </summary>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'> 
///     // Compile with: /D "WCP_DEBUG_XML"
/// &nbsp;
///     wcp::uint_t fibonachi(wcp::uint_t v)
///     {
///         \ref DBGM("HEX Fibonachi: 0x" &lt;&lt;std::hex &lt;&lt;std::setw(8) &lt;&lt;std::setfill(TEXT('0')) &lt;&lt;v);
///         return v &lt; 2 ? 1 : (fibonachi(v - 1) + fibonachi(v - 2));
///     }
/// &nbsp;
///     void main2()
///     {
///         DBGF();
/// &nbsp;
///         \ref std::tcout &lt;&lt;fibonachi(4);
/// &nbsp;
///         DBGL();
///     }
/// &nbsp;
///     void main()
///     {
///         if(wcp::library_init())
///         {
///             \ref NONTHROWABLE(main2());
///             wcp::library_uninit();
///         }
///     }
/// </pre></code>
/// debug output is:
/// <code><pre class='_code'> 
///     &lt;WCPDebugLog wcp-version="1.0"&gt;
///     &lt;Debug function="main2" line="47" file="d:\devel\root\devel\wcp\wcp.cpp" /&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000004 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000003 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000002 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000001 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000000 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000001 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000002 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000001 ]]&gt;&lt;/Message&gt;
///     &lt;Message&gt;&lt;![CDATA[ HEX Fibonachi: 0x00000000 ]]&gt;&lt;/Message&gt;
///     &lt;Debug line="51" file="d:\devel\root\devel\wcp\wcp.cpp" /&gt;
///     &lt;/WCPDebugLog&gt;
/// </pre></code>
/// </remarks>
#define WCP_DEBUG_XML

/// <summary>
/// Forces WCP to include thread identifier to each debug message.
/// </summary>
#define WCP_DEBUG_TID

#endif /*DOXYGEN*/

#endif

#ifndef DOXYGEN

#ifdef WCP_DEBUG_TID
#define _WCP_XDBG_TID " TID=\"" <<GetCurrentThreadId() <<"\" "
#define _WCP_PDBG_TID <<"[TID: " <<GetCurrentThreadId() <<"] "
#else
#define _WCP_XDBG_TID ""
#define _WCP_PDBG_TID
#endif

#define _PDBGM(seq)                                             \
    {                                                           \
        std::basic_ostringstream<WCP_NS::tchar_t> ss;           \
        ss _WCP_PDBG_TID <<seq;                                 \
        OutputDebugString(ss.str().c_str());                    \
    }

#define _PDBGL()     _PDBGM("### Line " TOSTR(__LINE__) " {" __FILE__ "}")
#define _PDBGF()     _PDBGM("### Function " __FUNCTION__ ":" TOSTR(__LINE__) " {" __FILE__ "}")
#define _PDBGE(msg)  _PDBGM("### Error \"" <<msg <<"\" in "__FUNCTION__ ":" TOSTR(__LINE__) " {" __FILE__ "}")

#define _WCP_XDBGM(seq)                                         \
    {                                                           \
        std::basic_ostringstream<WCP_NS::tchar_t> ss;           \
        ss <<seq;                                               \
        OutputDebugString(ss.str().c_str());                    \
    }

#define _XDBGM(seq)  _WCP_XDBGM("<Message" _WCP_XDBG_TID"><![CDATA[\r\n\t"              \
                        <<seq <<"\r\n]]></Message>")
#define _XDBGL()     _WCP_XDBGM("<Debug line=\"" TOSTR(__LINE__)                        \
                        "\" file=\"" __FILE__ "\" " _WCP_XDBG_TID "/>")
#define _XDBGF()     _WCP_XDBGM("<Debug function=\"" __FUNCTION__ "\" line=\""          \
                        TOSTR(__LINE__) "\" file=\"" __FILE__ "\" " _WCP_XDBG_TID "/>")
#define _XDBGE(msg)  _WCP_XDBGM("<Error function=\"" __FUNCTION__ "\" line=\""          \
                        TOSTR(__LINE__) "\" file=\"" __FILE__ "\" "                     \
                        _WCP_XDBG_TID "><![CDATA[\r\n\t" <<msg <<"\r\n]]</Error>")

#define _DEBUGGER DebugBreak()

#ifdef WCP_DEBUG_XML
#define _DBGM    _XDBGM
#define _DBGL    _XDBGL
#define _DBGF    _XDBGF
#define _DBGE    _XDBGE
#else /*WCP_DEBUG_XML*/
#define _DBGM    _PDBGM
#define _DBGL    _PDBGL
#define _DBGF    _PDBGF
#define _DBGE    _PDBGE
#endif /*WCP_DEBUG_XML*/

#endif /*DOXYGEN*/

#ifdef WCP_DEBUG

/// <summary>
/// Prints out a debug message. Accepts a C++ stream-like sequence of input parameters.
/// Define a \ref WCP_DEBUG_XML to XML formatted debug output.
/// </summary>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      int i = 0;
///      DBGM("i = " &lt;&lt;i)
/// </pre></code>
/// </remarks>
#define DBGM    _DBGM

/// <summary>
/// Prints out a debug message about a source line where the macro is called.
/// Define a \ref WCP_DEBUG_XML to XML formatted debug output.
/// </summary>
#define DBGL    _DBGL

/// <summary>
/// Prints out a debug message about a source function where the macro is called.
/// Define a \ref WCP_DEBUG_XML to XML formatted debug output.
/// </summary>
#define DBGF    _DBGF

/// <summary>
/// Prints out a debug message about an error occured.
/// Accepts a C++ stream-like sequence of input parameters.
/// Define a \ref WCP_DEBUG_XML to XML formatted debug output.
/// </summary>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'> 
///     int i = 0;
///     DBGE("Error code: " &lt;&lt;GetLastError())
/// </pre></code>
/// </remarks>
#define DBGE    _DBGE


#define DEBUGGER _DEBUGGER

#else /*WCP_DEBUG*/

#define DBGM(seq)
#define DBGL()
#define DBGF()
#define DBGE(msg)
#define DEBUGGER

#endif /*WCP_DEBUG*/

#ifdef WCP_DEBUG

    /// <summary>
    /// Sets a %thread name visible only for Visual Studio C++ debugger.
    /// Helps to distinct threads in a %thread list in Visual Studio C++ debugger.
    /// </summary>
    /// <param name='thread_name'>Thread name.</param>
    /// <param name='dwThreadID'>Thread identifier. 
    /// By default a calling thread identifier is used.</param>
    inline void set_thread_name(const char* thread_name, 
                                DWORD dwThreadID = GetCurrentThreadId())
    {
        __try
        {
#include <pshpack8.h>

            struct threadname_info_t
            {
               dword_t type;        // Must be 0x1000.
               const char* name;    // Pointer to name (in user addr space).
               dword_t tid;         // Thread ID (-1=caller thread).
               dword_t reserved;    // Reserved for future use, must be zero.

            } info = { 0x1000, thread_name, dwThreadID, 0 };

#include <poppack.h>

            RaiseException(0x406D1388 /*MS_VC_EXCEPTION*/, 0, 
                sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        { }
    }

#endif /*WCP_DEBUG*/

} // namespace WCP_NS


#undef __DEBUG_H_CYCLE__
#endif /*__DEBUG_H__*/
