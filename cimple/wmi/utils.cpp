/*
**==============================================================================
**
** Copyright (c) 2003, 2004, 2005, 2006, Michael Brasher, Karl Schopmeyer
** 
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**
**==============================================================================
*/

#include "utils.h"
#include <initguid.h>

#ifdef __GNUC__
extern "C" 
{
DEFINE_GUID(IID_IWbemServices, 0x9556DC99, 0x828C, 0x11CF, 0xA3, 0x7E, 0x00, 0xAA, 0x00, 0x32, 0x40, 0xC7);
DEFINE_GUID(IID_IWbemLocator, 0xdc12a687, 0x737f, 0x11cf, 0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24);
DEFINE_GUID(CLSID_WbemLocator, 0x4590f811, 0x1d3a, 0x11d0, 0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24);
DEFINE_GUID(IID_IWbemEventProvider, 0xe245105b, 0xb06e, 0x11d0, 0xad, 0x61, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff);
DEFINE_GUID(IID_IWbemEventProviderQuerySink, 0x580acaf8, 0xfa1c, 0x11d0, 0xad, 0x72, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff);
DEFINE_GUID(IID_IWbemProviderInit, 0x1be41572, 0x91dd, 0x11d1, 0xae, 0xb2, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20);
DEFINE_GUID(IID_IWbemUnboundObjectSink, 0xe246107b, 0xb06e, 0x11d0, 0xad, 0x61, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff);

DEFINE_GUID(IID_IWbemHiPerfProvider, 0x49353c93, 0x516b, 0x11d1, 0xae, 0xa6, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20);
DEFINE_GUID(IID_IWbemEventConsumerProvider, 0xe246107a, 0xb06e, 0x11d0, 0xad, 0x61, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff);
DEFINE_GUID(IID_IWbemEventProviderSecurity, 0x631f7d96, 0xd993, 0x11d2, 0xb3, 0x39, 0x00, 0x10, 0x5a, 0x1f, 0x4a, 0xaf);
DEFINE_GUID(IID_IWbemPropertyProvider, 0xCE61E841, 0x65BC, 0x11d0, 0xb6, 0xbd, 0x00, 0xAA, 0x00, 0x32, 0x40, 0xC7);
};
#endif

CIMPLE_NAMESPACE_BEGIN

String bstr2str(const BSTR bstr)
{
    UINT blen = SysStringLen(bstr);
    UINT alen = SysStringByteLen(bstr);
    char buf[1024];
    char* astr;

    if (alen + 1 > sizeof(buf))
        astr = (char*)operator new(alen + 1);
    else
        astr = buf;

    memset(astr, 0, alen + 1);
    WideCharToMultiByte(CP_ACP, 0, bstr, blen, astr, alen, NULL, NULL);

    String result = astr;

    if (astr != buf)
        operator delete(astr);

    return result;
}

BSTR str2bstr(const String& str)
{
    size_t n = lstrlenA(str.c_str());
    BSTR bstr = SysAllocStringLen(NULL, n);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), n, bstr, n);
    return bstr;
}

String wstr2str(LPWSTR str)
{
    char buf[1024];
    char* astr;

    // Get length of required buffer (including null terminator).

    int alen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

    if (alen <= (int)sizeof(buf))
        astr = buf;
    else
        astr = (char*)::operator new(alen);

    // Convert string:

    WideCharToMultiByte(CP_ACP, 0, str, -1, astr, alen, NULL, NULL);

    String result(astr);

    if (astr != buf)
        operator delete(astr);

    return result;
}

BSTR cstr2bstr(const char* cstr)
{
    size_t n = lstrlenA(cstr);
    BSTR bstr = SysAllocStringLen(NULL, n);
    MultiByteToWideChar(CP_ACP, 0, cstr, n, bstr, n);
    return bstr;
}

char* bstr2cstr(const BSTR bstr)
{
    UINT blen = SysStringLen(bstr);
    UINT clen = SysStringByteLen(bstr);
    char* cstr = new char[clen + 1];

    if (!cstr)
        return 0;

    memset(cstr, 0, clen + 1);
    WideCharToMultiByte(CP_ACP, 0, bstr, blen, cstr, clen, NULL, NULL);

    return cstr;
}

CIMPLE_NAMESPACE_END
