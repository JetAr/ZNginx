#ifdef ___REFERENCE_H_CYCLE__
#error Cyclic dependency discovered: reference.h
#endif

#ifndef __REFERENCE_H__


#ifndef DOXYGEN

#define __REFERENCE_H__
#define __REFERENCE_H_CYCLE__

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
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains a reference adapter implementation.
*/


#include "_wcpdef.h"



namespace WCP_NS
{
/// <summary>
/// Implements a %reference adaptor.
/// </summary>
/// <typeparam name='T'>Reference type.</typeparam>
/// <remarks>
/// Normally, you cannot pass an agrument of type %reference to %reference.
/// Class wcp::reference&lt;T&gt; wraps a %reference and pass its %reference
/// to another function. Look at the wcp::basic_task&lt;T&gt; template description for an example.
/// </remarks>
template<class T>
class reference
{
    T& ref;

public:

    /// <summary>
    /// Constructs from a %reference.
    /// </summary>
    /// <param name='ref'>%reference to an object to construct from.</param>
    reference(T& ref) : ref(ref)
    { }

    /// <summary>
    /// Copies a wcp::reference&lt;T&gt; object.
    /// </summary>
    /// <param name='rhs'>An object to construct from.</param>
    reference(const reference& rhs) : ref(rhs.ref)
    { }

    /// <summary>
    /// Obtains an owned %reference.
    /// </summary>
    /// <returns>Owned %reference.</returns>
    operator T& ()
    {
        return ref;
    }

    /// <summary>
    /// Obtains an owned %reference.
    /// </summary>
    /// <returns>Owned %reference.</returns>
    operator const T& () const
    {
        return ref;
    }

    /// <summary>
    /// Obtains an owned %reference.
    /// </summary>
    /// <returns>Owned %reference.</returns>
    T& deref()
    {
        return ref;
    }

    /// <summary>
    /// Obtains an owned %reference.
    /// </summary>
    /// <returns>Owned %reference.</returns>
    const T& deref() const
    {
        return ref;
    }
};

} // namespace WCP_NS


#undef __REFERENCE_H_CYCLE__
#endif /*__REFERENCE_H__*/
