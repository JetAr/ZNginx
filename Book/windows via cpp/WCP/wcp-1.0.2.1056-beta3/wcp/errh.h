#ifdef ___ERRH_H_CYCLE__
#error Cyclic dependency discovered: errh.h
#endif

#ifndef __ERRH_H__


#ifndef DOXYGEN

#define __ERRH_H__
#define __ERRH_H_CYCLE__

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
///      11/08/2009 - Initial release.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Error handling support.
*/


#include "debug.h"
#include "_win_exception.h"



/// <summary>
/// Performs an action that is likely to throw any
/// kind of C++ %exception. All %exceptions are omitted.
/// </summary>
/// <param name='action'>Action to execute.</param>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'>
///     NONTHROWABLE(this->CallCallback());
/// </pre></code>
/// identical to
/// <code><pre class='_code'>
///     try
///     {
///         this->CallCallback());
///     }
///     catch(...) { }
/// </pre></code>
/// </remarks>
#define NONTHROWABLE(action)        try { action; }                                     \
                                    catch(...)                                          \
                                    { DBGE("Exception in NONTHROWABLE(" #action ")"); }



/// <summary>
/// Performs an action that is likely to throw any kind of %exception.
/// If %exception is thrown, the handler gets executed.
/// </summary>
/// <param name='action'>Action to execute.</param>
/// <param name='handler'>Error handler.</param>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'>
///     ONTHROW(this->CallCallback(), this->RemoveCallback());
/// </pre></code>
/// identical to
/// <code><pre class='_code'>
///     try
///     {
///         this->CallCallback());
///     }
///     catch(...)
///     {
///         this->RemoveCallback();
///     }
/// </pre></code>
/// </remarks>
#define ONTHROW(action, handler)    \
    try { action; }                 \
    catch(...) { handler; }



/// <summary>
/// Performs an action that is likely to throw any kind of %exception.
/// If an %exception occurs, a finalizer gets executed and then %exception continue execution.
/// </summary>
/// <param name='action'>Action to execute.</param>
/// <param name='finalizer'>Action to execute .</param>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'>
///     FILE* pFile = fopen(fileName, "rb");
///     if(pFile != NULL)
///         FINALIZE(ProcessFile(pFile), fclose(pFile));
/// </pre></code>
/// identical to
/// <code><pre class='_code'>
///     FILE* pFile = fopen(fileName, "rb");
///     if(pFile != NULL)
///     {
///         try
///         {
///             ProcessFile(pFile);
///             fclose(pFile);
///         }
///         catch(...)
///         {
///             fclose(pFile);
///             throw;
///         }
///     }
/// </pre></code>
/// </remarks>
#define FINALIZE(action, finalizer)                 \
{                                                   \
    try { action; finalizer; }                      \
    catch(...) { NONTHROWABLE(finalizer); throw; }  \
}


#undef __ERRH_H_CYCLE__
#endif /*__ERRH_H__*/
