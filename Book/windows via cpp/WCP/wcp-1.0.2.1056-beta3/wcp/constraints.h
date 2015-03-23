#ifdef ___CONSTRAINTS_H_CYCLE__
#error Cyclic dependency discovered: constraints.h
#endif

#ifndef __CONSTRAINTS_H__


#ifndef DOXYGEN

#define __CONSTRAINTS_H__
#define __CONSTRAINTS_H_CYCLE__

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
///      11/18/2009 - Initial release.
///      11/24/2009 - is_integral type trait added. 'and', 'not', 'or' constraints
///                   renamed respectively to 'and_', 'not_', 'or_'. Up to 20
///                   conditions added to 'or_' and 'and_' constraints.
///      01/02/2010 - Constraint usage syntax is changed. WCP_ENSURE_TYPE_CONSTRAINT
///                   and IS_MAINTAINED macros are added.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains basic type constraints implementation.
*/


#include "traits.h"


#ifndef DOXYGEN

/// <summary>
/// Defines a boolean expressiona as a type constraint.
/// </summary>
#define WCP_DEFINE_TYPE_CONSTRAINT static const bool $TypeConstraintsAreMaintained =

/// <summary>
/// Starts a type constraint expression.
/// </summary>
/// <remarks>
/// Type constraint verification can be inserted in almost any scope, be it
/// a function, a class definition, a try/catch block, etc.
/// </remarks>
#define WCP_ENSURE_TYPE_CONSTRAINT WCP_DEFINE_TYPE_CONSTRAINT

/// <summary>
/// Checks if a type constraint is maintained.
/// </summary>
#define IS_MAINTAINED       ::$TypeConstraintsAreMaintained == true

/// <summary>
/// Checks if a type constraint is not maintained.
/// </summary>
#define IS_NOT_MAINTAINED       ::$TypeConstraintsAreMaintained == false

#else /*DOXYGEN*/

/// <summary>
/// Defines a boolean expressiona as a type constraint.
/// </summary>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'>
///     struct is_dword_aligned
///     {
///         template&lt;class T&gt;
///         struct check_constraint
///         {
///             WCP_DEFINE_TYPE_CONSTRAINT(
///                 sizeof(T) > 0 && (sizeof(T) % sizeof(wcp::dword_t)) == 0);
///         };
///     };
/// &nbsp;
///     ...
/// &nbsp;
///     template&lt;class T&gt;
///     class Foo
///     {
///         WCP_ENSURE_TYPE_CONSTRAINT(
///             (wcp::constraint&lt;T, is_dword_aligned&gt; IS_MAINTAINED)
///             &amp;&amp; (wcp::constraint&lt;T, wcp::is_convertible_to_&lt;std::string&gt; &gt; IN_NOT_MAINTAINED)
///             &amp;&amp; (wcp::constraint&lt;T, wcp::is_convertible_to_&lt;std::wstring&gt; &gt; IN_NOT_MAINTAINED)
///             );
///     };
/// &nbsp;
///     ...
/// &nbsp;
///     Foo&lt;wcp::dword_t&gt; v1; // Ok
///     Foo&lt;wcp::short_t&gt; v2; // Compile-time error
///     Foo&lt;std::string&gt; v3; // Compile-time error
///     Foo&lt;std::wstring&gt; v4; // Compile-time error
/// </pre></code>
/// </remarks>
#define WCP_DEFINE_TYPE_CONSTRAINT(boolean_type_constraint)

/// <summary>
/// Checks a boolean type constraint check expression.
/// </summary>
/// <remarks>
/// Type constraint verification can be inserted in almost any scope, be it
/// a function, a class definition, a try/catch block, etc.
/// &nbsp;
/// Look at WCP_DEFINE_TYPE_CONSTRAINT macro description for an example.
/// </remarks>
#define WCP_ENSURE_TYPE_CONSTRAINT(boolean_type_constraint)

/// <summary>
/// Checks if a type constraint is maintained.
/// </summary>
/// <remarks>
/// Look at WCP_DEFINE_TYPE_CONSTRAINT macro description for an example.
/// </remarks>
#define IS_MAINTAINED

/// <summary>
/// Checks if a type constraint is not maintained.
/// </summary>
/// <remarks>
/// Look at WCP_DEFINE_TYPE_CONSTRAINT macro description for an example.
/// </remarks>
#define IS_NOT_MAINTAINED

#endif /*DOXYGEN*/


namespace WCP_NS
{

#ifndef DOXYGEN

namespace internals
{
struct good_condition
{
    template<class T>
    struct check_constraint
    {
        WCP_DEFINE_TYPE_CONSTRAINT(true);
    };
};

struct bad_condition
{
    template<class T>
    struct check_constraint
    {
        WCP_DEFINE_TYPE_CONSTRAINT(false);
    };
};

template<bool ConstraintMaintained>
struct check_constraint : check_constraint<!!ConstraintMaintained>
{
    /*
        If you got an 'a class cannot be its own base class' error,
        that means that a type constraint has been violated.
        Check the error trace to find out where a type constraint has been violated.
    */
};

template<> struct check_constraint<true>
{
    WCP_DEFINE_TYPE_CONSTRAINT(true);
};

} // namespace internals;

#endif /*DOXYGEN*/

/// <summary>
/// Checks if a checked by wcp::constraint type has a set of specified traits.
/// </summary>
/// <typeparam name='Traits'>Type traits to check a type for.
/// Use wcp::type_trait enumeration fields to check traits.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<int Traits>
struct has_traits_ : noninstantiable
{
#ifndef DOXYGEN

    template<class T>
    struct check_constraint : noninstantiable
    {
        WCP_DEFINE_TYPE_CONSTRAINT((type_traits<T>::traits & Traits) == Traits);
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Checks if a checked by wcp::constraint type is a base class of a type specified.
/// </summary>
/// <typeparam name='Of'>Type to check a base for.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<class Of>
struct is_base_of_ : noninstantiable
{
#ifndef DOXYGEN

    template<class Class>
    struct check_constraint : noninstantiable
    {
        WCP_DEFINE_TYPE_CONSTRAINT(__is_base_of(Class, Of));
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Checks if a checked by wcp::constraint type is derived from a type specified.
/// </summary>
/// <typeparam name='Parent'>Type to check.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<class Parent>
struct is_derived_from_ : noninstantiable
{
#ifndef DOXYGEN

    template<class Class>
    struct check_constraint : noninstantiable
    {
        WCP_DEFINE_TYPE_CONSTRAINT(__is_base_of(Parent, Class));
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Checks if a checked by wcp::constraint type is convertible to a type specified.
/// </summary>
/// <typeparam name='To'>Type to check a conversion ability for.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<class To>
struct is_convertible_to_ : noninstantiable
{
#ifndef DOXYGEN

    template<class From>
    struct check_constraint : noninstantiable
    {
        WCP_DEFINE_TYPE_CONSTRAINT(__is_convertible_to(From, To));
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Checks if a checked by wcp::constraint type is the same as a type specified.
/// </summary>
/// <typeparam name='To'>Type to check.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<class To>
struct is_equal_to_ : noninstantiable
{
#ifndef DOXYGEN

    template<class T>
    struct check_constraint : noninstantiable
    {
        WCP_DEFINE_TYPE_CONSTRAINT(__is_base_of(T, To) && __is_base_of(To, T));
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Negates a constraint.
/// </summary>
/// <typeparam name='Constraint'>Constraint to negate.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<class Constraint>
struct not_ : noninstantiable
{
#ifndef DOXYGEN

    template<class T>
    struct check_constraint : noninstantiable
    {
        WCP_ENSURE_TYPE_CONSTRAINT(
            Constraint::template check_constraint<T> IS_NOT_MAINTAINED);
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Checks if at least one of constraints is not violated.
/// </summary>
/// <typeparam name='C1'>1-st constraint.</typeparam>
/// <typeparam name='C2'>2-nd constraint.</typeparam>
/// <typeparam name='C3'>3-d constraint. Optional.</typeparam>
/// <typeparam name='C4'>4-th constraint.</typeparam>
/// <typeparam name='C5'>5-th constraint. Optional.</typeparam>
/// <typeparam name='C6'>6-th constraint. Optional.</typeparam>
/// <typeparam name='C7'>7-th constraint. Optional.</typeparam>
/// <typeparam name='C8'>8-th constraint. Optional.</typeparam>
/// <typeparam name='C9'>9-th constraint. Optional.</typeparam>
/// <typeparam name='C10'>10-th constraint. Optional.</typeparam>
/// <typeparam name='C11'>11-th constraint. Optional.</typeparam>
/// <typeparam name='C12'>12-th constraint. Optional.</typeparam>
/// <typeparam name='C13'>13-th constraint. Optional.</typeparam>
/// <typeparam name='C14'>14-th constraint. Optional.</typeparam>
/// <typeparam name='C15'>15-th constraint. Optional.</typeparam>
/// <typeparam name='C16'>16-th constraint. Optional.</typeparam>
/// <typeparam name='C17'>17-th constraint. Optional.</typeparam>
/// <typeparam name='C18'>18-th constraint. Optional.</typeparam>
/// <typeparam name='C19'>19-th constraint. Optional.</typeparam>
/// <typeparam name='C20'>20-th constraint. Optional.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<class C1,
         class C2,
         class C3 = internals::bad_condition,
         class C4 = internals::bad_condition,
         class C5 = internals::bad_condition,
         class C6 = internals::bad_condition,
         class C7 = internals::bad_condition,
         class C8 = internals::bad_condition,
         class C9 = internals::bad_condition,
         class C10 = internals::bad_condition,
         class C11 = internals::bad_condition,
         class C12 = internals::bad_condition,
         class C13 = internals::bad_condition,
         class C14 = internals::bad_condition,
         class C15 = internals::bad_condition,
         class C16 = internals::bad_condition,
         class C17 = internals::bad_condition,
         class C18 = internals::bad_condition,
         class C19 = internals::bad_condition,
         class C20 = internals::bad_condition>
struct or_ : noninstantiable
{
#ifndef DOXYGEN

    template<class T>
    struct check_constraint : noninstantiable
    {
        WCP_ENSURE_TYPE_CONSTRAINT(
            C1::template check_constraint<T> IS_MAINTAINED
            || C2::template check_constraint<T> IS_MAINTAINED
            || C3::template check_constraint<T> IS_MAINTAINED
            || C4::template check_constraint<T> IS_MAINTAINED
            || C5::template check_constraint<T> IS_MAINTAINED
            || C6::template check_constraint<T> IS_MAINTAINED
            || C7::template check_constraint<T> IS_MAINTAINED
            || C8::template check_constraint<T> IS_MAINTAINED
            || C9::template check_constraint<T> IS_MAINTAINED
            || C10::template check_constraint<T> IS_MAINTAINED
            || C11::template check_constraint<T> IS_MAINTAINED
            || C12::template check_constraint<T> IS_MAINTAINED
            || C13::template check_constraint<T> IS_MAINTAINED
            || C14::template check_constraint<T> IS_MAINTAINED
            || C15::template check_constraint<T> IS_MAINTAINED
            || C16::template check_constraint<T> IS_MAINTAINED
            || C17::template check_constraint<T> IS_MAINTAINED
            || C18::template check_constraint<T> IS_MAINTAINED
            || C19::template check_constraint<T> IS_MAINTAINED
            || C20::template check_constraint<T> IS_MAINTAINED
                                             );
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Checks if both constraints are not violated.
/// </summary>
/// <typeparam name='C1'>1-st constraint.</typeparam>
/// <typeparam name='C2'>2-nd constraint.</typeparam>
/// <typeparam name='C3'>3-d constraint. Optional.</typeparam>
/// <typeparam name='C4'>4-th constraint.</typeparam>
/// <typeparam name='C5'>5-th constraint. Optional.</typeparam>
/// <typeparam name='C6'>6-th constraint. Optional.</typeparam>
/// <typeparam name='C7'>7-th constraint. Optional.</typeparam>
/// <typeparam name='C8'>8-th constraint. Optional.</typeparam>
/// <typeparam name='C9'>9-th constraint. Optional.</typeparam>
/// <typeparam name='C10'>10-th constraint. Optional.</typeparam>
/// <typeparam name='C11'>11-th constraint. Optional.</typeparam>
/// <typeparam name='C12'>12-th constraint. Optional.</typeparam>
/// <typeparam name='C13'>13-th constraint. Optional.</typeparam>
/// <typeparam name='C14'>14-th constraint. Optional.</typeparam>
/// <typeparam name='C15'>15-th constraint. Optional.</typeparam>
/// <typeparam name='C16'>16-th constraint. Optional.</typeparam>
/// <typeparam name='C17'>17-th constraint. Optional.</typeparam>
/// <typeparam name='C18'>18-th constraint. Optional.</typeparam>
/// <typeparam name='C19'>19-th constraint. Optional.</typeparam>
/// <typeparam name='C20'>20-th constraint. Optional.</typeparam>
/// <remarks>
/// Look for an example in the description of wcp::constraint class.
/// </remarks>
template<class C1,
         class C2,
         class C3 = internals::good_condition,
         class C4 = internals::good_condition,
         class C5 = internals::good_condition,
         class C6 = internals::good_condition,
         class C7 = internals::good_condition,
         class C8 = internals::good_condition,
         class C9 = internals::good_condition,
         class C10 = internals::good_condition,
         class C11 = internals::good_condition,
         class C12 = internals::good_condition,
         class C13 = internals::good_condition,
         class C14 = internals::good_condition,
         class C15 = internals::good_condition,
         class C16 = internals::good_condition,
         class C17 = internals::good_condition,
         class C18 = internals::good_condition,
         class C19 = internals::good_condition,
         class C20 = internals::good_condition>
struct and_ : noninstantiable
{
#ifndef DOXYGEN

    template<class T>
    struct check_constraint : noninstantiable
    {
        WCP_ENSURE_TYPE_CONSTRAINT(
            C1::template check_constraint<T> IS_MAINTAINED
            && C2::template check_constraint<T> IS_MAINTAINED
            && C3::template check_constraint<T> IS_MAINTAINED
            && C4::template check_constraint<T> IS_MAINTAINED
            && C5::template check_constraint<T> IS_MAINTAINED
            && C6::template check_constraint<T> IS_MAINTAINED
            && C7::template check_constraint<T> IS_MAINTAINED
            && C8::template check_constraint<T> IS_MAINTAINED
            && C9::template check_constraint<T> IS_MAINTAINED
            && C10::template check_constraint<T> IS_MAINTAINED
            && C11::template check_constraint<T> IS_MAINTAINED
            && C12::template check_constraint<T> IS_MAINTAINED
            && C13::template check_constraint<T> IS_MAINTAINED
            && C14::template check_constraint<T> IS_MAINTAINED
            && C15::template check_constraint<T> IS_MAINTAINED
            && C16::template check_constraint<T> IS_MAINTAINED
            && C17::template check_constraint<T> IS_MAINTAINED
            && C18::template check_constraint<T> IS_MAINTAINED
            && C19::template check_constraint<T> IS_MAINTAINED
            && C20::template check_constraint<T> IS_MAINTAINED
                                             );
    };

#endif /*DOXYGEN*/
};

/// <summary>
/// Checks a constraint for a type specified. If a constraint is violated,
/// the compilation will be unsucessful.
/// </summary>
/// <typeparam name='T'>Type to check a constraint for.</typeparam>
/// <typeparam name='Constraint'>A constraint to check a type for.</typeparam>
/// <remarks>
/// Checking for type constraints at a compile time is a very useful feature
/// that reduces a time for debugging possible errors and prevents an unwished code
/// to be compiled. You can check type constraints either for template classes
/// by deriving wcp::constraint class with one of available constraints, or
/// template functions by calling wcp::constraint default constructor.<br><br>
/// Checking for constraints does not affect perfomance nor compiled code size.<br><br>
/// Available type constraints:
/// <ul>
///     <li>wcp::has_traits_ - checks a type traits</li>
///     <li>wcp::is_base_of_&lt;T&gt; - checks if a checked type is base
///         for a type specified.</li>
///     <li>wcp::is_derived_from_&lt;T&gt; - checks if a checked type is
///         derived from a type specified.</li>
///     <li>wcp::is_convertible_to_&lt;T&gt; - &lt;T&gt; -checks if a checked
///         type is convertible to a type specified.</li>
///     <li>wcp::is_equal_to_&lt;T&gt; - &lt;T&gt; -checks if a checked
///         type is the same as a type specified.</li>
///     <li>wcp::not_&lt;Constraint&gt; - negates a %constraint.</li>
///     <li>wcp::or_&lt;Constraint1, Constraint2, ...&gt; - checks if at least
///         one of constraints is not violated.</li>
///     <li>wcp::and_&lt;Constraint1, Constraint2, ...&gt; - checks if both
///         constraints are not violated.</li>
/// </ul>
/// For an example of using type constraints refer to \ref Serialization.cpp example.
/// </remarks>
template<class T, class Constraint>
struct constraint : noninstantiable
{
    WCP_DEFINE_TYPE_CONSTRAINT(internals::check_constraint<
                               (Constraint::template check_constraint<T> IS_MAINTAINED)> IS_MAINTAINED);
};

} // namespace WCP_NS


#undef __CONSTRAINTS_H_CYCLE__
#endif /*__CONSTRAINTS_H__*/
