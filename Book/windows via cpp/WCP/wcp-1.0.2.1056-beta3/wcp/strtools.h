#ifdef ___STRTOOLS_H_CYCLE__
#error Cyclic dependency discovered: strtools.h
#endif

#ifndef __STRTOOLS_H__


#ifndef DOXYGEN

#define __STRTOOLS_H__
#define __STRTOOLS_H_CYCLE__

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
///      11/18/2009 - Initial release.
///      12/15/2009 - Trimming of C strings added.
///
/////////////////////////////////////////////////////////////////////////////////

#endif /*DOXYGEN*/

/** \file
    Contains a set of string tools.
*/


#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <locale>


#include "errh.h"



namespace WCP_NS
{
/// <summary>
/// Maps a character string to a wide character (Unicode UTF-16) string.
/// </summary>
/// <param name='mbs'>Multi-byte string.</param>
/// <param name='wcs'>Output. Wide character string.</param>
/// <param name='codePage'>Code page flag. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is <kbd>CP_ACP</kbd>.</param>
/// <param name='flags'>Conversion flags. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is 0.</param>
/// <returns>Converted string %reference.</returns>
std::wstring& mbs2wcs(const char* mbs,
                      __out std::wstring& wcs,
                      uint_t codePage = CP_ACP,
                      dword_t flags = 0)
{
    _ASSERTE("Invalid string pointer specified." && mbs);

    if(*mbs == '\0')
        wcs.clear();
    else
    {
        int srccch = (int)strlen(mbs);
        wchar_t ch;
        int cch = MultiByteToWideChar(codePage, flags, mbs, srccch, &ch, 0);
        if(cch == 0)
            throw win_exception(GetLastError());
        else
        {
            wcs.resize(cch);
            cch = MultiByteToWideChar(codePage, flags, mbs, srccch, &*wcs.begin(), cch);
            if(cch == 0)
                throw win_exception(GetLastError());
        }
    }

    return wcs;
}

/// <summary>
/// Maps a character string to a wide character (Unicode UTF-16) string.
/// </summary>
/// <param name='mbs'>Multi-byte string.</param>
/// <param name='codePage'>Code page flag. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is <kbd>CP_ACP</kbd>.</param>
/// <param name='flags'>Conversion flags. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is 0.</param>
/// <returns>Converted string.</returns>
std::wstring mbs2wcs(const char* mbs,
                     uint_t codePage = CP_ACP,
                     dword_t flags = 0)
{
    std::wstring wcs;
    return mbs2wcs(mbs, wcs, codePage, flags);
}

/// <summary>
/// Maps a wide character string to a new character string.
/// </summary>
/// <param name='wcs'>Wide character string.</param>
/// <param name='mbs'>Output. Multi-byte string.</param>
/// <param name='codePage'>Code page flag. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is <kbd>CP_ACP</kbd>.</param>
/// <param name='flags'>Conversion flags. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is 0.</param>
/// <param name='defaultChar'>A character to use if a wide character cannot
/// be represented in the specified code page. Default value is 0,
/// which means that a default system character is used.</param>
/// <returns>Converted string %reference.</returns>
std::string& wcs2mbs(const wchar_t* wcs,
                     __out std::string& mbs,
                     uint_t codePage = CP_ACP,
                     dword_t flags = 0,
                     char defaultChar = '\0')
{
    _ASSERTE("Invalid string pointer specified." && wcs);

    if(*wcs == '\0')
        mbs.clear();
    else
    {
        int srccch = (int)wcslen(wcs);
        char ch;
        BOOL defCharUsed = FALSE;
        int cch = WideCharToMultiByte(codePage, flags, wcs, srccch, &ch, 0,
                                      defaultChar ? &defaultChar : NULL, &defCharUsed);
        if(cch == 0)
            throw win_exception(GetLastError());
        else
        {
            mbs.resize(cch);
            cch = WideCharToMultiByte(codePage, flags, wcs, srccch, &*mbs.begin(),
                                      cch, defaultChar ? &defaultChar : NULL, &defCharUsed);
            if(cch == 0)
                throw win_exception(GetLastError());
        }
    }

    return mbs;
}

/// <summary>
/// Maps a wide character string to a new character string.
/// </summary>
/// <param name='wcs'>Wide character string.</param>
/// <param name='codePage'>Code page flag. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is <kbd>CP_ACP</kbd>.</param>
/// <param name='flags'>Conversion flags. Refer to <kbd>MultiByteToWideChar</kbd>
/// for possible values. Default value is 0.</param>
/// <param name='defaultChar'>A character to use if a wide character cannot
/// be represented in the specified code page. Default value is 0,
/// which means that a default system character is used.</param>
/// <returns>Converted string.</returns>
std::string wcs2mbs(const wchar_t* wcs,
                    uint_t codePage = CP_ACP,
                    dword_t flags = 0,
                    char defaultChar = '\0')
{
    std::string mbs;
    return wcs2mbs(wcs, mbs, codePage, flags);
}

/// <summary>
/// Convert a string to lowercase.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to convert.</param>
/// <param name='loc'>Optional. String locale. Default value is a global locale.</param>
template<class T>
inline void tolower(__inout std::basic_string<T>& str,
                    const std::locale& loc = std::locale())
{
    if(!str.empty())
    {
        T* ptr = &*str.begin();
        std::use_facet<std::ctype<T> >(loc).tolower(ptr, ptr + str.size());
    }
}

/// <summary>
/// Convert a string to lowercase.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='pstr'>String to convert.</param>
/// <param name='loc'>Optional. String locale. Default value is a global locale.</param>
template<class T>
inline void tolower(T* pstr,
                    const std::locale& loc = std::locale())
{
    _ASSERTE("Invalid string pointer specified." && pstr);

    if(*pstr)
        std::use_facet<std::ctype<T> >(loc).tolower(
            pstr, pstr + std::char_traits<T>::length(pstr));
}

/// <summary>
/// Convert a string to uppercase.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to convert.</param>
/// <param name='loc'>Optional. String locale. Default value is a global locale.</param>
template<class T>
inline void toupper(__inout std::basic_string<T>& str,
                    const std::locale& loc = std::locale())
{
    if(!str.empty())
    {
        T* ptr = &*str.begin();
        std::use_facet<std::ctype<T> >(loc).toupper(ptr, ptr + str.size());
    }
}

/// <summary>
/// Convert a string to uppercase.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='pstr'>String to convert.</param>
/// <param name='loc'>Optional. String locale. Default value is a global locale.</param>
template<class T>
inline void toupper(T* pstr,
                    const std::locale& loc = std::locale())
{
    _ASSERTE("Invalid string pointer specified." && pstr);

    if(*pstr)
        std::use_facet<std::ctype<T> >(loc).toupper(
            pstr, pstr + std::char_traits<T>::length(pstr));
}

#ifndef DOXYGEN

namespace internals
{
// ATL base64 encoding/decoding implementation.
// Copied from atlenc.h in order to be able to use base64
// even if ATL is not available.

#ifndef ATL_BASE64_FLAG_NONE
#define ATL_BASE64_FLAG_NONE    0
#endif

#ifndef ATL_BASE64_FLAG_NOPAD
#define ATL_BASE64_FLAG_NOPAD   1
#endif

#ifndef ATL_BASE64_FLAG_NOCRLF
#define ATL_BASE64_FLAG_NOCRLF  2
#endif

inline size_t Base64EncodeGetRequiredLength(size_t nSrcLen,
        dword_t dwFlags = ATL_BASE64_FLAG_NONE)
{
    int64_t nSrcLen4=static_cast<int64_t>(nSrcLen)*4;

    size_t nRet = static_cast<size_t>(nSrcLen4/3);

    if ((dwFlags & ATL_BASE64_FLAG_NOPAD) == 0)
        nRet += nSrcLen % 3;

    size_t nCRLFs = nRet / 76 + 1;
    size_t nOnLastLine = nRet % 76;

    if (nOnLastLine)
    {
        if (nOnLastLine % 4)
            nRet += 4-(nOnLastLine % 4);
    }

    nCRLFs *= 2;

    if ((dwFlags & ATL_BASE64_FLAG_NOCRLF) == 0)
        nRet += nCRLFs;

    return nRet;
}

template<class CH>
inline void Base64Encode(const byte_t *pbSrcData,
                         size_t nSrcLen,
                         CH* szDest,
                         size_t *pnDestLen,
                         dword_t dwFlags = ATL_BASE64_FLAG_NONE)
{
    static const CH s_chBase64EncodingTable[64] =
    {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
        'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
        'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };

    size_t nWritten( 0 );
    size_t nLen1( (nSrcLen/3)*4 );
    size_t nLen2( nLen1/76 );
    size_t nLen3( 19 );

    for (size_t i=0; i<=nLen2; i++)
    {
        if (i==nLen2)
            nLen3 = (nLen1%76)/4;

        for (size_t j=0; j<nLen3; j++)
        {
            dword_t dwCurr(0);
            for (size_t n=0; n<3; n++)
            {
                dwCurr |= *pbSrcData++;
                dwCurr <<= 8;
            }
            for (size_t k=0; k<4; k++)
            {
                dword_t b = (dword_t)(dwCurr>>26);
                *szDest++ = s_chBase64EncodingTable[b];
                dwCurr <<= 6;
            }
        }
        nWritten+= nLen3*4;

        if ((dwFlags & ATL_BASE64_FLAG_NOCRLF)==0)
        {
            *szDest++ = '\r';
            *szDest++ = '\n';
            nWritten+= 2;
        }
    }

    if (nWritten && (dwFlags & ATL_BASE64_FLAG_NOCRLF)==0)
    {
        szDest-= 2;
        nWritten -= 2;
    }

    nLen2 = (nSrcLen%3) ? (nSrcLen%3 + 1) : 0;
    if (nLen2)
    {
        dword_t dwCurr(0);
        for (size_t n=0; n<3; n++)
        {
            if (n<(nSrcLen%3))
                dwCurr |= *pbSrcData++;
            dwCurr <<= 8;
        }
        for (size_t k=0; k<nLen2; k++)
        {
            dword_t b = (dword_t)(dwCurr>>26);
            *szDest++ = s_chBase64EncodingTable[b];
            dwCurr <<= 6;
        }
        nWritten+= nLen2;
        if ((dwFlags & ATL_BASE64_FLAG_NOPAD)==0)
        {
            nLen3 = nLen2 ? 4-nLen2 : 0;
            for (size_t j=0; j<nLen3; j++)
            {
                *szDest++ = '=';
            }
            nWritten+= nLen3;
        }
    }

    *pnDestLen = nWritten;
}

inline size_t DecodeBase64Char(int ch)
{
    // returns -1 if the character is invalid
    // or should be skipped
    // otherwise, returns the 6-bit code for the character
    // from the encoding table
    if (ch >= 'A' && ch <= 'Z')
        return ch - 'A' + 0;    // 0 range starts at 'A'
    if (ch >= 'a' && ch <= 'z')
        return ch - 'a' + 26;   // 26 range starts at 'a'
    if (ch >= '0' && ch <= '9')
        return ch - '0' + 52;   // 52 range starts at '0'
    if (ch == '+')
        return 62;
    if (ch == '/')
        return 63;

    return ~0;
}

template<class CH>
inline bool Base64Decode(const CH* szSrc,
                         size_t nSrcLen,
                         byte_t *pbDest,
                         size_t *pnDestLen)
{
    // walk the source buffer
    // each four character sequence is converted to 3 bytes
    // CRLFs and =, and any characters not in the encoding table
    // are skiped

    if (szSrc == NULL || pnDestLen == NULL)
        return false;

    const CH* szSrcEnd = szSrc + nSrcLen;
    size_t nWritten = 0;

    bool bOverflow = (pbDest == NULL) ? true : false;

    while (szSrc < szSrcEnd &&(*szSrc) != 0)
    {
        dword_t dwCurr = 0;
        size_t i;
        size_t nBits = 0;
        for (i=0; i<4; i++)
        {
            if (szSrc >= szSrcEnd)
                break;
            size_t nCh = DecodeBase64Char(*szSrc);
            szSrc++;
            if (nCh == ~0)
            {
                // skip this char
                i--;
                continue;
            }
            dwCurr <<= 6;
            dwCurr |= nCh;
            nBits += 6;
        }

        if(!bOverflow && nWritten + (nBits/8) > (*pnDestLen))
            bOverflow = true;

        // dwCurr has the 3 bytes to write to the output buffer
        // left to right
        dwCurr <<= 24-nBits;
        for (i=0; i<nBits/8; i++)
        {
            if(!bOverflow)
            {
                *pbDest = (byte_t) ((dwCurr & 0x00ff0000) >> 16);
                pbDest++;
            }
            dwCurr <<= 8;
            nWritten++;
        }

    }

    *pnDestLen = nWritten;

    return !bOverflow;
}

} // namespace internals

#endif /*DOXYGEN*/

/// <summary>
/// Converts a binary %buffer to Base64 encoded string.
/// </summary>
/// <typeparam name='T'>Output string element type.</typeparam>
/// <param name='p'>Pointer to a binary %buffer.</param>
/// <param name='cb'>Size in bytes of input %buffer.</param>
/// <param name='s'>Reference to output base64 string.</param>
/// <returns>Encoded string length.</returns>
template<class T>
inline size_t tobase64(const void* p,
                       size_t cb,
                       __out std::basic_string<T>& s)
{
    _ASSERTE("Invalid memory pointer specified." && p);

    if(p == NULL || cb == 0)
        s.clear();
    {
        size_t len = internals::Base64EncodeGetRequiredLength(cb);
        if(len == 0)
            s.clear();
        else
        {
            s.resize(len);
            internals::Base64Encode((const byte_t*)p, cb, &*s.begin(), &len,
                                    ATL_BASE64_FLAG_NOPAD | ATL_BASE64_FLAG_NOCRLF);
        }
    }

    return s.size();
}

/// <summary>
/// Decodes Base64 encoded string.
/// </summary>
/// <typeparam name='T'>Source string element type.</typeparam>
/// <param name='base64'>Base64 input string.</param>
/// <param name='buff'>Output vector %reference.</param>
template<class T>
inline size_t frombase64(const T* base64,
                         __out std::vector<byte_t>& buff)
{
    _ASSERTE("Invalid string pointer specified." && base64);

    if(base64 == NULL || *base64 == T('\0'))
        buff.clear();
    else
    {
        size_t cch = std::char_traits<T>::length(base64);
        buff.resize(cch);
        if(!internals::Base64Decode(base64, cch, &buff.front(), &cch))
            throw invalid_argument_exception(TEXT("base64"));
        buff.resize(cch);
    }

    return buff.size();
}

/// <summary>
/// Decodes Base64 encoded string.
/// </summary>
/// <typeparam name='T'>Source string element type.</typeparam>
/// <param name='base64'>Base64 input string.</param>
/// <param name='buff'>Output vector %reference.</param>
template<class T>
inline size_t frombase64(const std::basic_string<T>& base64,
                         __out std::vector<byte_t>& buff)
{
    if(base64.empty())
        buff.clear();
    else
    {
        size_t cch = base64.size();
        buff.resize(cch);
        if(!internals::Base64Decode(base64.c_str(), cch, &buff.front(), &cch))
            throw invalid_argument_exception(TEXT("base64"));
        buff.resize(cch);
    }

    return buff.size();
}

/// <summary>
/// Transforms string via ROT-13 symmertrical algorithm.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to stransform.</param>
/// <returns>Transformed string pointer.</returns>
template<class T>
inline T* rot13(__inout T* str)
{
    _ASSERTE("Invalid string pointer specified." && str);

    while(*str)
    {
        int ch = *str;

        if(ch >= 'a' && ch <= 'z')
        {
            if((ch += 13) > 'z')
                ch = 'a' + (ch - 'z') - 1;
        }
        else if(ch >= 'A' && ch <= 'Z')
        {
            if((ch += 13) > 'Z')
                ch = 'A' + (ch - 'Z') - 1;
        }

        *str++ = (T)ch;
    }

    return str;
}

/// <summary>
/// Transforms string via ROT-13 symmertrical algorithm.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to stransform.</param>
/// <returns>Transformed string %reference.</returns>
template<class T>
inline std::basic_string<T>& rot13(__inout std::basic_string<T>& str)
{
    rot13(const_cast<T*>(str.c_str()));
    return str;
}

/// <summary>
/// STL I/O manipulator function type.
/// </summary>
typedef std::ios_base& (*iomanipulator_t)(std::ios_base&);

/// <summary>
/// Reads an object from a string stream.
/// </summary>
/// <typeparam name='CH'>String element type.</typeparam>
/// <typeparam name='T'>Type of object to read.</typeparam>
/// <param name='str'>String to read an object from.</param>
/// <param name='obj'>Object read.</param>
/// <param name='manips'>Optional. Pointer to an arrays of STL I/O manipulators.</param>
/// <param name='cmanips'>Optional. Nnumber of STL I/O manipulators in manips array.</param>
/// <returns>true if an object was succesfully read from a string stream.</returns>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'>
///     int vi = 0;
///     wcp::iomanipulator_t manips[] = { std::hex, std::boolalpha };
/// &nbsp;
///     std::string str = "0x10";
///     wcp::str2obj(str, vi, manips, ARRAYSIZE(manips)); // vi equals 16 now.
/// &nbsp;
///     bool vb = false;
///     str = "true";
///     wcp::str2obj(str, vb, manips, ARRAYSIZE(manips)); // vb equals 'true' now.
/// </pre></code>
/// </remarks>
template<class CH, class T>
bool str2obj(const std::basic_string<CH>& str,
             __out T& obj,
             __in_opt const iomanipulator_t* manips,
             __in_opt size_t cmanips)
{
    _ASSERTE("Invalid IO manipulators array pointer specified." \
             && (cmanips > 0 ? manips != NULL : true));

    typedef std::basic_istringstream<CH> isstm_t;

    isstm_t istm(str);
    if(manips && cmanips)
        for(size_t i = 0; i < cmanips; ++i)
            istm >>manips[i];

    istm >>obj;
    return !istm.fail();
}

/// <summary>
/// Writes an object to a string stream.
/// </summary>
/// <typeparam name='CH'>String element type.</typeparam>
/// <typeparam name='T'>Type of object to read.</typeparam>
/// <param name='obj'>Object to write.</param>
/// <param name='str'>String to write an object to.</param>
/// <param name='manips'>Optional. Pointer to an arrays of STL I/O manipulators.</param>
/// <param name='cmanips'>Optional. Nnumber of STL I/O manipulators in manips array.</param>
/// <remarks>
/// <b>Example:</b>
/// <code><pre class='_code'>
///     wcp::iomanipulator_t manips[] = { std::hex, std::boolalpha, std::uppercase };
/// &nbsp;
///     std::string str;
///     wcp::obj2str(11259375L, str, manips, ARRAYSIZE(manips)); // str equals ABCDEF.
///     wcp::obj2str(true, str, manips, ARRAYSIZE(manips)); // str equals 'true'.
///     wcp::obj2str(3.14159f, str, manips, ARRAYSIZE(manips)); // str equals '3.14159'.
/// </pre></code>
/// </remarks>
template<class CH, class T>
void obj2str(const T& obj,
             std::basic_string<CH>& str,
             __in_opt const iomanipulator_t* manips,
             __in_opt size_t cmanips)
{
    _ASSERTE("Invalid IO manipulators array pointer specified." \
             && (cmanips > 0 ? manips != NULL : true));

    std::basic_ostringstream<CH> ostm;
    if(manips && cmanips)
        for(size_t i = 0; i < cmanips; ++i)
            ostm <<manips[i];

    ostm <<obj;
    str = ostm.str();
}

/// <summary>
/// Reads an object from a string stream.
/// </summary>
/// <typeparam name='CH'>String element type.</typeparam>
/// <typeparam name='T'>Type of object to read.</typeparam>
/// <param name='str'>String to read an object from.</param>
/// <param name='obj'>Object read.</param>
/// <param name='manips'>Optional. Pointer to an arrays of STL I/O manipulators.</param>
/// <param name='cmanips'>Optional. Nnumber of STL I/O manipulators in manips array.</param>
/// <returns>true if an object was succesfully read from a string stream.</returns>
template<class CH, class T>
bool str2obj(const CH* str,
             __out T& obj,
             __in_opt const iomanipulator_t* manips,
             __in_opt size_t cmanips)
{
    return str2obj(std::basic_string<CH>(str), obj, manips, cmanips);
}

/// <summary>
/// Reads an object from a string stream.
/// </summary>
/// <typeparam name='CH'>String element type.</typeparam>
/// <typeparam name='T'>Type of object to read.</typeparam>
/// <param name='str'>String to read an object from.</param>
/// <param name='obj'>Object read.</param>
/// <returns>true if an object was succesfully read from a string stream.</returns>
template<class CH, class T>
bool str2obj(const std::basic_string<CH>& str,
             __out T& obj)
{
    return str2obj(str, obj, NULL, 0);
}

/// <summary>
/// Writes an object to a string stream.
/// </summary>
/// <typeparam name='CH'>String element type.</typeparam>
/// <typeparam name='T'>Type of object to read.</typeparam>
/// <param name='obj'>Object to write.</param>
/// <param name='str'>String to write an object to.</param>
template<class CH, class T>
void obj2str(const T& obj,
             std::basic_string<CH>& str)
{
    return obj2str(obj, str, NULL, 0);
}

/// <summary>
/// Reads an object from a string stream.
/// </summary>
/// <typeparam name='CH'>String element type.</typeparam>
/// <typeparam name='T'>Type of object to read.</typeparam>
/// <param name='str'>String to read an object from.</param>
/// <param name='obj'>Object read.</param>
/// <returns>true if an object was succesfully read from a string stream.</returns>
template<class CH, class T>
bool str2obj(const CH* str,
             __out T& obj)
{
    return str2obj(std::basic_string<CH>(str), obj, NULL, 0);
}

/// <summary>
/// Normalizes a string.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to normalize.</param>
/// <param name='loc'>String locale. Default value is set to global locale.</param>
/// <returns>Length of normalized string.</returns>
/// <remarks>
/// Normalization means trimming spaces at the beginning and at the
/// end of the string, and replacing sequential groups of spaces inside
/// a string to one space. Refer to a documentation of std::isspace
/// to determine what symbols are treated as spaces.
/// </remarks>
template<class T>
size_t normalize(__inout std::basic_string<T>& str,
                 const std::locale& loc = std::locale())
{
    struct is
    {
        static bool space(T ch, const std::locale* loc)
        {
            return std::isspace(ch, *loc);
        }
    };

    std::basic_string<T> outs;
    outs.reserve(str.length());
    typename std::basic_string<T>::iterator ptr = std::find_if(str.begin(),
            str.end(), std::not1(std::bind2nd(std::ptr_fun(is::space), &loc)));
    if(ptr != str.end())
    {
        std::basic_string<T>::iterator end = std::find_if(str.rbegin(),
                                             str.rend(), std::not1(std::bind2nd(std::ptr_fun(is::space), &loc))).base();

        bool space = false;
        for(; ptr != end; ++ptr)
        {
            T ch = *ptr;
            if(!std::isspace(ch, loc))
            {
                space = false;
                outs.push_back(ch);
            }
            else if(!space)
            {
                space = true;
                outs.push_back(T(' '));
            }
        }
    }

    str = outs;
    return str.length();
}

/// <summary>
/// Trims a string from left.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to trim.</param>
/// <param name='loc'>String locale. Default value is set to global locale.</param>
/// <returns>Length of trimmed string.</returns>
/// <remarks>
/// Refer to a documentation of std::isspace
/// to determine what symbols are treated as spaces.
/// </remarks>
template<class T>
size_t trimleft(__inout std::basic_string<T>& str,
                const std::locale& loc = std::locale())
{
    long spc_index = -1;
    const T* ps = str.c_str();
    for(; *ps && std::isspace(*ps, loc); ++ps)
        ++spc_index;

    if(spc_index != -1)
        str.erase(str.begin(), str.begin() + (spc_index + 1));

    return str.size();
}

/// <summary>
/// Trims a string from left.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='ps'>String pointer to trim.</param>
/// <param name='str'>Trimmed string.</param>
/// <param name='loc'>String locale. Default value is set to global locale.</param>
/// <returns>Length of trimmed string.</returns>
/// <remarks>
/// Refer to a documentation of std::isspace
/// to determine what symbols are treated as spaces.
/// </remarks>
template<class T>
size_t trimleft(const T* ps,
                __out std::basic_string<T>& str,
                const std::locale& loc = std::locale())
{
    _ASSERTE("Invalid string pointer specified." && ps);

    for(; *ps; ++ps)
        if(!std::isspace(*ps, loc))
            break;

    str.assign(ps);

    return str.size();
}

/// <summary>
/// Trims a string from right.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to trim.</param>
/// <param name='loc'>String locale. Default value is set to global locale.</param>
/// <returns>Length of trimmed string.</returns>
/// <remarks>
/// Refer to a documentation of std::isspace
/// to determine what symbols are treated as spaces.
/// </remarks>
template<class T>
size_t trimright(__inout std::basic_string<T>& str,
                 const std::locale& loc = std::locale())
{
    long spc_index = -1;
    if(!str.empty())
    {
        const T* ps = str.c_str();
        const T* p = ps + str.length() - 1;
        for(; p >= ps && std::isspace(*p, loc); --p)
            ++spc_index;

        if(spc_index != -1)
            str.erase((str.rbegin() + (spc_index + 1)).base(), str.rbegin().base());
    }

    return str.size();
}

/// <summary>
/// Trims a string from right.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='ps'>String to trim.</param>
/// <param name='str'>Trimmed string.</param>
/// <param name='loc'>String locale. Default value is set to global locale.</param>
/// <returns>Length of trimmed string.</returns>
/// <remarks>
/// Refer to a documentation of std::isspace
/// to determine what symbols are treated as spaces.
/// </remarks>
template<class T>
size_t trimright(const T* ps,
                 __out std::basic_string<T>& str,
                 const std::locale& loc = std::locale())
{
    _ASSERTE("Invalid string pointer specified." && ps);

    size_t cch = std::char_traits<T>::length(ps);
    if(cch == 0)
        str.clear();
    else
    {
        const T* pe = cch - 1;
        for(; pe >= ps; --pe)
            if(!std::isspace(pe[-1], loc))
                break;

        str.assign(ps, pe);
    }

    return str.size();
}

/// <summary>
/// Trims a string from left and right.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>String to trim.</param>
/// <param name='loc'>String locale. Default value is set to global locale.</param>
/// <returns>Length of trimmed string.</returns>
/// <remarks>
/// Refer to a documentation of std::isspace
/// to determine what symbols are treated as spaces.
/// </remarks>
template<class T>
size_t trim(__inout std::basic_string<T>& str,
            const std::locale& loc = std::locale())
{
    trimleft(str);
    return trimright(str);
}

/// <summary>
/// Trims a string from left and right.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='ps'>Pointer to the beginning of the string to trim.</param>
/// <param name='pe'>Pointer to the end of the string to trim.</param>
/// <param name='str'>Trimmed string.</param>
/// <param name='loc'>String locale. Default value is set to global locale.</param>
/// <returns>Length of trimmed string.</returns>
/// <remarks>
/// Refer to a documentation of std::isspace
/// to determine what symbols are treated as spaces.
/// </remarks>
template<class T>
size_t trim(const T* ps,
            const T* pe,
            __out std::basic_string<T>& str,
            const std::locale& loc = std::locale())
{
    _ASSERTE("Invalid string pointer specified." && (ps && pe));
    _ASSERTE("String start pointer cannot be greater string end epointer." && ps <= pe);

    size_t cch = 0;

    if(ps < pe)
    {
        for(; *ps; ++ps)
            if(!std::isspace(*ps, loc))
                break;

        for(; pe >= ps; --pe)
            if(!std::isspace(pe[-1], loc))
                break;

        str.assign(ps, pe);
    }
    else if(ps == pe)
        str.clear();

    return cch;
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='substrings'>Array of strings.</param>
/// <param name='n'>Number of strings in array of strings.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
/// <remarks>
/// The function performs a string formatting in the same manner as .Net class
/// <kbd>StringBuilder</kbd> does. Format patter is a string which contains an optional
/// number of substring indices surrounded by curly braces: <kbd>"Hello {0}!"</kbd> -
/// a substring with index 0 will be inserted instead of <kbd>{0}</kbd>. To specify
/// a curly brace character, use double curly braces: <kbd>"A = {{x, y, z}};"</kbd>.
/// <br><br><b>Example:</b>
/// <code><pre class='_code'>
///     const char* substrings[] = { "Alexander", "25" };
/// &nbsp;
///     std::string formattedString;
/// &nbsp;
///     wcp::format_string(formattedString, "My name is {0}, I'm {1}.",
///         substrings, ARRAYSIZE(substrings));
///     std::cout &lt;&lt;formattedString &lt;&lt;std::endl;
/// &nbsp;
///     wcp::format_string(formattedString, "Hi! I'm {0}.",
///         substrings, ARRAYSIZE(substrings));
///     std::cout &lt;&lt;formattedString &lt;&lt;std::endl;
/// &nbsp;
///     wcp::format_string(formattedString, "I'm {1}.",
///         substrings, ARRAYSIZE(substrings));
///     std::cout &lt;&lt;formattedString &lt;&lt;std::endl;
/// </pre></code>
/// Output is:
/// <code><pre class='_code'>
///     My name is Alexander, I'm 25.
///     Hi! I'm Alexander.
///     I'm 25.
/// </pre></code>
/// </remarks>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T** substrings,
                          size_t n)
{
    _ASSERTE("Invalid string pointer specified." && format);
    _ASSERTE("Invalid substring array pointer specified." && (n > 0 && substrings != NULL));

    std::basic_ostringstream<T> ostm;

    const T* p = format;
    const T* pi = NULL;
    const T* ps = p;
    for(; *p; ++p)
    {
        switch(*p)
        {
        case '{':
        {
            if(ps)
            {
                size_t cch = p - ps;
                if(cch)
                {
                    ostm <<std::basic_string<T>(ps, p);
                    ps = NULL;
                }
            }

            switch(*(p + 1))
            {
            case 0:
                return false; // Unexpected end.
            case '{':
                ostm <<*p++;
                ps = p + 1;
                break;
            default:
                if(pi) // Non-closed curly brace found.
                    return false;
                pi = p + 1;
                ps = NULL;
                break;
            }
        }
        break;
        case '}':
        {
            T next_ch = *(p + 1);
            if(next_ch == '}' && !pi)
            {
                size_t cch = p - ps;
                if(cch)
                    ostm <<std::basic_string<T>(ps, p);

                ostm <<*p++;
                ps = p + 1;
            }
            else if(pi == NULL)
                return false; // Non-opened curly brace found.
            else
            {
                if(ps)
                    return false; // Unexpected '}'.

                size_t cch = p - pi;
                if(cch == 0)
                    return false; // Invalid syntax '{}'.

                const T* pch = pi;
                bool has_value = false, out_of_value = false;
                for(; pch != p; ++pch)
                {
                    int is_dig = isdigit(*pch);
                    int is_spc = isspace(*pch);
                    if(!is_dig && !is_spc)
                        return false; // Invalid string index value.

                    if(is_dig)
                    {
                        if(out_of_value)
                            return false; // Invalid syntax {X  Y}
                        has_value = true;
                    }
                    else if(is_spc && has_value)
                        out_of_value = true;
                }

                std::basic_string<T> index_str(pi, p);
                size_t index;
                if(!str2obj(index_str, index))
                    return false; // Unable to obtain string index.

                if(index >= n)
                    return false; // Invalid string index.

                _ASSERTE("Invalid string pointer specified." && substrings[index]);
                if(!substrings[index])
                    return false; // Invalid string pointer.

                ostm <<substrings[index];

                ps = p + 1;
                pi = NULL;
            }
        }
        break;
        }
    }

    if(ps && *ps)
        ostm <<ps;

    str = ostm.str();

    return true;
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0)
{
    return format_string(str, format, &s0, 1);
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1)
{
    const T* v[] = { s0, s1 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2)
{
    const T* v[] = { s0, s1, s2 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <param name='s3'>Substring with index 3.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2,
                          const T* s3)
{
    const T* v[] = { s0, s1, s2, s3 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <param name='s3'>Substring with index 3.</param>
/// <param name='s4'>Substring with index 4.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2,
                          const T* s3,
                          const T* s4)
{
    const T* v[] = { s0, s1, s2, s3, s4 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <param name='s3'>Substring with index 3.</param>
/// <param name='s4'>Substring with index 4.</param>
/// <param name='s5'>Substring with index 5.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2,
                          const T* s3,
                          const T* s4,
                          const T* s5)
{
    const T* v[] = { s0, s1, s2, s3, s4, s5 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <param name='s3'>Substring with index 3.</param>
/// <param name='s4'>Substring with index 4.</param>
/// <param name='s5'>Substring with index 5.</param>
/// <param name='s6'>Substring with index 6.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2,
                          const T* s3,
                          const T* s4,
                          const T* s5,
                          const T* s6)
{
    const T* v[] = { s0, s1, s2, s3, s4, s5, s6 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <param name='s3'>Substring with index 3.</param>
/// <param name='s4'>Substring with index 4.</param>
/// <param name='s5'>Substring with index 5.</param>
/// <param name='s6'>Substring with index 6.</param>
/// <param name='s7'>Substring with index 7.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2,
                          const T* s3,
                          const T* s4,
                          const T* s5,
                          const T* s6,
                          const T* s7)
{
    const T* v[] = { s0, s1, s2, s3, s4, s5, s6, s7 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <param name='s3'>Substring with index 3.</param>
/// <param name='s4'>Substring with index 4.</param>
/// <param name='s5'>Substring with index 5.</param>
/// <param name='s6'>Substring with index 6.</param>
/// <param name='s7'>Substring with index 7.</param>
/// <param name='s8'>Substring with index 8.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2,
                          const T* s3,
                          const T* s4,
                          const T* s5,
                          const T* s6,
                          const T* s7,
                          const T* s8)
{
    const T* v[] = { s0, s1, s2, s3, s4, s5, s6, s7, s8 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

/// <summary>
/// Performs a string formation according to the pattern specified in a .Net-like manner.
/// </summary>
/// <typeparam name='T'>String element type.</typeparam>
/// <param name='str'>Output. Formatted string.</param>
/// <param name='format'>Format pattern.</param>
/// <param name='s0'>Substring with index 0.</param>
/// <param name='s1'>Substring with index 1.</param>
/// <param name='s2'>Substring with index 2.</param>
/// <param name='s3'>Substring with index 3.</param>
/// <param name='s4'>Substring with index 4.</param>
/// <param name='s5'>Substring with index 5.</param>
/// <param name='s6'>Substring with index 6.</param>
/// <param name='s7'>Substring with index 7.</param>
/// <param name='s8'>Substring with index 8.</param>
/// <param name='s9'>Substring with index 9.</param>
/// <returns>true is a string is sucessfully formatted.</returns>
template<class T>
inline bool format_string(__out std::basic_string<T>& str,
                          const T* format,
                          const T* s0,
                          const T* s1,
                          const T* s2,
                          const T* s3,
                          const T* s4,
                          const T* s5,
                          const T* s6,
                          const T* s7,
                          const T* s8,
                          const T* s9)
{
    const T* v[] = { s0, s1, s2, s3, s4, s5, s6, s7, s8, s9 };
    return format_string(str, format, v, _ARRAYSIZE(v));
}

} // namespace WCP_NS



#undef __STRTOOLS_H_CYCLE__
#endif /*__STRTOOLS_H__*/
