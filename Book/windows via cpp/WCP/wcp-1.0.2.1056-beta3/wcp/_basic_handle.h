#ifdef ____BASIC_HANDLE_H_CYCLE__
#error Cyclic dependency discovered: _basic_handle.h
#endif

#ifndef ___BASIC_HANDLE_H__


#ifndef DOXYGEN

#define ___BASIC_HANDLE_H__
#define ___BASIC_HANDLE_H_CYCLE__

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
///      01/21/2010 - Added type constraints for the wcp::basic_handle class.
///                   Adde IsValid and MakeInvalid template parameters and got
///                   rid of comparison to a predefined invalid handle value.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/



#include "constraints.h"

/** \file
    Handle basic adaptor implementation.

    To use this file include wcp/handles.h.
*/



namespace WCP_NS
{
#ifndef DOXYGEN

namespace internals
{
template<class T>
inline bool default_is_handle_valid(const T& h)
{
    return h != T();
}

template<class T>
inline void default_make_invalid_handle(T& h)
{
    h = T();
}

} // namespace internals

#endif /*DOXYGEN*/

/// <summary>
/// Handle wrapper.
/// </summary>
/// <typeparam name='T'>Wrapped handle type.</typeparam>
/// <typeparam name='Close'>Handle close routine.</typeparam>
/// <typeparam name='IsValid'>Handle validation routine. Optional.</typeparam>
/// <typeparam name='MakeInvalid'>Makes handle invalid. Optional.</typeparam>
/// <remarks>
/// To use this class include wcp/handles.h.
/// </remarks>
template<class T,
         void(*Close)(T&),
         bool(*IsValid)(const T&) = internals::default_is_handle_valid<T>,
         void(*MakeInvalid)(T&) = internals::default_make_invalid_handle<T> >
class basic_handle : noncopyable
{
    WCP_ENSURE_TYPE_CONSTRAINT
    constraint<T,
               or_<
               has_traits_<is_integral>,
               not_<has_traits_<has_user_destructor> >
               >
               > IS_MAINTAINED;

    T handle;

public:

    /// <summary>
    /// Represents wrapped handle type.
    /// </summary>
    typedef T handle_type; //test13

    /// <summary>
    /// Constructs an empty handle object and initializes a
    /// wrapped handle value to InvalidHandle value.
    /// </summary>
    basic_handle() //test3
    {
        MakeInvalid(handle);
    }

    /// <summary>
    /// Constructs a handle object from a handle specified.
    /// </summary>
    /// <param name='handle'>Handle to construct from.</param>
    basic_handle(const T& handle) //test4
        : handle(handle)
    { }

    /// <summary>
    /// Handles handle object object destruction.
    /// A wrapped handle will be automatically closed.
    /// </summary>
    ~basic_handle()
    {
        reset();
    }

    /// <summary>
    /// Closes a wrapped handle if opened and sets its value to InvalidHandle.
    /// </summary>
    void reset() //test5
    {
        if(IsValid(handle))
        {
            Close(handle);
            MakeInvalid(handle);
        }
    }

    /// <summary>
    /// Closes a wrapped handle if opened and sets new handle value.
    /// </summary>
    /// <param name='new_handle'>New handle value to set.</param>
    void reset(const T& new_handle) //test6
    {
        reset();
        handle = new_handle;
    }

    /// <summary>
    /// Closes a wrapped handle if opened and sets new handle value.
    /// </summary>
    /// <param name='new_handle'>New handle value to set.</param>
    /// <returns>Reference to itself.</returns>
    basic_handle& operator = (const T& new_handle) //test7
    {
        reset(new_handle);
        return *this;
    }

    /// <summary>
    /// Compares with a native handle.
    /// </summary>
    /// <param name='handle'>Handle to compare with.</param>
    /// <returns>true if two handles are equal.</returns>
    bool operator == (const T& handle) const //test14
    {
        return handle == this->handle;
    }

    /// <summary>
    /// Detaches a wrapped handle from a handle object. Sets
    /// wrapped handle value to InvalidHandle.
    /// </summary>
    /// <returns>Detached handle value.</returns>
    T detach() //test8
    {
        T h = handle;
        MakeInvalid(handle);
        return h;
    }

    /// <summary>
    /// Casts a handle object to a handle type.
    /// </summary>
    /// <returns>Wrapped handle value.</returns>
    operator const T& ()  const //test10
    {
        return handle;
    }

    /// <summary>
    /// Checks if a wrapped handle value is valid, i.e. not equal to InvalidHandle.
    /// </summary>
    /// <returns>true if a wrapped handle is valid.</returns>
    bool valid() const //test11
    {
        return IsValid(handle);
    }

    /// <summary>
    /// Resets wrapped handle value and returns its pointer.
    /// </summary>
    /// <returns>Wrapped handle pointer.</returns>
    T* operator & () //test12
    {
        reset();
        return &handle;
    }
};


} // namespace WCP_NS

#undef ___BASIC_HANDLE_H_CYCLE__
#endif /*___BASIC_HANDLE_H__*/
