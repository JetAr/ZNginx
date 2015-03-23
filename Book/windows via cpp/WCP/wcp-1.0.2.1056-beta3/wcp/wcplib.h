#ifdef ___WCPLIB_H_CYCLE__
#error Cyclic dependency discovered: wcplib.h
#endif

#ifndef __WCPLIB_H__


#ifndef DOXYGEN

#define __WCPLIB_H__
#define __WCPLIB_H_CYCLE__

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
///      07/20/2009 - Initial release.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///      12/15/2009 - Core implementation separated to _core.h
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/**

\file
Declares global WCP macros and includes all necessary headers.

\example Serialization.cpp
Example of using of typed buffers, type constraints, STL extensions and reference wrappers.

\example ConsoleCtrlHandler.cpp
Example of using of unicast delegates and STL extensions.

\example ConsoleKbdHandler.cpp
Example of using of multicast events, unicast delegates, singleton and STL extensions.

\example UserAccountInfo.cpp
Example of using of smart handles, exceptions and STL extensions.

\example ProcessTracker.cpp
General WCP usage example.

*/

/// <summary>
/// \mainpage
/// <i>WCP</i> is an acronynim for the <b>Windows via C++</b> library. The purpose
/// of the library is to take advantage of C++ programming language when you write
/// a program in <i>Windows API</i> using Microsoft Visual Studio 2005 or higher,
/// The <i>Windows API</i> has commonly C interface so Windows developers often face with
/// cross-language issues and are forced to refuse most of high-level language benefits.
/// You may argue the relevance of this library, because there are a huge amount of work done
/// in this scope: <i>MFC</i>, <i>ATL</i>, <i>WTL</i>, <i>Qt</i>, <i>wxWidgets</i> and other
/// very popular C++ libraries. What can provide such a small library as <i>WCP</i>?
/// <br><br>
/// Years of my experience of writing programs using different libraries proved
/// the strong need in having some set of classes and tools that I could use in any project,
/// regardless what libraries it uses. With a course of time I have collected a big number
/// of commonly used classes and tools and decided to organize my work in a single and
/// universal library.<br><br>
///
/// Here are some arguments that prove a need in <i>WCP</i>:
///
/// <ul>
///     <li>Whatever library you use, if you program requires intensive usage of <i>Windows API</i>
/// you are forced to use this <i>API</i> directly because no one library would provide you a set of
/// classes for all cases. For example, if you need to call a system function that returns a
/// pointer to <kbd>HLOCAL</kbd> (which requires a call to <kbd>LocalFree</kbd>),
/// neither library allow you to release this memory automatically, and you are forced
/// to insert bunch of <kbd>try...catch</kbd> blocks, to track a pointer all across a module
/// in order to avoid memory leaks. It's too much work. And it's now a secrect that redundant work
/// is usually error prone and increase program complexity in times. <br>
/// <i>WCP</i> provides smart handles for almost user-level Windows handles, that release resources
/// on destruction. Here is an example of HLOCAL smart handle using:
/// <code><pre class='_code'>
///     // This is what all Windows developers are used to
///    HANDLE hMutex = CreateMutex(NULL, TRUE, NULL);
///    if(hMutex != NULL)
///    {
///        try
///        {
///            // Here goes a code that may throw any exception
///        }
///        catch(...)
///        {
///            CloseHandle(hMutex);
///            hMutex = NULL;
///            throw;
///        }
///    }
/// </pre></code>
/// Too much code, isn't it? Wouldn't it be much easier to write something like this:
/// <code><pre class='_code'>
///    wcp::handles::handle_t hMutex = CreateMutex(NULL, TRUE, NULL);
///    if(hMutex)
///    {
///        // Here goes a code that may throw any exception.
///        // Note: No CloseHandle is needed any more.
///    }
/// </pre></code>
/// Look at the \ref UserAccountInfo.cpp example to start off with <i>WCP</i> smart handles.
/// <br><br>
/// </li>
/// <li>Second argument is about synchronization. Synchronization is always a big issue.
/// Using <i>WCP</i> you can achive almost transparent, but at the same time simple and reliable
/// synchronzation:<br><br>
///     <ul>
///     <li>Derive classes from wcp::synchronizable&lt;T&gt; to be able to synchronize
///         object access using wcp::synchronizable::sync method or \ref LOCK[n] statements
///         (<i>C#</i>-like syntax of object synchronization).
/// <code><pre class='_code'>
///    class Person : public wcp::synchronizable&lt;Person&gt;
///    {
///        std::string m_name;
///    public:
///        const std::string& getName() const
///        { return m_name; }
/// &nbsp;
///        void setName(const char* name)
///        { m_name.assign(name); }
///    };
/// &nbsp;
///    Person person;
/// &nbsp;
///    void do_test()
///    {
///        // wcp::synchronizable&lt;T&gt;::sync retrurns
///        person.sync()->setName("Jack"); // This line of code will be executed synchronously.
/// &nbsp;
///        // Is identical to
/// &nbsp;
///        LOCK(person) // This code will be executed synchronously too.
///        {
///            person.setName("Jack");
///        }
///    }
/// </pre></code>
///         </li>
///         <li>Use and create new synchronization objects derived from wcp::sync_object
///         and perform synchronization using \ref GUARD[n] macros.
/// <code><pre class='_code'>
///    wcp::critical_section cs;
///    GUARD(cs)
///    {
///        ...
///    }
///    ...
///    wcp::monitor monitor;
///    GUARD2(monitor, cs)
///    {
///        ...
///    }
/// </pre></code>
///         </li>
///         <li>Use \ref SYNCHRONIZED statement to achive simple in-place syncronization:
/// <code><pre class='_code'>
///     \ref SYNCHRONIZED
///     {
///         // Critical code goes here. Only one thread can execute this code simultaneously.
///     }
/// </pre></code>
///         </li>
///         <li>Take advantage of the wcp::syncronized object wrapper:
/// <code><pre class='_code'>
///     wcp::synchronized&lt;std::string&gt; name;
///     LOCK(name)
///     {
///         *name = "John";
///     }
/// &nbsp;
///     // or even much simpler
/// &nbsp;
///     name.sync()->assign("John");
/// </pre></code>
///         </li>
///     </ul>
/// </li>
/// <li>The third argument, is advanced debugging capabilities. If you use <kbd>OutputDebugString</kbd>
/// to print debug messages, apparently you got tired of writing formatting code wherever
/// you call <kbd>OutputDebugString</kbd>. In the best case you have a function that takes a variable
/// number of arguments, like printf. <i>WCP</i> provides you \ref DBGM, \ref DBGF, \ref DBGL and \ref DBGE
/// to print respectively any message, calling function name, line information and error message.
/// Furthermore, defining a \ref WCP_DEBUG_XML macro enforces <i>WCP</i> to print out debug information in <i>XML</i>
/// format so that you can save debug output as an <i>XML</i> file and apply an <i>XSLT</i> transformation
/// to it to get a formatted <i>HTML</i> report about your program workflow. <i>C++</i>-stream-like interface
/// gives you lots of flexible formatting options.
/// <code><pre class='_code'>
///     // Compile with: /D "WCP_DEBUG_XML"
/// &nbsp;
///     wcp::uint_t fibonachi(wcp::uint_t v)
///     {
///         \ref DBGM("HEX Fibonachi: 0x" &lt;&lt;std::hex &lt;&lt;std::setw(8) &lt;&lt;std::setfill(TEXT('0')) &lt;&lt;v);
///         return v &lt; 2 ? 1 : (fibonachi(v - 1) + fibonachi(v - 2));
///     }
/// &nbsp;
///     void main()
///     {
///         DBGF();
/// &nbsp;
///         \ref std::tcout &lt;&lt;fibonachi(4);
/// &nbsp;
///         DBGL();
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
/// </li>
/// <li>The fourth argument is about thread safety. It's not the same as syncronization.
/// Thread safety means a shared data to be safe across working threads.
/// <i>WCP</i> provides a set of smart pointers and task objects that are thread safe.
/// For example, if you have an instance of <kbd>std::auto_ptr&lt;SomeClass&gt;</kbd> type,
/// then you cannot assign values to such a pointer from different threads simultaneously,
/// because in the best case you will have a memory leak. Which is most real, your program will crash.
/// You can avoid this problem by using wcp::safe_shared_ptr&lt;T&gt; and wcp::safe_weak_ptr&lt;T&gt;,
/// wich have completely <i>TR1</i> compatible interface.
/// <br><br></li>
/// <li>The next, the fifth argument, regards <i>C++</i> language extenstion. Besides
/// \ref GUARD[n], \ref LOCK[n]  and \ref SYNCHRONIZED statements <i>WCP</i> also provides
/// tremendously flexible and exandable type constraints which allow you to prevent up to 90%
/// of errors caused by "wrong" template type. Using <i>WCP</i> type constraints you can check
/// up to 19 common type traits, you can perform logic operations over constraints checks, you can
/// create your new type constraint templates, etc. The very good example to start of is \ref Serialization.cpp.<br><br>
/// <code><pre class='_code'>
///     // C# example of generic type constraint.
///     using System;
///     public class MyGenericClass &lt;T&gt; where T: IComparable, IClonable
///     { ... }
/// </pre></code>
/// and this is a small part or what <i>WCP</i> type constraints allow you to do:
/// <code><pre class='_code'>
///     template&lt;class T&gt;
///     class Serializable
///     {
///        // A type constrained to be either POD or integral
///        // and to have at least one data member.
///        \ref WCP_ENSURE_TYPE_CONSTRAINT(
///            wcp::constraint&lt;T,
///                wcp::and_&lt;
///                    wcp::or_&lt;
///                        wcp::has_traits_&lt;wcp::is_pod&gt;,
///                        wcp::has_traits_&lt;wcp::is_integral&gt; &gt;,
///                    wcp::not_&lt;wcp::has_traits_&lt;wcp::is_empty&gt; &gt;
///                &gt;
///            &gt; \ref IS_MAINTAINED);
///         ...
///     };
/// </pre></code>
/// Thus, <i>WCP</i> has the following type constraints:
/// <ul>
///     <li>wcp::has_traits_ - checks a type traits defined by wcp::type_traits.</li>
///     <li>wcp::is_base_of_&lt;T&gt; - checks if a checked type is base
///         for a type specified.</li>
///     <li>wcp::is_derived_from_&lt;T&gt; - checks if a checked type is
///         derived from a type specified.</li>
///     <li>wcp::is_convertible_to_&lt;T&gt; - checks if a checked
///         type is convertible to a type specified.</li>
///     <li>wcp::is_equal_to_&lt;T&gt; - checks if a checked
///         type is the same as a type specified.</li>
///     <li>wcp::not_&lt;Constraint&gt; - negates a constraint.</li>
///     <li>wcp::or_&lt;Constraint1, Constraint2&gt; - checks if at least
///         one of constraints is not violated.</li>
///     <li>wcp::and_&lt;Constraint1, Constraint2&gt; - checks if both
///         constraints are not violated.</li>
/// </ul>
/// <br><br></li>
/// <li>And finally C++ developers may take advantage of multicast events and unicast delegates,
/// just almost like <i>.Net</i> event handling:
/// <code><pre class='_code'>
///     struct EventHandler
///     {
///         void OnEvent(const wcp::char_t* name)
///         { std::tcout &lt;&lt;TEXT("Event: ") &lt;&lt;name &lt;&lt;std::endl; }
///     };
/// &nbsp;
///     struct EventSource
///     {
///         typedef wcp::multicast_event&lt;void, const wcp::char_t*&gt; event_t;
///         event_t SomeEvent;
///     };
/// &nbsp;
///     EventHandler handler[2];
///     EventSource source;
/// &nbsp;
///     typedef EventSource::event_t::delegate_t delegate_t;
///     delegate_t d1(handler[0], &EventHandler::OnEvent);
///     delegate_t d2(handler[1], &EventHandler::OnEvent);
///     source.SomeEvent += d1;
///     source.SomeEvent += d2;
///     source.SomeEvent("Event Name");
///     source.SomeEvent -= d1;
///     source.SomeEvent -= d2;
/// </pre></code>
/// Look at \ref ConsoleCtrlHandler.cpp and \ref ConsoleKbdHandler.cpp to see using of <i>WCP</i>
/// delegates and events in practice.<br><br>
/// </li>
/// <li>Moreover, there are also a whole bunch of useful classes and macros in <i>WCP</i>:
/// <i>STL</i>-compatible case insensitive string (std::stringi, std::wstringi, std::tstringi)
/// and other <i>STL</i> extensions (wcp/stlex.h), string tools (wcp/strtools.h), extremely
/// convenient thread local storage (wcp::tls), typed buffers (wcp::buffer) that
/// allows you to operate with a buffer as with a vector of bytes, threads and threadpools
/// (wcp::thread, wcp::thread_class, wcp::threadpool), runable tasks (wcp::task, wcp::safe_task),
/// any object adaptor (wcp::any), %reference adaptor (wcp::reference), Windows error handling
/// (wcp::win_exception, wcp::win_verif), thread safe singleton with lazy initialization
/// (wcp::safe_singleton), etc.<br><br></li>
/// </ul>
/// <br>
/// <br>
/// <b>Just enjoy!</b>
/// </summary>


// Global WCP headers
#include "_core.h"

/// <summary>
/// WCP working namespace.
/// </summary>
namespace WCP_NS
{ }


#undef __WCPLIB_H_CYCLE__
#endif /*__WCPLIB_H__*/
