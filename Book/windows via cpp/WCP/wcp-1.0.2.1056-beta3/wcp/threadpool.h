#ifdef ___THREADPOOL_H_CYCLE__
#error Cyclic dependency discovered: threadpool.h
#endif

#ifndef __THREADPOOL_H__


#ifndef DOXYGEN

#define __THREADPOOL_H__
#define __THREADPOOL_H_CYCLE__

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
///      11/09/2009 - Initial release.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///      11/22/2009 - Thread pool algorithm changed completely.
///      11/24/2009 - Thread pool algorithm changed completely again.
///      12/15/2009 - Asserts added. Empty tasks are ignored.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Thread pool implementation.
*/


#include <climits>
#include <map>
#include <deque>
#include <list>
#include <vector>
#include <iterator>

#include "thread.h"



#ifndef WCP_THREADPOOL_IDLE_DELAY

/// <summary>
/// Delay (in milliseconds) of threadpool idle threads. Default value is 500.
/// </summary>
#define WCP_THREADPOOL_IDLE_DELAY 500

#endif

#ifndef WCP_THREADPOOL_DEFAULT_THREADS_PER_CPU

/// <summary>
/// Default average number of threads per processor in threadpool. Default value is 8.
/// </summary>
#define WCP_THREADPOOL_DEFAULT_THREADS_PER_CPU 8
#endif

/// <summary>
/// Forces threadpool to to set minimum number of idle threads equal to number of processors.
/// </summary>
#define WCP_THREADPOOL_DEFAULT_MIN_THREADS   LONG_MIN

/// <summary>
/// Forces threadpool to to set maximum number of threads equal to number
/// of processors multiply by WCP_THREADPOOL_DEFAULT_THREADS_PER_CPU.
/// </summary>
#define WCP_THREADPOOL_DEFAULT_MAX_THREADS LONG_MAX



namespace WCP_NS
{

/// <summary>
/// Implements a threadpool. Do not create global
/// </summary>
class threadpool : noncopyable
{
public:

    /// <summary>
    /// Constructs a threadpool object.
    /// </summary>
    /// <param name='maxthreads'>Maximum number of threads in threadpool.
    /// Default value is \ref WCP_THREADPOOL_DEFAULT_MAX_THREADS.</param>
    /// <param name='minthreads'>Miminum number of threads in threadpool.
    /// Must not be greater than maxthreads. Default value is
    /// \ref WCP_THREADPOOL_DEFAULT_MIN_THREADS.</param>
    threadpool(long maxthreads = WCP_THREADPOOL_DEFAULT_MAX_THREADS,
               long minthreads = WCP_THREADPOOL_DEFAULT_MIN_THREADS)
        : data_ptr(new threadpool_data_t())
        , data(*data_ptr)
    {
        data.new_task_event = CreateEvent(NULL, TRUE, FALSE, NULL);
        if(!data.new_task_event)
            throw win_exception(GetLastError());

        set_min_threads(minthreads);
        set_max_threads(maxthreads);
    }

    /// <summary>
    /// Handles threadpool deconstruction. Won't return untill all threads
    /// in a threadpool are stopped.
    /// </summary>
    ~threadpool()
    { }

    /// <summary>
    /// Returns minimum number of threads in threadpool.
    /// </summary>
    /// <returns>Minimum number of threads in threadpool.</returns>
    long get_min_threads() const
    {
        return data.min_threads;
    }

    /// <summary>
    /// Returns maximum number of threads in threadpool.
    /// </summary>
    /// <returns>Maximum number of threads in threadpool.</returns>
    long get_max_threads() const
    {
        return data.max_threads;
    }

    /// <summary>
    /// Sets maximum number of threads in threadpool.
    /// </summary>
    /// <param name='maxthreads'>Maximum number of threads in threadpool.
    /// If less than minimum number of threads, then maximum number of threads
    /// will be equal to minimum number of threads. If \ref WCP_THREADPOOL_DEFAULT_MAX_THREADS
    /// is passed, the maximum number of threads will be equal to number of
    /// CPUs multiplied by \ref WCP_THREADPOOL_DEFAULT_THREADS_PER_CPU.</param>
    void set_max_threads(long maxthreads)
    {
        GUARD(access)
        {
            if(maxthreads == WCP_THREADPOOL_DEFAULT_MAX_THREADS)
                maxthreads = data.min_threads * WCP_THREADPOOL_DEFAULT_THREADS_PER_CPU;

            if(maxthreads == data.max_threads)
                return;

            _ASSERTE("MaxThreads cannot be less than MinThreads" \
                     && maxthreads > data.min_threads);
            if(maxthreads < data.min_threads)
                maxthreads = data.min_threads;

            InterlockedExchange(&data.max_threads, maxthreads);
        }
    }

    /// <summary>
    /// Sets maximum number of threads in threadpool.
    /// </summary>
    /// <param name='minthreads'>Minimum number of threads in threadpool.
    /// Must not be greater than maximum number of threads. If \ref WCP_THREADPOOL_DEFAULT_MIN_THREADS
    /// is passed, the minimum number of threads will be equal to number of CPUs.</param>
    void set_min_threads(long minthreads)
    {
        GUARD(access)
        {
            if(minthreads == WCP_THREADPOOL_DEFAULT_MIN_THREADS)
                minthreads = get_number_of_processors();

            if(minthreads == data.min_threads)
                return;

            _ASSERTE("MinThreads cannot be greater than MaxThreads" \
                     && minthreads < data.max_threads);
            if(minthreads > data.max_threads)
                throw invalid_argument_exception(TEXT("minthreads"));

            InterlockedExchange(&data.min_threads, minthreads);
        }
    }

    /// <summary>
    /// Enqueues a task to be executed in context of one of threadpool threads.
    /// Avoid calling TerminateThread and _endthread from inside a task passed
    /// because it could cause an unpredicated behaviour and memory leaks.
    /// Threre is no way to cancel task after it is enqueued.
    /// </summary>
    /// <param name='t'>Task to enqueue.</param>
    void enqueue_task(const safe_task& t)
    {
        _ASSERTE("Empty task specified" && t);
        if(!t)
            return;

        GUARD(access)
        {
            data.tasks.push_back(t);
            InterlockedIncrement(&data.tasks_enqueued);
            SetEvent(data.new_task_event);

            LOCK(threads)
            {
                if((long)(*threads).size() < data.max_threads)
                    spawn_new_thread();
            }
        }
    }

    /// <summary>
    /// Checks if a threadpool is busy, i.e. it has at
    /// leats one task is enqueued and still not complete.
    /// </summary>
    bool busy() const
    {
        return data.tasks_enqueued > 0;
    }

    /// <summary>
    /// Returns number of tasks in task queue.
    /// </summary>
    /// <returns>Number of tasks in task queue.</returns>
    size_t number_of_tasks() const
    {
        return data.tasks_enqueued;
    }

    /// <summary>
    /// Waits until a threadpool is busy, i.e. it has at leats one
    /// task is enqueued and still not complete.
    /// </summary>
    /// <param name='timeout'>The time-out interval in milliseconds. Default valeu is INFINITE.</param>
    bool wait(dword_t timeout = INFINITE)
    {
        dword_t started_at = GetTickCount();
        dword_t ticks = started_at;
        do
        {
            if(!busy())
                return true;

            Sleep(WCP_THREADPOOL_IDLE_DELAY);

            ticks += GetTickCount() - ticks;
        }
        while((ticks - started_at) < timeout);

        return false;
    }

private:

    typedef std::deque<safe_task> task_queue_t;

    struct threadpool_data_t
    {
        threadpool_data_t()
            : max_threads(WCP_THREADPOOL_DEFAULT_MAX_THREADS)
            , min_threads(WCP_THREADPOOL_DEFAULT_MIN_THREADS)
            , tasks_enqueued(0)
            , number_of_threads(0)
        {
        }

        task_queue_t tasks;
        long tasks_enqueued;

        thread th;
        long number_of_threads;

        long max_threads, min_threads;

        handles::handle_t new_task_event;
    };

    typedef safe_shared_ptr<threadpool_data_t> threadpool_data_ptr_t;
    threadpool_data_ptr_t data_ptr;
    threadpool_data_t& data;
    monitor access;

    typedef safe_shared_ptr<thread> thread_ptr_t;
    typedef std::list<thread_ptr_t> threads_t;
    synchronized<threads_t> threads;


    // Returns number of CPUs
    static size_t get_number_of_processors()
    {
        struct _
        {
            static size_t get()
            {
                SYSTEM_INFO sysinfo = { 0 };
                GetSystemInfo(&sysinfo);
                return sysinfo.dwNumberOfProcessors;
            }
        };

        static size_t processors = _::get();
        return processors;
    }

    // Spawns new worker thread
    void spawn_new_thread()
    {
        thread_ptr_t th(new thread());
        th->self_ptr = th;
        (*threads).push_back(th);
        threads_t::iterator iter = --(*threads).end();

        try
        {
            th->start(task(*this, &threadpool::working_thread, data_ptr, iter));
        }
        catch(...)
        {
            (*threads).erase(iter);
            th->self_ptr.reset();
            throw;
        }
    }

    // Working thread entry
    void working_thread(threadpool_data_ptr_t pdata, threads_t::iterator pthread)
    {
        thread& th = **pthread;
        threadpool_data_t& data = *pdata;
        InterlockedIncrement(&data.number_of_threads);

        // Set thread affinity
        static long processor = 0;
        if(!InterlockedIncrement(&processor))
            InterlockedIncrement(&processor);
        dword_t aff_mask = 1 << (processor % get_number_of_processors());
        SetThreadAffinityMask(GetCurrentThread(), aff_mask);

        // Repeat until need to stop
        while(!th.need_stop())
        {
            // Wait for a new task event
            if(WaitForSingleObject(data.new_task_event, WCP_THREADPOOL_IDLE_DELAY)
                    == WAIT_OBJECT_0)
            {
                do
                {
                    safe_task t;

                    GUARD(access)
                    {
                        if(data.tasks.empty()) // No more tasks
                        {
                            ResetEvent(data.new_task_event);
                            goto thread_end;
                        }

                        // Check if number of threads exceeded maximum allowed
                        if(data.number_of_threads > data.max_threads)
                            goto thread_end;

                        // Dequeue task
                        t = data.tasks.front();
                        data.tasks.pop_front();

                        // Spawn new working thread if there are tasks enqueued
                        // and number of threads is less than maximum allowed
                        if(!data.tasks.empty() && data.number_of_threads < data.max_threads)
                            NONTHROWABLE(spawn_new_thread());
                    }

                    NONTHROWABLE(t());
                }
                while(!th.need_stop() // Stop if no more tasks or need to stop
                        && InterlockedDecrement(&data.tasks_enqueued) > 0);
            }
        }

thread_end:

        InterlockedDecrement(&data.number_of_threads);

        // Delete thread from thread list
        LOCK(threads)
        {
            (*threads).erase(pthread);
        }
    }
};

} // namespace WCP_NS


#undef __THREADPOOL_H_CYCLE__
#endif /*__THREADPOOL_H__*/
