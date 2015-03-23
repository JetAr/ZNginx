#ifdef ____SYNCOBJ_H_CYCLE__
#error Cyclic dependency discovered: _syncobj.h
#endif

#ifndef ___SYNCOBJ_H__


#ifndef DOXYGEN

#define ___SYNCOBJ_H__
#define ___SYNCOBJ_H_CYCLE__

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
///      08/01/2009 - Initial release.
///      09/19/2009 - XML documentation added.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains a synchronization object interface.

    To use this file include wcp/sync.h.
*/


#include "_wcpdef.h"



namespace WCP_NS 
{
    /// <summary>
    /// Describes a synchronization object interface.
    /// </summary>
    /// <remarks>
    /// Include <i>wcp/sync.h</i> to use this class.<br><br>
    /// In order to be able to use derived class as a 
    /// syncronization object for wcp::synchronizable,
    /// the derived class from wcp::sync_object has to implement
    /// an additional static method called <kbd>create_instance</kbd>:
    /// <br><br>
    /// <b>Example:</b>
    /// <code><pre class='_code'> 
    /// static std::auto_ptr&lt;DerivedClassName&gt; create_instance([optional parameters])
    /// {
    ///     return std::auto_ptr&lt;DerivedClassName&gt;(new DerivedClassName());
    /// }
    /// </pre></code>
    /// Note: Optional parameters means any number of parameters that 
    ///      have a default value.
    /// </remarks>
    struct sync_object
    {
        /// <summary>
        /// Tries to take ownership of a synchronized resource.
        /// </summary>
        /// <returns>true if ownership is taken.</returns>
        virtual bool try_seize() const = 0;

        /// <summary>
        /// Takes ownership of a synchronized resource. 
        /// Does not return until ownership is obtained.
        /// </summary>
        virtual void seize() const = 0;

        /// <summary>
        /// Releases ownership of a synchronized resource.
        /// </summary>
        virtual void release() const = 0;
    };

} // namespace WCP_NS


#undef ___SYNCOBJ_H_CYCLE__
#endif /*___SYNCOBJ_H__*/
