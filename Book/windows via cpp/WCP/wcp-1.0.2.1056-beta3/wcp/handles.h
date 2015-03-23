#ifdef ___HANDLES_H_CYCLE__
#error Cyclic dependency discovered: handles.h
#endif

#ifndef __HANDLES_H__


#ifndef DOXYGEN

#define __HANDLES_H__
#define __HANDLES_H_CYCLE__

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
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Windows API handles smart wrappers.
*/


#include "_basic_handle.h"

#ifndef WCP_FORCE_USE_WINHTTP_HANDLES
#include <Wininet.h>
#else
#include <Winhttp.h>
#endif

#include <Userenv.h>
#include <WinCred.h>
#include <Winsock.h>

#if !defined(SECURITY_WIN32) && !defined(SECURITY_KERNEL) && !defined(DOXYGEN)
#define SECURITY_WIN32
#endif

#include <Security.h>
#include <Ntsecapi.h>
#include <Ntsecpkg.h>
#include <Authz.h>
#include <Wincrypt.h>

#ifdef DOXYGEN

/// <summary>
/// Forces WCP to use <kbd>WinHttp</kbd> instead of <kbd>WinInet</kbd>.
/// </summary>
#define WCP_FORCE_USE_WINHTTP_HANDLES

#endif /*DOXYGEN*/



namespace WCP_NS 
{

#ifndef DOXYGEN

    namespace internals
    {
        inline void hlocal_close(HLOCAL& h)
        { ::LocalFree(h); }

        inline void hglobal_close(HGLOBAL& h)
        { ::GlobalFree(h); }

        inline void handle_close(HANDLE& h)
        { ::CloseHandle(h); }

        inline void hfind_close(HANDLE& h)
        { ::FindClose(h); }

        inline void hkey_close(HKEY& h)
        { ::RegCloseKey(h); }

        inline void hdesk_close(HDESK& h)
        { ::CloseDesktop(h); }

        inline void henvironment_close(void*& p)
        { ::DestroyEnvironmentBlock(p); }

        inline void hinternet_close(HINTERNET& h)
        { 
#ifndef WCP_FORCE_USE_WINHTTP_HANDLES
            ::InternetCloseHandle(h); 
#else
            ::WinHttpCloseHandle(h);
#endif
        }

        inline void sc_handle_close(SC_HANDLE& h)
        { ::CloseServiceHandle(h); }

        inline void hwinsta_close(HWINSTA& h)
        { ::CloseWindowStation(h); }

        inline void lzfile_close(int& h)
        { ::LZClose(h); }

        inline void mapviewoffile_close(LPCVOID& p)
        { ::UnmapViewOfFile(p); }

        inline void hmodule_close(HMODULE& h)
        { ::FreeLibrary(h); }

        inline void socket_close(SOCKET& h)
        { ::closesocket(h); }

        inline bool socket_is_valid(const SOCKET& h)
        { return h != INVALID_SOCKET; }

        inline void socket_make_invalid(SOCKET& h)
        { h = INVALID_SOCKET; }

        inline void cred_free(void*& p)
        { ::CredFree(p); }

        inline void lsa_close(LSA_HANDLE& h)
        { ::LsaClose(h); }

        inline void lsabuffer_free(void*& p)
        { ::LsaFreeMemory(p); }

        inline void audit_free(void*& p)
        { ::AuditFree(p); }

        inline void authz_client_context_handle_free(AUTHZ_CLIENT_CONTEXT_HANDLE& h)
        { ::AuthzFreeContext(h); }

        inline void authz_access_check_results_handle_free(AUTHZ_ACCESS_CHECK_RESULTS_HANDLE& h)
        { ::AuthzFreeHandle(h); }

        inline void authz_audit_event_handle_free(AUTHZ_AUDIT_EVENT_HANDLE& h)
        { ::AuthzFreeAuditEvent(h); }

        inline void authz_resource_manager_handle_free(AUTHZ_RESOURCE_MANAGER_HANDLE& h)
        { ::AuthzFreeResourceManager(h); }

        inline void sid_free(PSID& p)
        { ::FreeSid(p); }

#if _WIN32_WINNT >= 0x0600 || defined(DOXYGEN)

        inline void ptp_pool_close(PTP_POOL& p)
        { ::CloseThreadpool(p); }

#endif /*_WIN32_WINNT >= 0x0600 */

        inline void pccert_chain_context_free(PCCERT_CHAIN_CONTEXT& p)
        { ::CertFreeCertificateChain(p); }

        inline void hcertchainengine_free(HCERTCHAINENGINE& h)
        { ::CertFreeCertificateChainEngine(h); }

        inline void pccert_context_free(PCCERT_CONTEXT& p)
        { ::CertFreeCertificateContext(p); }

        inline void pccrl_context_free(PCCRL_CONTEXT& p)
        { ::CertFreeCRLContext(p); }

        inline void pcctl_context_free(PCCTL_CONTEXT& p)
        { ::CertFreeCTLContext(p); }

        inline void crypt_mem_free(void*& p)
        { ::CryptMemFree(p); }

        inline void hcryptmsg_close(HCRYPTMSG& h)
        { ::CryptMsgClose(h); }

        inline void hmenu_destroy(HMENU& h)
        { ::DestroyMenu(h); }

        inline void hicon_destroy(HICON& h)
        { ::DestroyIcon(h); }

        inline void hcursor_destroy(HCURSOR& h)
        { ::DestroyCursor(h); }

        inline void hgdiobj_destroy(HGDIOBJ& h)
        { ::DeleteObject(h); }
    }

#endif /*DOXYGEN*/

    namespace handles
    {
        /// <summary>
        /// <kbd>HLOCAL</kbd> handle wrapper. You must not and cannot pass hkey_t to <kbd>::LocalFree</kbd>.
        /// </summary>
        typedef basic_handle<HLOCAL, internals::hlocal_close> hlocal_t;

        /// <summary>
        /// <kbd>HGLOBAL</kbd> handle wrapper. You must not and cannot pass hkey_t to <kbd>::GlobalFree</kbd>.
        /// </summary>
        typedef basic_handle<HGLOBAL, internals::hglobal_close> hglobal_t;

        /// <summary>
        /// <kbd>HANDLE</kbd> handle wrapper. Note, some WinAPI functions treat <kbd>INVALID_HANDLE_VALUE</kbd>
        /// as an invalid handle value, while some function tread <kbd>NULL</kbd> as invalid handle value.
        /// wcp::handles::handle_t treats <kbd>NULL</kbd> as invalid handle value, so you need to insert an additional
        /// check when you use functions that can return <kbd>INVALID_HANDLE_VALUE</kbd>, such as CreateFile.
        /// In addition, you have to be careful when using <kbd>DuplicateHandle</kbd> with 
        /// <kbd>DUPLICATE_CLOSE_SOURCE</kbd> key, because on destruction the handle closes itself.
        /// You must not and cannot pass wcp::handles::handle_t to <kbd>::CloseHandle</kbd>.
        /// </summary>
        typedef basic_handle<HANDLE, internals::handle_close> handle_t;

        /// <summary>
        /// Find handle wrapper. You must not and cannot pass wcp::handles::hkey_t to <kbd>::FindClose</kbd>.
        /// </summary>
        typedef basic_handle<HANDLE, internals::hfind_close> hfind_t;

        /// <summary>
        /// <kbd>HKEY</kbd> handle wrapper. You must not and cannot pass wcp::handles::hkey_t to <kbd>::RegCloseKey</kbd>.
        /// </summary>
        typedef basic_handle<HKEY, internals::hkey_close> hkey_t;

        /// <summary>
        /// <kbd>HDESK</kbd> handle wrapper. You must not pass and cannot wcp::handles::hdesktop_t to <kbd>::CloseDesktop</kbd>.
        /// </summary>
        typedef basic_handle<HDESK, internals::hdesk_close> hdesktop_t;

        /// <summary>
        /// Environment block handle wrapper. You must not pass and cannot 
        /// wcp::handles::henvironment_t to <kbd>::DestroyEnvironmentBlock</kbd>.
        /// </summary>
        typedef basic_handle<void*, internals::henvironment_close> henvironment_t;

        /// <summary>
        /// <kbd>HINTERNET</kbd> handle wrapper. You must not pass and cannot 
        /// wcp::handles::henvironment_t to <kbd>::InternetCloseHandle</kbd> or <kbd>::WinHttpCloseHandle</kbd> depending
        /// on whether \ref WCP_FORCE_USE_WINHTTP_HANDLES is defined or not.
        /// </summary>
        typedef basic_handle<HINTERNET, internals::hinternet_close> hinternet_t;

        /// <summary>
        /// <kbd>SC_HANDLE</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::sc_handle_t to <kbd>::CloseServiceHandle</kbd>.
        /// </summary>
        typedef basic_handle<SC_HANDLE, internals::sc_handle_close> sc_handle_t;

        /// <summary>
        /// <kbd>HWINSTA</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::hwinsta_t to <kbd>::CloseWindowStation</kbd>.
        /// </summary>
        typedef basic_handle<HWINSTA, internals::hwinsta_close> hwinsta_t;

        /// <summary>
        /// LZ file handle wrapper. You must not and cannot pass 
        /// wcp::handles::hwinsta_t to <kbd>::LZClose</kbd>.
        /// </summary>
        typedef basic_handle<int, internals::lzfile_close> lzfile_t;

        /// <summary>
        /// Map view of file handle wrapper. You must not and cannot pass 
        /// wcp::handles::mapviewoffile_t to <kbd>::UnmapViewOfFile</kbd>.
        /// </summary>
        typedef basic_handle<LPCVOID, internals::mapviewoffile_close> mapviewoffile_t;

        /// <summary>
        /// <kbd>HMODULE</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::hmodule_t to <kbd>::FreeLibrary</kbd>.
        /// </summary>
        typedef basic_handle<HMODULE, internals::hmodule_close> hmodule_t;

        /// <summary>
        /// <kbd>SOCKET</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::socket_t to <kbd>::closesocket</kbd>.
        /// </summary>
        typedef basic_handle<SOCKET, internals::socket_close, 
            internals::socket_is_valid, internals::socket_make_invalid> socket_t;

        /// <summary>
        /// Credentials handle wrapper. You must not and cannot pass 
        /// wcp::handles::credbuffer_t to <kbd>::CredFree</kbd>.
        /// </summary>
        typedef basic_handle<void*, internals::cred_free> credbuffer_t;

        /// <summary>
        /// Credentials handle wrapper. You must not and cannot pass 
        /// wcp::handles::auditbuffer_t to <kbd>::AuditFree</kbd>.
        /// </summary>
        typedef basic_handle<void*, internals::audit_free> auditbuffer_t;

        /// <summary>
        /// <kbd>LSA_HANDLE</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::lsa_hanlde_t to <kbd>::LsaClose</kbd>.
        /// </summary>
        typedef basic_handle<LSA_HANDLE, internals::lsa_close> lsa_handle_t;

        /// <summary>
        /// LSA %buffer pointer wrapper. You must not and cannot pass 
        /// wcp::handles::lsa_hanlde_t to <kbd>::LsaFreeBuffer</kbd>.
        /// </summary>
        typedef basic_handle<void*, internals::lsabuffer_free> lsabuffer_t;

        /// <summary>
        /// <kbd>AUTHZ_CLIENT_CONTEXT_HANDLE</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::authz_client_context_handle_t to <kbd>::AuthzFreeContext</kbd>.
        /// </summary>
        typedef basic_handle<AUTHZ_CLIENT_CONTEXT_HANDLE, internals
            ::authz_client_context_handle_free> authz_client_context_handle_t;

        /// <summary>
        /// <kbd>AUTHZ_ACCESS_CHECK_RESULTS_HANDLE</kbd>  handle wrapper. You must not and cannot pass 
        /// wcp::handles::authz_client_context_handle_t to <kbd>::AuthzFreeHandle</kbd>.
        /// </summary>
        typedef basic_handle<AUTHZ_ACCESS_CHECK_RESULTS_HANDLE, internals
            ::authz_access_check_results_handle_free> authz_access_check_results_handle_t;

        /// <summary>
        /// <kbd>AUTHZ_AUDIT_EVENT_HANDLE</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::authz_client_context_handle_t to <kbd>::AuthzFreeAuditEvent</kbd>.
        /// </summary>
        typedef basic_handle<AUTHZ_AUDIT_EVENT_HANDLE, internals
            ::authz_audit_event_handle_free> authz_audit_event_handle_t;

        /// <summary>
        /// <kbd>AUTHZ_RESOURCE_MANAGER_HANDLE</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::authz_client_context_handle_t to <kbd>::AuthzFreeResourceManager</kbd>.
        /// </summary>
        typedef basic_handle<AUTHZ_RESOURCE_MANAGER_HANDLE, internals
            ::authz_resource_manager_handle_free> authz_resource_manager_handle_t;

        /// <summary>
        /// <kbd>PSID</kbd> wrapper. You must not and cannot pass wcp::handles::psid_t to <kbd>::FreeSid</kbd>.
        /// </summary>
        typedef basic_handle<PSID, internals::sid_free> psid_t;

#if _WIN32_WINNT >= 0x0600 || defined(DOXYGEN) // Vista and later

        /// <summary>
        /// <kbd>PTP_POOL</kbd> handle wrapper. You must not and cannot pass 
        /// wcp::handles::ptp_pool_t to <kbd>::CloseThreadpool</kbd>.
        /// </summary>
        typedef basic_handle<PTP_POOL, internals::ptp_pool_close> ptp_pool_t;

#endif /*_WIN32_WINNT >= 0x0600 */

        /// <summary>
        /// <kbd>PCCERT_CHAIN_CONTEXT</kbd> wrapper. You must not and cannot 
        /// pass wcp::handles::pccert_chain_context_t to <kbd>::CertFreeCertificateChain</kbd>.
        /// </summary>
        typedef basic_handle<PCCERT_CHAIN_CONTEXT, internals::pccert_chain_context_free> pccert_chain_context_t;

        /// <summary>
        /// <kbd>HCERTCHAINENGINE</kbd> wrapper. You must not and cannot 
        /// pass wcp::handles::hcertchainengine_t to <kbd>::CertFreeCertificateChainEngine</kbd>.
        /// </summary>
        typedef basic_handle<HCERTCHAINENGINE, internals::hcertchainengine_free> hcertchainengine_t;

        /// <summary>
        /// <kbd>PCCERT_CONTEXT</kbd> wrapper. You must not and cannot 
        /// pass wcp::handles::pccert_context_t to <kbd>::CertFreeCertificateContext</kbd>.
        /// </summary>
        typedef basic_handle<PCCERT_CONTEXT, internals::pccert_context_free> pccert_context_t;

        /// <summary>
        /// <kbd>PCCRL_CONTEXT</kbd> wrapper. You must not and cannot 
        /// pass wcp::handles::pccrl_context_t to <kbd>::CertFreeCRLContext</kbd>.
        /// </summary>
        typedef basic_handle<PCCRL_CONTEXT, internals::pccrl_context_free> pccrl_context_t;

        /// <summary>
        /// <kbd>PCCTL_CONTEXT</kbd> wrapper. You must not and cannot 
        /// pass wcp::handles::pcctl_context_t to <kbd>::CertFreeCTLContext</kbd>.
        /// </summary>
        typedef basic_handle<PCCTL_CONTEXT, internals::pcctl_context_free> pcctl_context_t;

        /// <summary>
        /// Cryptographic %buffer wrapper. You must not and cannot 
        /// pass wcp::handles::crypt_mem_t to <kbd>::CryptMemFree</kbd>.
        /// </summary>
        typedef basic_handle<void*, internals::crypt_mem_free> crypt_mem_t;

        /// <summary>
        /// <kbd>HCRYPTMSG</kbd> handle wrapper. You must not and cannot 
        /// pass wcp::handles::hcryptmsg_t to <kbd>::CryptMsgClose</kbd>.
        /// </summary>
        typedef basic_handle<HCRYPTMSG, internals::hcryptmsg_close> hcryptmsg_t;

        /// <summary>
        /// <kbd>HMENU</kbd> handle wrapper. You must not and cannot 
        /// pass wcp::handles::hmenu_t to <kbd>::DestroyMenu</kbd>.
        /// </summary>
        typedef basic_handle<HMENU, internals::hmenu_destroy> hmenu_t ;

        /// <summary>
        /// <kbd>HICON</kbd> handle wrapper. You must not and cannot 
        /// pass wcp::handles::hicon_t to <kbd>::DestroyIcon</kbd>.
        /// </summary>
        typedef basic_handle<HICON, internals::hicon_destroy> hicon_t;

        /// <summary>
        /// <kbd>HCURSOR</kbd> handle wrapper. You must not and cannot 
        /// pass wcp::handles::hcursor_t to <kbd>::DestroyCursor</kbd>.
        /// </summary>
        typedef basic_handle<HCURSOR, internals::hcursor_destroy> hcursor_t;

        /// <summary>
        /// <kbd>HGDIOBJ</kbd> handle wrapper. You must not and cannot 
        /// pass wcp::handles::hgdiobj_t to <kbd>::DeleteObject</kbd>.
        /// </summary>
        typedef basic_handle<HGDIOBJ, internals::hgdiobj_destroy> hgdiobj_t;
    }
}

#ifndef DOXYGEN

// Prevents from passing wcp::handles::hlocal_t to ::LocalFree.
void LocalFree(WCP_NS::handles::hlocal_t);

// Prevents from passing wcp::handles::hglobal_t to ::GlobalFree.
void GlobalFree(WCP_NS::handles::hglobal_t);

// Prevents from passing wcp::handles::handle_t to ::CloseHandle.
void CloseHandle(WCP_NS::handles::handle_t);

// Prevents from passing wcp::handles::hfind_t to ::FindClose.
void FindClose(WCP_NS::handles::hfind_t);

// Prevents from passing wcp::handles::hkey_t to ::RegCloseKey.
void RegCloseKey(WCP_NS::handles::hkey_t);

// Prevents from passing wcp::handles::hdesktop_t to ::CloseDesktop.
void CloseDesktop(WCP_NS::handles::hdesktop_t);

// Prevents from passing wcp::handles::henvironment_t to ::DestroyEnvironmentBlock.
void DestroyEnvironmentBlock(WCP_NS::handles::henvironment_t);

#ifndef WCP_FORCE_USE_WINHTTP_HANDLES

// Prevents from passing wcp::handles::hinternet_t to ::InternetCloseHandle.
void InternetCloseHandle(WCP_NS::handles::hinternet_t);

#else

// Prevents from passing wcp::handles::hinternet_t to ::WinHTTPCloseHandle.
void WinHttpCloseHandle(WCP_NS::handles::hinternet_t);

#endif /*WCP_FORCE_USE_WINHTTP_HANDLES*/

// Prevents from passing wcp::handles::sc_handle_t to ::CloseServiceHandle.
void CloseServiceHandle(WCP_NS::handles::sc_handle_t);

// Prevents from passing wcp::handles::hwinsta_t to ::CloseWindowStation.
void CloseWindowStation(WCP_NS::handles::hwinsta_t);

// Prevents from passing wcp::handles::lzfile_t to ::LZClose.
void LZClose(WCP_NS::handles::lzfile_t);

// Prevents from passing wcp::handles::mapviewoffile_t to ::UnmapViewOfFile.
void UnmapViewOfFile(WCP_NS::handles::mapviewoffile_t);

// Prevents from passing wcp::handles::hmodule_t to ::FreeLibrary.
void FreeLibrary(WCP_NS::handles::hmodule_t);

// Prevents from passing wcp::handles::socket_t to ::closesocket.
void closesocket(WCP_NS::handles::socket_t);

// Prevents from passing wcp::handles::credbuffer_t to ::CredFree.
void CredFree(WCP_NS::handles::credbuffer_t);

// Prevents from passing wcp::handles::auditbuffer_t to ::AuditFree.
void AuditFree(WCP_NS::handles::auditbuffer_t);

// Prevents from passing wcp::handles::lsa_handle_t to ::LsaClose.
void LsaClose(WCP_NS::handles::lsa_handle_t);

// Prevents from passing wcp::handles::lsabuffer_t to ::LsaFreeMemory.
void LsaFreeMemory(WCP_NS::handles::lsabuffer_t);

// Prevents from passing wcp::handles::authz_client_context_handle_t to ::AuthzFreeContext.
void AuthzFreeContext(WCP_NS::handles::authz_client_context_handle_t);

// Prevents from passing wcp::handles::authz_access_check_results_handle_t to ::AuthzFreeHandle.
void AuthzFreeHandle(WCP_NS::handles::authz_access_check_results_handle_t);

// Prevents from passing wcp::handles::authz_audit_event_handle_t to ::AuthzFreeAuditEvent.
void AuthzFreeAuditEvent(WCP_NS::handles::authz_audit_event_handle_t);

// Prevents from passing wcp::handles::authz_resource_manager_handle_t to ::AuthzFreeResourceManager.
void AuthzFreeResourceManager(WCP_NS::handles::authz_resource_manager_handle_t);

// Prevents from passing wcp::handles::psid_t to ::FreeSid.
void FreeSid(WCP_NS::handles::psid_t);

#if _WIN32_WINNT >= 0x0600 // Vista and later

// Prevents from passing wcp::handles::ptp_pool_t to ::CloseThreadpool.
void CloseThreadpool(WCP_NS::handles::ptp_pool_t);

#endif /*_WIN32_WINNT >= 0x0600 */

// Prevents from passing wcp::handles::pccert_chain_context_t to ::CertFreeCertificateChain.
void CertFreeCertificateChain(WCP_NS::handles::pccert_chain_context_t);

// Prevents from passing wcp::handles::hcertchainengine_t to ::CertFreeCertificateChainEngine.
void CertFreeCertificateChainEngine(WCP_NS::handles::hcertchainengine_t);

// Prevents from passing wcp::handles::pccert_context_t to ::CertFreeCertificateContext.
void CertFreeCertificateContext(WCP_NS::handles::pccert_context_t);

// Prevents from passing wcp::handles::pccrl_context_t to ::CertFreeCRLContext.
void CertFreeCRLContext(WCP_NS::handles::pccrl_context_t);

// Prevents from passing wcp::handles::pcctl_context_t to ::CertFreeCTLContext.
void CertFreeCTLContext(WCP_NS::handles::pcctl_context_t);

// Prevents from passing wcp::handles::crypt_mem_t to ::CryptMemFree.
void CryptMemFree(WCP_NS::handles::crypt_mem_t);

// Prevents from passing wcp::handles::hcryptmsg_t to ::CryptMsgClose.
void CryptMsgClose(WCP_NS::handles::hcryptmsg_t);

// Prevents from passing wcp::handles::hmenu_t to ::DestroyMenu.
void DestroyMenu(WCP_NS::handles::hmenu_t);

// Prevents from passing wcp::handles::hicon_t to ::DestroyIcon.
void DestroyIcon(WCP_NS::handles::hicon_t);

// Prevents from passing wcp::handles::hcursor_t to ::DestroyCursor.
void DestroyCursor(WCP_NS::handles::hcursor_t);

// Prevents from passing wcp::handles::hgdiobj_t to ::DeleteObject.
void DeleteObject(WCP_NS::handles::hgdiobj_t);

#endif /*DOXYGEN*/


#undef __HANDLES_H_CYCLE__
#endif /*__HANDLES_H__*/
