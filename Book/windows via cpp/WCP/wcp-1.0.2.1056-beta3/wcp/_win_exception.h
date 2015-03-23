#ifdef ____WIN_EXCEPTION_H_CYCLE__
#error Cyclic dependency discovered: _win_exception.h
#endif

#ifndef ___WIN_EXCEPTION_H__


#ifndef DOXYGEN

#define ___WIN_EXCEPTION_H__
#define ___WIN_EXCEPTION_H_CYCLE__

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
    Contains Windows exception class implementation.

    To use this file include wcp/errh.h.
*/


#include <sstream>

#include "_exception.h"



#if !defined(WCP_SHOW_WINDOWS_ERROR_CODE) && defined(_DEBUG)

/// <summary>
/// Forces library to include error code value into the description
/// of Windows %exception. By default is enable in Debug, and disable in Release.
/// </summary>
#define WCP_SHOW_WINDOWS_ERROR_CODE

#endif /*WCP_SHOW_WINDOWS_ERROR_CODE*/


namespace WCP_NS 
{
    /// <summary>
    /// Describes a Windows %exception.
    /// </summary>
    /// <remarks>
    /// Include <i>wcp/errh.h</i> to use this class.
    /// </remarks>
    class win_exception : public exception
    {
        dword_t error_code;

    public:

        /// <summary>
        /// Constructs a windows %exception object.
        /// </summary>
        /// <param name='error_code'>Windows error code.</param>
        win_exception(dword_t error_code)
            : error_code(error_code)
        { }

        /// <summary>
        /// Returns a windows error code passed into constructor.
        /// </summary>
        /// <returns>Windows error code passed into constructor</returns>
        dword_t code() const
        { return error_code; }

        /// <summary>
        /// Obtains an error code description.
        /// </summary>
        /// <param name='error_code'>Windows error code to obtain a description for.</param>
        /// <param name='message'>String where to write an error description.</param>
        /// <returns>true if description is obtained.</returns>
        static bool error_message(dword_t error_code,
                                  __out std::tstring& message)
        {
            bool description_found = false;

            LPTSTR msg_buff = NULL;
            if(!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                NULL, error_code, GetUserDefaultLangID(), (LPTSTR)&msg_buff, 0, NULL))
            {
                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                    NULL, error_code, GetSystemDefaultLangID(), (LPTSTR)&msg_buff, 0, NULL);
            }

            if(msg_buff)
            {
#ifdef WCP_SHOW_WINDOWS_ERROR_CODE
                std::tostringstream ss;
                ss <<"[0x" <<std::hex <<std::uppercase <<error_code <<"] " <<msg_buff;
                message = ss.str();
#else
                message = msg_buff;
#endif
                LocalFree(msg_buff);

                description_found = true;
            }
            else
            {
#ifdef WCP_SHOW_WINDOWS_ERROR_CODE
                std::tostringstream ss;
                ss <<"Undefined error #" <<std::hex <<std::uppercase <<error_code;
                message = ss.str();
#else
                message = TEXT("Undefined error");
#endif
            }

            return description_found;
        }

    protected:

        win_exception(const win_exception& e)
            : exception(e)
            , error_code(e.error_code)
        { }

        virtual void build_error_message(std::tstring& msg) const
        { error_message(error_code, msg); }
    };

    //-----------------------------------------------------------------------------

    /// <summary>
    /// Throws a wcp::win_exception if an error code specified means an error.
    /// </summary>
    /// <param name='error_code'>Windows error code to verify.</param>
    /// <remarks>
    /// Include <i>wcp/errh.h</i> to use this function.
    /// </remarks>
    inline void win_verif(dword_t error_code)
    {
        if(error_code != ERROR_SUCCESS)
            throw win_exception(error_code);
    }

} // namespace WCP_NS


#undef ___WIN_EXCEPTION_H_CYCLE__
#endif /*___WIN_EXCEPTION_H__*/
