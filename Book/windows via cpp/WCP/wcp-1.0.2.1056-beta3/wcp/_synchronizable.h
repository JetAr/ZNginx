#ifdef ____SYNCHRONIZABLE_H_CYCLE__
#error Cyclic dependency discovered: _synchronizable.h
#endif

#ifndef ___SYNCHRONIZABLE_H__


#ifndef DOXYGEN

#define ___SYNCHRONIZABLE_H__
#define ___SYNCHRONIZABLE_H_CYCLE__

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
///      11/22/2009 - wcp::synchronized&lt;T&gt; template added.
///
/////////////////////////////////////////////////////////////////////////////////


#endif /*DOXYGEN*/

/** \file
    Contains classes and macros which provides a basic synchronizable
    object interface and tooling.

    To use this file include wcp/sync.h.
*/


#include "_guards.h"
#include "_sync_ptr.h"



namespace WCP_NS
{
template<class T, class SO> class synchronizable;

#ifndef DOXYGEN

namespace internals
{
/// <summary>
/// Describes a basic interface for an object that is supposed to be synchronized.
/// </summary>
struct basic_synchronizable_
{
    /// <summary>
    /// Returns an associated syncronization object %reference.
    /// </summary>
    /// <returns>Associated syncronizable object %reference.</returns>
    virtual sync_object& get_sync_object() = 0;

    /// <summary>
    /// Returns an associated syncronization object %reference.
    /// </summary>
    /// <returns>Associated syncronizable object %reference.</returns>
    virtual const sync_object& get_sync_object() const = 0;
};

// Implements a basic interface for an object that is supposed to be synchronized.
template<class SO>
class basic_synchronizable
    : public basic_synchronizable_
{
    template<class T, class SO>
    friend class synchronizable;

    mutable std::auto_ptr<SO> synchronization_object;

    /// <summary>
    /// Constructs a basic synchronizable object
    /// with a synchronization object return by a user defined function.
    /// Normally, it has to be SO::create_instance.
    /// </summary>
    template<class Init>
    explicit basic_synchronizable(Init fn_init)
        : synchronization_object(fn_init())
    { }

public:

    /// <summary>
    /// Returns an associated syncronization object %reference.
    /// </summary>
    /// <returns>Associated syncronizable object %reference.</returns>
    sync_object& get_sync_object()
    {
        return *synchronization_object;
    }

    /// <summary>
    /// Returns an associated syncronization object %reference.
    /// </summary>
    /// <returns>Associated syncronizable object %reference.</returns>
    const sync_object& get_sync_object() const
    {
        return *synchronization_object;
    }

    /// <summary>
    /// Handles a synchronizable object destruction.
    /// </summary>
    virtual ~basic_synchronizable() = 0
                                      { }
};

} // namespace internals

#endif /*DOXYGEN*/

/// <summary>
/// A synchronizable object interface implementation.
/// </summary>
/// <typeparam name='T'>Class type that derives a synchronizable class.</typeparam>
/// <typeparam name='SO'>Synchronization object type that provides object
/// synchronization. By default wcp::monitor is used.</typeparam>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this class.<br><br>
/// Look at the wcp::sync_ptr&lt;T&gt; description for an example.
/// </remarks>
template<class T, class SO = monitor>
class synchronizable
    : public internals::basic_synchronizable<SO>
{
public:

    /// <summary>
    /// Synchronizes an object access.
    /// </summary>
    /// <returns>Synchronizable pointer to an object to synchronize access for.</returns>
    sync_ptr<T> sync()
    {
        return sync_ptr<T>(*synchronization_object,
                           static_cast<T&>(*this));
    }

    /// <summary>
    /// Synchronizes an object access.
    /// </summary>
    /// <returns>Synchronizable pointer to an object to synchronize access for.</returns>
    sync_ptr<const T> sync() const
    {
        return sync_ptr<const T>(*synchronization_object,
                                 static_cast<const T&>(*this));
    }

protected:

    /// <summary>
    /// Constructs a synchronizable object with a default synchronization object.
    /// </summary>
    synchronizable()
        : internals::basic_synchronizable<SO>(SO::create_instance)
    { }

    /// <summary>
    /// Constructs a synchronizable object with a synchronization
    /// object returned by fn_init functor.
    /// </summary>
    /// <typeparam name='Init'>Synchronization object creator type.</typeparam>
    /// <param name='fn_init'>Synchronization object creator functor.</param>
    template<class Init>
    explicit synchronizable(Init fn_init)
        : internals::basic_synchronizable<SO>(fn_init)
    { }

    /// <summary>
    /// Handles synchronizable object destruction.
    /// </summary>
    virtual ~synchronizable() = 0
                                { }
};

//------------------------------------------------------------------------------

/// <summary>
/// Synchronizable adapter for non-synchronizable object.
/// </summary>
/// <typeparam name='T'>Adaptee type.</typeparam>
/// <remarks>
/// If an object required to be synchronized without new synchronizable class
/// creation or new synchronization variable, WCP provides a convenient way
/// of wrapping a non-synchronizable objects to synchronizable adaptor.
/// <br><br><b>Example:</b>
/// <code><pre class='_code'>
///      wcp::synchronized&lt;std::tstringi&gt; gGlobalName;
///      ...
///      LOCK(gGlobalName) // This code block is synchronous
///      {
///         std::tcout <<TEXT("Input new global name: ");
///         std::tcin >>(*gGlobalName);
///      }
/// </pre></code>
/// <br>
/// Include <i>wcp/sync.h</i> to use this class.<br><br>
/// </remarks>
template<class T>
class synchronized
    : public synchronizable<synchronized<T> >
{
    T adaptee;

public:

    /// <summary>
    /// Returns adaptee %reference.
    /// </summary>
    /// <returns>Adaptee %reference.</returns>
    T& operator * ()
    {
        return adaptee;
    }

    /// <summary>
    /// Returns adaptee %reference.
    /// </summary>
    /// <returns>Adaptee %reference.</returns>
    const T& operator * () const
    {
        return adaptee;
    }

    /// <summary>
    /// Returns adaptee pointer.
    /// </summary>
    /// <returns>Adaptee pointer.</returns>
    T* operator -> ()
    {
        return &adaptee;
    }

    /// <summary>
    /// Returns adaptee pointer.
    /// </summary>
    /// <returns>Adaptee pointer.</returns>
    const T* operator -> () const
    {
        return &adaptee;
    }
};

} // namespace WCP_NS

/// <summary>
/// Locks a synchronizable object. Provides a C#-like object locking syntax.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      struct object : public wcp::syncronizable&lt;object&gt; { ... };
///      <br>
///      object obj1, obj2;
///      LOCK(obj1)
///      {
///          ...
///      }
/// </pre></code>
/// or
/// <code><pre class='_code'>
///      LOCK2(obj1, obj2)
///      {
///          ...
///      }
/// </pre></code>
/// identical to
/// <code><pre class='_code'>
///      bool locked = false;
///      try
///      {
///          obj1.get_sync_object().seize();
///          locked = true;
///          ...
///          obj1.get_sync_object().release();
///      }
///      catch(...)
///      {
///          if(locked) obj1.get_sync_object().release();
///      }
/// </pre></code>
/// identical to
/// <code><pre class='_code'>
///      {
///          wcp::guard guard(obj1.get_sync_object());
///          ...
///      }
/// </pre></code>
/// and identical to
/// <code><pre class='_code'>
///      GUARD(obj1.get_sync_object())
///      {
///          ...
///      }
/// </pre></code>
/// </summary>
#define LOCK    LOCK1

/// <summary>
/// Locks a synchronizable object.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj;
///      LOCK1(obj)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// </remarks>
#define LOCK1(what1)    GUARD1((what1).get_sync_object())

/// <summary>
/// Locks two synchronizable objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj1, obj2;
///      LOCK2(obj1, obj2)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// </remarks>
#define LOCK2(what1, what2)                                     \
    GUARD2((what1).get_sync_object(),                           \
           (what2).get_sync_object())

/// <summary>
/// Locks three synchronizable objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj1, obj2, obj3;
///      LOCK3(obj1, obj2, obj3)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// <remarks>
#define LOCK3(what1, what2, what3)                              \
    GUARD3((what1).get_sync_object(),                           \
           (what2).get_sync_object(),                           \
           (what3).get_sync_object())

/// <summary>
/// Locks four synchronizable objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj1, obj2, obj3, obj4;
///      LOCK4(obj1, obj2, obj3, obj4)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// <remarks>
#define LOCK4(what1, what2, what3, what4)                       \
    GUARD4((what1).get_sync_object(),                           \
           (what2).get_sync_object(),                           \
           (what3).get_sync_object(),                           \
           (what4).get_sync_object())

/// <summary>
/// Locks five synchronizable objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj1, obj2, obj3, obj4, obj5;
///      LOCK5(obj1, obj2, obj3, obj4, obj5)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// <remarks>
#define LOCK5(what1, what2, what3, what4, what5)                \
    GUARD5((what1).get_sync_object(),                           \
           (what2).get_sync_object(),                           \
           (what3).get_sync_object(),                           \
           (what4).get_sync_object(),                           \
           (what5).get_sync_object())



#undef ___SYNCHRONIZABLE_H_CYCLE__
#endif /*___SYNCHRONIZABLE_H__*/
