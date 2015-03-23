#ifdef ____GUARDS_H_CYCLE__
#error Cyclic dependency discovered: _guards.h
#endif

#ifndef ___GUARDS_H__


#ifndef DOXYGEN

#define ___GUARDS_H__
#define ___GUARDS_H_CYCLE__

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
///      11/07/2009 - GUARDx macros was rebuilt based on _WCP_DECLARE_STATEMENT_BLOCK.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains implementation of %guard classes and macros.

    To use this file include wcp/sync.h.
*/



#include <memory>

#include "_syncobj.h"
#include "_stmtblock.h"

#include "noncopyable.h"



namespace WCP_NS
{
/// <summary>
/// Guards a synchronizable object. Releases a synchronizable object on destruction.
/// </summary>
/// <remarks>
/// To use this class include <i>wcp/sync.h</i>.
/// </remarks>
struct guard_1 : noncopyable
{
    /// <summary>
    /// Constructs a guard and takes ownership of a synchronizable object.
    /// </summary>
    /// <param name='so1'>Synchronizable object to guard.</param>
    explicit guard_1(const sync_object& so1)
        : so1(so1)
        , locked1(false)
    {
        so1.seize();
        locked1 = true;
    }

    /// <summary>
    /// Constructs a guard and try to take ownership of a synchronizable object.
    /// </summary>
    /// <param name='so1'>Synchronizable object to guard.</param>
    /// <param name='tryseize'>true if need to try to take ownership
    /// of synchronizable object; otherwise, false.</param>
    guard_1(const sync_object& so1,
            bool tryseize)
        : so1(so1)
        , locked1(false)
    {
        if(tryseize)
            locked1 = so1.try_seize();
        else
        {
            so1.seize();
            locked1 = true;
        }
    }

    /// <summary>
    /// Checks if guard is locked.
    /// </summary>
    /// <returns>true if guard is locked.</returns>
    operator bool () const
    {
        return locked1;
    }

    /// <summary>
    /// Handles guard destruction. Releases synchronizable object if ownership is taken.
    /// </summary>
    virtual ~guard_1()
    {
        if(locked1)
            so1.release();
    }

private:

    const sync_object& so1;
    bool locked1;
};

/// <summary>
/// guard_1 shorter alias.
/// </summary>
/// <remarks>
/// To use this class include <i>wcp/sync.h</i>.
/// </remarks>
typedef guard_1 guard;

//------------------------------------------------------------------------------

/// <summary>
/// Guards two synchronizable objects. Releases synchronizable objects on destruction.
/// </summary>
/// <remarks>
/// To use this class include <i>wcp/sync.h</i>.
/// </remarks>
struct guard_2 : noncopyable
{
    /// <summary>
    /// Constructs a guard and takes ownership of two synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    guard_2(const sync_object& so1,
            const sync_object& so2)
        : so1(so1)
        , so2(so2)
        , locked1(false)
        , locked2(false)
    {
        so1.seize();
        locked1 = true;

        so2.seize();
        locked2 = true;
    }

    /// <summary>
    /// Constructs a guard and try to take ownership of two synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    /// <param name='tryseize'>true if need to try to take ownership
    /// of synchronizable objects; otherwise, false.</param>
    guard_2(const sync_object& so1,
            const sync_object& so2,
            bool tryseize)
        : so1(so1)
        , so2(so2)
        , locked1(false)
        , locked2(false)
    {
        if(tryseize)
        {
            locked1 = so1.try_seize();
            locked2 = so2.try_seize();
        }
        else
        {
            so1.seize();
            locked1 = true;

            so2.seize();
            locked2 = true;
        }
    }

    /// <summary>
    /// Checks if guard is locked.
    /// </summary>
    /// <returns>true if guard is locked.</returns>
    operator bool () const
    {
        return locked1
               || locked2;
    }

    /// <summary>
    /// Handles guard destruction. Releases synchronizable objects if ownership is taken.
    /// </summary>
    virtual ~guard_2()
    {
        if(locked1)
            so1.release();
        if(locked2)
            so2.release();
    }

private:

    const sync_object& so1;
    const sync_object& so2;
    bool locked1;
    bool locked2;
};

//------------------------------------------------------------------------------

/// <summary>
/// Guards three synchronizable objects. Releases synchronizable objects on destruction.
/// </summary>
/// <remarks>
/// To use this class include <i>wcp/sync.h</i>.
/// </remarks>
struct guard_3 : noncopyable
{
    /// <summary>
    /// Constructs a guard and takes ownership of three synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    /// <param name='so3'>3-d synchronizable object to guard.</param>
    guard_3(const sync_object& so1,
            const sync_object& so2,
            const sync_object& so3)
        : so1(so1)
        , so2(so2)
        , so3(so3)
        , locked1(false)
        , locked2(false)
        , locked3(false)
    {
        so1.seize();
        locked1 = true;

        so2.seize();
        locked2 = true;

        so3.seize();
        locked3 = true;
    }

    /// <summary>
    /// Constructs a guard and try to take ownership of three synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    /// <param name='so3'>3-d synchronizable object to guard.</param>
    /// <param name='tryseize'>true if need to try to take ownership
    /// of synchronizable objects; otherwise, false.</param>
    guard_3(const sync_object& so1,
            const sync_object& so2,
            const sync_object& so3,
            bool tryseize)
        : so1(so1)
        , so2(so2)
        , so3(so3)
        , locked1(false)
        , locked2(false)
        , locked3(false)
    {
        if(tryseize)
        {
            locked1 = so1.try_seize();
            locked2 = so2.try_seize();
            locked3 = so3.try_seize();
        }
        else
        {
            so1.seize();
            locked1 = true;

            so2.seize();
            locked2 = true;

            so3.seize();
            locked3 = true;
        }
    }

    /// <summary>
    /// Checks if guard is locked.
    /// </summary>
    /// <returns>true if guard is locked.</returns>
    operator bool () const
    {
        return locked1
               || locked2
               || locked3;
    }

    /// <summary>
    /// Handles guard destruction. Releases synchronizable objects if ownership is taken.
    /// </summary>
    virtual ~guard_3()
    {
        if(locked1)
            so1.release();
        if(locked2)
            so2.release();
        if(locked3)
            so3.release();
    }

private:

    const sync_object& so1;
    const sync_object& so2;
    const sync_object& so3;
    bool locked1;
    bool locked2;
    bool locked3;
};

//------------------------------------------------------------------------------

/// <summary>
/// Guards four synchronizable objects. Releases synchronizable objects on destruction.
/// </summary>
/// <remarks>
/// To use this class include <i>wcp/sync.h</i>.
/// </remarks>
struct guard_4 : noncopyable
{
    /// <summary>
    /// Constructs a guard and takes ownership of four synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    /// <param name='so3'>3-d synchronizable object to guard.</param>
    /// <param name='so4'>4-th synchronizable object to guard.</param>
    guard_4(const sync_object& so1,
            const sync_object& so2,
            const sync_object& so3,
            const sync_object& so4)
        : so1(so1)
        , so2(so2)
        , so3(so3)
        , so4(so4)
        , locked1(false)
        , locked2(false)
        , locked3(false)
        , locked4(false)
    {
        so1.seize();
        locked1 = true;

        so2.seize();
        locked2 = true;

        so3.seize();
        locked3 = true;

        so4.seize();
        locked4 = true;
    }

    /// <summary>
    /// Constructs a guard and try to take ownership of four synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    /// <param name='so3'>3-d synchronizable object to guard.</param>
    /// <param name='so4'>4-th synchronizable object to guard.</param>
    /// <param name='tryseize'>true if need to try to take ownership
    /// of synchronizable objects; otherwise, false.</param>
    guard_4(const sync_object& so1,
            const sync_object& so2,
            const sync_object& so3,
            const sync_object& so4,
            bool tryseize)
        : so1(so1)
        , so2(so2)
        , so3(so3)
        , so4(so4)
        , locked1(false)
        , locked2(false)
        , locked3(false)
        , locked4(false)
    {
        if(tryseize)
        {
            locked1 = so1.try_seize();
            locked2 = so2.try_seize();
            locked3 = so3.try_seize();
            locked4 = so4.try_seize();
        }
        else
        {
            so1.seize();
            locked1 = true;

            so2.seize();
            locked2 = true;

            so3.seize();
            locked3 = true;

            so4.seize();
            locked4 = true;
        }
    }

    /// <summary>
    /// Checks if guard is locked.
    /// </summary>
    /// <returns>true if guard is locked.</returns>
    operator bool () const
    {
        return locked1
               || locked2
               || locked3
               || locked4;
    }

    /// <summary>
    /// Handles guard destruction. Releases synchronizable objects if ownership is taken.
    /// </summary>
    virtual ~guard_4()
    {
        if(locked1)
            so1.release();
        if(locked2)
            so2.release();
        if(locked3)
            so3.release();
        if(locked4)
            so4.release();
    }

private:

    const sync_object& so1;
    const sync_object& so2;
    const sync_object& so3;
    const sync_object& so4;
    bool locked1;
    bool locked2;
    bool locked3;
    bool locked4;
};

//------------------------------------------------------------------------------

/// <summary>
/// Guards five synchronizable objects. Releases synchronizable objects on destruction.
/// </summary>
/// <remarks>
/// To use this class include <i>wcp/sync.h</i>.
/// </remarks>
struct guard_5 : noncopyable
{
    /// <summary>
    /// Constructs a guard and takes ownership of five synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    /// <param name='so3'>3-d synchronizable object to guard.</param>
    /// <param name='so4'>4-th synchronizable object to guard.</param>
    /// <param name='so5'>5-th synchronizable object to guard.</param>
    guard_5(const sync_object& so1,
            const sync_object& so2,
            const sync_object& so3,
            const sync_object& so4,
            const sync_object& so5)
        : so1(so1)
        , so2(so2)
        , so3(so3)
        , so4(so4)
        , so5(so5)
        , locked1(false)
        , locked2(false)
        , locked3(false)
        , locked4(false)
        , locked5(false)
    {
        so1.seize();
        locked1 = true;

        so2.seize();
        locked2 = true;

        so3.seize();
        locked3 = true;

        so4.seize();
        locked4 = true;

        so5.seize();
        locked5 = true;
    }

    /// <summary>
    /// Constructs a guard and try to take ownership of five synchronizable objects.
    /// </summary>
    /// <param name='so1'>1-st synchronizable object to guard.</param>
    /// <param name='so2'>2-nd synchronizable object to guard.</param>
    /// <param name='so3'>3-d synchronizable object to guard.</param>
    /// <param name='so4'>4-th synchronizable object to guard.</param>
    /// <param name='so5'>5-th synchronizable object to guard.</param>
    /// <param name='tryseize'>true if need to try to take ownership
    /// of synchronizable objects; otherwise, false.</param>
    guard_5(const sync_object& so1,
            const sync_object& so2,
            const sync_object& so3,
            const sync_object& so4,
            const sync_object& so5,
            bool tryseize)
        : so1(so1)
        , so2(so2)
        , so3(so3)
        , so4(so4)
        , so5(so5)
        , locked1(false)
        , locked2(false)
        , locked3(false)
        , locked4(false)
        , locked5(false)
    {
        if(tryseize)
        {
            locked1 = so1.try_seize();
            locked2 = so2.try_seize();
            locked3 = so3.try_seize();
            locked4 = so4.try_seize();
            locked5 = so5.try_seize();
        }
        else
        {
            so1.seize();
            locked1 = true;

            so2.seize();
            locked2 = true;

            so3.seize();
            locked3 = true;

            so4.seize();
            locked4 = true;

            so5.seize();
            locked5 = true;
        }
    }

    /// <summary>
    /// Checks if guard is locked.
    /// </summary>
    /// <returns>true if guard is locked.</returns>
    operator bool () const
    {
        return locked1
               || locked2
               || locked3
               || locked4
               || locked5;
    }

    /// <summary>
    /// Handles guard destruction. Releases synchronizable objects if ownership is taken.
    /// </summary>
    virtual ~guard_5()
    {
        if(locked1)
            so1.release();
        if(locked2)
            so2.release();
        if(locked3)
            so3.release();
        if(locked4)
            so4.release();
        if(locked5)
            so5.release();
    }

private:

    const sync_object& so1;
    const sync_object& so2;
    const sync_object& so3;
    const sync_object& so4;
    const sync_object& so5;
    bool locked1;
    bool locked2;
    bool locked3;
    bool locked4;
    bool locked5;
};

//-----------------------------------------------------------------------------

#ifndef DOXYGEN

namespace internals
{
template<class SO_GUARD>
class guard_adapter
    : public statement_block_expression
{
    std::auto_ptr<SO_GUARD> pguard;

public:

    guard_adapter(SO_GUARD* pguard)
        : pguard(pguard) { }
};

} // namespace internals

#endif /*DOXYGEN*/

} // namespace WCP_NS

/// <summary>
/// Guards a synchronization object. Provides a convenient
/// way of guarding synchronization objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      wcp::critical_section cs;
///      GUARD(cs)
///      {
///          ...
///      }
/// </pre></code>
/// identical to
/// <code><pre class='_code'>
///      bool locked = false;
///      try
///      {
///          cs.seize();
///          locked = true;
///          ...
///          cs.release();
///      }
///      catch(...)
///      {
///          if(locked) cs.release();
///      }
/// </pre></code>
/// and identical to
/// <code><pre class='_code'>
///      wcp::critical_section cs;
///      {
///          wcp::guard guard(cs);
///          ...
///      }
/// </pre></code>
/// </remarks>
#define GUARD    \
    GUARD1

/// <summary>
/// Guards a synchronization object.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj;
///      GUARD(obj)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// </summary>
#define GUARD1(what1)                                               \
    _WCP_DECLARE_STATEMENT_BLOCK(WCP_NS::internals::guard_adapter<  \
        WCP_NS::guard_1>(new WCP_NS::guard_1(what1)))


/// <summary>
/// Guards two synchronization objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///     object obj1, obj2;
///     GUARD2(obj1, obj2)
///     {
///         // Critical code goes here.
///     }
/// </pre></code>
/// </remarks>
#define GUARD2(what1, what2)                                        \
    _WCP_DECLARE_STATEMENT_BLOCK(WCP_NS::internals::guard_adapter<  \
        WCP_NS::guard_2>(new WCP_NS::guard_2(what1, what2)))


/// <summary>
/// Guards three synchronization objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj1, obj2, obj3;
///      GUARD3(obj1, obj2, obj3)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// </remarks>
#define GUARD3(what1, what2, what3)                                 \
    _WCP_DECLARE_STATEMENT_BLOCK(WCP_NS::internals::guard_adapter<  \
        WCP_NS::guard_3>(new WCP_NS::guard_3(what1, what2, what3)))

/// <summary>
/// Guards four synchronization objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj1, obj2, obj3, obj4;
///      GUARD4(obj1, obj2, obj3, obj4)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// </remarks>
#define GUARD4(what1, what2, what3, what4)                          \
    _WCP_DECLARE_STATEMENT_BLOCK(WCP_NS::internals::guard_adapter<  \
        WCP_NS::guard_4>(new WCP_NS::guard_4(what1, what2, what3, what4)))

/// <summary>
/// Guards five synchronization objects.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this macro.<br><br>
/// <b>Example:</b>
/// <code><pre class='_code'>
///      object obj1, obj2, obj3, obj4, obj5;
///      GUARD5(obj1, obj2, obj3, obj4, obj5)
///      {
///          // Critical code goes here.
///      }
/// </pre></code>
/// </remarks>
#define GUARD5(what1, what2, what3, what4, what5)                   \
    _WCP_DECLARE_STATEMENT_BLOCK(WCP_NS::internals::guard_adapter<  \
        WCP_NS::guard_5>(new WCP_NS::guard_5(what1, what2, what3, what4, what5)))


#undef ___GUARDS_H_CYCLE__
#endif /*___GUARDS_H__*/
