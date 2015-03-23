#ifdef ____TYPES_H_CYCLE__
#error Cyclic dependency discovered: _types.h
#endif

#ifndef ___TYPES_H__


#ifndef DOXYGEN

#define ___TYPES_H__
#define ___TYPES_H_CYCLE__

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
///      07/20/2009 - Initial release.
///      09/18/2009 - XML documentation added.
///      10/16/2009 - tchar_t type added.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///      12/17/2009 - Base type changed in order to comply with Windows SDK data types.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    The file contains global WCP basic types declarations.

    To use this file include wcp/sync.h.
*/


namespace WCP_NS 
{
    //
    // Basic signed types
    //

    /// <summary>
    /// Signed 8-bit character.
    /// </summary>
    typedef CHAR char_t;

    /// <summary>
    /// Signed 16-bit integer.
    /// </summary>
    typedef SHORT short_t;

    /// <summary>
    /// Signed integer.
    /// </summary>
    typedef INT int_t;

    /// <summary>
    /// Signed long integer.
    /// </summary>
    typedef LONG long_t;

    /// <summary>
    /// Signed 64-bit integer.
    /// </summary>
    typedef LONGLONG longlong_t;

    //
    // Basic unsigned types
    //

    /// <summary>
    /// Unsigned 8-bit character.
    /// </summary>
    typedef UCHAR uchar_t;

    /// <summary>
    /// Unsigned 16-bit integer.
    /// </summary>
    typedef USHORT ushort_t;

    /// <summary>
    /// Unsigned integer.
    /// </summary>
    typedef UINT uint_t;

    /// <summary>
    /// Unsigned long integer.
    /// </summary>
    typedef ULONG ulong_t;

    /// <summary>
    /// Unsigned 64-bit integer.
    /// </summary>
    typedef ULONGLONG ulonglong_t;

    //
    // Basic signed sized types
    //

    /// <summary>
    /// Signed 8-bit integer.
    /// </summary>
    typedef INT8 int8_t;

    /// <summary>
    /// Signed 16-bit integer.
    /// </summary>
    typedef INT16 int16_t;

    /// <summary>
    /// Signed 32-bit integer.
    /// </summary>
    typedef INT32 int32_t;

    /// <summary>
    /// Signed 64-bit integer.
    /// </summary>
    typedef INT64 int64_t;

    //
    // Basic unsigned sized types
    //

    /// <summary>
    /// Unsigned 8-bit integer.
    /// </summary>
    typedef UINT8 uint8_t;

    /// <summary>
    /// Unsigned 8-bit integer.
    /// </summary>
    typedef BYTE byte_t;

    /// <summary>
    /// Unsigned 16-bit integer.
    /// </summary>
    typedef UINT16  uint16_t;

    /// <summary>
    /// Unsigned 16-bit integer.
    /// </summary>
    typedef WORD word_t;

    /// <summary>
    /// Unsigned 32-bit integer.
    /// </summary>
    typedef UINT32 uint32_t;

    /// <summary>
    /// Unsigned 32-bit integer.
    /// </summary>
    typedef DWORD dword_t;

    /// <summary>
    /// Unsigned 64-bit integer.
    /// </summary>
    typedef UINT64 uint64_t;

    /// <summary>
    /// Unsigned 64-bit integer.
    /// </summary>
    typedef ULONG64 qword_t;

    //
    // 64-bit compatible types
    //

    /// <summary>
    /// 64-bit compatible signed integer.
    /// </summary>
    typedef INT_PTR intptr_t;

    /// <summary>
    /// 64-bit compatible signed long integer.
    /// </summary>
    typedef LONG_PTR longptr_t;

    /// <summary>
    /// 64-bit compatible unsigned integer.
    /// </summary>
    typedef UINT_PTR uintptr_t;

    /// <summary>
    /// 64-bit compatible unsigned long integer.
    /// </summary>
    typedef ULONG_PTR ulongptr_t;

    //
    // Miscellaneous
    //

    /// <summary>
    /// Default application char type.
    /// </summary>
    typedef TCHAR tchar_t;

} // namespace tools


#undef ___TYPES_H_CYCLE__
#endif /*___TYPES_H__*/
