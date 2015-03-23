#ifdef ___SMARTPTR_H_CYCLE__
#error Cyclic dependency discovered: smartptr.h
#endif

#ifndef __SMARTPTR_H__


#ifndef DOXYGEN

#define __SMARTPTR_H__
#define __SMARTPTR_H_CYCLE__

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
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains thread unsafe and thread safe shared pointer implementations.
    Shared pointer interface is TR1 compatible. A resource cannot be shared
    between thread safe and thread unsafe shared pointers.
*/


#include <new>
#include <memory>

#include "_so_monitor.h"
#include "errh.h"



namespace WCP_NS
{

#ifndef DOXYGEN

namespace internals
{
// Describes a pointer storage interface.
struct ptr_storage
{
    // Returns a raw data pointer.
    virtual void* get() = 0;

    // Increments a pointer storage %reference counter.
    virtual long inc_stg_ref() throw() = 0;

    // Decrements a pointer storage %reference counter.
    virtual long dec_stg_ref() throw() = 0;

    // Increments a data and pointer storage %reference counters.
    virtual long inc_ptr_ref() throw() = 0;

    // Decrements a data and pointer storage %reference counters.
    virtual long dec_ptr_ref() throw() = 0;

    // Returns a data %reference counter value.
    virtual long ptr_ref_count() throw() = 0;
};

//------------------------------------------------------------------------------

// Default deleter.
template<class T>
struct default_stg_deleter
{
    // Deletes a object pointed by p.
    void operator () (T* p)
    {
        delete p;
    }
};

//------------------------------------------------------------------------------

// Thread unsafe pointer storage.
template<class T, class D = default_stg_deleter<T> >
class st_ptr_storage : public ptr_storage
{
    // Data deleter. Deleter MUST not throw any exception.
    D deleter;

    // Pointer storage reference counter.
    long stg_refcnt;

    // Data reference counter.
    long ptr_refcnt;

    // Data pointer.
    T* ptr;

public:

    // Template provider.
    template<class T_ = T, class D_ = D>
    struct template_provider
    {
        typedef st_ptr_storage<T_, D_> type;
    };

    // Constructs a new thread unsafe pointer storage.
    st_ptr_storage(T* ptr)
        : stg_refcnt(1)
        , ptr_refcnt(1)
        , ptr(ptr)
    { }

    // Constructs a new thread unsafe pointer storage with a custom deleter specified.
    st_ptr_storage(T* ptr, D deleter)
        : stg_refcnt(1)
        , ptr_refcnt(1)
        , deleter(deleter)
        , ptr(ptr)
    { }

    //
    // internals::ptr_storage interface implementation
    //

    // Returns a raw data pointer.
    void* get()
    {
        return ptr;
    }

    // Increments a pointer storage reference counter.
    long inc_stg_ref() throw()
    {
        return ++stg_refcnt;
    }

    // Decrements a pointer storage reference counter.
    long dec_stg_ref() throw()
    {
        if(!--stg_refcnt)
        {
            delete this;
            return 0;
        }

        return stg_refcnt;
    }

    // Increments a data and pointer storage reference counters.
    long inc_ptr_ref() throw()
    {
        ++ptr_refcnt;
        return inc_stg_ref();
    }

    // Decrements a data and pointer storage reference counters.
    long dec_ptr_ref() throw()
    {
        if(!--ptr_refcnt)
        {
            // Deleter MUST not throw any exception.
            NONTHROWABLE(deleter(ptr));
            ptr = 0;
        }

        return dec_stg_ref();
    }

    // Returns a data reference counter value.
    long ptr_ref_count()
    {
        return ptr_refcnt;
    }
};

//------------------------------------------------------------------------------

// Thread safe pointer storage.
template<class T, class D = default_stg_deleter<T> >
class mt_ptr_storage : public ptr_storage
{
    // Data deleter. Deleter MUST not throw any exception.
    D deleter;

    // Pointer storage reference counter.
    long stg_refcnt;

    // Data reference counter.
    long ptr_refcnt;

    // Data pointer.
    T* ptr;

    // Synchronizes storage operations.
    monitor lock;

public:

    // Template provider.
    template<class T_ = T, class D_ = D>
    struct template_provider
    {
        typedef mt_ptr_storage<T_, D_> type;
    };

    // Constructs a new thread safe pointer storage.
    mt_ptr_storage(T* ptr)
        : stg_refcnt(0)
        , ptr_refcnt(0)
        , ptr(ptr)
    { }

    // Constructs a new thread safe pointer storage with a custom deleter specified.
    mt_ptr_storage(T* ptr, D deleter)
        : stg_refcnt(0)
        , ptr_refcnt(0)
        , deleter(deleter)
        , ptr(ptr)
    { }

    //
    // internals::ptr_storage interface implementation
    //

    // Returns a raw data pointer.
    void* get()
    {
        return ptr;
    }

    // Increments a pointer storage reference counter.
    long inc_stg_ref() throw()
    {
        return InterlockedIncrement(&stg_refcnt);
    }

    // Decrements a pointer storage reference counter.
    long dec_stg_ref() throw()
    {
        // Decrement storage reference counter.
        long c = InterlockedDecrement(&stg_refcnt);
        if(c == 0)
        {
            // Delete pointer storate.
            delete this;
            return 0;
        }

        return c;
    }

    // Increments a data and pointer storage reference counters.
    long inc_ptr_ref() throw()
    {
        long c = inc_stg_ref();

        lock.seize();

        // Increment data pointer reference counter
        // if data pointer isn't zero.
        if(ptr)
            InterlockedIncrement(&ptr_refcnt);

        lock.release();

        return c;
    }

    // Decrements a data and pointer storage reference counters.
    long dec_ptr_ref() throw()
    {
        lock.seize();

        // Decrement data pointer reference counter
        // if data pointer isn't zero.
        if(ptr)
        {
            long c = InterlockedDecrement(&ptr_refcnt);

            // If no data pointer reference, zero data pointer
            // and call deleter for an old data pointer.
            if(c == 0)
            {
                T* p = (T*)InterlockedExchangePointer(&ptr, 0);

                // Deleter MUST not throw any exception.
                NONTHROWABLE(deleter(p));
            }

        }

        lock.release();

        return dec_stg_ref();
    }

    // Returns a data reference counter value.
    long ptr_ref_count()
    {
        return ptr_refcnt;
    }
};

} // namespace internals

#endif /*DOXYGEN*/

//------------------------------------------------------------------------------

/// <summary>
/// Thread unsafe shared pointer template. Holds %reference to a
/// shared resource and a pointer storage.
/// </summary>
/// <typeparam name='T'>Data type.</typeparam>
template<class T>
class shared_ptr
{
    template<class O> friend class weak_ptr;
    template<class O> friend class shared_ptr;

    internals::ptr_storage* pstg;

public:

    /// <summary>
    /// Data type.
    /// </summary>
    typedef T element_type;

    /// <summary>
    /// Constructs empty shared pointer object.
    /// </summary>
    shared_ptr()
        : pstg(0)
    { }

    /// <summary>
    /// Constructs from data pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    template<class O>
    explicit shared_ptr(O* ptr)
        : pstg(0)
    {
        reset(ptr);
    }

    /// <summary>
    /// Constructs from data pointer and deleter specified.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <typeparam name='D'>Data deleter.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    /// <param name='deleter'>Data deleter.</param>
    /// <remarks>Deleter is a functor that receives a data pointer as a single parameter.
    /// Deleter MUST not throw any %exception.</remarks>
    template<class O, class D>
    shared_ptr(O* ptr, D deleter)
        : pstg(0)
    {
        reset(ptr, deleter);
    }

    /// <summary>
    /// Constructs a shared pointer that owns the same resource as rhs.
    /// </summary>
    /// <param name='rhs'>Shared pointer to construct from.</param>
    shared_ptr(const shared_ptr& rhs)
        : pstg(0)
    {
        set_storage(rhs.pstg, true);
    }

    /// <summary>
    /// Constructs a shared pointer that owns the same resource as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared pointer to construct from.</param>
    template<class O>
    shared_ptr(const shared_ptr<O>& rhs)
        : pstg(0)
    {
        set_storage(rhs.pstg, true);
    }

    /// <summary>
    /// Constructs a shared pointer that owns the same resource as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to construct from.</param>
    template<class O>
    shared_ptr(const weak_ptr<O>& rhs)
        : pstg(0)
    {
        set_storage(rhs.pstg, true);
    }

    /// <summary>
    /// Constructs from a data pointer specified. Releases an auto pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Auto pointer to sontruct from (will be released).</param>
    template<class O>
    shared_ptr(std::auto_ptr<O>& ptr)
        : pstg(0)
    {
        reset(ptr.release());
    }

    /// <summary>
    /// Handles shared pointer destruction.
    /// </summary>
    ~shared_ptr()
    {
        reset();
    }

    /// <summary>
    /// Assigns shared ownership of resource owned by rhs.
    /// </summary>
    /// <param name='rhs'>Shared pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    shared_ptr& operator=(const shared_ptr& rhs)
    {
        set_storage(rhs.pstg, true);
        return *this;
    }

    /// <summary>
    /// Assigns shared ownership of resource owned by rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared poitner to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    shared_ptr& operator=(const shared_ptr<O>& rhs)
    {
        set_storage(rhs.pstg, true);
        return *this;
    }

    /// <summary>
    /// Assigns shared ownership of resource owned by rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    shared_ptr& operator=(const weak_ptr<O>& rhs)
    {
        set_storage(rhs.pstg, true);
        return *this;
    }

    /// <summary>
    /// Assigns data pointer. Releases an auto pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Auto pointer to assign to (will be released).</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    shared_ptr& operator=(std::auto_ptr<O>& ptr)
    {
        reset(ptr.release());
        return *this;
    }

    /// <summary>
    /// Swaps with another shared pointer.
    /// </summary>
    /// <param name='rhs'>Shared pointer to swap pointer value with.</param>
    void swap(shared_ptr& rhs)
    {
        internals::ptr_storage* tmp = pstg;
        pstg = rhs.pstg;
        rhs.pstg = tmp;
    }

    /// <summary>
    /// Releases owned resource.
    /// </summary>
    void reset()
    {
        set_storage(0, false);
    }

    /// <summary>
    /// Releases owned resource and takes ownership of ptr.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    template<class O>
    void reset(O* ptr)
    {
        set_storage(new internals::st_ptr_storage<O>(ptr), false);
    }

    /// <summary>
    /// Releases owned resource and takes ownership of
    /// ptr and holds a deleter specified.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <typeparam name='D'>Data deleter.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    /// <param name='deleter'>Data deleter.</param>
    /// <remarks>Deleter is a functor that receives a data pointer as a single parameter.
    /// Deleter MUST not throw any %exception.</remarks>
    template<class O, class D>
    void reset(O* ptr, D deleter)
    {
        set_storage(new internals::st_ptr_storage<O, D>(ptr, deleter), false);
    }

    /// <summary>
    /// Returns an owned data pointer.
    /// </summary>
    /// <returns>Owned data pointer or 0 if empty.</returns>
    T *get() const
    {
        return pstg ? reinterpret_cast<T*>(pstg->get()) : 0;
    }

    /// <summary>
    /// Returns a %reference to owned data.
    /// </summary>
    /// <returns>Owned data %reference.</returns>
    T& operator*() const
    {
        return *get();
    }

    /// <summary>
    /// Returns an owned data pointer.
    /// </summary>
    /// <returns>Owned data pointer or 0 if empty.</returns>
    T *operator->() const
    {
        return get();
    }

    /// <summary>
    /// Returns a data %reference counter value.
    /// </summary>
    long use_count() const
    {
        return pstg ? pstg->ptr_ref_count() : 0;
    }

    /// <summary>
    /// Checks if a shared pointer is a single owner of a resource.
    /// </summary>
    /// <returns>true if a resource is owned only by this shared pointer instance.
    bool unique() const
    {
        return use_count() < 2;
    }

    /// <summary>
    /// Checks if a resource is owned.
    /// </summary>
    /// <returns>true if a data is owned.</returns>
    operator bool () const
    {
        return pstg != 0;
    }

private:

    void set_storage(internals::ptr_storage* p, bool addref)
    {
        if(p != pstg)
        {
            if(pstg)
                pstg->dec_ptr_ref();

            if(p && p->ptr_ref_count())
            {
                pstg = p;
                if(addref)
                    p->inc_ptr_ref();
            }
            else
                pstg = 0;
        }
    }
};

//------------------------------------------------------------------------------

/// <summary>
/// Thread safe shared pointer template. Holds %reference to a
/// shared resource and a pointer storage.
/// </summary>
/// <typeparam name='T'>Data type.</typeparam>
template<class T>
class safe_shared_ptr
{
    template<class O> friend class safe_weak_ptr;
    template<class O> friend class safe_shared_ptr;

    monitor access;
    mutable internals::ptr_storage* pstg;

public:

    /// <summary>
    /// Data type.
    /// </summary>
    typedef T element_type;

    /// <summary>
    /// Constructs empty shared pointer object.
    /// </summary>
    safe_shared_ptr()
        : pstg(0)
    { }

    /// <summary>
    /// Constructs from data pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    template<class O>
    explicit safe_shared_ptr(O* ptr)
        : pstg(0)
    {
        reset(ptr);
    }

    /// <summary>
    /// Constructs from data pointer and deleter specified.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <typeparam name='D'>Data deleter.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    /// <param name='deleter'>Data deleter.</param>
    /// <remarks>Deleter is a functor that receives a data pointer as a single parameter.
    /// Deleter MUST not throw any %exception.</remarks>
    template<class O, class D>
    safe_shared_ptr(O* ptr, D deleter)
        : pstg(0)
    {
        reset(ptr, deleter);
    }

    /// <summary>
    /// Constructs a shared pointer that owns the same resource as rhs.
    /// </summary>
    /// <param name='rhs'>Shared pointer to construct from.</param>
    safe_shared_ptr(const safe_shared_ptr& rhs)
        : pstg(0)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
    }

    /// <summary>
    /// Constructs a shared pointer that owns the same resource as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared pointer to construct from.</param>
    template<class O>
    safe_shared_ptr(const safe_shared_ptr<O>& rhs)
        : pstg(0)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
    }

    /// <summary>
    /// Constructs a shared pointer that owns the same resource as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to construct from.</param>
    template<class O>
    safe_shared_ptr(const safe_weak_ptr<O>& rhs)
        : pstg(0)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
    }

    /// <summary>
    /// Constructs from a data pointer specified. Releases an auto pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Auto pointer to sontruct from (will be released).</param>
    template<class O>
    safe_shared_ptr(std::auto_ptr<O>& ptr)
        : pstg(0)
    {
        reset(ptr.release());
    }

    /// <summary>
    /// Handles shared pointer destruction.
    /// </summary>
    ~safe_shared_ptr()
    {
        reset();
    }

    /// <summary>
    /// Assigns shared ownership of resource owned by rhs.
    /// </summary>
    /// <param name='rhs'>Shared pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    safe_shared_ptr& operator=(const safe_shared_ptr& rhs)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
        return *this;
    }

    /// <summary>
    /// Assigns shared ownership of resource owned by rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared poitner to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    safe_shared_ptr& operator=(const safe_shared_ptr<O>& rhs)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
        return *this;
    }

    /// <summary>
    /// Assigns shared ownership of resource owned by rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    safe_shared_ptr& operator=(const safe_weak_ptr<O>& rhs)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
        return *this;
    }

    /// <summary>
    /// Assigns data pointer. Releases an auto pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Auto pointer to assign to (will be released).</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    safe_shared_ptr& operator=(std::auto_ptr<O>& ptr)
    {
        reset(ptr.release());
        return *this;
    }

    /// <summary>
    /// Swaps with another shared pointer.
    /// </summary>
    /// <param name='rhs'>Shared pointer to swap pointer value with.</param>
    void swap(safe_shared_ptr& rhs)
    {
        rhs.access.seize();
        access.seize();

        internals::ptr_storage* p = pstg;
        pstg = rhs.pstg;
        rhs.pstg = p;

        access.release();
        rhs.access.release();
    }

    /// <summary>
    /// Releases owned resource.
    /// </summary>
    void reset()
    {
        set_storage(0);
    }

    /// <summary>
    /// Releases owned resource and takes ownership of ptr.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    template<class O>
    void reset(O* ptr)
    {
        internals::ptr_storage* volatile ps =
            new internals::mt_ptr_storage<O>(ptr);
        set_storage(ps);
    }

    /// <summary>
    /// Releases owned resource and takes ownership of
    /// ptr and holds a deleter specified.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <typeparam name='D'>Data deleter.</typeparam>
    /// <param name='ptr'>Data pointer.</param>
    /// <param name='deleter'>Data deleter.</param>
    /// <remarks>Deleter is a functor that receives a data pointer as a single parameter.
    /// Deleter MUST not throw any %exception.</remarks>
    template<class O, class D>
    void reset(O* ptr, D deleter)
    {
        internals::ptr_storage* volatile ps =
            new internals::mt_ptr_storage<O, D>(ptr, deleter);
        set_storage(ps);
    }

    /// <summary>
    /// Returns an owned data pointer.
    /// </summary>
    /// <returns>Owned data pointer or 0 if empty.</returns>
    T *get() const
    {
        access.seize();
        T* ptr = pstg ? reinterpret_cast<T*>(pstg->get()) : 0;
        access.release();
        return ptr;
    }

    /// <summary>
    /// Returns a %reference to owned data.
    /// </summary>
    /// <returns>Owned data %reference.</returns>
    T& operator*() const
    {
        return *get();
    }

    /// <summary>
    /// Returns an owned data pointer.
    /// </summary>
    /// <returns>Owned data pointer or 0 if empty.</returns>
    T *operator->() const
    {
        return get();
    }

    /// <summary>
    /// Returns a data %reference counter value.
    /// </summary>
    long use_count() const
    {
        access.seize();
        long n = pstg ? pstg->ptr_ref_count() : 0;
        access.release();
        return n;
    }

    /// <summary>
    /// Checks if a shared pointer is a single owner of a resource.
    /// </summary>
    /// <returns>true if a resource is owned only by this shared pointer instance.</returns>
    bool unique() const
    {
        return use_count() == 1;
    }

    /// <summary>
    /// Checks if a resource is owned.
    /// </summary>
    /// <returns>true if a data is owned.</returns>
    operator bool () const
    {
        access.seize();
        bool f = pstg != 0;
        access.release();
        return f;
    }

private:

    void set_storage(internals::ptr_storage* p)
    {
        access.seize();

        if(p != pstg)
        {
            internals::ptr_storage* pold = pstg;

            if(p)
            {
                (pstg = p)->inc_ptr_ref();
                if(pstg->ptr_ref_count() == 0)
                {
                    pstg->dec_ptr_ref();
                    pstg = 0;
                }
            }
            else
                pstg = 0;

            if(pold)
                pold->dec_ptr_ref();
        }

        access.release();
    }
};

//------------------------------------------------------------------------------

/// <summary>
/// Thread unsafe weak pointer template. Holds %references to a pointer storage only.
/// </summary>
/// <typeparam name='T'>Data type.</typeparam>
template<class T>
class weak_ptr
{
    template<class O> friend class shared_ptr;
    template<class O> friend class weak_ptr;

    internals::ptr_storage* pstg;

public:

    /// <summary>
    /// Data type.
    /// </summary>
    typedef T element_type;

    /// <summary>
    /// Constructs empty pointer.
    /// </summary>
    weak_ptr()
        : pstg(0)
    { }

    /// <summary>
    /// Constructs from another weak pointer.
    /// </summary>
    /// <param name='rhs'>Weak pointer to contruct from.</param>
    weak_ptr(const weak_ptr& rhs)
        : pstg(0)
    {
        set_storage(rhs.pstg, true);
    }

    /// <summary>
    /// Constructs a weak pointer that owns the same pointer storage as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to contruct from.</param>
    template<class O>
    weak_ptr(const weak_ptr<O>& rhs)
        : pstg(0)
    {
        set_storage(rhs.pstg, true);
    }

    /// <summary>
    /// Constructs a weak pointer that owns the same pointer storage as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared pointer to construct from.</param>
    template<class O>
    weak_ptr(const shared_ptr<O>& rhs)
        : pstg(0)
    {
        set_storage(rhs.pstg, true);
    }

    /// <summary>
    /// Handles weak pointer destruction.
    /// </summary>
    ~weak_ptr()
    {
        reset();
    }

    /// <summary>
    /// Assigns to another weak pointer.
    /// </summary>
    /// <param name='rhs'>Weak pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    weak_ptr& operator=(const weak_ptr& rhs)
    {
        set_storage(rhs.pstg, true);
        return *this;
    }

    /// <summary>
    /// Assigns to another weak pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    weak_ptr& operator=(const weak_ptr<O>& rhs)
    {
        set_storage(rhs.pstg, true);
        return *this;
    }

    /// <summary>
    /// Assigns to a shared pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    weak_ptr& operator=(const shared_ptr<O>& rhs)
    {
        set_storage(rhs.pstg, true);
        return *this;
    }

    /// <summary>
    /// Swaps with another weak pointer.
    /// </summary>
    /// <param name='rhs'>Weak pointer to swap with.</param>
    void swap(weak_ptr& rhs)
    {
        internals::ptr_storage* tmp = pstg;
        pstg = rhs.pstg;
        rhs.pstg = tmp;
    }

    /// <summary>
    /// Releases a pointer storage ownership.
    /// </summary>
    void reset()
    {
        set_storage(0, false);
    }

    /// <summary>
    /// Returns data pointer %reference counter value.
    /// </summary>
    /// <returns>Data pointer %reference counter value.</returns>
    long use_count() const
    {
        return pstg ? pstg->ptr_ref_count() : 0;
    }

    /// <summary>
    /// Checks if data pointer is available.
    /// </summary>
    /// <returns>true if no data pointer is not valid.</returns>
    bool expired() const
    {
        return use_count() == 0;
    }

    /// <summary>
    /// Locks a data pointer held by owned pointer storage.
    /// </summary>
    /// <returns>Shared pointer the holds the same resource that the weak pointer.</returns>
    shared_ptr<T> lock() const
    {
        return shared_ptr<T>(*this);
    }

private:

    void set_storage(internals::ptr_storage* p, bool addref)
    {
        if(p != pstg)
        {
            if(pstg)
                pstg->dec_stg_ref();

            if(p && p->ptr_ref_count())
            {
                pstg = p;
                if(addref)
                    p->inc_stg_ref();
            }
            else
                pstg = 0;
        }
    }
};

//------------------------------------------------------------------------------

/// <summary>
/// Thread safe weak pointer template. Holds %references to a pointer storage only.
/// </summary>
/// <typeparam name='T'>Data type.</typeparam>
template<class T>
class safe_weak_ptr
{
    template<class O> friend class safe_shared_ptr;
    template<class O> friend class safe_weak_ptr;

    monitor access;
    mutable internals::ptr_storage* pstg;

public:

    /// <summary>
    /// Data type.
    /// </summary>
    typedef T element_type;

    /// <summary>
    /// Constructs empty pointer.
    /// </summary>
    safe_weak_ptr()
        : pstg(0)
    { }

    /// <summary>
    /// Constructs from another weak pointer
    /// </summary>
    /// <param name='rhs'>Weak pointer to contruct from.</param>
    safe_weak_ptr(const safe_weak_ptr& rhs)
        : pstg(0)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
    }

    /// <summary>
    /// Constructs a weak pointer that owns the same pointer storage as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to contruct from.</param>
    template<class O>
    safe_weak_ptr(const safe_weak_ptr<O>& rhs)
        : pstg(0)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
    }

    /// <summary>
    /// Constructs a weak pointer that owns the same pointer storage as rhs.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared pointer to construct from.</param>
    template<class O>
    safe_weak_ptr(const safe_shared_ptr<O>& rhs)
        : pstg(0)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
    }

    /// <summary>
    /// Handles weak pointer destruction.
    /// </summary>
    ~safe_weak_ptr()
    {
        reset();
    }

    /// <summary>
    /// Assigns to another weak pointer.
    /// </summary>
    /// <param name='rhs'>Weak pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    safe_weak_ptr& operator=(const safe_weak_ptr& rhs)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
        return *this;
    }

    /// <summary>
    /// Assigns to another weak pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Weak pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    safe_weak_ptr& operator=(const safe_weak_ptr<O>& rhs)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
        return *this;
    }

    /// <summary>
    /// Assigns to a shared pointer.
    /// </summary>
    /// <typeparam name='O'>Data type.</typeparam>
    /// <param name='rhs'>Shared pointer to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template<class O>
    safe_weak_ptr& operator=(const safe_shared_ptr<O>& rhs)
    {
        rhs.access.seize();
        set_storage(rhs.pstg);
        rhs.access.release();
        return *this;
    }

    /// <summary>
    /// Swaps with another weak pointer.
    /// </summary>
    /// <param name='rhs'>Weak pointer to swap with.</param>
    void swap(safe_weak_ptr& rhs)
    {
        rhs.access.seize();
        access.seize();

        internals::ptr_storage* p = pstg;
        pstg = rhs.pstg;
        rhs.pstg = p;

        access.release();
        rhs.access.release();
    }

    /// <summary>
    /// Releases a pointer storage ownership.
    /// </summary>
    void reset()
    {
        set_storage(0);
    }

    /// <summary>
    /// Returns data pointer %reference counter value.
    /// </summary>
    /// <returns>Data pointer %reference counter value.</returns>
    long use_count() const
    {
        access.seize();
        long n = pstg ? pstg->ptr_ref_count() : 0;
        access.release();
        return n;
    }

    /// <summary>
    /// Checks if data pointer is available.
    /// </summary>
    /// <returns>true if no data pointer is not valid.</returns>
    bool expired() const
    {
        return use_count() <= 0;
    }

    /// <summary>
    /// Locks a data pointer held by owned pointer storage.
    /// </summary>
    /// <returns>Shared pointer the holds the same resource that the weak pointer.</returns>
    safe_shared_ptr<T> lock() const
    {
        return safe_shared_ptr<T>(*this);
    }

private:

    void set_storage(internals::ptr_storage* p)
    {
        access.seize();

        if(p != pstg)
        {
            internals::ptr_storage* pold = pstg;

            if(p)
                (pstg = p)->inc_stg_ref();
            else
                pstg = 0;

            if(pold)
                pold->dec_stg_ref();
        }

        access.release();
    }
};

//------------------------------------------------------------------------------

/// <summary>
/// Shared pointer template provider.
/// </summary>
struct shared_ptr_template_provider
{
    /// <summary>
    /// Template type.
    /// </summary>
    /// <typeparam name='T'>Shared pointer type.</typeparam>
    template<class T> struct template_provider
    {
        typedef shared_ptr<T> type;
    };
};

/// <summary>
/// Safe shared pointer template provider.
/// </summary>
struct safe_shared_ptr_template_provider
{
    /// <summary>
    /// Template type.
    /// </summary>
    /// <typeparam name='T'>Safe shared pointer type.</typeparam>
    template<class T> struct template_provider
    {
        typedef safe_shared_ptr<T> type;
    };
};

} // namepsace WCP_NS


#undef __SMARTPTR_H_CYCLE__
#endif /*__SMARTPTR_H__*/
