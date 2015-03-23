#ifdef ___THREAD_H_CYCLE__
#error Cyclic dependency discovered: thread.h
#endif

#ifndef __THREAD_H__


#ifndef DOXYGEN

#define __THREAD_H__
#define __THREAD_H_CYCLE__

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
///      11/07/2009 - Synchronization was added to thread::get_tls.
///      11/08/2009 - Thread flags were added.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///      11/22/2009 - Static thread initializer added.
///      11/24/2009 - Ability to suspend/resume threads added. Selft destruction
///                   bug in main thread procedure is fixed.
///      12/13/2009 - Allocator template type are added to wcp::thread_class declaration.
///                   wcp::thread::suspended and wcp::thread::resume are removed.
///                   Thread class kind added.
///      12/15/2009 - Crash during termination of joinable thread is fixed.
///                   Asserts added.
///
/////////////////////////////////////////////////////////////////////////////////


#endif /*DOXYGEN*/

/** \file
    Threading support.
*/


#include <process.h>

#include "wcplib.h"
#include "task.h"
#include "tls.h"
#include "handles.h"


namespace WCP_NS
{
template<class Allocator>
class thread_class;

class threadpool;

#ifndef DOXYGEN

namespace internals
{
struct thread_initializer;

} // namespace internals

#endif /*DOXYGEN*/

/// <summary>
/// Implements a thread object.
/// </summary>
class thread : noncopyable
{
    template<class Allocator>
    friend class thread_class;

    friend class threadpool;
    friend struct internals::thread_initializer;

    HANDLE handle;
    handles::handle_t evt;
    task thread_task;
    monitor mon;

    enum
    {
        flag_stop = 0x1,
        flag_completed = 0x2,
    };

    long flags;

    static const dword_t InvalidTID = ~0;
    dword_t tid;

    static tls<thread*>& get_tls()
    {
        static tls<thread*> tls;
        return tls;
    }

public:

    /// <summary>
    /// Constructs a thread object.
    /// </summary>
    thread()
        : handle(INVALID_HANDLE_VALUE)
        , evt(NULL)
        , tid(InvalidTID)
        , flags(0)
    { }

    ~thread()
    {
        if(handle != INVALID_HANDLE_VALUE)
            CloseHandle(handle);
    }

    /// <summary>
    /// Starts a thread.
    /// </summary>
    /// <param name='t'>Task to start in a newly created thread. Avoid calling TerminateThread
    /// and _endthread from inside a thread function because it could cause an
    /// unpredicated behaviour and memory leaks.</param>
    /// <param name='suspended'>If true, a newly created thread will be suspended.</param>
    /// <remarks>
    /// Should not be called for a working thread.
    /// </remarks>
    void start(task t, bool suspended = false)
    {
        _ASSERTE("Empty task specified" && t);
        _ASSERTE("Thread is already working" && tid == InvalidTID);

        if(tid == InvalidTID)
        {
            GUARD(mon)
            {
                flags = 0;
                thread_task = t;

                // Create or reset thread event
                if(evt)
                    ResetEvent(evt);
                else
                {
                    evt = CreateEvent(NULL, TRUE, FALSE, NULL);
                    if(!evt)
                        throw win_exception(GetLastError());
                }

                // Start native thread
                handle = CreateThread(NULL, 0, thread_proc, this,
                                      suspended ? CREATE_SUSPENDED : 0, NULL);
                if(!(handle && handle != INVALID_HANDLE_VALUE))
                {
                    dword_t code = GetLastError();
                    evt.reset();
                    throw win_exception(code);
                }
            }
        }
    }

    /// <summary>
    /// Returns a native handle value.
    /// </summary>
    /// <returns>Native handle value.</returns>
    HANDLE get_native_handle() const
    {
        return handle;
    }

    /// <summary>
    /// Returns a thread identifier.
    /// </summary>
    /// <returns>Thread identifier.</returns>
    dword_t get_id() const
    {
        return tid;
    }

    /// <summary>
    /// Checks if a thread is working.
    /// </summary>
    operator bool () const
    {
        return tid != InvalidTID;
    }

    /// <summary>
    /// Returns a %reference to a thread object for a calling thread.
    /// </summary>
    /// <returns>Reference to a thread object for a calling thread.</returns>
    static thread& get_current()
    {
        return **get_tls();
    }

    /// <summary>
    /// Suspends a calling thread until a joined thread is working.
    /// Has no effect when called from a working thread.
    /// </summary>
    /// <remarks>
    /// Should not be called from an owned thread.
    /// </remarks>
    void join()
    {
        _ASSERTE("Cannot join from the working thread context" && GetCurrentThreadId() != tid);
        if(*this && GetCurrentThreadId() != tid)
            WaitForSingleObject(evt, INFINITE);
    }

    /// <summary>
    /// Sets stop flag for a thread. Has no effect when called from a working thread.
    /// </summary>
    /// <param name='wait'>If true, waits until thread is stopped.</param>
    /// <remarks>
    /// Should not be called from an owned thread.
    /// </remarks>
    void stop(bool wait = false)
    {
        _ASSERTE("Cannot stop from the working thread context" && GetCurrentThreadId() != tid);
        if(*this && GetCurrentThreadId() != tid)
        {
            ::InterlockedExchange(&flags, flags | flag_stop);
            if(wait)
                join();
        }
    }

    /// <summary>
    /// Returns a stop flag value.
    /// </summary>
    /// <returns>Stop flag value.</returns>
    bool need_stop() const
    {
        return (flags & flag_stop) == flag_stop;
    }

    /// <summary>
    /// Check if a thread is completed.
    /// </summary>
    /// <returns>true if a thread is completed.</returns>
    bool completed() const
    {
        return (flags & flag_completed) == flag_completed;
    }

    /// <summary>
    /// Checks if a thread is currently working.
    /// </summary>
    /// <returns>true if thread is working.</returns>
    bool busy() const
    {
        if(*this && GetCurrentThreadId() != tid)
            return *this ? WaitForSingleObject(evt, 0) == WAIT_OBJECT_0 : false;
        return true;
    }

private:

    void reset_flags()
    {
        ::InterlockedExchange(&flags, 0);
    }

    safe_shared_ptr<thread> self_ptr;

    static DWORD __stdcall thread_proc(void* pdata)
    {
        NONTHROWABLE(reinterpret_cast<thread*>(pdata)->thread_proc());
        return 0;
    }

    void thread_proc()
    {
        tid = GetCurrentThreadId();
        get_tls().set(this);

        NONTHROWABLE(thread_task());

        // Either thread object was owned by a detached thread
        // or was a standalone object.
        _ASSERTE("Invalid number of references to a thread object" \
                 && (self_ptr.use_count() == 1 || self_ptr.use_count() == 0));

        get_tls().clear_slot();

        if(self_ptr.use_count() != 0)
            // Thread object was owned by a detached thread
        {
            // Just destroy itself
            safe_shared_ptr<thread> pself = self_ptr;
            self_ptr.reset();
        }
        else
        {
            // Reset thread data in order to be able to be run again
            GUARD(mon)
            {
                ::InterlockedExchange(&flags, flags | flag_completed);
                ::InterlockedExchange((long*)&tid, InvalidTID);

                CloseHandle(handle);
                handle = INVALID_HANDLE_VALUE;

                SetEvent(evt);
            }
        }
    }
};

//------------------------------------------------------------------------------

#ifndef DOXYGEN

namespace internals
{
struct thread_initializer
    : public singleton<thread_initializer>
{
    thread_initializer() throw()
    {
        init_library();
        thread::get_tls();
    }
};

STATIC_INITIALIZATION(thread_initializer);

} // namespace internals

#endif /*DOXYGEN*/

//------------------------------------------------------------------------------

/// <summary>
/// Base thread class.
/// </summary>
template<class Allocator = std_allocator_template_provider>
class thread_class
{
public:

    /// <summary>
    /// Defines a thread kind.
    /// </summary>
    enum thread_kind
    {
        /// <summary>
        /// Defines a detached thread kind.
        /// </summary>
        /// <remarks>
        /// A deatched thread destroys its object on completion.
        /// </remarks>
        detached,

        /// <summary>
        /// Defines a joinable thread.
        /// </summary>
        /// <remarks>
        /// A joinable thread does not destroy its object on completion,
        /// instead it requires thread object owner to call thread::join
        /// to wait for a thread to complete and then destroy a thread_class object.
        /// </remarks>
        joinable
    };

private:

    typedef typename Allocator::template_provider<thread_class>::type allocator_t;

    safe_shared_ptr<thread> pthread;
    thread_kind kind;

protected:

    /// <summary>
    /// Constructs a thread object.
    /// </summary>
    /// <param name='kind'>Defines a thread kind.</param>
    thread_class(thread_kind kind) : kind(kind)
    { }

    /// <summary>
    /// Thread entry. Must be implemented by a derived class.
    /// </summary>
    /// <param name='t'>Current thread object %reference.</param>
    virtual void entry(thread& t) = 0;

    /// <summary>
    /// Gets invoked on thread completion.
    /// </summary>
    virtual void oncompletion() { }

public:

    /// <summary>
    /// Handles thread destruction.
    /// </summary>
    virtual ~thread_class()
    {
        if(pthread)
            pthread->stop(true);
    }

    /// <summary>
    /// Runs a thread object.
    /// </summary>
    /// <remarks>
    /// Should be called only one time.
    /// </remarks>
    void run_thread()
    {
        _ASSERTE("The thread is already working" && !pthread);

        if(!pthread)
        {
            pthread.reset(new thread());
            pthread->start(task(*this, &thread_class::thread_entry));
        }
    }

    /// <summary>
    /// Returns a %reference to a current thread object.
    /// </summary>
    /// <returns>Reference to a current thread object.</returns>
    thread& get_thread()
    {
        return *pthread;
    }

    /// <summary>
    /// Returns a %reference to a current thread object.
    /// </summary>
    /// <returns>Reference to a current thread object.</returns>
    const thread& get_thread() const
    {
        return *pthread;
    }

private:

    void thread_entry()
    {
        FINALIZE(FINALIZE(entry(*pthread), oncompletion()), finalize());
    }

    void finalize()
    {
        if(kind == detached)
        {
            // Storage detached thread reference
            pthread->self_ptr = pthread;
            _ASSERTE("Invalid number of references to the thread object" \
                     &&pthread->self_ptr.use_count() == 2);
            pthread.reset();

            // Destroy itself
            allocator_t allocator;
            allocator.destroy(this);
        }
    }
};


} // namespace WCP_NS


#undef __THREAD_H_CYCLE__
#endif /*__THREAD_H__*/
