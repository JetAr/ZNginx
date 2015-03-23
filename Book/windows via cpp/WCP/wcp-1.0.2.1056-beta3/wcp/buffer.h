#ifdef ___BUFFER_H_CYCLE__
#error Cyclic dependency discovered: buffer.h
#endif

#ifndef __BUFFER_H__


#ifndef DOXYGEN

#define __BUFFER_H__
#define __BUFFER_H_CYCLE__

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
///      07/21/2009 - Initial release.
///      09/18/2009 - XML documentation added.
///      11/18/2009 - Added a restriction to accept only POD-types as a typed
///                   buffer type. The class renamed from wcp::tbuff to wcp::buffer.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains a typed buffer class implementation.
*/


#include <new>
#include <vector>

#include "errh.h"
#include "constraints.h"


namespace WCP_NS
{
/// <summary>
/// Implements a typed buffer.
/// </summary>
/// <typeparam name='T'>Buffer type. Must be a POD type. Default value is wcp::byte_t.
/// Note: element type is always wcp::byte_t, regardless T. Only aggregate types
/// are accepted.</typeparam>
/// <remarks>
/// Typed buffers provide a more convenient way of working with aggretate data structures
/// of various size. Such an approach simplifies memory allocation and guaranties
/// that all allocated resources will be freed on a typed buffer destruction.
/// &nbsp;
/// Refer to \ref Serialization.cpp and \ref UserAccountInfo.cpp for examples of using typed buffers.
/// </remarks>
template<class T = byte_t>
class buffer : public std::vector<byte_t>
{
    WCP_ENSURE_TYPE_CONSTRAINT(constraint<T,
                               or_<has_traits_<is_pod>,
                               has_traits_<is_integral>
                               >
                               > IS_MAINTAINED);

public:

    /// <summary>
    /// Raw data buffer type definition.
    /// </summary>
    typedef std::vector<byte_t> vector_t;

    /// <summary>
    /// Constructs an empty typed buffer.
    /// </summary>
    buffer() { }

    /// <summary>
    /// Copies an existing typed buffer.
    /// </summary>
    /// <param name='rhs'>Refenrence to a typed buffer of the same type.</param>
    buffer(const buffer& rhs)
        : vector_t(rhs) { }

    /// <summary>
    /// Copies a range of data to a typed buffer.
    /// </summary>
    /// <typeparam name='Iter'>Iterator type.</typeparam>
    /// <param name='from'>Points to the first element from a range specified.</param>
    /// <param name='to'>Points to the last element from a range specified.</param>
    template<typename Iter> buffer(Iter from, Iter to)
        : vector_t(from, to) { }

    /// <summary>
    /// Allocates a typed buffer with a number of bytes.
    /// </summary>
    /// <param name='cb'>Number of bytes to allocate.</param>
    explicit buffer(size_t cb)
    {
        alloc(cb);
    }

    /// <summary>
    /// Copies a buffer with different type to a current buffer.
    /// </summary>
    /// <typeparam name='E'>Source vector element type.</typeparam>
    /// <param name='rhs'>Reference to a data vector of the different type.</param>
    template<typename E> buffer(const std::vector<E>& rhs)
    {
        assign(rhs);
    }

    /// <summary>
    /// Copies a block of memory to a current typed buffer.
    /// </summary>
    /// <typeparam name='E'>Source buffer element type.</typeparam>
    /// <param name='p'>Data pointer.</param>
    /// <param name='c'>Number of bytes to copy.</param>
    template<typename E> buffer(const E* p, size_t c)
    {
        _ASSERTE("Invalid buffer pointer" && p != NULL);

        if(p && c)
            assign(p, c);
    }

    /// <summary>
    /// Copies a block of memory to a current typed buffer.
    /// </summary>
    /// <typeparam name='E'>Source buffer element type.</typeparam>
    /// <param name='p'>Data pointer.</param>
    /// <param name='c'>Number of bytes to copy.</param>
    /// <returns>Reference to itself.</returns>
    template<typename E> buffer& assign(const E* p, size_t c)
    {
        _ASSERTE("Invalid buffer pointer" && p != NULL);

        if(!(p && c))
            clear();
        else
        {
            size_t cb = c * sizeof(E);
            resize(cb);
            memcpy(ptr(), p, cb);
        }
        return *this;
    }

    /// <summary>
    /// Allocates a typed buffer with a number of bytes.
    /// </summary>
    /// <param name='cb'>Number of bytes to allocate.</param>
    /// <returns>Reference to itself.</returns>
    buffer& alloc(size_t cb)
    {
        resize(cb);
        return *this;
    }

    /// <summary>
    /// Allocates a typed buffer with a number of elements.
    /// </summary>
    /// <param name='ce'>Number of elements of typed buffer type to allocate.</param>
    /// <returns>Reference to itself.</returns>
    buffer& calloc(size_t ce)
    {
        return alloc(ce * sizeof(T));
    }

    /// <summary>
    /// Adds an empty space of size specified to the end of buffer.
    /// </summary>
    /// <param name='cb'>Number of bytes to add.</param>
    /// <returns>Reference to itself.</returns>
    buffer& add(size_t cb)
    {
        return alloc(size() + cb);
    }

    /// <summary>
    /// Adds a memory block of size specified to the end of buffer.
    /// </summary>
    /// <param name='ptr'>Data pointer.</param>
    /// <param name='cb'>Number of bytes to add.</param>
    /// <returns>Reference to itself.</returns>
    buffer& add(const void* ptr, size_t cb)
    {
        _ASSERTE("Invalid buffer pointer" && ptr != NULL);
        _ASSERTE("Buffer pointer points to the current buffer." \
                 && (empty() ? true :                                \
                     ((const byte_t*)ptr < &front() ||                   \
                      (const byte_t*)ptr >= (&front() + size()))));

        if(empty() ? false :
                ((const byte_t*)ptr >= &front() &&
                 (const byte_t*)ptr < (&front() + size())))
            throw invalid_argument_exception(TEXT("ptr"));

        if(ptr && cb)
            insert(end(), (const byte_t*)ptr, (const byte_t*)ptr + cb);

        return *this;
    }

    /// <summary>
    /// Adds a buffer of different type to the end of buffer.
    /// </summary>
    /// <param name='v'>Data vector to add of the same data type as type buffer's data.</param>
    /// <returns>Reference to itself.</returns>
    buffer& add(const vector_t& v)
    {
        _ASSERTE("Cannot add itself" && &v != this);

        if(&v == this)
            throw invalid_argument_exception(TEXT("v"));

        return add(&v.front(), v.size());
    }

    /// <summary>
    /// Zeroes a typed buffer.
    /// </summary>
    void zerobuff()
    {
        memset(&front(), 0, size());
    }

    /// <summary>
    /// Returns a pointer to a contained data.
    /// </summary>
    /// <returns>A pointer to a contained data.</returns>
    T* operator -> ()
    {
        return ptr();
    }

    /// <summary>
    /// Returns a pointer to a readonly contained data.
    /// </summary>
    /// <returns>A pointer to a readonly contained data.</returns>
    const T* operator -> () const
    {
        return ptr();
    }

    /// <summary>
    /// Returns a pointer to a contained data.
    /// </summary>
    /// <returns>A pointer to a contained data.</returns>
    operator T* ()
    {
        return ptr();
    }

    /// <summary>
    /// Returns a pointer to a readonly contained data.
    /// </summary>
    /// <returns>A pointer to a readonly contained data.</returns>
    operator const T* () const
    {
        return ptr();
    }

    /// <summary>
    /// Returns a pointer to a contained data.
    /// </summary>
    /// <returns>A pointer to a contained data.</returns>
    T* ptr()
    {
        _ASSERTE("Buffer is empty" && !empty());
        return reinterpret_cast<T*>(&front());
    }

    /// <summary>
    /// Returns a pointer to a readonly contained data.
    /// </summary>
    /// <returns>A pointer to a readonly contained data.</returns>
    const T* ptr() const
    {
        _ASSERTE("Buffer is empty" && !empty());
        return reinterpret_cast<const T*>(&front());
    }

    /// <summary>
    /// Returns a %reference to a contained data.
    /// </summary>
    /// <returns>A %reference to a contained data.</returns>
    T& ref()
    {
        return *ptr();
    }

    /// <summary>
    /// Returns a %reference to a readonly contained data.
    /// </summary>
    /// <returns>A %reference to a readonly contained data.</returns>
    const T& ref() const
    {
        return *ptr();
    }

    /// <summary>
    /// Returns a %reference to a contained data.
    /// </summary>
    /// <returns>A %reference to a contained data.</returns>
    T& operator * ()
    {
        return ref();
    }

    /// <summary>
    /// Returns a %reference to a readonly contained data.
    /// </summary>
    /// <returns>A %reference to a readonly contained data.</returns>
    const T& operator * () const
    {
        return ref();
    }

    /// <summary>
    /// Returns a %reference to a contained data.
    /// </summary>
    /// <returns>A %reference to a contained data.</returns>
    operator T& ()
    {
        return ref();
    }

    /// <summary>
    /// Returns a %reference to a readonly contained data.
    /// </summary>
    /// <returns>A %reference to a readonly contained data.</returns>
    operator const T& () const
    {
        return ref();
    }

    /// <summary>
    /// Returns a pointer of type specified to a contained data.
    /// </summary>
    /// <typeparam name='P'>Pointer type to convert to.</typeparam>
    /// <returns>A pointer of type specified to a contained data.</returns>
    template<typename P> P* ptrT()
    {
        return reinterpret_cast<P*>(ptr());
    }

    /// <summary>
    /// Returns a pointer of type specified to a readonly contained data.
    /// </summary>
    /// <typeparam name='P'>Pointer type to convert to.</typeparam>
    /// <returns>A pointer of type specified to a readonly contained data.</returns>
    template<typename P> const P* ptrT() const
    {
        return return reinterpret_cast<const P*>(ptr());
    }

    /// <summary>
    /// Returns a %reference of type specified to a contained data.
    /// </summary>
    /// <typeparam name='P'>Reference type to convert to.</typeparam>
    /// <returns>A %reference of type specified to a contained data.</returns>
    template<typename P> P& refT()
    {
        return *reinterpret_cast<P*>(ptr());
    }

    /// <summary>
    /// Returns a %reference of type specified to a readonly contained data.
    /// </summary>
    /// <typeparam name='P'>Reference type to convert to.</typeparam>
    /// <returns>A %reference of type specified to a readonly contained data.</returns>
    template<typename P> const P& refT() const
    {
        return *reinterpret_cast<const P*>(ptr());
    }
};

} // namespace WCP_NS


#undef __BUFFER_H_CYCLE__
#endif /*__BUFFER_H__*/
