#ifdef ____SO_MONITOR_H_CYCLE__
#error Cyclic dependency discovered: _so_monitor.h
#endif

#ifndef ___SO_MONITOR_H__


#ifndef DOXYGEN

#define ___SO_MONITOR_H__
#define ___SO_MONITOR_H_CYCLE__

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
///      11/09/2009 - Changed working algorithm.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains implementation of a monitor class. The monitor class provides a 
    simple way to syncronize a shared resource between threads within a process.

    To use this file include wcp/sync.h.
*/


#include <memory>

#include "_syncobj.h"
#include "noncopyable.h"



namespace WCP_NS 
{

#ifndef WCP_DEFAULT_MONITOR_DELAY

/// <summary>
/// Default delay in milliseconds for wcp::monitor.
/// </summary>
#define WCP_DEFAULT_MONITOR_DELAY WCP_SYNC_DELAY

#endif

    /// <summary>
    /// Provides a mutually exclusive access to a shared resource within a process.
    /// </summary>
    /// <remarks>
    /// Behaves the same way as CRITICAL_SECTION does. But unlike CRITICAL_SECTION, 
    /// wcp::monitor has open structure and waranties that a class consumes a constant as small 
    /// as possible amount of resources.<br><br>
    /// Include <i>wcp/sync.h</i> to use this class.
    /// </remarks>
    struct monitor
        : public sync_object
        , noncopyable
    {
        /// <summary>
        /// Constructs an monitor object.
        /// </summary>
        monitor() 
            : lock_counter(0)
            , owning_tid(0)
        { }

        /// <summary>
        /// Tries to take ownership of an monitor object.
        /// </summary>
        /// <returns>true if ownership is taken.</returns>
        bool try_seize() const
        { 
            dword_t tid = GetCurrentThreadId();

            if(lock_counter > 0 && tid != owning_tid)
                return false;

            if(InterlockedIncrement(&lock_counter) == 1)
            {
                InterlockedExchange((long*)&owning_tid, tid);
                return true;
            }
            else if(tid != owning_tid)
            {
                InterlockedDecrement(&lock_counter);
                return false;
            }

            return true;
        }

        /// <summary>
        /// Takes ownership of an monitor object. Does not return until ownership is taken.
        /// </summary>
        void seize() const
        { 
            dword_t tid = GetCurrentThreadId();

            for(;;)
            {
                while(lock_counter > 0 && tid != owning_tid)
                    Sleep(WCP_DEFAULT_MONITOR_DELAY);

                if(InterlockedIncrement(&lock_counter) == 1)
                {
                    InterlockedExchange((long*)&owning_tid, tid);
                    break;
                }
                else if(tid != owning_tid)
                    InterlockedDecrement(&lock_counter);
                else
                    break;
            }
        }

        /// <summary>
        /// Releases ownership of an monitor object.
        /// </summary>
        void release() const
        { 
            _ASSERTE("Monitor is not locked" && lock_counter > 0);

            if(GetCurrentThreadId() == owning_tid)
            {
                if((lock_counter - 1) == 0)
                    owning_tid = 0;

                InterlockedDecrement(&lock_counter);
            }
        }

        /// <summary>
        /// Creates a new instance of an monitor class.
        /// </summary>
        /// <returns>A smart pointer to a newly create instance.</returns>
        static std::auto_ptr<monitor> create_instance()
        { return std::auto_ptr<monitor>(new monitor()); }

    private:

        mutable long lock_counter;
        mutable dword_t owning_tid;
    };

} // namespace WCP_NS

#undef ___SO_MONITOR_H_CYCLE__
#endif /*___SO_MONITOR_H__*/
