#ifdef ___STLEX_H_CYCLE__
#error Cyclic dependency discovered: stlex.h
#endif

#ifndef __STLEX_H__


#ifndef DOXYGEN

#define __STLEX_H__
#define __STLEX_H_CYCLE__

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
///      11/6/2009 - Initial release.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains and includes STL extensions.
*/


#include "_wcpdef.h"
#include "stringi.h"

#include <sstream>
#include <fstream>
#include <iostream>



namespace std
{
    //*****************************************************************************
    // Character set depended type definitions
    //*****************************************************************************

    /// <summary>
    /// Identifies a unicode STL string if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL string.
    /// </summary>
    typedef std::basic_string<WCP_NS::tchar_t> tstring;

    /// <summary>
    /// Identifies a unicode STL case-insensitive string if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies 
    /// ANSI STL case-insensitive string.
    /// </summary>
    typedef std::basic_stringi<WCP_NS::tchar_t> tstringi;

    /// <summary>
    /// Identifies a unicode STL file buffer if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL file buffer.
    /// </summary>
    typedef std::basic_filebuf<WCP_NS::tchar_t> tfilebuff;

    /// <summary>
    /// Identifies a unicode STL file stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL file stream.
    /// </summary>
    typedef std::basic_fstream<WCP_NS::tchar_t> tfstream;

    /// <summary>
    /// Identifies a unicode STL input file stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL input file stream.
    /// </summary>
    typedef std::basic_ifstream<WCP_NS::tchar_t> tifstream;

    /// <summary>
    /// Identifies a unicode STL ios class if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL ios class.
    /// </summary>
    typedef std::basic_ios<WCP_NS::tchar_t> tios;

    /// <summary>
    /// Identifies a unicode STL I/O stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL I/O stream.
    /// </summary>
    typedef std::basic_iostream<WCP_NS::tchar_t> tiostream;

    /// <summary>
    /// Identifies a unicode STL input stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL input stream.
    /// </summary>
    typedef std::basic_istream<WCP_NS::tchar_t> tistream;

    /// <summary>
    /// Identifies a unicode STL input string stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL input string stream.
    /// </summary>
    typedef std::basic_istringstream<WCP_NS::tchar_t> tistringstream;

    /// <summary>
    /// Identifies a unicode STL output file stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL output file stream.
    /// </summary>
    typedef std::basic_ofstream<WCP_NS::tchar_t> tofstream;

    /// <summary>
    /// Identifies a unicode STL output stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL output stream.
    /// </summary>
    typedef std::basic_ostream<WCP_NS::tchar_t> tostream;

    /// <summary>
    /// Identifies a unicode STL output string stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL output string stream.
    /// </summary>
    typedef std::basic_ostringstream<WCP_NS::tchar_t> tostringstream;

    /// <summary>
    /// Identifies a unicode STL stream buffer if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL stream buffer.
    /// </summary>
    typedef std::basic_streambuf<WCP_NS::tchar_t> tstreambuf;

    /// <summary>
    /// Identifies a unicode STL string buffer if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL string buffer.
    /// </summary>
    typedef std::basic_stringbuf<WCP_NS::tchar_t> tstringbuf;

    /// <summary>
    /// Identifies a unicode STL string stream if a UNICODE preprocessor 
    /// directive is defined during compilation. Otherwise, identifies ANSI STL string stream.
    /// </summary>
    typedef std::basic_stringstream<WCP_NS::tchar_t> tstringstream;





    //*****************************************************************************
    // Character ste depended global variables
    //*****************************************************************************

#ifndef DOXYGEN

    namespace
    {

#endif /*DOXYGEN*/

        /// <summary>
        /// Identifies a global unicode STL output stream if a UNICODE preprocessor 
        /// directive is defined during compilation. Otherwise, identifies a global
        /// ANSI STL output stream.
        /// </summary>
        tostream& tcout = 

#ifdef UNICODE
            wcout
#else
            cout
#endif
            ;

        /// <summary>
        /// Identifies a global unicode STL input stream if a UNICODE preprocessor 
        /// directive is defined during compilation. Otherwise, identifies a global
        /// ANSI STL input stream.
        /// </summary>
        tistream& tcin = 

#ifdef UNICODE
            wcin
#else
            cin
#endif
            ;


        /// <summary>
        /// Identifies a global unicode STL input stream if a UNICODE preprocessor 
        /// directive is defined during compilation. Otherwise, identifies a global
        /// ANSI STL input stream.
        /// </summary>
        tostream& tcerr = 

#ifdef UNICODE
            wcerr
#else
            cerr
#endif
            ;


        /// <summary>
        /// Identifies a global unicode STL input stream if a UNICODE preprocessor 
        /// directive is defined during compilation. Otherwise, identifies a global
        /// ANSI STL input stream.
        /// </summary>
        tostream& tclog = 

#ifdef UNICODE
            wclog
#else
            clog
#endif
            ;

#ifndef DOXYGEN
    }

#endif /*DOXYGEN*/


} // namespace std







namespace WCP_NS 
{
    //*****************************************************************************
    // STD Template providers
    //*****************************************************************************

    /// <summary>
    /// std::allocator template provider.
    /// </summary>
    struct std_allocator_template_provider
    {
        /// <summary>
        /// Template type.
        /// </summary>
        /// <typeparam name='T'>The type of object for which storage is 
        /// being allocated or deallocated.</typeparam>
        template<class Type> struct template_provider
        { typedef std::allocator<Type> type; };
    };

} // namespace WCP_NS


#undef __STLEX_H_CYCLE__
#endif /*__STLEX_H__*/
