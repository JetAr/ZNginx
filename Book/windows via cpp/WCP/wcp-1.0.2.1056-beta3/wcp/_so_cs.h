#ifdef ____SO_CS_H_CYCLE__
#error Cyclic dependency discovered: _so_cs.h
#endif

#ifndef ___SO_CS_H__


#ifndef DOXYGEN

#define ___SO_CS_H__
#define ___SO_CS_H_CYCLE__

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
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Critical section based synchronizable object implementation.

    To use this file include wcp/sync.h.
*/


#include "_syncobj.h"
#include "noncopyable.h"



namespace WCP_NS
{
/// <summary>
/// Wraps a standard critical section object.
/// </summary>
/// <remarks>
/// Include <i>wcp/sync.h</i> to use this class.
/// </remarks>
struct critical_section
    : public sync_object
    , noncopyable
{
    /// <summary>
    /// Construct a new critical section.
    /// </summary>
    critical_section()
    {
        InitializeCriticalSection(&cs);
    }

    /// <summary>
    /// Handles critical section destruction.
    /// </summary>
    ~critical_section()
    {
        DeleteCriticalSection(&cs);
    }

    /// <summary>
    /// Tries to take critical section ownership.
    /// </summary>
    /// <returns>true is ownership is taken; otherwise, return false.</returns>
    bool try_seize() const
    {
        return TryEnterCriticalSection(&cs) == TRUE;
    }

    /// <summary>
    /// Takes critical section ownerhip. Does not return until ownership is taken.
    /// </summary>
    void seize() const
    {
        EnterCriticalSection(&cs);
    }

    /// <summary>
    /// Releases critical section ownership.
    /// </summary>
    void release() const
    {
        LeaveCriticalSection(&cs);
    }

    /// <summary>
    /// Creates a new instance of critical_section class.
    /// </summary>
    /// <returns>Smart pointer to a new instance of critical_section class.</returns>
    static std::auto_ptr<critical_section> create_instance()
    {
        return std::auto_ptr<critical_section>(new critical_section());
    }

private:

    mutable CRITICAL_SECTION cs;
};

} // namespace WCP_NS


#undef ___SO_CS_H_CYCLE__
#endif /*___SO_CS_H__*/
