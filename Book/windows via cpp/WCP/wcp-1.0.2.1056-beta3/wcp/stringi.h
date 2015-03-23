#ifdef ___STRINGI_H_CYCLE__
#error Cyclic dependency discovered: stringi.h
#endif

#ifndef __STRINGI_H__


#ifndef DOXYGEN

#define __STRINGI_H__
#define __STRINGI_H_CYCLE__

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
///      11/06/2009 - Default char traits and allocator added to basic_stringi class template.
///                   The file and the class were renamed to stringi, because istring
///                   conflicts with notion of prefix 'i' in STL classes, where it means
///                   'input', e.g.: basic_ifstream, basic_istringstream etc.
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains an implementation of basic_stringi class. The basic_stringi
    interface is compatible with a standard basic_string and provides a
    convenient way of managing case insensitive strings. Also this headers
    contains implementation of overloaded global functions and operators
    that are supposed to work with basic_stringi the same way they work
    with basic_string.
*/


#include <string>
#include <istream>
#include <ostream>
#include <locale>
#include <stdexcept>
#include <functional>
#include <hash_map>



namespace std
{

/// <summary>Represents a standard C++ std::basic_string compatible template
/// for case insensitive string.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.
/// The Standard C++ Library provides two specializations of this template class, with the type
/// definitions string, for elements of type char, and wstring, for elements of type wchar_t.</typeparam>
/// <typeparam name='Traits'>Various important properties of the CharT elements
/// in a basic_string specialization are described by the class Traits.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object
/// that encapsulates details about the string's allocation and deallocation of memory.
/// The default value is allocator&lt;Type&gt;.</typeparam>
template<class CharT, class Traits = char_traits<CharT>, class Alloc = allocator<CharT> >
class basic_stringi
{
public:

    /// <summary>
    /// Standard string type.
    /// </summary>
    typedef basic_string<CharT, Traits, Alloc> stdstring_t;

private:

    stdstring_t m_str;
    locale m_loc;

public:


    /// <summary>self_t type definition.</summary>
    typedef basic_stringi<CharT, Traits, Alloc> self_t;

    //
    // Typedefs
    //

    /// <summary>The type of object, CharT, stored in the string.  </summary>
    typedef typename stdstring_t::value_type value_type;

    /// <summary>Container  Pointer to CharT.</summary>
    typedef typename stdstring_t::pointer pointer;

    /// <summary>Reference to CharT.</summary>
    typedef typename stdstring_t::reference reference;

    /// <summary>Const %reference to CharT.</summary>
    typedef typename stdstring_t::const_reference const_reference;

    /// <summary>An unsigned integral type.</summary>
    typedef typename stdstring_t::size_type size_type;

    /// <summary>A signed integral type.</summary>
    typedef typename stdstring_t::difference_type difference_type;

    /// <summary>The largest possible value of type size_type. That is, size_type(-1).</summary>
    static const size_type npos = size_type(-1);

    /// <summary>Iterator used to iterate through a string.
    /// A basic_string supplies Random Access Iterators.</summary>
    typedef typename stdstring_t::iterator iterator;

    /// <summary>Const iterator used to iterate through a string.</summary>
    typedef typename stdstring_t::const_iterator const_iterator;

    /// <summary>Iterator used to iterate backwards through a string.</summary>
    typedef typename stdstring_t::reverse_iterator reverse_iterator;

    /// <summary>Const iterator used to iterate backwards through a string.</summary>
    typedef typename stdstring_t::const_reverse_iterator const_reverse_iterator;

    //
    // Nonstandard basic_stringi only contructors
    //


    /// <summary>Creates an empty string with a locale specified.</summary>
    /// <param name='loc'>String locale.</param>
    explicit basic_stringi<CharT, Traits, Alloc>(const locale& loc) : m_loc(loc) { }

    /// <summary>Generalization of the copy constructor with a locale specified.</summary>
    /// <param name='loc'>String locale.</param>
    /// <param name='s'>String to copy.</param>
    /// <param name='pos'>Position to copy from.</param>
    /// <param name='n'>Number of characters to copy.</param>
    basic_stringi<CharT, Traits, Alloc>(const locale& loc, const self_t& s,
                                        size_type pos = 0, size_type n = npos)
        : m_str(s.m_str, pos, n), m_loc(loc) { }

    /// <summary>Construct a string from a null-terminated character array with a locale specified.</summary>
    /// <param name='loc'>String locale.</param>
    /// <param name='pstr'>String to construct from.</param>
    basic_stringi<CharT, Traits, Alloc>(const locale& loc, const CharT* pstr)
        : m_str(pstr), m_loc(loc) { }

    /// <summary>Construct a string from a character array and a length with a locale specified.</summary>
    /// <param name='loc'>String locale.</param>
    /// <param name='s'>String to construct from.</param>
    /// <param name='n'>Number of characters to copy from a source string.</param>
    basic_stringi<CharT, Traits, Alloc>(const locale& loc, const CharT* s, size_type n)
        : m_str(s, n), m_loc(loc) { }

    /// <summary>Create a string with n copies of c with a locale specified.</summary>
    /// <param name='loc'>String locale.</param>
    /// <param name='n'>Number of chars to allocate.</param>
    /// <param name='c'>Character to build a string from.</param>
    basic_stringi<CharT, Traits, Alloc>(const locale& loc, size_type n, CharT c)
        : m_str(n, c), m_loc(loc) { }

    /// <summary>Create a string from a range with a locale specified.</summary>
    /// <typeparam name='InputIterator'>Iterator type.</typeparam>
    /// <param name='loc'>String locale.</param>
    /// <param name='first'>Points to the first element in a range.</param>
    /// <param name='last'>Points to the last element in a range.</param>
    template <class InputIterator>
    basic_stringi<CharT, Traits, Alloc>(const locale& loc,
                                        InputIterator first, InputIterator last)
        : m_str(first, last), m_loc(loc) { }

    //
    // Constructors
    //

    /// <summary>Creates an empty string.</summary>
    basic_stringi<CharT, Traits, Alloc>() { }

    /// <summary>Generalization of the copy constructor.</summary>
    /// <param name='s'>String to copy.</param>
    /// <param name='pos'>Position to copy from.</param>
    /// <param name='n'>Number of characters to copy.</param>
    basic_stringi<CharT, Traits, Alloc>(const self_t& s,
                                        size_type pos = 0, size_type n = npos)
        : m_str(s.m_str, pos, n) { }

    /// <summary>Construct a string from a null-terminated character array.</summary>
    /// <param name='pstr'>String to construct from.</param>
    basic_stringi<CharT, Traits, Alloc>(const CharT* pstr) : m_str(pstr) { }

    /// <summary>Construct a string from a character array and a length.</summary>
    /// <param name='s'>String to construct from.</param>
    /// <param name='n'>Number of characters to copy from a source string.</param>
    basic_stringi<CharT, Traits, Alloc>(const CharT* s, size_type n) : m_str(s, n) { }

    /// <summary>Create a string with n copies of c.</summary>
    /// <param name='n'>Number of chars to allocate.</param>
    /// <param name='c'>Character to build a string from.</param>
    basic_stringi<CharT, Traits, Alloc>(size_type n, CharT c) : m_str(n, c) { }

    /// <summary>Create a string from a range.</summary>
    /// <typeparam name='InputIterator'>Iterator type.</typeparam>
    /// <param name='first'>Points to the first element in a range.</param>
    /// <param name='last'>Points to the last element in a range.</param>
    template <class InputIterator>
    basic_stringi<CharT, Traits, Alloc>(InputIterator first, InputIterator last)
        : m_str(first, last) { }

    //
    // Methods
    //

    /// <summary>Returns an iterator pointing to the beginning of the string.</summary>
    /// <returns>An iterator pointing to the beginning of the string.</returns>
    iterator begin()
    {
        return m_str.begin();
    }

    /// <summary>Returns an iterator pointing to the end of the string.</summary>
    /// <returns>An iterator pointing to the end of the string.</returns>
    iterator end()
    {
        return m_str.end();
    }

    /// <summary>Returns a const_iterator pointing to the beginning of the string.</summary>
    /// <returns>A const_iterator pointing to the beginning of the string.</returns>
    const_iterator begin() const
    {
        return m_str.begin();
    }

    /// <summary>Returns a const_iterator pointing to the end of the string.</summary>
    /// <returns>A const_iterator pointing to the end of the string.</returns>
    const_iterator end() const
    {
        return m_str.end();
    }

    /// <summary>Returns a reverse_iterator pointing to the beginning of the reversed string.</summary>
    /// <returns>A reverse_iterator pointing to the beginning of the reversed string.</returns>
    reverse_iterator rbegin()
    {
        return m_str.rbegin();
    }

    /// <summary>Returns a reverse_iterator pointing to the end of the reversed string.</summary>
    /// <returns>A reverse_iterator pointing to the end of the reversed string.</returns>
    reverse_iterator rend()
    {
        return m_str.rend();
    }

    /// <summary>Returns a const_reverse_iterator pointing to the beginning of the reversed string.</summary>
    /// <returns>A const_reverse_iterator pointing to the beginning of the reversed string.</returns>
    const_reverse_iterator rbegin() const
    {
        return m_str.rbegin();
    }

    /// <summary>Returns a const_reverse_iterator pointing to the end of the reversed string.</summary>
    /// <returns>A const_reverse_iterator pointing to the end of the reversed string.</returns>
    const_reverse_iterator rend() const
    {
        return m_str.rend();
    }

    /// <summary>Returns the size of the string.</summary>
    /// <returns>The size of the string.</returns>
    size_type size() const
    {
        return m_str.size();
    }

    /// <summary>Synonym for size().</summary>
    /// <returns>The size of the string.</returns>
    size_type length() const
    {
        return m_str.length();
    }

    /// <summary>Returns the largest possible size of the string.</summary>
    size_type max_size() const
    {
        return m_str.max_size();
    }

    /// <summary>Number of elements for which memory has been allocated.
    /// That is, the size to which the string can grow before memory
    /// must be reallocated. capacity() is always greater than or equal to size().</summary>
    /// <returns>The size of storage currently allocated in memory to hold the string.</returns>
    size_type capacity() const
    {
        return m_str.capacity();
    }

    /// <summary>Checks if a string is empty.</summary>
    /// <returns>true if the string's size is 0.</returns>
    bool empty() const
    {
        return m_str.empty();
    }

    /// <summary>Returns a pointer to a null-terminated array of characters
    /// representing the string's contents.</summary>
    /// <returns>A pointer to a null-terminated array of characters
    /// representing the string's contents.</returns>
    const CharT* c_str() const
    {
        return m_str.c_str();
    }

    /// <summary>Returns a pointer to an array of characters (not necessarily
    /// null-terminated) representing the string's contents.</summary>
    /// <returns>A pointer to an array of characters (not necessarily
    /// null-terminated) representing the string's contents.</returns>
    const CharT* data() const
    {
        return m_str.data();
    }

    /// <summary>
    /// Requests that the string's capacity be changed; the postcondition for
    /// this member function is that, after it is called, capacity() >= n.
    /// You may request that a string decrease its capacity by calling reserve()
    /// with an argument less than the current capacity. (If you call reserve()
    /// with an argument less than the string's size, however, the capacity will
    /// only be reduced to size(). A string's size can never be greater than its
    /// capacity.) reserve() throws length_error if n > max_size().</summary>
    /// <param name='n'>Size of characters to reserve.</param>
    void reserve(size_t n)
    {
        m_str.reserve(n);
    }

    /// <summary>Swaps the contents of two strings.</summary>
    /// <param name='s'>String to swap with.</param>
    void swap(self_t& s)
    {
        m_str.swap(s.m_str);
    }

    /// <summary>Inserts c before pos.</summary>
    /// <param name='pos'>Position to insert character to.</param>
    /// <param name='c'>Character to insert.</param>
    /// <returns>A position where a character was inserted to.</returns>
    iterator insert(iterator pos, const CharT& c)
    {
        return m_str.insert(pos, c);
    }

    /// <summary>Inserts the range [first, last) before pos.</summary>
    /// <typeparam name='InputIterator'>Iterator type.</typeparam>
    /// <param name='pos'>Position to insert a range to.</param>
    /// <param name='f'>Points to the first element of a range to insert.</param>
    /// <param name='l'>Points to the last element of a range to insert.</param>
    template <class InputIterator> void insert(iterator pos,
            InputIterator f, InputIterator l)
    {
        m_str.insert(pos, f, l);
    }

    /// <summary>Inserts n copies of c before pos.</summary>
    /// <param name='pos'>Position to insert n copies of c to.</param>
    /// <param name='n'>Number of character copies</param>
    /// <param name='c'>Character to insert.</param>
    void insert(iterator pos, size_type n, const CharT& c)
    {
        m_str.insert(pos, n, c);
    }

    /// <summary>Inserts s before pos.</summary>
    /// <param name='pos'>Position to insert a string to.</param>
    /// <param name='s'>String to insert.</param>
    /// <returns>Reference to itself.</returns>
    self_t& insert(size_type pos, const self_t& s)
    {
        m_str.insert(pos, s.m_str);
        return *this;
    }

    /// <summary>Inserts a substring of s before pos.</summary>
    /// <param name='pos'>Position to insert a string to.</param>
    /// <param name='s'>String to insert.</param>
    /// <param name='pos1'>Offset from the begin of the source string to insert.</param>
    /// <param name='n'>Number of charaters from a source string to insert.</param>
    /// <returns>Reference to itself.</returns>
    self_t& insert(size_type pos, const self_t& s,
                   size_type pos1, size_type n)
    {
        m_str.insert(pos, s.m_str, pos1, n);
        return *this;
    }

    /// <summary>Inserts s before pos.</summary>
    /// <param name='pos'>Offset to insert a string to.</param>
    /// <param name='s'>String to insert.</param>
    /// <returns>Reference to itself.</returns>
    self_t& insert(size_type pos, const CharT* s)
    {
        m_str.insert(pos, s);
        return *this;
    }

    /// <summary>Inserts the first n characters of s before pos.</summary>
    /// <param name='pos'>Offset to insert a string to.</param>
    /// <param name='s'>String to insert.</param>
    /// <param name='n'>Number of characters from a source string to insert.</param>
    /// <returns>Reference to itself.</returns>
    self_t& insert(size_type pos, const CharT* s, size_type n)
    {
        m_str.insert(pos, s, n);
        return *this;
    }

    /// <summary>Inserts n copies of c before pos.</summary>
    /// <param name='pos'>Offset to insert a character to.</param>
    /// <param name='n'>Number of copies of c to insert.</param>
    /// <param name='c'>Character to insert.</param>
    /// <returns>Reference to itself.</returns>
    self_t& insert(size_type pos, size_type n, CharT c)
    {
        m_str.insert(pos, n, c);
        return *this;
    }

    /// <summary>
    /// Append s to *this.
    /// </summary>
    /// <param name='s'>String to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& append(const self_t& s)
    {
        m_str.append(s.m_str);
        return *this;
    }

    /// <summary>Append a substring of s to *this.</summary>
    /// <param name='s'>String to append.</param>
    /// <param name='pos'>Offset from the beginning of the source string.</param>
    /// <param name='n'>Number of characters from the source string to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& append(const self_t& s, size_type pos, size_type n)
    {
        m_str.append(s.m_str, pos, n);
        return *this;
    }

    /// <summary>Append s to *this.</summary>
    /// <param name='s'>String to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& append(const CharT* s)
    {
        m_str.append(s);
        return *this;
    }

    /// <summary>Append the first n characters of s to *this.</summary>
    /// <param name='s'>String to append.</param>
    /// <param name='n'>Number of characters to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& append(const CharT* s, size_type n)
    {
        m_str.append(s, n);
        return *this;
    }

    /// <summary>Append n copies of c to *this.</summary>
    /// <param name='n'>Number of characters to append.</param>
    /// <param name='c'>Character to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& append(size_type n, CharT c)
    {
        m_str.append(n, c);
        return *this;
    }

    /// <summary>Append a range to *this.</summary>
    /// <typeparam name='InputIterator'>Iterator type.</typeparam>
    /// <param name='first'>Points to the first element from a range to append.</param>
    /// <param name='last'>Points to the last element from a range to append.</param>
    /// <returns>Reference to itself.</returns>
    template <class InputIterator>
    self_t& append(InputIterator first, InputIterator last)
    {
        m_str.append(first, last);
        return *this;
    }

    /// <summary>Append a single character to *this.</summary>
    /// <param name='c'>Character to append.</param>
    void push_back(CharT c)
    {
        m_str.push_back(c);
    }

    /// <summary>Erases the character at position p.</summary>
    /// <param name='p'>Position of a character to erase.</param>
    /// <returns>Iterator to a character after a character to remove.</returns>
    iterator erase(iterator p)
    {
        return m_str.erase(p);
    }

    /// <summary>Erases the range [first, last).</summary>
    /// <param name='first'>Points to the first element from a range to erase.</param>
    /// <param name='last'>Points to the last element from a range to erase.</param>
    /// <returns>Iterator to the first character after the last removed character.</returns>
    iterator erase(iterator first, iterator last)
    {
        return m_str.erase(first, last);
    }

    /// <summary>Erases a range.</summary>
    /// <param name='pos'>Offset to a range where to begin erasing.</param>
    /// <param name='n'>Number of characters to erase.</param>
    /// <returns>Reference to itself.</returns>
    self_t& erase(size_type pos = 0, size_type n = npos)
    {
        m_str.erase(pos, n);
        return *this;
    }

    /// <summary>Erases the entire container.</summary>
    void clear()
    {
        m_str.clear();
    }

    /// <summary>Appends characters, or erases characters from the end,
    /// as necessary to make the string's length exactly n characters.</summary>
    /// <param name='n'>Number of characters to resize a string to.</param>
    /// <param name='c'>Charater to fill empty space.</param>
    void resize(size_type n, CharT c = CharT())
    {
        m_str.resize(n, c);
    }

    /// <summary>Synonym for operator= .</summary>
    /// <param name='rhs'>String to assign to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& assign(const self_t& rhs)
    {
        return *this = rhs;
    }

    /// <summary>Assigns a substring of s to *this.</summary>
    /// <param name='s'>String to assign to.</param>
    /// <param name='pos'>Offset from a source string's beginning.</param>
    /// <param name='n'>Number of characters from a source string.</param>
    /// <returns>Reference to itself.</returns>
    self_t& assign(const self_t& s, size_type pos, size_type n)
    {
        m_str.assign(s.m_str, pos, n);
        return *this;
    }

    /// <summary>Assigns the first n characters of s to *this.</summary>
    /// <param name='s'>String to assign to.</param>
    /// <param name='n'>Number of characters from a string to assign to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& assign(const CharT* s, size_type n)
    {
        return *this;
    }

    /// <summary>Assigns a null-terminated array of characters to *this.</summary>
    /// <param name='s'>String to assign to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& assign(const CharT* s)
    {
        m_str.assign(s);
        return *this;
    }

    /// <summary>Erases the existing characters and replaces them by n copies of c.</summary>
    /// <param name='n'>Number of characters.</param>
    /// <param name='c'>Character to fill a string.</param>
    self_t& assign(size_type n, CharT c)
    {
        m_str.assign(n, c);
        return *this;
    }

    /// <summary>Erases the existing characters and replaces them by [first, last).</summary>
    /// <typeparam name='InputIterator'>Iterator type.</typeparam>
    /// <param name='first'>Points to the first element from a range to assign to.</param>
    /// <param name='last'>Points to the last element from a range to assign to.</param>
    /// <returns>Reference to itself.</returns>
    template <class InputIterator>
    self_t& assign(InputIterator first, InputIterator last)
    {
        m_str.assign(first, last);
        return *this;
    }

    /// <summary>Replaces a substring of *this with the string s.</summary>
    /// <param name='pos'>Offset to a range to replace.</param>
    /// <param name='n'>Number of characters to replace.</param>
    /// <param name='s'>String to replace a range to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(size_type pos, size_type n, const self_t& s)
    {
        m_str.replace(pos, n, s.m_str);
        return *this;
    }

    /// <summary>Replaces a substring of *this with a substring of s.</summary>
    /// <param name='pos'>Offset to a range to replace.</param>
    /// <param name='n'>Number of characters to replace.</param>
    /// <param name='s'>String to replace a range to.</param>
    /// <param name='pos1'>Offset from the beginning of the string to replace a range to.</param>
    /// <param name='n1'>Number of characters from a source string.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(size_type pos, size_type n, const self_t& s,
                    size_type pos1, size_type n1)
    {
        m_str.replace(pos, n, s.m_str, pos1, n1);
        return *this;
    }

    /// <summary>Replaces a substring of *this with the first n1 characters of s.</summary>
    /// <param name='pos'>Offset to a range to replace.</param>
    /// <param name='n'>Number of characters to replace.</param>
    /// <param name='s'>String to replace a range to.</param>
    /// <param name='n1'>Number of characters from a source string.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(size_type pos, size_type n, const CharT* s, size_type n1)
    {
        m_str.replace(pos, n, s, n1);
        return *this;
    }

    /// <summary>Replaces a substring of *this with a null-terminated character array.</summary>
    /// <param name='pos'>Offset to a range to replace.</param>
    /// <param name='n'>Number of characters to replace.</param>
    /// <param name='s'>String to replace a range to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(size_type pos, size_type n, const CharT* s)
    {
        m_str.replace(pos, n, s);
        return *this;
    }

    /// <summary>Replaces a substring of *this with n1 copies of c.</summary>
    /// <param name='pos'>Offset to a range to replace.</param>
    /// <param name='n'>Number of characters to replace.</param>
    /// <param name='n1'>Number of characters to replace to.</param>
    /// <param name='c'>Character to insert n1 copies of.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(size_type pos, size_type n, size_type n1, CharT c)
    {
        m_str.replace(pos, n, n1, c);
        return *this;
    }

    /// <summary>Replaces a substring of *this with the string s.</summary>
    /// <param name='first'>Points to the first element of a range to replace.</param>
    /// <param name='last'>Points to the last element of a range to replace.</param>
    /// <param name='s'>String to replace a range to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(iterator first, iterator last, const self_t& s)
    {
        m_str.replace(first, last, s.m_str);
        return *this;
    }

    /// <summary>Replaces a substring of *this with the first n characters of s.</summary>
    /// <param name='first'>Points to the first element of a range to replace.</param>
    /// <param name='last'>Points to the last element of a range to replace.</param>
    /// <param name='s'>String to replace a range to.</param>
    /// <param name='n'>Number of characters from a source string.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(iterator first, iterator last, const CharT* s, size_type n)
    {
        m_str.replace(first, last, s, n);
        return *this;
    }

    /// <summary>Replaces a substring of *this with a null-terminated character array.</summary>
    /// <param name='first'>Points to the first element of a range to replace.</param>
    /// <param name='last'>Points to the last element of a range to replace.</param>
    /// <param name='s'>String to replace a range to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(iterator first, iterator last, const CharT* s)
    {
        m_str.replace(first, last, s);
        return *this;
    }

    /// <summary>Replaces a substring of *this with n copies of c.</summary>
    /// <param name='first'>Points to the first element of a range to replace.</param>
    /// <param name='last'>Points to the last element of a range to replace.</param>
    /// <param name='n'>Number of copies of c.</param>
    /// <param name='c'>Character to to a range to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& replace(iterator first, iterator last, size_type n, CharT c)
    {
        m_str.replace(first, last, n, c);
        return *this;
    }

    /// <summary>Replaces a substring of *this with the range [f, l).</summary>
    /// <typeparam name='InputIterator'>Iterator type.</typeparam>
    /// <param name='first'>Points to the first element of a range to replace.</param>
    /// <param name='last'>Points to the last element of a range to replace.</param>
    /// <param name='f'>Points to the first element of a range to replace to.</param>
    /// <param name='l'>Points to the last element of a range to replace to.</param>
    /// <returns>Reference to itself.</returns>
    template <class InputIterator>
    self_t& replace(iterator first, iterator last, InputIterator f, InputIterator l)
    {
        m_str.replace(first, last, f, l);
        return *this;
    }

    /// <summary>Copies a substring of *this to a buffer.</summary>
    /// <param name='buf'>String to copy characters from.</param>
    /// <param name='n'>Number of characters to copy.</param>
    /// <param name='pos'>Offset to a first character where to copy characters to.</param>
    /// <returns>Number of characters copied.</returns>
    size_type copy(CharT* buf, size_type n, size_type pos = 0) const
    {
        return m_str.copy(buf, n, pos);
    }

private:

    const CharT* find_ch(const CharT* s, size_type n, CharT ch) const
    {
        ch = tolower(ch, m_loc);
        while(n--)
        {
            if(tolower(*s, m_loc) == ch)
                return s;

            ++s;
        }

        return NULL;
    }

public:


    /// <summary>Searches for s as a substring of *this, beginning at character pos of *this.</summary>
    /// <param name='s'>Substring to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a substring or npos if a substring was not found.</returns>
    size_type find(const self_t& s, size_type pos = 0) const
    {
        return find(s.m_str.c_str(), pos, s.m_str.size());
    }

    /// <summary>Searches for the first n characters of s as a substring of *this,
    /// beginning at character pos of *this.</summary>
    /// <param name='s'>Substring to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <param name='n'>Length of a substring to find.</param>
    /// <returns>Position of a substring or npos if a substring was not found.</returns>
    size_type find(const CharT* s, size_type pos, size_type n) const
    {
        size_type len = m_str.size();
        if(n == 0 && pos <= len)
            return pos;

        size_type nn;
        if (pos < len && n <= (nn = len - pos))
        {
            const CharT *p1, *p2, *mp = m_str.c_str();
            for(nn -= n - 1, p2 = mp + pos;
                    (p1 = find_ch(p2, nn, *s)) != 0;
                    nn -= p1 - p2 + 1, p2 = p1 + 1)
            {
                if(compare(p1, s, n) == 0)
                    return p1 - mp;
            }
        }

        return npos;
    }

    /// <summary>Searches for a null-terminated character array as a substring
    /// of *this, beginning at character pos of *this.</summary>
    /// <param name='s'>Substring to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a substring or npos if a substring was not found.</returns>
    size_type find(const CharT* s, size_type pos = 0) const
    {
        return find(s, pos, Traits::length(s));
    }

    /// <summary>Searches for the character c, beginning at character position pos.</summary>
    /// <param name='c'>Character to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find(CharT c, size_type pos = 0) const
    {
        return find(&c, pos, 1);
    }

    /// <summary>Searches backward for s as a substring of *this,
    /// beginning at character position min(pos, size())</summary>
    /// <param name='s'>Substring to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a substring or npos if a substring was not found.</returns>
    size_type rfind(const self_t& s, size_type pos = npos) const
    {
        return rfind(s.m_str.c_str(), pos, s.m_str.size());
    }

    /// <summary>Searches backward for the first n characters of s as a substring
    /// of *this, beginning at character position min(pos, size()).</summary>
    /// <param name='s'>Substring to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <param name='n'>Number of characters from a substring to search.</param>
    /// <returns>Position of a substring or npos if a substring was not found.</returns>
    size_type rfind(const CharT* s, size_type pos, size_type n) const
    {
        size_type len = m_str.size();
        if(n == 0)
            return pos < len ? pos : len;
        if(n <= len)
        {
            const CharT* ptr = m_str.c_str();
            const CharT* ps = ptr + (pos < len - n ? pos : len - n);
            for(;; --ps)
                if(Traits::eq(tolower(*ps, m_loc), tolower(*s, m_loc))
                        && compare(ps, s, n) == 0)
                {
                    return ps - ptr;
                }
                else if(ps == ptr)
                    break;
        }

        return npos;
    }

    /// <summary>Searches backward for a null-terminated character array as a substring
    /// of *this, beginning at character min(pos, size()).</summary>
    /// <param name='s'>Substring to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a substring or npos if a substring was not found.</returns>
    size_type rfind(const CharT* s, size_type pos = npos) const
    {
        return rfind(s, pos, Traits::length(s));
    }

    /// <summary>Searches backward for the character c,
    /// beginning at character position min(pos, size().</summary>
    /// <param name='c'>Character to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type rfind(CharT c, size_type pos = npos) const
    {
        return rfind(&c, pos, 1);
    }

    /// <summary>Searches within *this, beginning at pos, for the
    /// first character that is equal to any character within s. </summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a first found character or npos if a neither was not found.</returns>
    size_type find_first_of(const self_t& s, size_type pos = 0) const
    {
        return find_first_of(s.m_str.c_str(), pos, s.m_str.size());
    }

    /// <summary>Searches within *this, beginning at pos, for the first character that is
    /// equal to any character within the first n characters of s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <param name='n'>Number of characters from a set of characters to search for.</param>
    /// <returns>Position of a first found character or npos if a neither was not found.</returns>
    size_type find_first_of(const CharT* s, size_type pos, size_type n) const
    {
        size_type len = m_str.size();
        if (0 < n && pos < len)
        {
            const CharT* mp = m_str.c_str();
            const CharT* p1 = mp + len;
            for(const CharT *p2 = mp + pos; p2 < p1; ++p2)
                if(find_ch(s, n, *p2) != 0)
                    return p2 - mp;
        }

        return npos;
    }

    /// <summary>Searches within *this, beginning at pos, for the first
    /// character that is equal to any character within s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a first found character or npos if a neither was not found.</returns>
    size_type find_first_of(const CharT* s, size_type pos = 0) const
    {
        return find_first_of(s, pos, Traits::length(s));
    }

    /// <summary>Searches within *this, beginning at pos, for
    /// the first character that is equal to c.</summary>
    /// <param name='c'>Character to find.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_first_of(CharT c, size_type pos = 0) const
    {
        return find_first_of(&c, pos, 1);
    }

    /// <summary>Searches within *this, beginning at pos, for the first
    /// character that is not equal to any character within s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_first_not_of(const self_t& s, size_type pos = 0) const
    {
        return find_first_not_of(s.m_str.c_str(), pos, s.m_str.size());
    }

    /// <summary>Searches within *this, beginning at pos, for the first character that is
    /// not equal to any character within the first n characters of s. </summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <param name='n'>Number of character to search for.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_first_not_of(const CharT* s, size_type pos, size_type n) const
    {
        size_type len = m_str.size();
        if(pos < len)
        {
            const CharT* mp = m_str.c_str();
            const CharT* p1 = mp + len;
            for(const CharT *p2 = mp + pos; p2 < p1; ++p2)
                if(find_ch(s, n, *p2) == 0)
                    return p2 - mp;
        }

        return npos;
    }

    /// <summary>Searches within *this, beginning at pos, for the
    /// first character that is not equal to any character within s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_first_not_of(const CharT* s, size_type pos = 0) const
    {
        return find_first_not_of(s, pos, Traits::length(s));
    }

    /// <summary>Searches within *this, beginning at pos,
    /// for the first character that is not equal to c.</summary>
    /// <param name='c'>Character to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_first_not_of(CharT c, size_type pos = 0) const
    {
        return find_first_not_of(&c, pos, 1);
    }

    /// <summary>Searches backward within *this, beginning at min(pos, size()),
    /// for the first character that is equal to any character within s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_of(const self_t& s, size_type pos = npos) const
    {
        return find_last_of(s.m_str.c_str(), pos, s.m_str.size());
    }


    /// <summary>Searches backward within *this, beginning at min(pos, size()),
    /// for the first character that is equal to any character within the first
    /// n characters of s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <param name='n'>Number of character to search for.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_of(const CharT* s, size_type pos, size_type n) const
    {
        size_type len = m_str.size();
        if(0 < n && 0 < len)
        {
            const CharT* mp = m_str.c_str();
            for(const CharT* p = mp + (pos < len ? pos : len - 1); ; --p)
            {
                if(find_ch(s, n, *p) != 0)
                    return p - mp;
                else if(p == mp)
                    break;
            }
        }

        return npos;
    }

    /// <summary>Searches backward *this, beginning at min(pos, size()), for the
    /// first character that is equal to any character within s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_of(const CharT* s, size_type pos = npos) const
    {
        return find_last_of(s, pos, Traits::length(s));
    }

    /// <summary>Searches backward *this, beginning at min(pos, size()),
    /// for the first character that is equal to c.</summary>
    /// <param name='c'>Character to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_of(CharT c, size_type pos = npos) const
    {
        return find_last_of(&c, pos, 1);
    }

    /// <summary>Searches backward within *this, beginning at min(pos, size()),
    /// for the first character that is not equal to any character within s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_not_of(const self_t& s, size_type pos = npos) const
    {
        return find_last_not_of(s.m_str.c_str(), pos, s.m_str.size());
    }

    /// <summary>Searches backward within *this, beginning at min(pos, size()),
    /// for the first character that is not equal to any character within the
    /// first n characters of s. </summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <param name='n'>Number of character to search for.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_not_of(const CharT* s, size_type pos, size_type n) const
    {
        size_type len = m_str.size();
        if(0 < len)
        {
            const CharT* mp = m_str.c_str();
            for(const CharT* p = mp + (pos < len ? pos : len - 1); ; --p)
            {
                if(find_ch(s, n, *p) == 0)
                    return p - mp;
                else if(p == mp)
                    break;
            }
        }

        return npos;
    }

    /// <summary>Searches backward *this, beginning at min(pos, size()),
    /// for the first character that is not equal to any character within s.</summary>
    /// <param name='s'>Set of characters to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_not_of(const CharT* s, size_type pos = npos) const
    {
        return find_last_not_of(s, pos, Traits::length(s));
    }

    /// <summary>Searches backward *this, beginning at min(pos, size()),
    /// for the first character that is not equal to c.</summary>
    /// <param name='c'>Character to search for.</param>
    /// <param name='pos'>Position to start a search from.</param>
    /// <returns>Position of a character found or npos if a character was not found.</returns>
    size_type find_last_not_of(CharT c, size_type pos = npos) const
    {
        return find_last_not_of(&c, pos, 1);
    }

    /// <summary>Returns a substring of *this.</summary>
    /// <param name='pos'>Substring start position.</param>
    /// <param name='n'>Length of substring to return.</param>
    /// <returns>Substring value.</returns>
    self_t substr(size_type pos = 0, size_type n = npos) const
    {
        return m_str.substr(pos, n).c_str();
    }

    /// <summary>Three-way case-insensitive lexicographical comparison of s and *this.</summary>
    /// <param name='s'>String to compare a string to.</param>
    /// <returns>A negative value if the operand string is less than the parameter string;
    /// zero if the two strings are equal; or a positive value if the operand string
    /// is greater than the parameter string.</returns>
    int compare(const self_t& s) const
    {
        return compare(0, m_str.size(), s.m_str.c_str(), s.m_str.size());
    }

    /// <summary>Three-way case-insensitive lexicographical comparison of s and a substring of *this.</summary>
    /// <param name='pos'>Offset to start comparison from.</param>
    /// <param name='n'>Number of characters to compare.</param>
    /// <param name='s'>String to compare to.</param>
    /// <returns>A negative value if the operand string is less than the parameter string;
    /// zero if the two strings are equal; or a positive value if the operand string
    /// is greater than the parameter string.</returns>
    int compare(size_type pos, size_type n, const self_t& s) const
    {
        return compare(pos, n, s.m_str.c_str(), 0, npos);
    }

    /// <summary>Three-way case-insensitive lexicographical comparison of a
    /// substring of s and a substring of *this.</summary>
    /// <param name='pos'>Offset to start comparison from.</param>
    /// <param name='n'>Number of characters to compare.</param>
    /// <param name='s'>String to compare to.</param>
    /// <param name='pos1'>Offset from the beginning of a string to compare with.</param>
    /// <param name='n1'>Number of characters from a string to compare with.</param>
    /// <returns>A negative value if the operand string is less than the parameter string;
    /// zero if the two strings are equal; or a positive value if the operand string
    /// is greater than the parameter string.</returns>
    int compare(size_type pos, size_type n, const self_t& s,
                size_type pos1, size_type n1) const
    {
        if (s.size() < pos1)
            throw std::runtime_error("Out of range");

        size_type slen = s.m_str.size();
        if (slen - pos1 < n1)
            n1 = slen - pos1;
        return compare(pos, n, s.m_str.c_str() + pos1, n1);
    }

    /// <summary>Three-way case-insensitive lexicographical comparison of s and *this.</summary>
    /// <param name='s'>String to compare to.</param>
    /// <returns>A negative value if the operand string is less than the parameter string;
    /// zero if the two strings are equal; or a positive value if the operand string
    /// is greater than the parameter string.</returns>
    int compare(const CharT* s) const
    {
        return compare(0, m_str.size(), s, Traits::length(s));
    }

    /// <summary>Three-way lexicographical comparison of the first min(len, traits::length(s))
    /// characters of s and a substring of *this.</summary>
    /// <param name='pos'>Position to start comparison from.</param>
    /// <param name='n'>Length of a substring to compare.</param>
    /// <param name='s'>String to compare to.</param>
    /// <param name='slen'>Length of the string to compare to.</param>
    /// <returns>A negative value if the operand string is less than the parameter string;
    /// zero if the two strings are equal; or a positive value if the operand string
    /// is greater than the parameter string.</returns>
    int compare(size_type pos, size_type n, const CharT* s,
                size_type slen = npos) const
    {
        size_type len = size();
        if(len < pos)
            throw std::runtime_error("Out of range");

        if(len - pos < n)
            n = len - pos;

        size_type r = compare(m_str.c_str() + pos, s, n < slen ? n : slen);
        return r != 0 ? (int)r : n < slen ? -1 : n == slen ? 0 : 1;
    }


private:

    int compare(const CharT* s1, const CharT* s2, size_type n) const
    {
        while(n--)
        {
            CharT ch1 = tolower(*s1, m_loc);
            CharT ch2 = tolower(*s2, m_loc);
            if(ch1 < ch2)
                return -1;
            if(ch1 > ch2)
                return 1;

            ++s1;
            ++s2;
        }

        return 0;
    }

public:

    //
    // Nonstandard basic_stringi only methods
    //

    /// <summary>Returns a locale associated with a string.</summary>
    /// <returns>A locale associated with a string.</returns>
    locale& getloc()
    {
        return m_loc;
    }

    /// <summary>Returns a locale associated with a string.</summary>
    /// <returns>A locale associated with a string.</returns>
    const locale& getloc() const
    {
        return m_loc;
    }

    /// <summary>
    /// Returns a %reference to a standard STL string.
    /// </summary>
    /// <returns>Reference to a standard STL string.</returns>
    stdstring_t& std_str()
    {
        return m_str;
    }

    /// <summary>
    /// Returns a %reference to a standard STL string.
    /// </summary>
    /// <returns>Reference to a standard STL string.</returns>
    const stdstring_t& std_str() const
    {
        return m_str;
    }

    //
    // Operators
    //

    /// <summary>Returns the n'th character.</summary>
    /// <param name='n'>Zero-based index of a character to return.</param>
    /// <returns>The n'th character.</returns>
    reference operator[](size_type n)
    {
        return m_str[n];
    }

    /// <summary>Returns the n'th character.</summary>
    /// <param name='n'>Zero-based index of a character to return.</param>
    /// <returns>The n'th character.</returns>
    const_reference operator[](size_type n) const
    {
        return m_str[n];
    }

    /// <summary>The assignment operator.</summary>
    /// <param name='rhs'>String to assign to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& operator=(const self_t& rhs)
    {
        m_str = rhs.m_str;
        return *this;
    }

    /// <summary>Assign a null-terminated character array to a string.</summary>
    /// <param name='rhs'>String to assign to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& operator=(const CharT* rhs)
    {
        m_str = rhs;
        return *this;
    }

    /// <summary>Assign a single character to a string.</summary>
    /// <param name='rhs'>Character to assign to.</param>
    /// <returns>Reference to itself.</returns>
    self_t& operator=(CharT rhs)
    {
        m_str = rhs;
        return *this;
    }

    /// <summary>Equivalent to append(s).</summary>
    /// <param name='s'>String to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& operator+=(const self_t& s)
    {
        m_str += s.m_str;
        return *this;
    }

    /// <summary>Equivalent to append(s).</summary>
    /// <param name='s'>String to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& operator+=(const CharT* s)
    {
        m_str += s;
        return *this;
    }

    /// <summary>Equivalent to append(s).</summary>
    /// <param name='c'>Character to append.</param>
    /// <returns>Reference to itself.</returns>
    self_t& operator+=(CharT c)
    {
        m_str += c;
        return *this;
    }

    //
    // Friend global operators and functions
    //

    // String concatenation. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend basic_stringi<CharT, Traits, Alloc> operator+(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    //String concatenation. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend basic_stringi<CharT, Traits, Alloc> operator+(
        const CharT* s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    //String concatenation. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend basic_stringi<CharT, Traits, Alloc> operator+(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const CharT* s2);
    //String concatenation. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend basic_stringi<CharT, Traits, Alloc> operator+(
        CharT c,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    //String concatenation. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend basic_stringi<CharT, Traits, Alloc> operator+(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        CharT c);
    // String equality. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator==(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    // String equality. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator==(
        const CharT* s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    // String equality. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator==(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const CharT* s2);
    // String inequality. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator!=(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    // String inequality. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator!=(
        const CharT* s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    // String inequality. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator!=(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const CharT* s2);
    // String comparison. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator<(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    // String comparison. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator<(
        const CharT* s1,
        const basic_stringi<CharT, Traits, Alloc>& s2);
    // String comparison. A global function, not a member function.
    template <class CharT, class Traits, class Alloc>
    friend bool operator<(
        const basic_stringi<CharT, Traits, Alloc>& s1,
        const CharT* s2);
    // Swaps the contents of two strings.
    template <class CharT, class Traits, class Alloc>
    friend void swap(
        basic_stringi<CharT, Traits, Alloc>& s1,
        basic_stringi<CharT, Traits, Alloc>& s2);
    // Reads s from the input stream is
    template <class CharT, class Traits, class Alloc>
    friend basic_istream<CharT, Traits>& operator>>(
        basic_istream<CharT, Traits>& is,
        basic_stringi<CharT, Traits, Alloc>& s);
    // Writes s to the output stream os
    template <class CharT, class Traits, class Alloc>
    friend basic_ostream<CharT, Traits>& operator<<(
        basic_ostream<CharT, Traits>& os,
        const basic_stringi<CharT, Traits, Alloc>& s);

}; // class basic_stringi

//
// Global functions
//


/// <summary>Concatenates two strings.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>1-st string.</param>
/// <param name='s2'>2-nd string.</param>
/// <returns>Concatenated strings.</returns>
template <class CharT, class Traits, class Alloc>
basic_stringi<CharT, Traits, Alloc> operator+(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return (s1.m_str + s2.m_str).c_str();
}


/// <summary>Concatenates two strings.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>1-st string.</param>
/// <param name='s2'>2-nd string.</param>
/// <returns>Concatenated strings.</returns>
template <class CharT, class Traits, class Alloc>
inline basic_stringi<CharT, Traits, Alloc> operator+(
    const CharT* s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return (s1 + s2.m_str).c_str();
}


/// <summary>Concatenates two strings.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>1-st string.</param>
/// <param name='s2'>2-nd string.</param>
/// <returns>Concatenated strings.</returns>
template <class CharT, class Traits, class Alloc>
inline basic_stringi<CharT, Traits, Alloc> operator+(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const CharT* s2)
{
    return (s1.m_str + s2).c_str();
}


/// <summary>Concatenates a character with a string.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='c'>A character.</param>
/// <param name='s2'>A string.</param>
/// <returns>A string with a character appended at the beginning.</returns>
template <class CharT, class Traits, class Alloc>
inline basic_stringi<CharT, Traits, Alloc> operator+(
    CharT c,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return (c + s2.m_str).c_str();
}


/// <summary>Concatenates a string with a character.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>A string.</param>
/// <param name='c'>A character.</param>
/// <returns>A string with a character appended at the end.</returns>
template <class CharT, class Traits, class Alloc>
inline basic_stringi<CharT, Traits, Alloc> operator+(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    CharT c)
{
    return (s1.m_str + c).c_str();
}


/// <summary>Checks if strings are equal (case-insensitive).</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if strings are equal; otherwise false.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator==(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return s1.compare(s2) == 0;
}


/// <summary>Checks if strings are equal (case-insensitive).</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if strings are equal; otherwise false.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator==(
    const CharT* s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return s2.compare(s1) == 0;
}


/// <summary>Checks if strings are equal (case-insensitive).</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if strings are equal; otherwise false.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator==(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const CharT* s2)
{
    return s1.compare(s2) == 0;
}


/// <summary>Checks if strings are not equal (case-insensitive).</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if strings are not equal; otherwise false.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator!=(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return s1.compare(s2) != 0;
}


/// <summary>Checks if strings are not equal (case-insensitive).</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if strings are not equal; otherwise false.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator!=(
    const CharT* s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return s2.compare(s1) != 0;
}


/// <summary>Checks if strings are not equal (case-insensitive).</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if strings are not equal; otherwise false.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator!=(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const CharT* s2)
{
    return s1.compare(s2) != 0;
}


/// <summary>Performs case-insensitive strign comparison.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if the first string is less then the second.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator<(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return s1.compare(s2) < 0;
}


/// <summary>Performs case-insensitive strign comparison.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if the first string is less then the second.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator<(
    const CharT* s1,
    const basic_stringi<CharT, Traits, Alloc>& s2)
{
    return s2.compare(s1) > 0;
}


/// <summary>Performs case-insensitive strign comparison.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>String to check.</param>
/// <param name='s2'>String to compare to.</param>
/// <returns>true if the first string is less then the second.</returns>
template <class CharT, class Traits, class Alloc>
inline bool operator<(
    const basic_stringi<CharT, Traits, Alloc>& s1,
    const CharT* s2)
{
    return s1.compare(s2) < 0;
}


/// <summary>Swaps the contents of two strings.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='s1'>First string.</param>
/// <param name='s2'>Second string.</param>
template <class CharT, class Traits, class Alloc>
inline void swap(
    basic_stringi<CharT, Traits, Alloc>& s1,
    basic_stringi<CharT, Traits, Alloc>& s2)
{
    s1.swap(s2);
}


/// <summary>Reads s from the input stream is.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='is'>A stream to read a string from.</param>
/// <param name='s'>A string to read.</param>
/// <returns>Source stream.</returns>
template <class CharT, class Traits, class Alloc>
inline basic_istream<CharT, Traits>& operator>>(
    basic_istream<CharT, Traits>& is,
    basic_stringi<CharT, Traits, Alloc>& s)
{
    is >>s.m_str;
    return is;
}


/// <summary>Writes s to the output stream os.</summary>
/// <typeparam name='CharT'>The data type of a single character to be stored in the string.</typeparam>
/// <typeparam name='Traits'>The CharT elements properties.</typeparam>
/// <typeparam name='Allocator'>The type that represents the stored allocator object.</typeparam>
/// <param name='os'>A stream to write a string to.</param>
/// <param name='s'>A string to write.</param>
/// <returns>Target stream.</returns>
template <class CharT, class Traits, class Alloc>
inline basic_ostream<CharT, Traits>& operator<<(
    basic_ostream<CharT, Traits>& os,
    const basic_stringi<CharT, Traits, Alloc>& s)
{
    os <<s.m_str;
    return os;
}

//
// Global typedefs
//

/// <summary>
/// Case insensitive ANSI string.
/// </summary>
typedef basic_stringi<char> stringi;

/// <summary>
/// Case insensitive Unicode string.
/// </summary>
typedef basic_stringi<wchar_t> wstringi;

} // namespace std

#ifndef DOXYGEN

namespace stdext
{

template<class CharT, class Traits, class Alloc>
class basic_stringi_hash_compare
{
public:

    enum
    {
        bucket_size = 4,
        min_buckets = 8,
    };

    size_t operator() (const std::basic_stringi<CharT, Traits, Alloc>& str) const
    {
        const std::locale& loc = str.getloc();
        std::basic_string<CharT, Traits, Alloc>  lws(str.begin(), str.end());
        typename std::basic_string<CharT, Traits, Alloc>::iterator
        ptr = lws.begin(), end = lws.end();
        for(; ptr != end; ++ptr)
            *ptr = std::tolower(*ptr, loc);

        long hash = hash_value(lws) & LONG_MAX;
        ldiv_t d = ldiv(hash, 127773);

        d.rem = 16807 * d.rem - 2836 * d.quot;
        if(d.rem < 0)
            d.rem += LONG_MAX;

        return (size_t)d.rem;
    }

    bool operator()(const std::basic_stringi<CharT, Traits, Alloc>& str1,
                    const std::basic_stringi<CharT, Traits, Alloc>& str2) const
    {
        return str1 < str2;
    }
};

//
// hash_compare class specializations
//

template<>
class hash_compare<std::stringi, std::less<std::stringi> >
    : public basic_stringi_hash_compare<
      char, std::char_traits<char>, std::allocator<char> >
{
};


template<>
class hash_compare<std::wstringi, std::less<std::wstringi> >
    : public basic_stringi_hash_compare<
      wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >
{
};

}; // namespace stdext

#endif /*DOXYGEN*/


#undef __STRINGI_H_CYCLE__
#endif /*__STRINGI_H__*/
