#ifdef ____DELEGATE_H_CYCLE__
#error Cyclic dependency discovered: _delegate.h
#endif

#ifndef ___DELEGATE_H__

#ifndef DOXYGEN

#define ___DELEGATE_H__
#define ___DELEGATE_H_CYCLE__

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
///      12/19/2009 - Initial release.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains unicast delagate implementation.

    To use this file include wcp/functional.h.
*/

#include "errh.h"
#include "_routine.h"

namespace WCP_NS
{
/// <summary>
/// Delegates a call to either routine or class method that accepts 7 parameters.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <typeparam name='P5'>5-th parameter type.</typeparam>
/// <typeparam name='P6'>6-th parameter type.</typeparam>
/// <typeparam name='P7'>7-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R,
         class P1 = void,
         class P2 = void,
         class P3 = void,
         class P4 = void,
         class P5 = void,
         class P6 = void,
         class P7 = void>
struct unicast_delegate
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R, P1, P2, P3, P4, P5, P6, P7> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <typeparam name='P1'>1-st parameter.</typeparam>
    /// <typeparam name='P2'>2-nd parameter.</typeparam>
    /// <typeparam name='P3'>3-d parameter.</typeparam>
    /// <typeparam name='P4'>4-th parameter.</typeparam>
    /// <typeparam name='P5'>5-th parameter.</typeparam>
    /// <typeparam name='P6'>6-th parameter.</typeparam>
    /// <typeparam name='P7'>7-th parameter.</typeparam>
    /// <returns>Calling routine return value.</returns>
    R operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)(p1, p2, p3, p4, p5, p6, p7);
        return routine(p1, p2, p3, p4, p5, p6, p7);
    }

private:

    template<class C>
    R do_method_call(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)(p1, p2, p3, p4, p5, p6, p7);
    }

    template<class C>
    R do_const_method_call(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)(p1, p2, p3, p4, p5, p6, p7);
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)(P1, P2, P3, P4, P5, P6, P7) const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};


/// <summary>
/// Delegates a call to either routine or class method that accepts 6 parameters.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <typeparam name='P5'>5-th parameter type.</typeparam>
/// <typeparam name='P6'>6-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R,
         class P1,
         class P2,
         class P3,
         class P4,
         class P5,
         class P6>
struct unicast_delegate<R, P1, P2, P3, P4, P5, P6, void>
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R, P1, P2, P3, P4, P5, P6> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <typeparam name='P1'>1-st parameter.</typeparam>
    /// <typeparam name='P2'>2-nd parameter.</typeparam>
    /// <typeparam name='P3'>3-d parameter.</typeparam>
    /// <typeparam name='P4'>4-th parameter.</typeparam>
    /// <typeparam name='P5'>5-th parameter.</typeparam>
    /// <typeparam name='P6'>6-th parameter.</typeparam>
    /// <returns>Calling routine return value.</returns>
    R operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)(p1, p2, p3, p4, p5, p6);
        return routine(p1, p2, p3, p4, p5, p6);
    }

private:

    template<class C>
    R do_method_call(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)(p1, p2, p3, p4, p5, p6);
    }

    template<class C>
    R do_const_method_call(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)(p1, p2, p3, p4, p5, p6);
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)(P1, P2, P3, P4, P5, P6) const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};


/// <summary>
/// Delegates a call to either routine or class method that accepts 5 parameters.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <typeparam name='P5'>5-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R,
         class P1,
         class P2,
         class P3,
         class P4,
         class P5>
struct unicast_delegate<R, P1, P2, P3, P4, P5, void, void>
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R, P1, P2, P3, P4, P5> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <typeparam name='P1'>1-st parameter.</typeparam>
    /// <typeparam name='P2'>2-nd parameter.</typeparam>
    /// <typeparam name='P3'>3-d parameter.</typeparam>
    /// <typeparam name='P4'>4-th parameter.</typeparam>
    /// <typeparam name='P5'>5-th parameter.</typeparam>
    /// <returns>Calling routine return value.</returns>
    R operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)(p1, p2, p3, p4, p5);
        return routine(p1, p2, p3, p4, p5);
    }

private:

    template<class C>
    R do_method_call(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)(p1, p2, p3, p4, p5);
    }

    template<class C>
    R do_const_method_call(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)(p1, p2, p3, p4, p5);
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)(P1, P2, P3, P4, P5) const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};


/// <summary>
/// Delegates a call to either routine or class method that accepts 4 parameters.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R,
         class P1,
         class P2,
         class P3,
         class P4>
struct unicast_delegate<R, P1, P2, P3, P4, void, void, void>
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R, P1, P2, P3, P4> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <typeparam name='P1'>1-st parameter.</typeparam>
    /// <typeparam name='P2'>2-nd parameter.</typeparam>
    /// <typeparam name='P3'>3-d parameter.</typeparam>
    /// <typeparam name='P4'>4-th parameter.</typeparam>
    /// <returns>Calling routine return value.</returns>
    R operator () (P1 p1, P2 p2, P3 p3, P4 p4) const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)(p1, p2, p3, p4);
        return routine(p1, p2, p3, p4);
    }

private:

    template<class C>
    R do_method_call(P1 p1, P2 p2, P3 p3, P4 p4) const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)(p1, p2, p3, p4);
    }

    template<class C>
    R do_const_method_call(P1 p1, P2 p2, P3 p3, P4 p4) const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)(p1, p2, p3, p4);
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)(P1, P2, P3, P4) const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};


/// <summary>
/// Delegates a call to either routine or class method that accepts 3 parameters.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R,
         class P1,
         class P2,
         class P3>
struct unicast_delegate<R, P1, P2, P3, void, void, void, void>
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R, P1, P2, P3> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <typeparam name='P1'>1-st parameter.</typeparam>
    /// <typeparam name='P2'>2-nd parameter.</typeparam>
    /// <typeparam name='P3'>3-d parameter.</typeparam>
    /// <returns>Calling routine return value.</returns>
    R operator () (P1 p1, P2 p2, P3 p3) const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)(p1, p2, p3);
        return routine(p1, p2, p3);
    }

private:

    template<class C>
    R do_method_call(P1 p1, P2 p2, P3 p3) const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)(p1, p2, p3);
    }

    template<class C>
    R do_const_method_call(P1 p1, P2 p2, P3 p3) const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)(p1, p2, p3);
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)(P1, P2, P3) const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};


/// <summary>
/// Delegates a call to either routine or class method that accepts 2 parameters.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R,
         class P1,
         class P2>
struct unicast_delegate<R, P1, P2, void, void, void, void, void>
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R, P1, P2> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <typeparam name='P1'>1-st parameter.</typeparam>
    /// <typeparam name='P2'>2-nd parameter.</typeparam>
    /// <returns>Calling routine return value.</returns>
    R operator () (P1 p1, P2 p2) const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)(p1, p2);
        return routine(p1, p2);
    }

private:

    template<class C>
    R do_method_call(P1 p1, P2 p2) const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)(p1, p2);
    }

    template<class C>
    R do_const_method_call(P1 p1, P2 p2) const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)(p1, p2);
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)(P1, P2) const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};


/// <summary>
/// Delegates a call to either routine or class method that accepts 1 parameter.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R,
         class P1>
struct unicast_delegate<R, P1, void, void, void, void, void, void>
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R, P1> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <typeparam name='P1'>1-st parameter.</typeparam>
    /// <returns>Calling routine return value.</returns>
    R operator () (P1 p1) const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)(p1);
        return routine(p1);
    }

private:

    template<class C>
    R do_method_call(P1 p1) const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)(p1);
    }

    template<class C>
    R do_const_method_call(P1 p1) const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)(p1);
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)(P1) const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};


/// <summary>
/// Delegates a call to either routine or class method that accepts 0 parameters.
/// </summary>
/// <typeparam name='R'>Calling routine return value.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<class R>
struct unicast_delegate<R, void, void, void, void, void, void, void>
{
    /// <summary>
    /// Calling routine type.
    /// </summary>
    typedef routine<R> routine_t;

    /// <summary>
    /// Construct an empty unicast delegate.
    /// </summary>
    unicast_delegate()
        : routine(NULL)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL) { }

    /// <summary>
    /// Copies a unicast delegate.
    /// </summary>
    /// <param name='rhs'>A delegate instance to copy from.</param>
    unicast_delegate(const unicast_delegate& rhs)
        : hash(rhs.hash)
        , routine(rhs.routine)
        , method(rhs.method)
        , const_method(rhs.const_method)
        , instance(rhs.instance)
        , method_caller(rhs.method_caller) { }

    /// <summary>
    /// Constructs a unicast delegate from a routine pointer.
    /// </summary>
    /// <param name='routine'>Routine pointer.</param>
    explicit unicast_delegate(typename routine_t::ptr routine)
        : hash(routine)
        , routine(routine)
        , method(NULL)
        , const_method(NULL)
        , instance(NULL)
        , method_caller(NULL)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a unicast delegate from a class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a method for.</typeparam>
    /// <param name='obj'>Object reference to call a method for.</param>
    /// <param name='method'>Class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::ptr method)
        : hash(obj, method)
        , routine(NULL)
        , method(*(void**)&method)
        , const_method(NULL)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && method);
    }

    /// <summary>
    /// Constructs a unicast delegate from a constant class method pointer.
    /// </summary>
    /// <typeparam name='C'>Class type to call a constant method for.</typeparam>
    /// <param name='obj'>Object reference to call a constant method for.</param>
    /// <param name='const_method'>Constant class method pointer.</param>
    template<class C>
    explicit unicast_delegate(const C& obj, typename routine_t::template method<C>::const_ptr const_method)
        : hash(obj, const_method)
        , routine(NULL)
        , method(NULL)
        , const_method(*(void**)&const_method)
        , instance(&obj)
        , method_caller(&unicast_delegate::do_const_method_call<C>)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && const_method);
    }

    /// <summary>
    /// Determines if a delegate object is valid.
    /// </summary>
    /// <returns>true is a delegate object is valid.</returns>
    operator bool () const
    {
        return *this != empty_delegate();
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are equal.</returns>
    bool operator == (const unicast_delegate& rhs) const
    {
        return hash == rhs.hash;
    }

    /// <summary>
    /// Compares two delegate objects.
    /// </summary>
    /// <param name='rhs'>A delegate object to compare to.</param>
    /// <returns>true if both delegates are not equal.</returns>
    bool operator != (const unicast_delegate& rhs) const
    {
        return hash != rhs.hash;
    }

    /// <summary>
    /// Returns delegate's routine hash code.
    /// </summary>
    /// <returns>Delegate's routine hash code.</returns>
    const routine_hash_code& hash_code() const
    {
        return hash;
    }

    /// <summary>
    /// Assigns a delegate.
    /// </summary>
    /// <param name='rhs'>Delegate to assign.</param>
    /// <returns>Reference to itself.</returns>
    unicast_delegate& operator = (const unicast_delegate& rhs)
    {
        if(&rhs != this)
        {
            hash = rhs.hash;
            routine = rhs.routine;
            method_caller = rhs.method_caller;
            method = rhs.method;
            const_method = rhs.const_method;
            instance = rhs.instance;
        }

        return *this;
    }

    /// <summary>
    /// Returns an empty delegate object.
    /// </summary>
    /// <returns>Empty delegate object.</returns>
    static const unicast_delegate& empty_delegate()
    {
        static unicast_delegate d;
        return d;
    }

    /// <summary>
    /// Unicast delegate call operator.
    /// </summary>
    /// <returns>Calling routine return value.</returns>
    R operator () () const
    {
        if(!*this)
        {
            _ASSERTE("An attempt to call an empty delegate." && 0);
            throw invalid_object_state_exception(TEXT("unicast_delegate"));
        }

        if(method_caller)
            return (this->*method_caller)();
        return routine();
    }

private:

    template<class C>
    R do_method_call() const
    {
        typename routine_t::template method<C>::ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::ptr*>(&method);
        C& obj = const_cast<C&>(*(const C*)instance);
        return (obj.*m)();
    }

    template<class C>
    R do_const_method_call() const
    {
        typename routine_t::template method<C>::const_ptr m = *reinterpret_cast<
                typename routine_t::template method<C>::const_ptr*>(&const_method);
        const C& obj = *(const C*)instance;
        return (obj.*m)();
    }

    mutable typename routine_t::ptr routine;
    mutable R(unicast_delegate::* method_caller)() const;
    mutable const void* method;
    mutable const void* const_method;
    mutable const void* instance;
    routine_hash_code hash;
};

} // namespace WCP_NS

#undef ___DELEGATE_H_CYCLE__
#endif /*___DELEGATE_H__*/
