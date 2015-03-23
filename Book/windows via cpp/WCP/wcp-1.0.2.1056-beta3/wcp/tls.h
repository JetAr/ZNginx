#ifdef ___TLS_H_CYCLE__
#error Cyclic dependency discovered: tls.h
#endif

#ifndef __TLS_H__


#ifndef DOXYGEN

#define __TLS_H__
#define __TLS_H_CYCLE__

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
///      11/18/2009 - Comments adapted for Doxygen documentation generator.
///      01/16/2010 - wcp::thread_local class is renamed to wcp::tls in order to
///                   comply with future C++0x standard.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Thread local storage template implementation.
*/


#include <map>

#include "sync.h"



namespace WCP_NS 
{
    /// <summary>
    /// Indicates that TLS has no value for a thread.
    /// </summary>
    class tls_not_found_exception : public exception
    {
        dword_t tid;

    public:

        /// <summary>
        /// Constructs an %exception object.
        /// </summary>
        /// <param name='tid'>Thread identifier a TLS value was not found for.</param>
        tls_not_found_exception(dword_t tid)
        { }

    private:

        virtual void build_error_message(std::tstring& msg) const
        {
            std::tostringstream ss;
            ss <<"Thread #" <<tid <<" has no a value set in TLS.";
            msg = ss.str();
        }
    };

    //-----------------------------------------------------------------------------

    /// <summary>
    /// Thread local storage implementation.
    /// </summary>
    /// <typeparam name='T'>Storage type.</typeparam>
    template<class T> class tls : noncopyable
    {
        monitor tls_access;
        
        typedef std::map<dword_t, T> map_t;
        mutable map_t tls_map;

    public:

        /// <summary>
        /// Sets a value to a slot of a thread specified.
        /// </summary>
        /// <param name='v'>Value to set</param>
        /// <param name='tid'>Thread identifier to set a value for.</param>
        /// <param name='overwrite'>if true, overwrites a value if it is already exists in a slot.</param>
        /// <returns>true if value was set.</returns>
        bool set(const T& v, 
                 dword_t tid = GetCurrentThreadId(),
                 bool overwrite = false)
        {
            GUARD(tls_access)
            {
                map_t::iterator p = tls_map.find(tid);
                if(p != tls_map.end())
                {
                    if(overwrite)
                        (*p).second = v;
                    else
                        return false;
                }

                tls_map[tid] = v;
                return true;
            }
        }

        /// <summary>
        /// Obtains a value in TLS that was set for the thread specified.
        /// </summary>
        /// <param name='tid'>Thread identifier to get a value for.</param>
        /// <returns>TLS value %reference.</returns>
        T& get(dword_t tid) const
        {
            GUARD(tls_access)
            {
                map_t::iterator p = tls_map.find(tid);
                if(p == tls_map.end())
                {
                    _ASSERTE("TLS item not found." && false);
                    throw tls_not_found_exception(tid);
                }

                return (*p).second;
            }
        }

        /// <summary>
        /// Obtains a value in TLS that was set for the calling thread.
        /// </summary>
        /// <returns>TLS value %reference.</returns>
        T& get() const
        { return get(GetCurrentThreadId()); }

        /// <summary>
        /// Obtains a value in TLS that was set for the calling thread.
        /// </summary>
        /// <returns>TLS value %reference.</returns>
        T& operator * () const
        { return get(); }

        /// <summary>
        /// Obtains a value in TLS that was set for the calling thread.
        /// </summary>
        /// <returns>TLS value pointer.</returns>
        T* operator -> () const
        { return  &get(); }

        /// <summary>
        /// Clears a TLS value in the slot specified by a thread identifier passed.
        /// </summary>
        /// <param name='tid'>Thread identifier to clear a TLS value for.</param>
        void clear_slot(dword_t tid = GetCurrentThreadId())
        {
            GUARD(tls_access)
            {
                tls_map.erase(tid);
            }
        }

        /// <summary>
        /// Checks if a TLS value exists for the thread specified.
        /// </summary>
        /// <param name='tid'>Thread identifier to check if a value exists for.</param>
        /// <returns>true if TLS value exists for the thread specified.</returns>
        bool exists(dword_t tid = GetCurrentThreadId()) const
        {
            GUARD(tls_access)
            {
                return tls_map.find(tid) != tls_map.end();
            }
        }

        /// <summary>
        /// Checks if a TLS value exists for the calling thread.
        /// </summary>
        /// <returns>true if TLS value exists for the calling thread.</returns>
        operator bool () const
        { return exists(); }
    };

} // namespace WCP_NS


#undef __TLS_H_CYCLE__
#endif /*__TLS_H__*/
