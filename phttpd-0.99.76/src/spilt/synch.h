/*
 * SYNCH.H - Solaris to POSIX Interface Layer for Threads.
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

#if !defined(SYNCH_INCLUDED)
#define	SYNCH_INCLUDED

#pragma ident "@(#) synch.h.txt 1.1 94/10/06	Richard.Marejka@Canada.Sun.COM"

/* Feature Test Macros		*/

#define	_POSIX_THREADS

#if !defined(SPILT_MACROS)
#	define	SPILT_MACROS	1	/* use macros where possible		*/
#endif

/* Include Files		*/

#include <pthread.h>
#include <sys/types.h>

/* Constants & Macros		*/

#if defined(SPILT_MACROS)
#	define	mutex_t			pthread_mutex_t
#	define	cond_t			pthread_cond_t

#	define	mutex_destroy		pthread_mutex_destroy
#	define	mutex_lock		pthread_mutex_lock
#	define	mutex_trylock		pthread_mutex_trylock
#	define	mutex_unlock		pthread_mutex_unlock
#	define	cond_destroy		pthread_cond_destroy
#	define	cond_wait		pthread_cond_wait
#	define	cond_signal		pthread_cond_signal
#	define	cond_broadcast		pthread_cond_broadcast
#endif  /* SPILT_MACROS	*/

#define	USYNC_THREAD	0x0001
#define	USYNC_PROCESS	0x0002

/* Data Declarations	*/

typedef struct _timestruc
{
    time_t	tv_sec;
    long	tv_nsec;
} timestruc_t;

#if !defined(SPILT_MACROS)
typedef pthread_mutex_t	mutex_t;
typedef pthread_cond_t	cond_t;
#endif	/* SPILT_MACROS	*/

typedef struct _rwlock
{
    pthread_mutex_t	lock;		/* lock for structure		*/
    pthread_cond_t	readers;	/* waiting readers		*/
    pthread_cond_t	writers;	/* waiting writers		*/
    int		state;		/* -1:writer,0:free,>0:readers	*/
    int		waiters;	/* number of waiting writers	*/
} rwlock_t;

typedef struct _sema
{
    pthread_mutex_t	lock;		/* lock for structure		*/
    pthread_cond_t	waiters;	/* waiters			*/
    unsigned	count;		/* current value		*/
} sema_t;

/* External References		*/

#if defined(__cplusplus)
extern  "C" {
#endif

extern	int	mutex_init( mutex_t *, int, void * );
#if !defined(SPILT_MACROS)
extern	int	mutex_destroy( mutex_t * );
extern	int	mutex_lock( mutex_t * );
extern	int	mutex_trylock( mutex_t * );
extern	int	mutex_unlock( mutex_t * );
#endif	/* SPILT_MACROS	*/

extern	int	cond_init( cond_t *, int type, void * );
#if !defined(SPILT_MACROS)
extern	int	cond_destroy( cond_t * );
extern	int	cond_wait( cond_t *, mutex_t * );
#endif	/* SPILT_MACROS	*/
extern	int	cond_timedwait( cond_t *, mutex_t *, timestruc_t * );
#if !defined(SPILT_MACROS)
extern	int	cond_signal( cond_t * );
extern	int	cond_broadcast( cond_t * );
#endif	/* SPILT_MACROS	*/

extern	int	sema_init( sema_t *, unsigned int, int , void * );
extern	int	sema_destroy( sema_t * );
extern	int	sema_wait( sema_t * );
extern	int	sema_trywait( sema_t * );
extern	int	sema_post( sema_t * );

extern	int	rwlock_init( rwlock_t *, int, void * );
extern	int	rwlock_destroy( rwlock_t * );
extern	int	rw_rdlock( rwlock_t * );
extern	int	rw_tryrdlock( rwlock_t * );
extern	int	rw_wrlock( rwlock_t * );
extern	int	rw_trywrlock( rwlock_t * );
extern	int	rw_unlock( rwlock_t * );

#if defined(__cplusplus)
}
#endif
#endif	/* SYNCH_INCLUDED	*/
