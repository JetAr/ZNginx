#ifdef ____EXCEPTION_H_CYCLE__
#error Cyclic dependency discovered: _exception.h
#endif

#ifndef ___EXCEPTION_H__


#ifndef DOXYGEN

#define ___EXCEPTION_H__
#define ___EXCEPTION_H_CYCLE__

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
///      12/15/2009 - invalid_argument_exception class added.
///      12/19/2009 - not_implemented_exception class added.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains basic exception class implementation.

    To use this file include wcp/errh.h.
*/

#include <string>

#include "stlex.h"
#include "noncopyable.h"



namespace WCP_NS 
{
    /// <summary>
    /// Describes a basic interface for all WCP exceptions.
    /// </summary>
    /// <remarks>
    /// To use this class include <i>wcp/errh.h</i>.
    /// </remarks>
    class exception : noncopyable
    {
        mutable std::tstring message;

    public:

        /// <summary>
        /// Constructs an exception object.
        /// </summary>
        /// <param name='message'>Exception message. May be empty but in this case 
        /// a derived class must to implement build_error_message method.</param>
        explicit exception(const std::tstring& message)
            : message(message)
        { }

        /// <summary>
        /// Constructs an exception object.
        /// </summary>
        /// <param name='message'>Exception message. May be empty but in this case 
        /// a derived class must to implement build_error_message method.</param>
        explicit exception(const wcp::tchar_t* message)
        { 
            if(message)
                this->message.assign(message);
        }

        /// <summary>
        /// Returns an %exception message.
        /// </summary>
        /// <returns>Exception message.</returns>
        const std::tstring& what() const
        { 
            if(message.empty())
                build_error_message(message);

            return message; 
        }

        /// <summary>
        /// Handles exception destruction.
        /// </summary>
        virtual ~exception()
        { }

    protected:

        exception() { }

        /// <summary>
        /// Must be implemented be derived classes that pass empty message
        /// to an exception constructor.
        /// </summary>
        /// <param name='msg'>String where to store %exception message.</param>
        virtual void build_error_message(std::tstring& msg) const
        { }

        /// <summary>
        /// Constructs an exception from another exception object.
        /// </summary>
        /// <param name='e'>An exception to construct from.</param>
        exception(const exception& e)
            : message(e.message)
        { }
    };







    //******************************************************************************
    // Predefined exceptions
    //******************************************************************************

    /// <summary>
    /// Represent an 'Invalid Argument' exception.
    /// </summary>
    /// <remarks>
    /// To use this class include <i>wcp/errh.h</i>.
    /// </remarks>
    class invalid_argument_exception 
        : public exception
    {
    public:

        /// <summary>
        /// Constructs an exception object with an invalid argument name specified.
        /// </summary>
        /// <param name='arg_name'>Invalid argument name.</param>
        invalid_argument_exception(const std::tstring& arg_name)
            : arg_name(arg_name)
        { _ASSERTE("An empty argument name specified" && !arg_name.empty()); }

    private:

        std::tstring arg_name;

        virtual void build_error_message(std::tstring& msg) const
        { 
            std::tostringstream ss;
            ss <<TEXT("Argument '") <<arg_name << TEXT("' has invalid value.");
            msg = ss.str();
        }
    };





    /// <summary>
    /// Represent a 'Not Implemented' exception.
    /// </summary>
    /// <remarks>
    /// To use this class include <i>wcp/errh.h</i>.
    /// </remarks>
    class not_implemented_exception
        : public exception
    {
    public:

        /// <summary>
        /// Constructs an exception object with an optional routine name specified.
        /// </summary>
        /// <param name='routine_name'>Routine name. Optional. May be NULL.</param>
        not_implemented_exception(__in_opt const tchar_t* routine_name = NULL)
        { 
            if(routine_name)
                this->routine_name.assign(routine_name);
        }

    private:

        std::tstring routine_name;

        virtual void build_error_message(std::tstring& msg) const
        { 
            if(routine_name.empty())
                msg = TEXT("Routine is not implemented.");
            else
            {
                std::tostringstream ss;
                ss <<TEXT("The '") <<routine_name << TEXT("' routine is not implemented.");
                msg = ss.str();
            }
        }
    };





    /// <summary>
    /// Represent a 'Invalid Object State' exception.
    /// </summary>
    /// <remarks>
    /// To use this class include <i>wcp/errh.h</i>.
    /// </remarks>
    class invalid_object_state_exception
        : public exception
    {
    public:

        /// <summary>
        /// Constructs an exception object with an optional object name specified.
        /// </summary>
        /// <param name='object_name'>Object name. May be NULL.</param>
        invalid_object_state_exception(__in_opt const tchar_t* object_name = NULL)
        { 
            if(object_name)
                this->object_name.assign(object_name);
        }

    private:

        std::tstring object_name;

        virtual void build_error_message(std::tstring& msg) const
        { 
            if(object_name.empty())
                msg = TEXT("Object state is invalid.");
            else
            {
                std::tostringstream ss;
                ss <<TEXT("'") <<object_name << TEXT("' object state is invalid.");
                msg = ss.str();
            }
        }
    };
} // namespace WCP_NS

#undef ___EXCEPTION_H_CYCLE__
#endif /*___EXCEPTION_H__*/
