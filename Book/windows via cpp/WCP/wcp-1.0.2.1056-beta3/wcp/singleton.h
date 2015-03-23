#ifdef ___SINGLETON_H_CYCLE__
#error Cyclic dependency discovered: singleton.h
#endif

#ifndef __SINGLETON_H__


#ifndef DOXYGEN

#define __SINGLETON_H__
#define __SINGLETON_H_CYCLE__

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
///      12/13/2009 - Initial release.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Singleton template implementation.
*/


#include "_synchronized.h"
#include "constraints.h"



namespace WCP_NS 
{
    /// <summary>
    /// Implements Singleton design pattern.
    /// </summary>
    /// <typeparam name='Class'>Singleton object type.</typeparam>
    /// <remarks>
    /// As far as singletons can be initialized statically via 
    /// STATIC_INITIALIZATION macro, only classes that have nothrow
    /// constructors are allowed to be singletons.
    /// </remarks>
    template<class Class>
    class singleton 
    {
    public:

        /// <summary>
        /// Singleton object type.
        /// </summary>
        typedef Class singleton_object_type_t;

        /// <summary>
        /// Returns a singleton object reference.
        /// </summary>
        /// <returns>Singleton object reference.</returns>
        static Class& instance()
        {
            WCP_ENSURE_TYPE_CONSTRAINT(constraint<Class, 
                    has_traits_<has_nothrow_constructor> > IS_MAINTAINED);

            static Class object;
            return object;
        }
    };

    /// <summary>
    /// Implements thread safe Singleton design pattern.
    /// </summary>
    /// <typeparam name='Class'>Singleton object type.</typeparam>
    /// <remarks>
    /// As far as singletons can be initialized statically via 
    /// STATIC_INITIALIZATION macro, only classes that have nothrow
    /// constructors are allowed to be singletons.
    /// </remarks>
    template<class Class>
    class safe_singleton
    {
    public:

        /// <summary>
        /// Singleton object type.
        /// </summary>
        typedef Class singleton_object_type_t;

        /// <summary>
        /// Returns a singleton object reference. Thread safe.
        /// </summary>
        /// <returns>Singleton object reference.</returns>
        static Class& instance()
        {
            static Class& object = safe_instance();
            return object;
        }

    private:

        static Class& safe_instance()
        {
            WCP_ENSURE_TYPE_CONSTRAINT(constraint<Class, 
                has_traits_<has_nothrow_constructor> > IS_MAINTAINED);

            SYNCHRONIZED
            {
                static Class object;
                return object;
            }
        }
    };

    /// <summary>
    /// wcp::singleton template provider.
    /// </summary>
    struct singleton_template_provider
    {
        /// <summary>
        /// Template type.
        /// </summary>
        /// <typeparam name='Class'>Singleton object type.</typeparam>
        template<class Class>
        struct template_provider
        { typedef singleton<Class> type; };
    };

    /// <summary>
    /// wcp::safe_singleton template provider.
    /// </summary>
    struct safe_singleton_template_provider
    {
        /// <summary>
        /// Template type.
        /// </summary>
        /// <typeparam name='Class'>Singleton object type.</typeparam>
        template<class Class>
        struct template_provider
        { typedef safe_singleton<Class> type; };
    };

} // namespace WCP_NS


#undef __SINGLETON_H_CYCLE__
#endif /*__SINGLETON_H__*/
