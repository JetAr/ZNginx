#ifdef ____EVENT_H_CYCLE__
#error Cyclic dependency discovered: _event.h
#endif

#ifndef ___EVENT_H__


#ifndef DOXYGEN

#define ___EVENT_H__
#define ___EVENT_H_CYCLE__

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
///      12/30/2009 - Initial release.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file 
    Contains a multicast event implementation.

    To use this file include wcp/functional.h.
*/


#include <list>
#include <algorithm>

#include "_delegate.h"


namespace WCP_NS 
{
    /// <summary>
    /// Identifies event result.
    /// </summary>
    enum event_result
    {
        /// <summary>
        /// Default event result. Exent execution propagates to all subscribed delegates
        /// in the revers order of subscription.
        /// </summary>
        event_result_default,

        /// <summary>
        /// Event execution propagation is canceled.
        /// </summary>
        event_result_canceled
    };

#ifndef DOXYGEN

    namespace internals
    {
        template<class T>
        struct event_result_check { };

        template<>
        struct event_result_check<event_result> 
        { enum { ok }; };



        template<class D>
        struct unicast_delegate_storage
        {
        private:

            typedef std::list<D> delegates_t;
            typedef typename delegates_t::iterator iter_t;
            typedef typename delegates_t::const_reverse_iterator enumerator_t;
            delegates_t delegates;
            mutable enumerator_t cur;

        public:

            void add(const D& d)
            {
                iter_t p = find_by_hash(d.hash_code());
                if(p == delegates.end())
                    delegates.push_back(d);
            }

            void remove(const D& d)
            {
                iter_t p = find_by_hash(d.hash_code());
                if(p != delegates.end())
                    delegates.erase(p);
            }

            bool start_new_enum() const
            {
                cur = delegates.rbegin();
                return cur != delegates.rend();
            }

            typedef const D* const_pd_t;
            bool next(__out const_pd_t& d) const
            {
                if(cur != delegates.rend())
                {
                    d = &*cur;
                    ++cur;
                    return true;
                }

                return false;
            }

            bool empty() const
            { return delegates.empty(); }

        private:

            iter_t find_by_hash(const routine_hash_code& hash)
            {
                iter_t iter = delegates.begin(), end = delegates.end();
                for(; iter != end; ++iter)
                    if((*iter).hash_code() == hash)
                        return iter;

                return end;
            }
        };

    } // namespace internals

#endif /*DOXYGEN*/

    /// <summary>
    /// Multicast event object that accepts 7 parameters. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <typeparam name='P1'>A delegate's 1-st parameter type.</typeparam>
    /// <typeparam name='P2'>A delegate's 2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>A delegate's 3-d parameter type.</typeparam>
    /// <typeparam name='P4'>A delegate's 4-th parameter type.</typeparam>
    /// <typeparam name='P5'>A delegate's 5-th parameter type.</typeparam>
    /// <typeparam name='P6'>A delegate's 6-th parameter type.</typeparam>
    /// <typeparam name='P7'>A delegate's 7-th parameter type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition. 
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<
        class R,
        class P1 = void, 
        class P2 = void, 
        class P3 = void, 
        class P4 = void, 
        class P5 = void, 
        class P6 = void,
        class P7 = void>
    struct multicast_event 
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R, P1, P2, P3, P4, P5, P6, P7> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <param name='p1'>1-st event parameter.</param>
        /// <param name='p2'>2-nd event parameter.</param>
        /// <param name='p3'>3-d event parameter.</param>
        /// <param name='p4'>4-th event parameter.</param>
        /// <param name='p5'>5-th event parameter.</param>
        /// <param name='p6'>6-th event parameter.</param>
        /// <param name='p7'>7-th event parameter.</param>
        /// <returns>false if event was canceled.</returns>
        bool operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)(p1, p2, p3, p4, p5, p6, p7) == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)(p1, p2, p3, p4, p5, p6, p7);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

    /// <summary>
    /// Multicast event object that accepts 6 parameters. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <typeparam name='P1'>A delegate's 1-st parameter type.</typeparam>
    /// <typeparam name='P2'>A delegate's 2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>A delegate's 3-d parameter type.</typeparam>
    /// <typeparam name='P4'>A delegate's 4-th parameter type.</typeparam>
    /// <typeparam name='P5'>A delegate's 5-th parameter type.</typeparam>
    /// <typeparam name='P6'>A delegate's 6-th parameter type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition.
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<class R,
        class P1, 
        class P2, 
        class P3, 
        class P4, 
        class P5, 
        class P6>
    struct multicast_event<R, P1, P2, P3, P4, P5, P6, void>
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R, P1, P2, P3, P4, P5, P6> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <param name='p1'>1-st event parameter.</param>
        /// <param name='p2'>2-nd event parameter.</param>
        /// <param name='p3'>3-d event parameter.</param>
        /// <param name='p4'>4-th event parameter.</param>
        /// <param name='p5'>5-th event parameter.</param>
        /// <param name='p6'>6-th event parameter.</param>
        /// <returns>false if event was canceled.</returns>
        bool operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)(p1, p2, p3, p4, p5, p6) == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)(p1, p2, p3, p4, p5, p6);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

    /// <summary>
    /// Multicast event object that accepts 5 parameters. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <typeparam name='P1'>A delegate's 1-st parameter type.</typeparam>
    /// <typeparam name='P2'>A delegate's 2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>A delegate's 3-d parameter type.</typeparam>
    /// <typeparam name='P4'>A delegate's 4-th parameter type.</typeparam>
    /// <typeparam name='P5'>A delegate's 5-th parameter type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition.
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<class R,
        class P1, 
        class P2, 
        class P3, 
        class P4, 
        class P5>
    struct multicast_event<R, P1, P2, P3, P4, P5, void, void>
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R, P1, P2, P3, P4, P5> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <param name='p1'>1-st event parameter.</param>
        /// <param name='p2'>2-nd event parameter.</param>
        /// <param name='p3'>3-d event parameter.</param>
        /// <param name='p4'>4-th event parameter.</param>
        /// <param name='p5'>5-th event parameter.</param>
        /// <returns>false if event was canceled.</returns>
        bool operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)(p1, p2, p3, p4, p5) == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)(p1, p2, p3, p4, p5);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

    /// <summary>
    /// Multicast event object that accepts 4 parameters. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <typeparam name='P1'>A delegate's 1-st parameter type.</typeparam>
    /// <typeparam name='P2'>A delegate's 2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>A delegate's 3-d parameter type.</typeparam>
    /// <typeparam name='P4'>A delegate's 4-th parameter type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition.
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<class R,
        class P1, 
        class P2, 
        class P3, 
        class P4>
    struct multicast_event<R, P1, P2, P3, P4, void, void, void>
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R, P1, P2, P3, P4> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <param name='p1'>1-st event parameter.</param>
        /// <param name='p2'>2-nd event parameter.</param>
        /// <param name='p3'>3-d event parameter.</param>
        /// <param name='p4'>4-th event parameter.</param>
        /// <returns>false if event was canceled.</returns>
        bool operator () (P1 p1, P2 p2, P3 p3, P4 p4) const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)(p1, p2, p3, p4) == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)(p1, p2, p3, p4);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

    /// <summary>
    /// Multicast event object that accepts 3 parameters. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <typeparam name='P1'>A delegate's 1-st parameter type.</typeparam>
    /// <typeparam name='P2'>A delegate's 2-nd parameter type.</typeparam>
    /// <typeparam name='P3'>A delegate's 3-d parameter type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition.
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<class R,
        class P1, 
        class P2, 
        class P3>
    struct multicast_event<R, P1, P2, P3, void, void, void, void>
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R, P1, P2, P3> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <param name='p1'>1-st event parameter.</param>
        /// <param name='p2'>2-nd event parameter.</param>
        /// <param name='p3'>3-d event parameter.</param>
        /// <returns>false if event was canceled.</returns>
        bool operator () (P1 p1, P2 p2, P3 p3) const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)(p1, p2, p3) == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)(p1, p2, p3);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

    /// <summary>
    /// Multicast event object that accepts 3 parameters. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <typeparam name='P1'>A delegate's 1-st parameter type.</typeparam>
    /// <typeparam name='P2'>A delegate's 2-nd parameter type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition.
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<class R,
        class P1, 
        class P2>
    struct multicast_event<R, P1, P2, void, void, void, void, void>
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R, P1, P2> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <param name='p1'>1-st event parameter.</param>
        /// <param name='p2'>2-nd event parameter.</param>
        /// <returns>false if event was canceled.</returns>
        /// <returns>false if event was canceled.</returns>
        bool operator () (P1 p1, P2 p2) const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)(p1, p2) == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)(p1, p2);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

    /// <summary>
    /// Multicast event object that accepts 1 parameter. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <typeparam name='P1'>A delegate's 1-st parameter type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition.
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<class R,
        class P1>
    struct multicast_event<R, P1, void, void, void, void, void, void>
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R, P1> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <param name='p1'>1-st event parameter.</param>
        /// <returns>false if event was canceled.</returns>
        bool operator () (P1 p1) const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)(p1) == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)(p1);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

    /// <summary>
    /// Multicast event object that accepts 0 parameters. Any number of unicast delegates
    /// may be added to the event invocation list.
    /// </summary>
    /// <typeparam name='R'>A delegate's return value type.</typeparam>
    /// <remarks>
    /// On firing an event the delegate execution propagates in the reverse order of addition.
    /// Multicast event is not thread safe object.
    /// &nbsp;
    /// To use this class include <i>wcp/functional.h</i>.
    /// </remarks>
    template<class R>
    struct multicast_event<R, void, void, void, void, void, void, void>
    {
        /// <summary>
        /// A delegate type.
        /// </summary>
        typedef unicast_delegate<R> delegate_t;

        /// <summary>
        /// Adds a delegate to an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to add.</param>
        /// <remarks>
        /// When an event object gets fired, all registered delegates get invoked
        /// in the order defined by internal implementation, not in the order of addition.
        /// </remarks>
        multicast_event& operator += (const delegate_t& d)
        {
            stg.add(d);
            return *this;
        }

        /// <summary>
        /// Removes a delegate from an event invocation list.
        /// </summary>
        /// <param name='d'>A delegate to remove.</param>
        multicast_event& operator -= (const delegate_t& d)
        {
            stg.remove(d);
            return *this;
        }

        /// <summary>
        /// Fires an event.
        /// </summary>
        /// <returns>false if event was canceled.</returns>
        bool operator () () const
        {
            if(stg.start_new_enum())
            {
                const delegate_t* pd;
                while(stg.next(pd))
                {
                    __if_exists(internals::event_result_check<R>::ok)
                    {
                        if((*pd)() == event_result_canceled)
                            return false;
                    }

                    __if_not_exists(internals::event_result_check<R>::ok)
                    {
                        (*pd)();
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Handles multicast event object destruction.
        /// </summary>
        /// <remarks>
        /// All delegates should be removed prior event object destruction.
        /// </remarks>
        ~multicast_event()
        {
            _ASSERTE("Multicast event is not empty. All delegates should be removed prior event destruction." && stg.empty());
        }

    private:
        
        mutable internals::unicast_delegate_storage<delegate_t> stg;
    };

} // namespace WCP_NS

#undef ___EVENT_H_CYCLE__
#endif /*___EVENT_H__*/