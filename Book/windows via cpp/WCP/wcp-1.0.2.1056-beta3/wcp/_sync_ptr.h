#ifdef ____SYNC_PTR_H_CYCLE__
#error Cyclic dependency discovered: _sync_ptr.h
#endif

#ifndef ___SYNC_PTR_H__


#ifndef DOXYGEN

#define ___SYNC_PTR_H__
#define ___SYNC_PTR_H_CYCLE__

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
///      12/15/2009 - Checking pointer validity is removed (a synchronization pointer
///                   cannot have wrong value since it is constructed accepts from 
///                   object reference). Synchronization pointer template provider 
///                   is removed.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Synchronizable pointer implementation. Synchronizable pointer locks 
    a synchronization object on initialization and releases it's ownership on 
    destruction thus not allowing a concurrent access to a dereferenced resource.

    To use this file include wcp/sync.h.
*/


#include <memory>

#include "smartptr.h"



namespace WCP_NS 
{
    /// <summary>
    /// Provides a synchronized pointer functional.
    /// </summary>
    /// <typeparam name='T'>Object type.</typeparam>
    /// <remarks>
    /// Include <i>wcp/sync.h</i> to use this class.<br><br>
    /// <b>Example:</b>
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
    ///    Person Jack;
    /// &nbsp;
    ///    void do_test()
    ///    {
    ///        // wcp::synchronizable&lt;T&gt;::sync retrurns 
    ///        Jack.sync()->setName("Jack"); // This line of code will be executed synchronously.
    /// &nbsp;
    ///        // Is identical to
    /// &nbsp;
    ///        LOCK(Jack) // This code will be executed synchronously too.
    ///        {
    ///            Jack.setName("Jack");
    ///        }
    ///    }
    /// </pre></code>
    /// </remarks>
    template<class T> class sync_ptr : noncopyable
    {
        shared_ptr<guard> pguard;
        mutable T& pobj;

    public:

        /// <summary>
        /// Object type redefinition.
        /// </summary>
        typedef T value_type;


        /// <summary>
        /// Constructs a synchronizable pointer.
        /// </summary>
        /// <param name='so'>Syncronizable object to control resource access with.</param>
        /// <param name='object'>Object to syncronize access for.</param>
        /// <param name='tryseize'>Determines the behaviour of an guard object that guars so.</param>
        sync_ptr(const sync_object& so, 
                 T& object, 
                 bool tryseize = false)
                 : pguard(new guard(so, tryseize))
                 , pobj(object)
        { }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Pointer to a resource held.</returns>
        const T* operator -> () const
        { return &pobj; }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Pointer to a resource held.</returns>
        const T& operator * () const
        { return pobj; }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Reference to a resource held.</returns>
        const T& ref() const
        { return pobj; }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Pointer to a resource held.</returns>
        const T* ptr() const
        { return &pobj; }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Pointer to a resource held.</returns>
        T* operator -> ()
        { return &pobj; }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Reference to a resource held.</returns>
        T& operator * ()
        { return pobj; }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Reference to a resource held.</returns>
        T& ref()
        { return pobj; }

        /// <summary>
        /// Dereferences a synchronizable pointer.
        /// </summary>
        /// <returns>Pointer to a resource held.</returns>
        T* ptr()
        { return &pobj; }

        /// <summary>
        /// Checks is a pointer is locked.
        /// </summary>
        /// <returns>true if a pointer is locked.</returns>
        bool locked() const
        { return *pguard; }
    };

    //-----------------------------------------------------------------------------

} // namespace WCP_NS


#undef ___SYNC_PTR_H_CYCLE__
#endif /*___SYNC_PTR_H__*/
