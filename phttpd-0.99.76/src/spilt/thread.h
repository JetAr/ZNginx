/*
 * THREAD.H - Solaris to POSIX Interface Layer for Threads.
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

#if !defined(THREAD_INCLUDED)
#define	THREAD_INCLUDED

#pragma ident "@(#) thread.h.txt 1.1 94/10/06	Richard.Marejka@Canada.Sun.COM"

/* Feature Test Macros		*/

#define	_POSIX_THREADS

#if !defined(SPILT_MACROS)
#	define	SPILT_MACROS	1	/* use macros where possible	*/
#endif

/* Include Files		*/

#include <pthread.h>
#include <signal.h>

/* Constants & Macros		*/

#define	_SPILT_LIBERAL_DAEMON		0x0001	/* default	*/
#define	_SPILT_LIBERAL_SUSPENDED	0x0002
#define	_SPILT_LIBERAL_NEW_LWP		0x0004	/* default	*/
#define	_SPILT_LIBERAL_SETCONCURRENCY	0x0008	/* default	*/

#define	_SPILT_LIBERAL_GETCONCURRENCY	0x0010	/* default	*/
#define	_SPILT_LIBERAL_SUSPEND		0x0020
#define	_SPILT_LIBERAL_CONTINUE		0x0040
#define	_SPILT_LIBERAL_BOUND		0x0080

#define	_SPILT_LIBERAL_STACKADDR	0x0100	/* default	*/
#define	_SPILT_LIBERAL_STACKSIZE	0x0200	/* default	*/
#define	_SPILT_LIBERAL_SETPRIO		0x0400	/* default	*/
#define	_SPILT_LIBERAL_GETPRIO		0x0800	/* default	*/

#define	_SPILT_LIBERAL_DEFAULT		(0x0f1d)

#define	SPILT_LIBERAL			_SPILT_LIBERAL_DEFAULT

#if defined(SPILT_MACROS)
#	define	thread_t		pthread_t
#	define	thread_key_t		pthread_key_t

#	define	thr_self		pthread_self
#	define	thr_exit		pthread_exit
#	define	thr_yield		sched_yield
#	define	thr_sigsetmask		pthread_sigmask
#	define	thr_kill		pthread_kill
#	define	thr_keycreate		pthread_key_create
#	define	thr_setspecific		pthread_setspecific
#endif	/* SPILT_MACROS	*/

#define	THR_DAEMON		0x0001
#define	THR_NEW_LWP		0x0002
#define	THR_BOUND		0x0004
#define	THR_SUSPENDED		0x0008
#define	THR_DETACHED		0x0010

/* Data Declarations		*/

#if !defined(SPILT_MACROS)
typedef pthread_t	thread_t;
typedef pthread_key_t	thread_key_t;
#endif	/* SPILT_MACROS	*/

/* External References		*/

#if defined(__cplusplus)
extern "C" {
#endif

extern	int	thr_create( void *, size_t, void *(*)( void * ), void *, long, thread_t * );
extern	size_t	thr_min_stack( void );
#if !defined(SPILT_MACROS)
extern	thread_t	thr_self( void );
extern	void	thr_exit( void * );
#endif	/* SPILT_MACROS	*/
extern	int	thr_join( thread_t, thread_t *, void ** );
extern	int	thr_setprio( thread_t, int  );
extern	int	thr_getprio( thread_t, int * );
extern	int	thr_suspend( thread_t );
extern	int	thr_continue( thread_t );
extern	int	thr_setconcurrency( int );
extern	int	thr_getconcurrency( void );
#if !defined(SPILT_MACROS)
extern	int	thr_yield( void );
extern	int	thr_sigsetmask( int, const sigset_t *, sigset_t * );
extern	int	thr_kill( thread_t, int );
extern	int	thr_keycreate( thread_key_t *, void (*)( void * ) );
extern	int	thr_setspecific( thread_key_t, void * );
#endif	/* SPILT_MACROS	*/
extern	int	thr_getspecific( thread_key_t, void ** );

#if defined(__cplusplus)
}
#endif
#endif	/* THREAD_INCLUDED	*/
