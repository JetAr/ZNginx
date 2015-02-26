/*
 * THREAD.C - Solaris to POSIX Interface Layer for Threads.
 *
 * This code is public domain.  It may be freely distributed provided the
 * Copyright notice is retained. This code is not supported in any way.
 * Usage of the code is solely at the user's/programmer's discretion.
 * Sun Microsystems makes no claim as to the reliabilty, correctness or
 * suitability of this code. Sun Microsystems will not be held liable in
 * any way as a result loss or damage arising from the use this code.
 *
 * Written by: Richard.Marejka@Canada.Sun.COM
 * Copyright (c) 1994, by Sun Microsystems, Inc.
 */

/*
 *	Richard Marejka 	Solaris 2 Migration Support Centre
 *	October 6, 1994
 *      richard@canada.sun.com
 *	405-477-1159
 *
 * This package can be used to run a Solaris threads application on a
 * system that support POSIX threads.
 *
 * This package demonstrates that
 *
 * a) Solaris threads is not another proprietary threads package - it
 *    is almost identical to the POSIX proposed standard.
 *
 * b) Solaris threads programmers need not wait for POSIX to be
 *    finalized as a standard, before they begin multithreading
 *    their applications
 *
 * c) The differences between Solaris threads and POSIX threads
 *    are primarily syntactic.
 *
 * In addition, although application programmers may contact OpCom to
 * obtain a copy of the SunSoft POSIX.1c EA package, people should be
 * aware that the SunSoft package is based on Draft 8 of the POSIX
 * standard.   The SunSoft package is not a complete implementation of
 * Draft 8.
 *
 *
 * This package implements Solaris threads in terms of
 * POSIX.1c threads. This package allows an application programmer to
 * write a Solaris threads program, and then port the application to a
 * POSIX.1c based threads implentation.   This package  proves just
 * how small the differences are.
 *
 * This package should not be used on Solaris systems. At this time
 * our POSIX.1c threads are written in terms of Solaris threads.
 */



/*
 *
 * SPILT - Solaris to POSIX Interface Layer for Threads.
 *
 * This code is public-domain, it may be freely distributed provided the
 * Copyright notice is retained. This code is not supported in any way.
 * Usage of the code is solely at the user's/programmer's discretion.
 * Sun Microsystems makes no claim as to the reliabilty, correctness or
 * suitability of this code. Sun Microsystems will not be held liable in
 * any way as a result loss or damage arising from the use this code.
 *
 * This package implements an interface mapping between Solaris threads and
 * POSIX.1c threads. The POSIX.1c material is based on the May 1994/D9 document.
 *
 * This package should make porting Solaris thread source to a POSIX.1c based
 * library a matter of "wedging" this interface between the Solaris thread
 * source and the POSIX.1c library.
 *
 * In producing the package, the goal was to acheive a high degree of mapping
 * between Solaris and POSIX.1c. However, certain aspects of Solaris either
 * do not have a POSIX.1c equivalent have only an "approximate" equivalent.
 * Another small set of Solaris features can only be mapped into a POSIX.1c
 * implementation if the implementation supports the corresponding feature.
 *
 * The package uses two macros to control the characteristics of the
 * package. If the macro SPILT_MACROS is defined then certain functions
 * and typedef's will be implemented as macros to the POSIX.1c equivalent
 * functions and typedef's. Otherwise, all functions and typedefs are
 * strictly defined in the compilation. The second macro is SPILT_LIBERAL,
 * refered to as the "test macro". This macro is built up from the bit-wise
 * or'ing of _SPILT_LIBERAL_* macros. The "liberal" macros allows the
 * package builder to control how tolerant the package will be of semantic
 * incompleteness.
 *
 * The following points describe the "approximate" mappings, how they are
 * implemented and how the _SPILT_LIBERAL_* macro controls behavior.
 *
 * THR_DAEMON
 * 	The thread create option for daemon threads has no equivalent in
 * 	POSIX.1c (ENOTSUP). If the test macro includes the
 * 	_SPILT_LIBERAL_DAEMON flag the option is silently ignored.
 *
 * THR_SUSPENDED
 * 	The thread create option for suspended threads has no equivalent
 * 	in POSIX.1c (ENOTSUP). If the test macro includes the
 * 	_SPILT_LIBERAL_SUSPENDED flag the option is silently ignored.
 *
 * THR_NEW_LWP
 * 	The thread create option for new LWPs has no equivalent in POSIX.1c
 * 	(ENOTSUP). If the test macro includes the _SPILT_LIBERAL_NEW_LWP
 * 	flag the option is silently ignored.
 *
 * thr_setconcurrency
 * 	This function has no equivalent mapping in POSIX.1c (ENOSYS). If
 * 	the test macro includes the _SPILT_LIBERAL_SETCONCURRENCY flag
 * 	the funciton will return success (0).
 *
 * thr_getconcurrency
 * 	This function has no equivalent mapping in POSIX.1c (ENOSYS). If
 * 	the test macro includes the _SPILT_LIBERAL_GETCONCURRENCY flag
 * 	the function will always return zero (0).
 *
 * thr_suspend
 * 	This function has no equivalent mapping in POSIX.1c (ENOSYS). If
 * 	the test macro includes the _SPILT_LIBERAL_SUSPEND flag the
 * 	function will always return success (0).
 *
 * thr_continue
 * 	This function has no equivalent mapping in POSIX.1c (ENOSYS). If
 * 	the test macro includes the _SPILT_LIBERAL_CONTINUE flag the
 * 	function will always return success (0).
 *
 * thr_create
 * 	If the POSIX.1c macro _POSIX_THREAD_PRIORITY_SCHEDULING is defined
 * 	the pthread inheretsched attribute will be set to
 * 	PTHREAD_INHERIT_SCHED.
 *
 * THR_BOUND
 * 	The thread create option for "bound" threads is only supported in
 * 	POSIX.1c if the macro _POSIX_THREAD_PRIORITY_SCHEDULING is defined.
 * 	Attempts to use THR_BOUND when the option is not supported will
 * 	return failure (ENOTSUP). If the test macro includes the
 * 	_SPILT_LIBERAL_BOUND flag the THR_BOUND option will be ignored if
 * 	not supported.
 *
 * stack
 * 	The thread create attribute for stack address is only supported if
 * 	the macro _POSIX_THREAD_ATTR_STACKADDR is defined. Attempts to use
 * 	the stack option when the option is not supported will return
 * 	failure (ENOTSUP). If the test macro includes the
 * 	_SPILT_LIBERAL_STACKADDR flag the stack option will be ignored
 * 	if not supported.
 *
 * stacksize
 * 	The thread create option for stack size is only supported if the
 * 	macro _POSIX_THREAD_ATTR_STACKSIZE is defined. Attempts to use
 * 	the stacksize option when the attribute is not supported will
 * 	return failure (ENOTSUP). If the test macro include the
 * 	_SPILT_LIBERAL_STACKSIZE flag the stacksize option will be
 * 	ignored if not supported.
 *
 * thr_setprio
 * 	The thread priority management is only supported if the macro
 * 	_POSIX_THREAD_PRIORITY_SCHEDULING is defined. Attempts to use
 * 	thread scheduling will return failure (ENOSYS). If the test macro
 * 	includes the _SPILT_LIBERAL_SETPRIO flag attempts to modify
 * 	thread priority will always return success (0).
 *
 * 	[Note: Thread priority managment is crudely mapped onto POSIX.1c
 * 	scheduling (if supported by the implementation), since POSIX.1c
 * 	supports multiple thread scheduling classes (SCHED_FIFO, SCHED_RR,
 * 	SCHED_OTHER and possibly others) and Solaris only has one
 * 	scheduling class. The implemenation here is analogeous to allowing
 * 	a process to modify its priority without being able to determine
 * 	1) its scheduling class and 2) the bounds of allowable priority
 * 	values.]
 *
 * thr_getprio
 * 	The thread priority management is only supported if the macro
 * 	_POSIX_THREAD_PRIORITY_SCHEDULING is defined. Attempts to use
 * 	thread scheduling will return failure (ENOSYS). If the test macro
 * 	includes the _SPILT_LIBERAL_GETPRIO flag attempts to retrieve
 * 	thread priority will always return success (0).
 *
 * thr_join
 * 	POSIX.1c does not support the "non-targeted" join. Attempts
 * 	to do so are undefined.
 *
 *
 * XXX_init
 * 	Solaris synchronization variables are initialized to a default
 * 	state (USYNC_THREAD) by zero filling the variable. This is
 * 	achieved when the variable has external scope, i.e. it does not
 * 	have to be explicitly initialized, because ANSI C states that
 * 	all uninitialized, external data is zero-filled. In contrast,
 * 	POSIX.1c requires external synchronization objects to be
 * 	initialized by assignment of a type-specific macro. The result
 * 	of these differences is that to ensure "correct" semantics
 * 	of external Solaris synchronization objects when ported to
 * 	a POSIX.1c environment is that they must be initialized using
 * 	the appropriate XXX_init function.
 *
 * Written by Richard.Marejka@Canada.Sun.COM
 * Copyright (c) 1994, by Sun Microsystems, Inc.
 *
 */


#pragma ident "@(#) thread.c 1.2 94/10/05	Richard.Marejka@Canada.Sun.COM"

/* Feature Test Macros		*/
/* Include Files		*/

#include <thread.h>
#include <synch.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

/* Constants & Macros		*/
/* Data Declarations		*/
/* External References		*/
/* External Declarations	*/

/* Creation and Control			*/

int
thr_create( void *stackp, size_t stacksize, void *(*entry)( void * ), void *arg, long flags, thread_t *tidp )
{
    pthread_attr_t	attr;
    pthread_t	tid;

    if ( !tidp )
        tidp	= &tid;

    if ( stackp && !stacksize )	/* stack but no size		*/
        return( EINVAL );
    /* unsupported flags		*/
#if !(SPILT_LIBERAL & _SPILT_LIBERAL_NEW_LWP)
    if ( flags & THR_NEW_LWP )
        return( ENOTSUP );
#endif

#if !(SPILT_LIBERAL & _SPILT_LIBERAL_SUSPENDED)
    if ( flags & THR_SUSPENDED )
        return( ENOTSUP );
#endif

#if !(SPILT_LIBERAL & _SPILT_LIBERAL_DAEMON)
    if ( flags & THR_DAEMON )
        return( ENOTSUP );
#endif
    pthread_attr_init( &attr );

    if ( stacksize )
#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
        pthread_attr_setstacksize( &attr, stacksize );
#elif (SPILT_LIBERAL & _SPILT_LIBERAL_STACKSIZE)
        ;
#else
        return( ENOTSUP );
#endif

    if ( stackp )
#if defined(_POSIX_THREAD_ATTR_STACKADDR)
        pthread_attr_setstackaddr( &attr, stackp );
#elif (SPILT_LIBERAL & _SPILT_LIBERAL_STACKADDR)
        ;
#else
        return( ENOTSUP );
#endif

    if ( flags & THR_DETACHED )
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    else
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
    if ( flags & THR_BOUND )
        pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
    else
        pthread_attr_setscope( &attr, PTHREAD_SCOPE_PROCESS );

    pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
#elif !(SPILT_LIBERAL & _SPILT_LIBERAL_BOUND)
    if ( flags & THR_BOUND )
        return( ENOTSUP );
#endif
    return( pthread_create( tidp, &attr, entry, arg ) );
}

#if !defined(SPILT_MACROS)
size_t
thr_min_stack( void )
{
    return( PTHREAD_STACK_MIN );
}
#endif

#if !defined(SPILT_MACROS)
thread_t
thr_self( void )
{
    return( pthread_self() );
}
#endif

#if !defined(SPILT_MACROS)
void
thr_exit( void *status )
{
    pthread_exit( status );
    /* NOTREACHED */
    return;
}
#endif

int
thr_join( thread_t tid, thread_t *dtidp, void **statusp )
{
    int	status	= pthread_join( tid, statusp );

    if ( dtidp )
        *dtidp	= tid;

    return( status );
}

/* Scheduling					*/

int
thr_setprio( thread_t tid, int prio )
{
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
    int			policy;
    struct sched_param	param;

    pthread_getschedparam( tid, &policy, &param );

    if ( ( policy == SCHED_RR ) || ( policy == SCHED_FIFO ) )
    {
        param.sched_priority	= prio;
        pthread_setschedparam( tid, policy, &param );
    }
    return( 0 );
#elif (SPILT_LIBERAL & _SPILT_LIBERAL_SETPRIO)
    return( 0 );
#else
    return( ENOSYS );
#endif
}

int
thr_getprio( thread_t tid, int *priop )
{
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
    int			policy;
    struct sched_param	param;

    pthread_getschedparam( tid, &policy, &param );
    *priop	= param.sched_priority;

    return( 0 );
#elif (SPILT_LIBERAL & _SPILT_LIBERAL_GETPRIO)
    *priop	= 0;
    return( 0 );
#else
    return( ENOSYS );
#endif
}

int
thr_suspend( thread_t tid )
{
#if (SPILT_LIBERAL & _SPILT_LIBERAL_SUSPEND)
    return( 0 );
#else
    return( ENOSYS );
#endif
}

int
thr_continue( thread_t tid )
{
#if (SPILT_LIBERAL & _SPILT_LIBERAL_CONTINUE)
    return( 0 );
#else
    return( ENOSYS );
#endif
}

int
thr_setconcurrency( int value )
{
#if (SPILT_LIBERAL & _SPILT_LIBERAL_SETCONCURRENCY)
    return( 0 );
#else
    return( ENOSYS );
#endif
}

int
thr_getconcurrency( void )
{
#if (SPILT_LIBERAL & _SPILT_LIBERAL_GETCONCURRENCY)
    return( 0 );
#else
    return( ENOSYS );
#endif
}

#if !defined(SPILT_MACROS)
int
thr_yield( void )
{
    return( sched_yield() );
}
#endif


/* Synchronization				*/

static int
setpshared( int type, int *ptypep )
{
    int	status	= 0;

#ifdef PTHREAD_PROCESS_PRIVATE
    switch ( type )
    {
    case USYNC_THREAD	:
        *ptypep	= PTHREAD_PROCESS_PRIVATE;
        break;

    case USYNC_PROCESS	:
        *ptypep	= PTHREAD_PROCESS_SHARED;
        break;

    default		:
        status	= EINVAL;
        break;
    }
#else
    status = EINVAL;
#endif

    return( status );
}

/* Synchronization - mutex			*/

int
mutex_init( mutex_t *mp, int type, void *arg )
{
    int			ptype;
    pthread_mutexattr_t	attr;

    pthread_mutexattr_init( &attr );

    if ( setpshared( type, &ptype ) )
        return( EINVAL );

    pthread_mutexattr_setpshared( &attr, ptype );

    return( pthread_mutex_init( mp, &attr ) );
}

#if !defined(SPILT_MACROS)
int
mutex_destroy( mutex_t *mp )
{
    return( pthread_mutex_destroy( mp ) );
}
#endif

#if !defined(SPILT_MACROS)
int
mutex_lock( mutex_t *mp )
{
    return( pthread_mutex_lock( mp ) );
}
#endif

#if !defined(SPILT_MACROS)
int
mutex_trylock( mutex_t *mp )
{
    return( pthread_mutex_trylock( mp ) );
}
#endif

#if !defined(SPILT_MACROS)
int
mutex_unlock( mutex_t *mp )
{
    return( pthread_mutex_unlock( mp ) );
}
#endif

/* Synchronization - condition variable		*/

int
cond_init( cond_t *cvp, int type, void *arg )
{
    int			ptype;
    pthread_condattr_t	attr;

    pthread_condattr_init( &attr );

    if ( setpshared( type, &ptype ) )
        return( EINVAL );

    pthread_condattr_setpshared( &attr, ptype );

    return( pthread_cond_init( cvp, &attr ) );
}

#if !defined(SPILT_MACROS)
int
cond_destroy( cond_t *cvp )
{
    return( pthread_cond_destroy( cvp ) );
}
#endif

#if !defined(SPILT_MACROS)
int
cond_wait( cond_t *cvp, mutex_t *mp )
{
    return( pthread_cond_wait( cvp, mp ) );
}
#endif

int
cond_timedwait( cond_t *cvp, mutex_t *mp, timestruc_t *abstimep )
{
    struct timespec	tv;

    tv.tv_sec	= abstimep->tv_sec;
    tv.tv_nsec	= abstimep->tv_nsec;

    return( pthread_cond_timedwait( cvp, mp, &tv ) );
}

#if !defined(SPILT_MACROS)
int
cond_signal( cond_t *cvp )
{
    return( pthread_cond_signal( cvp ) );
}
#endif

#if !defined(SPILT_MACROS)
int
cond_broadcast( cond_t *cvp )
{
    return( pthread_cond_broadcast( cvp ) );
}
#endif

/* Synchronization - semaphore			*/

int
sema_init( sema_t *sp, unsigned int count, int type, void *arg )
{
    int			ptype;
    pthread_mutexattr_t	mutex_attr;
    pthread_condattr_t	cond_attr;

    if ( setpshared( type, &ptype ) )
        return( EINVAL );

    pthread_mutexattr_init( &mutex_attr );
    pthread_mutexattr_setpshared( &mutex_attr, ptype );
    pthread_mutex_init( &sp->lock, &mutex_attr );

    pthread_condattr_init( &cond_attr );
    pthread_condattr_setpshared( &cond_attr, ptype );
    pthread_cond_init( &sp->waiters, &cond_attr );

    sp->count	= count;

    return( 0 );
}

int
sema_destroy( sema_t *sp )
{
    pthread_mutex_destroy( &sp->lock );
    pthread_cond_destroy( &sp->waiters );

    return( 0 );
}

int
sema_wait( sema_t *sp )
{
    pthread_mutex_t	*lkp	= &sp->lock;

    pthread_mutex_lock( lkp );
    pthread_cleanup_push( pthread_mutex_unlock, lkp );

    while ( sp->count == 0 )
        pthread_cond_wait( &sp->waiters, lkp );

    sp->count--;

    pthread_cleanup_pop( 1 );

    return( 0 );
}

int
sema_trywait( sema_t *sp )
{
    int	status	= EBUSY;

    pthread_mutex_lock( &sp->lock );

    if ( sp->count )
    {
        sp->count--;
        status	= 0;
    }

    pthread_mutex_unlock( &sp->lock );

    return( status );
}

int
sema_post( sema_t *sp )
{
    pthread_mutex_lock( &sp->lock );

    if ( sp->count++ == 0 )
        pthread_cond_signal( &sp->waiters );

    pthread_mutex_unlock( &sp->lock );

    return( 0 );
}

/* Synchronization - readers / writer lock	*/

int
rwlock_init( rwlock_t *rwp, int type, void *arg )
{
    int			ptype;
    pthread_mutexattr_t	mutex_attr;
    pthread_condattr_t	cond_attr;

    if ( setpshared( type, &ptype ) )
        return( EINVAL );

    pthread_mutexattr_init( &mutex_attr );
    pthread_mutexattr_setpshared( &mutex_attr, ptype );
    pthread_mutex_init( &rwp->lock, &mutex_attr );

    pthread_condattr_init( &cond_attr );
    pthread_condattr_setpshared( &cond_attr, ptype );
    pthread_cond_init( &rwp->readers, &cond_attr );
    pthread_cond_init( &rwp->writers, &cond_attr );

    rwp->state	= 0;
    rwp->waiters	= 0;

    return( 0 );
}

int
rwlock_destroy( rwlock_t *rwp )
{
    pthread_mutex_destroy( &rwp->lock );
    pthread_cond_destroy( &rwp->readers );
    pthread_cond_destroy( &rwp->writers );

    return( 0 );
}

int
rw_rdlock( rwlock_t *rwp )
{
    pthread_mutex_t	*lkp	= &rwp->lock;

    pthread_mutex_lock( lkp );
    pthread_cleanup_push( pthread_mutex_unlock, lkp );
    /* active or queued writers		*/
    while ( ( rwp->state < 0 ) && rwp->waiters )
        cond_wait( &rwp->readers, lkp );

    rwp->state++;
    pthread_cleanup_pop( 1 );

    return( 0 );
}

int
rw_tryrdlock( rwlock_t *rwp )
{
    int	status	= EBUSY;

    pthread_mutex_lock( &rwp->lock );
    /* available and no writers queued	*/
    if ( ( rwp->state >= 0 ) && !rwp->waiters )
    {
        rwp->state++;
        status	= 0;
    }

    pthread_mutex_unlock( &rwp->lock );

    return( status );
}

static void
rw_wrlock_cleanup( void *arg )
{
    rwlock_t	*rwp	= (rwlock_t *) arg;
    /*
     * Was the only queued writer and lock is available for readers.
     * Called through cancellation clean-up so lock is held at entry.
     */
    if ( ( --rwp->waiters == 0 ) && ( rwp->state >= 0 ) )
        pthread_cond_broadcast( &rwp->readers );

    pthread_mutex_unlock( &rwp->lock );

    return;
}

int
rw_wrlock( rwlock_t *rwp )
{
    pthread_mutex_t	*lkp	= &rwp->lock;

    pthread_mutex_lock( lkp );
    rwp->waiters++;		/* another writer queued		*/
    pthread_cleanup_push( rw_wrlock_cleanup, rwp );

    while ( rwp->state )
        pthread_cond_wait( &rwp->writers, lkp );

    rwp->state	= -1;
    pthread_cleanup_pop( 1 );

    return( 0 );
}

int
rw_trywrlock( rwlock_t *rwp )
{
    int	status	= EBUSY;

    pthread_mutex_lock( &rwp->lock );
    /* no readers, no writers, no writers queued	*/
    if ( !rwp->state && !rwp->waiters )
    {
        rwp->state	= -1;
        status		= 0;
    }

    pthread_mutex_unlock( &rwp->lock );

    return( status );
}

int
rw_unlock( rwlock_t *rwp )
{
    pthread_mutex_lock( &rwp->lock );

    if ( rwp->state == -1 )  		/* writer releasing	*/
    {
        rwp->state	= 0;		/* mark as available	*/

        if ( rwp->waiters )		/* writers queued	*/
            pthread_cond_signal( &rwp->writers );
        else
            pthread_cond_broadcast( &rwp->readers );
    }
    else
    {
        if ( --rwp->state == 0 )	/* no more readers	*/
            pthread_cond_signal( &rwp->writers );
    }

    pthread_mutex_unlock( &rwp->lock );

    return( 0 );
}

/* Signals					*/

#if !defined(SPILT_MACROS)
int
thr_sigsetmask( int how, const sigset_t *setp, sigset_t *osetp )
{
    return( pthread_sigmask( how, setp, oldp ) );
}
#endif

#if !defined(SPILT_MACROS)
int
thr_kill( thread_t tid, int signo )
{
    return( pthread_kill( tid, signo ) );
}
#endif

/* Thread Specific Data				*/

#if !defined(SPILT_MACROS)
int
thr_keycreate( thread_key_t *keyp, void (*destructor)( void * ) )
{
    return( pthread_key_create( keyp, destructor ) );
}
#endif

#if !defined(SPILT_MACROS)
int
thr_setspecific( thread_key_t key, void *value )
{
    return( pthread_setspecific( key, value ) );
}
#endif

int
thr_getspecific( thread_key_t key, void **valuep )
{
    *valuep	= pthread_getspecific( key );
    return( 0 );
}
