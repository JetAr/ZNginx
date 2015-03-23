#ifdef ____CORE_H_CYCLE__
#error Cyclic dependency discovered: _core.h
#endif

#ifndef ___CORE_H__


#ifndef DOXYGEN

#define ___CORE_H__
#define ___CORE_H_CYCLE__

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
///      11/22/2009 - Static core initialization added.
///      12/13/2009 - add_exit_handler global routine was added.
///                   init_library routine added.
///      12/15/2009 - Separated from wcplib.h.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Implements WCP library core.

    To use this file include wcp/wcplib.h.
*/

// STL headers
#include <list>

// WCP headers
#include "debug.h"
#include "_synchronized.h"
#include "_stinit.h"



namespace WCP_NS 
{

#ifndef DOXYGEN

    namespace internals
    {
        struct wcp_core 
            : public singleton<wcp_core>
        {
            typedef std::list<void(*)()> handlers_t;
            handlers_t handlers;

            void add_exit_handler(void(*handler)())
            {
                SYNCHRONIZED
                {
                    handlers.push_back(handler);
                }
            }

        private:

            friend singleton<wcp_core>;

            wcp_core() throw()
            {
#ifdef WCP_DEBUG_XML
        _WCP_XDBGM("<WCPDebugLog wcp-version=\"" WCP_VERSION_STR "\">")
#endif
            }

            ~wcp_core()
            {
                for(std::list<void(*)()>::iterator iter = handlers.begin(),
                    end = handlers.end(); iter != end; ++iter)
                {
                    (*iter)();
                }

#ifdef WCP_DEBUG_XML
        _WCP_XDBGM("</WCPDebugLog>")
#endif
            }
        };

        STATIC_INITIALIZATION(wcp_core);

    } // namespace internals

#endif /*DOXYGEN*/

    /// <summary>
    /// Ensures that WCP library is initialized. Thread safe.
    /// </summary>
    /// <remarks>
    /// To use this function include wcp/wcplib.h.
    /// </remarks>
    inline void init_library() //test1
    {
        struct _
        {
            static void init()
            {
                SYNCHRONIZED
                {
                    internals::wcp_core::instance();
                }
            }
        };

        static int x = (_::init(), 0);
    }

    /// <summary>
    /// Adds an exit handler.
    /// </summary>
    /// <param name='handler'>Exit handler function pointer.</param>
    /// <remarks>
    /// WCP ensures that all exit handlers will be called on application
    /// termination in the reverse order of addition before WCP is
    /// completely uninitialized.
    /// &nbsp;
    /// To use this function include wcp/wcplib.h.
    /// </remarks>
    inline void add_exit_handler(void(*handler)()) //test2
    {
        internals::wcp_core::instance().add_exit_handler(handler);
    }


} // namespace WCP_NS

#undef ___CORE_H_CYCLE__
#endif /*___CORE_H__*/