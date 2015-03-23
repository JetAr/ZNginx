#ifdef ___ANY_H_CYCLE__
#error Cyclic dependency discovered: any.h
#endif

#ifndef __ANY_H__


#ifndef DOXYGEN

#define __ANY_H__
#define __ANY_H_CYCLE__

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
///      07/25/2009 - Initial release.
///      10/02/2009 - XML comments added.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains an any object adaptor class implementation. A resource cannot be shared 
    between thread safe and thread unsafe any object adapters.
*/


#include "smartptr.h"



namespace WCP_NS 
{
    /// <summary>
    /// Represents an adapter for an object of any type. 
    /// PtrTemplateProvider is a template provider for a smart 
    /// pointer that holds an object pointer.
    /// </summary>
    /// <typeparam name='PtrTemplateProvider'>Owned object type.</typeparam>
    template<class PtrTemplateProvider>
    class basic_any
    {
        /// <summary>
        /// Pointer a holder interface.
        /// </summary>
        struct holder
        {
            /// <summary>
            /// Returns a pointer to an owned object.
            /// </summary>
            /// <returns>Pointer to an owned object.</returns>
            virtual void* get() = 0;

            /// <summary>
            /// Handles adapter destruction.
            /// </summary>
            virtual ~holder() = 0 { }
        };

        /// <summary>
        /// Implements an object holder.
        /// </summary>
        /// <typeparam name='T'>Object type.</typeparam>
        template<typename T>
        class object_holder 
            : public holder
        {
            T obj;

        public:

            /// <summary>
            /// Construct a resource holder.
            /// </summary>
            /// <param name='rhs'>Object to hold a copy from.</param>
            explicit object_holder(const T& rhs) 
                : obj(rhs) { }

            /// <summary>
            /// Returns a pointer to an owned object.
            /// </summary>
            /// <returns>Pointer to an owned object.</returns>
            virtual void* get()
            { return &obj; }
        };

        /// <summary>
        /// Smart pointer type.
        /// </summary>
        typedef typename PtrTemplateProvider
            ::template template_provider<holder>::type ptr_t;
        
        /// <summary>
        /// Object holder smart pointer.
        /// </summary>
        ptr_t prc;

    public:

        /// <summary>
        /// Constructs an empty adapter.
        /// </summary>
        basic_any() { }

        /// <summary>
        /// Construct an adapter from an object specified. Makes a copy of an object specified.
        /// </summary>
        /// <typeparam name='T'>Object type to construct an adapter from.</typeparam>
        /// <param name='rhs'>Object to adapt a copy of.</param>
        template<typename T> basic_any(const T& rhs) 
            : prc(new object_holder<T>(rhs)) { }

        /// <summary>
        /// Shares an adaptee from an adapter specified.
        /// </summary>
        /// <param name='rhs'>Adapter to share an adaptee of.</param>
        basic_any(const basic_any& rhs) : prc(rhs.prc) { }

        /// <summary>
        /// Checks if an adapter is empty.
        /// </summary>
        /// <returns>true if an adapter not empty.</returns>
        operator bool () const 
        { return prc; }

        /// <summary>
        /// Sets a new adaptee. Makes a copy of an object passed.
        /// </summary>
        /// <typeparam name='T'>Object type.</typeparam>
        /// <param name='rhs'>Object to adapt a copy of.</param>
        /// <returns>Reference to itself.</returns>
        template<typename T> basic_any& operator = (const T& rhs)
        {
            prc.reset(new object_holder<T>(rhs));
            return *this;
        };

        /// <summary>
        /// Shares an adaptee from an adapter specified.
        /// </summary>
        /// <param name='rhs'>Adapter to shared an adaptee of.</param>
        /// <returns>Reference to itself.</returns>
        basic_any& operator = (const basic_any& rhs)
        {
            if(&rhs != this)
                prc = rhs.prc;
            return *this;
        }

        /// <summary>
        /// Returns a %reference of specified type to an owned object.
        /// </summary>
        /// <typeparam name='T'>Object type.</typeparam>
        /// <returns>Reference to a owned object.</returns>
        template<typename T> T& get()
        { return *reinterpret_cast<T*>(prc->get()); }

        /// <summary>
        /// Returns a constant %reference of specified type to an owned object.
        /// </summary>
        /// <typeparam name='T'>Object type.</typeparam>
        /// <returns>Constant %reference to an owned object.</returns>
        template<typename T> const T& get() const
        { return *reinterpret_cast<const T*>(prc->get()); }

        /// <summary>
        /// Releases an adaptee.
        /// </summary>
        void reset()
        { prc.reset(); }

        /// <summary>
        /// Releases an adaptee and make and set a new adaptee. 
        /// </summary>
        /// <typeparam name='T'>Object type.</typeparam>
        /// <param name='obj'>Object to adapt a copy of.</param>
        template<class T>
        void reset(const T& obj)
        { prc.reset(new object_holder<T>(obj)); }
    };

    /// <summary>
    /// Thread unsafe any object adaptor.
    /// </summary>
    typedef basic_any<shared_ptr_template_provider> any;

    /// <summary>
    /// Thread safe any object adaptor.
    /// </summary>
    typedef basic_any<safe_shared_ptr_template_provider> safe_any;

} // namespace WCP_NS



#undef __ANY_H_CYCLE__
#endif /*__ANY_H__*/
