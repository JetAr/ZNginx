#ifdef ___TRAITS_H_CYCLE__
#error Cyclic dependency discovered: traits.h
#endif

#ifndef __TRAITS_H__


#ifndef DOXYGEN

#define __TRAITS_H__
#define __TRAITS_H_CYCLE__

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
///      01/16/2010 - Initial release. Segregated from constraints.h.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Type traits definitions.
*/


#include "noninstantiable.h"



namespace WCP_NS 
{
    /// <summary>
    /// A set of possible type traits.
    /// </summary>
    enum type_trait
    {
        /// <summary>
        /// Indicates that a type is an integral type.
        /// </summary>
        is_integral                 = 1 << 1,

        /// <summary>
        /// Indicates an abstract type.
        /// </summary>
        is_abstract                 = 1 << 2,

        /// <summary>
        /// Indicates a class type.
        /// </summary>
        is_class                    = 1 << 3,

        /// <summary>
        /// Indicates a type that has no instance data members.
        /// </summary>
        is_empty                    = 1 << 4,

        /// <summary>
        /// Indicates a enumeration type.
        /// </summary>
        is_enum                     = 1 << 5,

        /// <summary>
        /// Indicates a POD type. A POD type is a class or union with no constructor 
        /// or private or protected non-static members, no base classes, and no virtual functions.
        /// </summary>
        is_pod                      = 1 << 6,

        /// <summary>
        /// Indicates a polymorhic type (a type has at least one virtual method).
        /// </summary>
        is_polymorphic              = 1 << 7,

        /// <summary>
        /// Indicates a union type.
        /// </summary>
        is_union                    = 1 << 8,

        /// <summary>
        /// Indicates that a type has a copy assignment operator.
        /// </summary>
        has_assign                  = 1 << 9,

        /// <summary>
        /// Indicates that a type has a copy constructor.
        /// </summary>
        has_copy                    = 1 << 10,

        /// <summary>
        /// Indicates that a type has a copy assignment operator 
        /// that doesn't throw any %exception.
        /// </summary>
        has_nothrow_assign          = 1 << 11,

        /// <summary>
        /// Indicates that a type has a constructor
        /// that doesn't throw any %exception.
        /// </summary>
        has_nothrow_constructor     = 1 << 12,

        /// <summary>
        /// Indicates that a type has a copy constructor
        /// that doesn't throw any %exception.
        /// </summary>
        has_nothrow_copy            = 1 << 13,
        
        /// <summary>
        /// Indicates that a type has a trivial 
        /// (compiler-generater) copy assignment operator.
        /// </summary>
        has_trivial_assign          = 1 << 14,

        /// <summary>
        /// Indicates that a type has a trivial 
        /// (compiler-generater) constructor.
        /// </summary>
        has_trivial_constructor     = 1 << 15,

        /// <summary>
        /// Indicates that a type has a trivial 
        /// (compiler-generater) copy constructor.
        /// </summary>
        has_trivial_copy            = 1 << 16,

        /// <summary>
        /// Indicates that a type has a trivial 
        /// (compiler-generater) destructor.
        /// </summary>
        has_trivial_destructor      = 1 << 17,
        
        /// <summary>
        /// Indicates that a type has a user-defined destructor.
        /// </summary>
        has_user_destructor         = 1 << 18,

        /// <summary>
        /// Indicates that a type has a virtual destructor.
        /// </summary>
        has_virtual_destructor      = 1 << 19,
    };

    /// <summary>
    /// Obtains type traits at copile time.
    /// </summary>
    /// <typeparam name='T'>Type to obtain traits for.</typeparam>
    template<class T>
    struct type_traits : noninstantiable
    {
        enum
        {
            /// <summary>
            /// Indicates an abstract type.
            /// </summary>
            is_abstract             = __is_abstract(T),

            /// <summary>
            /// Indicates a class type.
            /// </summary>
            is_class                = __is_class(T),

            /// <summary>
            /// Indicates a type that has no instance data members.
            /// </summary>
            is_empty                = __is_empty(T),

            /// <summary>
            /// Indicates a enumeration type.
            /// </summary>
            is_enum                 = __is_enum(T),

            /// <summary>
            /// Indicates a POD type. A POD type is a class or union with no constructor 
            /// or private or protected non-static members, no base classes, and no virtual functions.
            /// </summary>
            is_pod                  = __is_pod(T),

            /// <summary>
            /// Indicates a polymorhic type (a type has at least one virtual method).
            /// </summary>
            is_polymorphic          = __is_polymorphic(T),

            /// <summary>
            /// Indicates a union type.
            /// </summary>
            is_union                = __is_union(T),

            /// <summary>
            /// Indicates that a type has a copy assignment operator.
            /// </summary>
            has_assign              = __has_assign(T),

            /// <summary>
            /// Indicates that a type has a copy constructor.
            /// </summary>
            has_copy                = __has_copy(T),

            /// <summary>
            /// Indicates that a type has a copy assignment operator 
            /// that doesn't throw any %exception.
            /// </summary>
            has_nothrow_assign      = __has_nothrow_assign(T),

            /// <summary>
            /// Indicates that a type has a constructor
            /// that doesn't throw any %exception.
            /// </summary>
            has_nothrow_constructor = __has_nothrow_constructor(T),

            /// <summary>
            /// Indicates that a type has a copy constructor
            /// that doesn't throw any %exception.
            /// </summary>
            has_nothrow_copy        = __has_nothrow_copy(T),

            /// <summary>
            /// Indicates that a type has a trivial 
            /// (compiler-generater) copy assignment operator.
            /// </summary>
            has_trivial_assign      = __has_trivial_assign(T),

            /// <summary>
            /// Indicates that a type has a trivial 
            /// (compiler-generater) constructor.
            /// </summary>
            has_trivial_constructor = __has_trivial_constructor(T),

            /// <summary>
            /// Indicates that a type has a trivial 
            /// (compiler-generater) copy constructor.
            /// </summary>
            has_trivial_copy        = __has_trivial_copy(T),

            /// <summary>
            /// Indicates that a type has a trivial 
            /// (compiler-generater) destructor.
            /// </summary>
            has_trivial_destructor  = __has_trivial_destructor(T),

            /// <summary>
            /// Indicates that a type has a user-defined destructor.
            /// </summary>
            has_user_destructor     = __has_user_destructor(T),

            /// <summary>
            /// Indicates that a type has a virtual destructor.
            /// </summary>
            has_virtual_destructor  = __has_virtual_destructor(T),

            /// <summary>
            /// Indicates that a type is an integral type.
            /// </summary>
            is_integral                 = 0 == (
                WCP_NS::is_abstract * is_abstract
                | WCP_NS::is_class * is_class
                | WCP_NS::is_empty * is_empty
                | WCP_NS::is_enum * is_enum
                | WCP_NS::is_pod * is_pod
                | WCP_NS::is_polymorphic * is_polymorphic
                | WCP_NS::is_union * is_union
                | WCP_NS::has_assign * has_assign
                | WCP_NS::has_copy * has_copy
                | WCP_NS::has_nothrow_assign * has_nothrow_assign
                | WCP_NS::has_nothrow_constructor * has_nothrow_constructor
                | WCP_NS::has_nothrow_copy * has_nothrow_copy
                | WCP_NS::has_trivial_assign * has_trivial_assign
                | WCP_NS::has_trivial_constructor * has_trivial_constructor
                | WCP_NS::has_trivial_copy * has_trivial_copy
                | WCP_NS::has_trivial_destructor * has_trivial_destructor
                | WCP_NS::has_user_destructor * has_user_destructor
                | WCP_NS::has_virtual_destructor * has_virtual_destructor),

            /// <summary>
            /// All traits. Use wcp::type_trait enumeration fields to check traits.
            /// </summary>
            traits = WCP_NS::is_abstract * is_abstract
                | WCP_NS::is_class * is_class
                | WCP_NS::is_empty * is_empty
                | WCP_NS::is_enum * is_enum
                | WCP_NS::is_pod * is_pod
                | WCP_NS::is_polymorphic * is_polymorphic
                | WCP_NS::is_union * is_union
                | WCP_NS::has_assign * has_assign
                | WCP_NS::has_copy * has_copy
                | WCP_NS::has_nothrow_assign * has_nothrow_assign
                | WCP_NS::has_nothrow_constructor * has_nothrow_constructor
                | WCP_NS::has_nothrow_copy * has_nothrow_copy
                | WCP_NS::has_trivial_assign * has_trivial_assign
                | WCP_NS::has_trivial_constructor * has_trivial_constructor
                | WCP_NS::has_trivial_copy * has_trivial_copy
                | WCP_NS::has_trivial_destructor * has_trivial_destructor
                | WCP_NS::has_user_destructor * has_user_destructor
                | WCP_NS::has_virtual_destructor * has_virtual_destructor
                | WCP_NS::is_integral * is_integral
        };
    };

} // namespace WCP_NS



#undef __TRAITS_H_CYCLE__
#endif /*__TRAITS_H__*/
