#ifdef ____ROUTINE_H_CYCLE__
#error Cyclic dependency discovered: _routine.h
#endif

#ifndef ___ROUTINE_H__


#ifndef DOXYGEN

#define ___ROUTINE_H__
#define ___ROUTINE_H_CYCLE__

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
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///      12/19/2009 - Functions to resolve routines that accept 6 and 7 parameters
///                   are added.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains implementation of function and class method type resolvers.

    To use this file include wcp/functional.h.
*/


#include "_wcpdef.h"



namespace WCP_NS
{
/// <summary>
/// Defines types of routines that accept 7 parameters.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
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
template<
    class R,
    class P1 = void,
    class P2 = void,
    class P3 = void,
    class P4 = void,
    class P5 = void,
    class P6 = void,
    class P7 = void>
struct routine
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type(P1, P2, P3, P4, P5, P6, P7);

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type(P1, P2, P3, P4, P5, P6, P7) const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 7 parameters.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    /// <typeparam name='P1'>1-st parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>3-d parameter type.</typeparam>
    /// <typeparam name='P4'>4-th parameter type.</typeparam>
    /// <typeparam name='P5'>5-th parameter type.</typeparam>
    /// <typeparam name='P6'>6-th parameter type.</typeparam>
    /// <typeparam name='P7'>7-th parameter type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 7 parameters.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
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
template<
    class C,
    class R,
    class P1 = void,
    class P2 = void,
    class P3 = void,
    class P4 = void,
    class P5 = void,
    class P6 = void,
    class P7 = void>
struct method
    : public routine<R, P1, P2, P3, P4, P5, P6, P7>::template method<C>
{ };

//-----------------------------------------------------------------------------

/// <summary>
/// Defines types of routines that accept 6 parameters.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <typeparam name='P5'>5-th parameter type.</typeparam>
/// <typeparam name='P6'>6-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class R,
    class P1,
    class P2,
    class P3,
    class P4,
    class P5,
    class P6>
struct routine<R, P1, P2, P3, P4, P5, P6, void>
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type(P1, P2, P3, P4, P5, P6);

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type(P1, P2, P3, P4, P5, P6) const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 6 parameters.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    /// <typeparam name='P1'>1-st parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>3-d parameter type.</typeparam>
    /// <typeparam name='P4'>4-th parameter type.</typeparam>
    /// <typeparam name='P5'>5-th parameter type.</typeparam>
    /// <typeparam name='P6'>6-th parameter type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 6 parameters.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <typeparam name='P5'>5-th parameter type.</typeparam>
/// <typeparam name='P6'>6-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class C,
    class R,
    class P1,
    class P2,
    class P3,
    class P4,
    class P5,
    class P6>
struct method<C, R, P1, P2, P3, P4, P5, P6, void>
    : public routine<R, P1, P2, P3, P4, P5, P6>::template method<C>
{ };

//-----------------------------------------------------------------------------

/// <summary>
/// Defines types of routines that accept 5 parameters.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <typeparam name='P5'>5-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class R,
    class P1,
    class P2,
    class P3,
    class P4,
    class P5>
struct routine<R, P1, P2, P3, P4, P5, void, void>
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type(P1, P2, P3, P4, P5);

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type(P1, P2, P3, P4, P5) const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 5 parameters.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    /// <typeparam name='P1'>1-st parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>3-d parameter type.</typeparam>
    /// <typeparam name='P4'>4-th parameter type.</typeparam>
    /// <typeparam name='P5'>5-th parameter type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 5 parameters.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <typeparam name='P5'>5-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class C,
    class R,
    class P1,
    class P2,
    class P3,
    class P4,
    class P5>
struct method<C, R, P1, P2, P3, P4, P5, void, void>
    : public routine<R, P1, P2, P3, P4, P5>::template method<C>
{ };

//-----------------------------------------------------------------------------

/// <summary>
/// Defines types of routines that accept 4 parameters.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class R,
    class P1,
    class P2,
    class P3,
    class P4>
struct routine<R, P1, P2, P3, P4, void, void, void>
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type(P1, P2, P3, P4);

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type(P1, P2, P3, P4) const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 4 parameters.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    /// <typeparam name='P1'>1-st parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>3-d parameter type.</typeparam>
    /// <typeparam name='P4'>4-th parameter type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 4 parameters.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <typeparam name='P4'>4-th parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class C,
    class R,
    class P1,
    class P2,
    class P3,
    class P4>
struct method<C, R, P1, P2, P3, P4, void, void, void>
    : public routine<R, P1, P2, P3, P4>::template method<C>
{ };

//-----------------------------------------------------------------------------

/// <summary>
/// Defines types of routines that accept 3 parameters.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class R,
    class P1,
    class P2,
    class P3>
struct routine<R, P1, P2, P3, void, void, void, void>
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type(P1, P2, P3);

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type(P1, P2, P3) const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 3 parameters.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    /// <typeparam name='P1'>1-st parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>3-d parameter type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 3 parameters.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <typeparam name='P3'>3-d parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class C,
    class R,
    class P1,
    class P2,
    class P3>
struct method<C, R, P1, P2, P3, void, void, void, void>
    : public routine<R, P1, P2, P3>::template method<C>
{ };

//-----------------------------------------------------------------------------

/// <summary>
/// Defines types of routines that accept 2 parameters.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class R,
    class P1,
    class P2>
struct routine<R, P1, P2, void, void, void, void, void>
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type(P1, P2);

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type(P1, P2) const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 2 parameters.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    /// <typeparam name='P1'>1-st parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd parameter type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 2 parameters.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <typeparam name='P2'>2-nd parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class C,
    class R,
    class P1,
    class P2>
struct method<C, R, P1, P2, void, void, void, void, void>
    : public routine<R, P1, P2>::template method<C>
{ };

//-----------------------------------------------------------------------------

/// <summary>
/// Defines types of routines that accept 1 parameter.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class R,
    class P1>
struct routine<R, P1, void, void, void, void, void, void>
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type(P1);

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type(P1) const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 1 parameter.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    /// <typeparam name='P1'>1-st parameter type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 1 parameter.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
/// <typeparam name='P1'>1-st parameter type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class C,
    class R,
    class P1>
struct method<C, R, P1, void, void, void, void, void, void>
    : public routine<R, P1>::template method<C>
{ };

//-----------------------------------------------------------------------------

/// <summary>
/// Defines types of routines that accept 0 parameters.
/// </summary>
/// <typeparam name='R'>Method return type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class R>
struct routine<R, void, void, void, void, void, void, void>
{
    /// <summary>
    /// Routine type.
    /// </summary>
    typedef R type();

    /// <summary>
    /// Constant routine type.
    /// </summary>
    typedef R const_type() const;

    /// <summary>
    /// Routine pointer type.
    /// </summary>
    typedef type* ptr;

    /// <summary>
    /// Constant routine pointer type.
    /// </summary>
    typedef const_type* const_ptr;

    /// <summary>
    /// Defines types of class methods that accept 0 parameters.
    /// </summary>
    /// <typeparam name='C'>Class type.</typeparam>
    /// <typeparam name='R'>Method return type.</typeparam>
    template<class C>
    struct method
    {
        /// <summary>
        /// Method pointer type.
        /// </summary>
        typedef type C::* ptr;

        /// <summary>
        /// Constant method pointer type.
        /// </summary>
        typedef const_type C::* const_ptr;

        /// <summary>
        /// Returns an address of a method that corresponds to a template type.
        /// </summary>
        /// <param name='method'>Method to obtain an address for.</param>
        /// <returns>Method address.</returns>
        static ptr address_of(ptr method)
        {
            return method;
        }

        /// <summary>
        /// Returns an address of a constant method that corresponds to a template type.
        /// </summary>
        /// <param name='m'>Constant method to obtain an address for.</param>
        /// <returns>Constant method address.</returns>
        static const_ptr address_of_const(const_ptr m)
        {
            return m;
        }

    private:
        method(); // Class is not instantiable
    };

    /// <summary>
    /// Returns an address of a routine that corresponds to a template type.
    /// </summary>
    /// <param name='fn'>Routine to obtain an address for.</param>
    /// <returns>Routine address.</returns>
    static ptr address_of(ptr fn)
    {
        return fn;
    }

private:
    routine(); // Class is not instantiable
};


/// <summary>
/// Defines types of class methods that accept 0 parameters.
/// </summary>
/// <typeparam name='C'>Class type.</typeparam>
/// <typeparam name='R'>Method return type.</typeparam>
/// <remarks>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
template<
    class C,
    class R>
struct method<C, R, void, void, void, void, void, void, void>
    : public routine<R>::template method<C>
{ };











//*****************************************************************************
// routine_hash_code class implementation
//*****************************************************************************

/// <summary>
/// Routine hash code object. The class allows to perform routines comparison.
/// </summary>
/// <remarks>
/// The class was introduced in order to be able to compare routines.
/// Three class of routines can are accepted:
/// <ul>
/// <li>Static class routines or standalone routines (0-7 parameters)</li>
/// <li>Class methods (0-7 parameters)</li>
/// <li>Class constant methods (0-7 parameters)</li>
/// </ul>
/// Properties of routine hash codes:
/// <ul>
/// <li>Two hash codes of the same class static or standalone routine are equal.</li>
/// <li>Two hash codes of the same class method and the same object reference are equal.</li>
/// <li>Two hash codes of the same class method and different object references are not equal.</li>
/// <li>Collisions are impossible.</li>
/// </ul>
/// <br>
/// To use this class include <i>wcp/functional.h</i>.
/// </remarks>
struct routine_hash_code
{
    /// <summary>
    /// constructs an empty routine hash code object.
    /// </summary>
    routine_hash_code()
        : routine(NULL), instance(NULL) { }

    /// <summary>
    /// Copies a routine hash code object.
    /// </summary>
    /// <param name='rhs'>Object to make a copy from.</param>
    routine_hash_code(const routine_hash_code& rhs)
        : routine(rhs.routine), instance(rhs.instance) { }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R>
    explicit routine_hash_code(R(*routine)())
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <typeparam name='P1'>1-st routine parameter type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R, class P1>
    explicit routine_hash_code(R(*routine)(P1))
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <typeparam name='P1'>1-st routine parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd routine parameter type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R, class P1, class P2>
    explicit routine_hash_code(R(*routine)(P1, P2))
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <typeparam name='P1'>1-st routine parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd routine parameter type.</typeparam>
    /// <typeparam name='P3'>3-d routine parameter type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R, class P1, class P2, class P3>
    explicit routine_hash_code(R(*routine)(P1, P2, P3))
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <typeparam name='P1'>1-st routine parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd routine parameter type.</typeparam>
    /// <typeparam name='P3'>3-d routine parameter type.</typeparam>
    /// <typeparam name='P4'>4-th routine parameter type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R, class P1, class P2, class P3, class P4>
    explicit routine_hash_code(R(*routine)(P1, P2, P3, P4))
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <typeparam name='P1'>1-st routine parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd routine parameter type.</typeparam>
    /// <typeparam name='P3'>3-d routine parameter type.</typeparam>
    /// <typeparam name='P4'>4-th routine parameter type.</typeparam>
    /// <typeparam name='P5'>5-th routine parameter type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R, class P1, class P2, class P3, class P4, class P5>
    explicit routine_hash_code(R(*routine)(P1, P2, P3, P4, P5))
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <typeparam name='P1'>1-st routine parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd routine parameter type.</typeparam>
    /// <typeparam name='P3'>3-d routine parameter type.</typeparam>
    /// <typeparam name='P4'>4-th routine parameter type.</typeparam>
    /// <typeparam name='P5'>5-th routine parameter type.</typeparam>
    /// <typeparam name='P6'>6-th routine parameter type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R, class P1, class P2, class P3, class P4, class P5, class P6>
    explicit routine_hash_code(R(*routine)(P1, P2, P3, P4, P5, P6))
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a routine pointer.
    /// </summary>
    /// <typeparam name='R'>Routine return value type.</typeparam>
    /// <typeparam name='P1'>1-st routine parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd routine parameter type.</typeparam>
    /// <typeparam name='P3'>3-d routine parameter type.</typeparam>
    /// <typeparam name='P4'>4-th routine parameter type.</typeparam>
    /// <typeparam name='P5'>5-th routine parameter type.</typeparam>
    /// <typeparam name='P6'>6-th routine parameter type.</typeparam>
    /// <typeparam name='P7'>7-th routine parameter type.</typeparam>
    /// <param name='routine'>A routine pointer.</param>
    template<class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
    explicit routine_hash_code(R(*routine)(P1, P2, P3, P4, P5, P6, P7))
        : instance(NULL), routine(routine)
    {
        _ASSERTE("Invalid routine pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R>
    explicit routine_hash_code(const C& obj, R(C::*routine)())
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R, class P1>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1))
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R, class P1, class P2>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2))
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R, class P1, class P2, class P3>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3))
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4))
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class method parameter type.</typeparam>
    /// <typeparam name='P5'>5-th class method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4, class P5>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4, P5))
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class method parameter type.</typeparam>
    /// <typeparam name='P5'>5-th class method parameter type.</typeparam>
    /// <typeparam name='P6'>6-th class method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4, class P5, class P6>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4, P5, P6))
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class method pointer.
    /// </summary>
    /// <typeparam name='R'>Class method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class method parameter type.</typeparam>
    /// <typeparam name='P5'>5-th class method parameter type.</typeparam>
    /// <typeparam name='P6'>6-th class method parameter type.</typeparam>
    /// <typeparam name='P7'>7-th class method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4, P5, P6, P7))
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R>
    explicit routine_hash_code(const C& obj, R(C::*routine)() const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class constant method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R, class P1>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1) const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class constant method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class constant method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R, class P1, class P2>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2) const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class constant method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class constant method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class constant method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R, class P1, class P2, class P3>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3) const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class constant method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class constant method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class constant method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class constant method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4) const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class constant method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class constant method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class constant method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class constant method parameter type.</typeparam>
    /// <typeparam name='P5'>5-th class constant method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4, class P5>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4, P5) const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class constant method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class constant method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class constant method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class constant method parameter type.</typeparam>
    /// <typeparam name='P5'>5-th class constant method parameter type.</typeparam>
    /// <typeparam name='P6'>6-th class constant method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4, class P5, class P6>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4, P5, P6) const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Constructs a routine hash object from a class intance reference
    /// and a class constant method pointer.
    /// </summary>
    /// <typeparam name='R'>Class constant method return value type.</typeparam>
    /// <typeparam name='P1'>1-st class constant method parameter type.</typeparam>
    /// <typeparam name='P2'>2-nd class constant method parameter type.</typeparam>
    /// <typeparam name='P3'>3-d class constant method parameter type.</typeparam>
    /// <typeparam name='P4'>4-th class constant method parameter type.</typeparam>
    /// <typeparam name='P5'>5-th class constant method parameter type.</typeparam>
    /// <typeparam name='P6'>6-th class constant method parameter type.</typeparam>
    /// <typeparam name='P7'>7-th class constant method parameter type.</typeparam>
    /// <param name='obj'>A class object reference.</param>
    /// <param name='routine'>A class constant method pointer.</param>
    template<class C, class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
    explicit routine_hash_code(const C& obj, R(C::*routine)(P1, P2, P3, P4, P5, P6, P7) const)
        : instance(&obj), routine(*(void**)&routine)
    {
        _ASSERTE("Invalid object reference specified." && (&obj != NULL));
        _ASSERTE("Invalid method pointer specified." && routine);
    }

    /// <summary>
    /// Assigns a routine hash object value.
    /// </summary>
    /// <param name='rhs'>A routine hash object to assign.</param>
    /// <returns>Reference to itself.</returns>
    routine_hash_code& operator = (const routine_hash_code& rhs)
    {
        routine = rhs.routine;
        instance = rhs.instance;
        return *this;
    }

    /// <summary>
    /// Compares with a routine hash.
    /// </summary>
    /// <param name='rhs'>A routine hash to compare to.</param>
    /// <returns>true if a routine hash is less than a routine hash specified.</returns>
    bool operator < (const routine_hash_code& rhs) const
    {
        int r = memcmp(this, &rhs, sizeof(routine_hash_code));
        return r < 0;
    }

    /// <summary>
    /// Compares with a routine hash.
    /// </summary>
    /// <param name='rhs'>A routine hash to compare to.</param>
    /// <returns>true if a routine hash is greater than a routine hash specified.</returns>
    bool operator > (const routine_hash_code& rhs) const
    {
        int r = memcmp(this, &rhs, sizeof(routine_hash_code));
        return r > 0;
    }

    /// <summary>
    /// Compares with a routine hash.
    /// </summary>
    /// <param name='rhs'>A routine hash to compare to.</param>
    /// <returns>true if a routine hash is equal to a routine hash specified.</returns>
    bool operator == (const routine_hash_code& rhs) const
    {
        bool r = (routine == rhs.routine && instance == rhs.instance);
        return r;
    }

    /// <summary>
    /// Compares with a routine hash.
    /// </summary>
    /// <param name='rhs'>A routine hash to compare to.</param>
    /// <returns>true if a routine hash is not equal to a routine hash specified.</returns>
    bool operator != (const routine_hash_code& rhs) const
    {
        return !(*this == rhs);
    }

private:

    const void* routine;
    const void* instance;
};

} // namespace WCP_NS



#undef ___ROUTINE_H_CYCLE__
#endif /*___ROUTINE_H__*/
