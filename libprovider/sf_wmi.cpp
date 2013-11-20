/***************************************************************************//*! \file liprovider/sf_wmi.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include <wbemprov.h>
#include <cimple/wmi/BString.h>
#include <cimple/wmi/utils.h>
#include <sf_wmi.h>
#include "sf_logging.h"

namespace solarflare 
{
    using cimple::String;
    using cimple::BString;
    using cimple::bstr2cstr;
    using cimple::wstr2str;
    using cimple::uint8;
    using cimple::uint16;
    using cimple::Array;
    using cimple::Mutex;
    using cimple::Auto_Mutex;

    class WMIThreadContext {
        int            counter;
        bool           comInitialized;
        bool           ok;
    public:
        DWORD          threadId;
        IWbemServices *rootWMIConn;
        IWbemServices *cimWMIConn;

        WMIThreadContext();
        virtual ~WMIThreadContext();

        void inc()
        {
            counter++;
        }
        void dec()
        {
            counter--;
        }

        operator bool() const
        {
            return (counter > 0);
        }

        bool isOk() const
        {
            return ok;
        }
    };

    static int wmiEstablishConnNs(const char *ns,
                                  IWbemServices **svc)
    {
        HRESULT       hr;
        IWbemLocator *wbemLocator = NULL;

        hr = CoCreateInstance(CLSID_WbemLocator, NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IWbemLocator,
                              reinterpret_cast<LPVOID *>(&wbemLocator));
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("CoCreateInstance() failed, rc = %lx", hr);
            return -1;
        }

        hr = wbemLocator->ConnectServer(BString(ns).rep(),
                                        NULL, NULL, NULL, 0, NULL, NULL,
                                        svc);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("ConnectServer() failed, rc = %lx", hr);
            *svc = NULL;
            wbemLocator->Release();
            return -1;
        }

        wbemLocator->Release();
        return 0;
    }

    WMIThreadContext::WMIThreadContext() :
      counter(0), comInitialized(false), ok(false),
      threadId(GetCurrentThreadId()), rootWMIConn(NULL),
      cimWMIConn(NULL)
    {
        HRESULT hr;
        IWbemLocator *wbemLocator = NULL;

        /// Testing whether COM is initialized, initialize it if not
        /// (it is not in case of OpenPegasus)
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if (hr == S_FALSE)
            CoUninitialize();
        else if (hr == S_OK)
        {
            comInitialized = true;

            hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
                                      RPC_C_AUTHN_LEVEL_DEFAULT,
                                      RPC_C_IMP_LEVEL_IMPERSONATE,
                                      NULL, EOAC_NONE, NULL);
            if (hr != S_OK)
                PROVIDER_LOG_ERR("CoInitializeSecurity() "
                                 "returned %ld(0x%lx)",
                                 static_cast<long int>(hr),
                                 static_cast<long int>(hr));
        }

        if (wmiEstablishConnNs("\\\\.\\ROOT\\WMI",
                               &rootWMIConn) == 0 &&
            wmiEstablishConnNs("\\\\.\\ROOT\\CIMV2",
                               &cimWMIConn) == 0)
            ok = true;
    }

    WMIThreadContext::~WMIThreadContext()
    {
        if (rootWMIConn != NULL)
            rootWMIConn->Release();
        rootWMIConn = NULL;

        if (cimWMIConn != NULL)
            cimWMIConn->Release();
        cimWMIConn = NULL;

        if (comInitialized)
            CoUninitialize();
    }

    static Array<WMIThreadContext *> threadContexts;
    static Mutex                     ctxMutex;

    WMICtxRef::WMICtxRef() : myContext(NULL)
    {
        Auto_Mutex    guard(ctxMutex);
        unsigned int  i;

        DWORD threadId = GetCurrentThreadId();

        for (i = 0; i < threadContexts.size(); i++)
            if (threadContexts[i]->threadId == threadId)
                break;

        if (i == threadContexts.size())
            threadContexts.append(new WMIThreadContext());

        myContext = threadContexts[i];
        assert(myContext != NULL);
        myContext->inc();
    }

    WMICtxRef::~WMICtxRef()
    {
        Auto_Mutex    guard(ctxMutex);
        unsigned int  i;

        assert(myContext != NULL);
        myContext->dec(); 

        if (!*myContext)
        {
            for (i = 0; i < threadContexts.size(); i++)
                if (threadContexts[i]->threadId == myContext->threadId)
                    break;

            if (i < threadContexts.size())
                threadContexts.remove(i);

            delete myContext;
        }
    }

    IWbemServices *WMICtxRef::getRootWMIConn() const
    {
        return myContext->rootWMIConn;
    }

    IWbemServices *WMICtxRef::getCIMWMIConn() const
    {
        return myContext->cimWMIConn;
    }

    WMICtxRef::operator bool() const
    {
        return myContext->isOk();
    }

    /// Described in sf_wmi.h.
    int wmiGetStringProp(IWbemClassObject *wbemObj,
                         const char *propName,
                         String &str)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;
        char    *value;

        str = String("");

        if (wbemObj == NULL)
            return -1;

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("Failed to obtain value of '%s'", propName);
            return -1;
        }

        if (wbemObjProp.vt != VT_BSTR)
        {
            PROVIDER_LOG_ERR("Wrong variant type 0x%hx", wbemObjProp.vt);
            VariantClear(&wbemObjProp);
            return -1;
        }
        value = bstr2cstr(wbemObjProp.bstrVal);
        VariantClear(&wbemObjProp);

        if (value == NULL)
        {
            PROVIDER_LOG_ERR("%s():   null string obtained", __FUNCTION__);
            return -1;
        }

        str = String(value);
        delete[] value;

        return 0;
    }

    /// Described in sf_wmi.h.
    int wmiGetBoolProp(IWbemClassObject *wbemObj,
                       const char *propName,
                       bool *value,
                       bool quiet)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;

        if (wbemObj == NULL)
            return -1;

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            if (!quiet)
                PROVIDER_LOG_ERR("Failed to obtain value of '%s'",
                                 propName);
            return -1;
        }

        if (wbemObjProp.vt == VT_BOOL)
            *value = (wbemObjProp.boolVal ? true : false);
        else
        {
            if (!quiet)
                PROVIDER_LOG_ERR("Wrong variant type 0x%hx",
                                 wbemObjProp.vt);
            VariantClear(&wbemObjProp);
            return -1;
        }
        VariantClear(&wbemObjProp);

        return 0;
    }

    /// Described in sf_wmi.h.
    int wmiCollectInstances(IEnumWbemClassObject     *enumWbemObj,
                            Array<IWbemClassObject *> &instances)
    {
        IWbemClassObject *wbemObj = NULL;
        HRESULT           hr;
        int               rc = 0;
        ULONG             count;

        for (;;)
        {
            hr = enumWbemObj->Next(WBEM_INFINITE, 1, &wbemObj, &count);
            if (hr != WBEM_S_NO_ERROR && hr != WBEM_S_FALSE)
            {
                PROVIDER_LOG_ERR("%s():   IEnumWbemClassObject::Next() "
                                 "failed with rc=%lx", __FUNCTION__, hr);
                for (unsigned i = 0; i < instances.size(); i++)
                    instances[i]->Release();
                instances.clear();
                rc = -1;
                break;
            }
            if (count == 0)
                break;

            instances.append(wbemObj);
        }
        return rc;
    }

    /// Described in sf_wmi.h.
    int wmiEnumInstances(IWbemServices *wbemSvc,
                         const char *className,
                         Array<IWbemClassObject *> &instances)
    {
        HRESULT               hr;
        IEnumWbemClassObject *enumWbemObj = NULL;
        int                   rc = 0;
        unsigned int          i;
        BString               bstrQuery(className);

        WMICtxRef ctxRef;

        if (!ctxRef)
            return -1;

        if (wbemSvc == NULL)
            wbemSvc = ctxRef.getRootWMIConn();

        hr = wbemSvc->CreateInstanceEnum(bstrQuery.rep(),
                                         WBEM_FLAG_SHALLOW, NULL,
                                         &enumWbemObj);

        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("CreateInstancesEnum() failed, rc = %lx",
                             hr);
            return -1;
        }

        rc = wmiCollectInstances(enumWbemObj, instances);

        enumWbemObj->Release();
        return rc;
    }

    /// Described in sf_wmi.h.
    int wmiEnumInstancesQuery(IWbemServices *wbemSvc,
                              const char *search,
                              Array<IWbemClassObject *> &instances)
    {
        HRESULT               hr;
        IEnumWbemClassObject *enumWbemObj = NULL;
        int                   rc = 0;
        ULONG                 count;
        unsigned int          i;

        static BString wqlName("WQL");
        BString        searchBstr(search);

        WMICtxRef ctxRef;

        if (!ctxRef)
            return -1;

        if (wbemSvc == NULL)
            wbemSvc = ctxRef.getRootWMIConn();

        hr = wbemSvc->ExecQuery(wqlName.rep(),
                                searchBstr.rep(),
                                WBEM_FLAG_FORWARD_ONLY, NULL,
                                &enumWbemObj);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("ExecQuery('%s') failed, rc = %lx",
                             search, hr);
            return -1;
        }

        rc = wmiCollectInstances(enumWbemObj, instances);

        enumWbemObj->Release();
        return rc;
    }

    /// Described in sf_wmi.h.
    int wmiGetObject(IWbemServices *wbemSvc, 
                     const char *objPath, IWbemClassObject **obj)
    {
        HRESULT hr;

        WMICtxRef ctxRef;

        if (!ctxRef)
            return -1;

        if (wbemSvc == NULL)
            wbemSvc = ctxRef.getCIMWMIConn();
        
        hr = wbemSvc->GetObject(BString(objPath).rep(),
                                WBEM_FLAG_RETURN_WBEM_COMPLETE,
                                NULL, obj, NULL);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("%s():   IWbemClassObject::GetObject(%s) "
                             "failed with rc = %lx",
                             __FUNCTION__, objPath, hr);
            return -1;
        }

        return 0;
    }

    /// Described in sf_wmi.h.
    int wmiPrepareMethodCall(IWbemClassObject *instance,
                             String &methodName,
                             String &objPath,
                             IWbemClassObject **pIn)
    {
        HRESULT hr;

        VARIANT  className;
        VARIANT  path;
        char    *value = NULL;

        IWbemClassObject *classObj = NULL;
        IWbemClassObject *pInDef = NULL;

        int       rc = 0;
        WMICtxRef ctxRef;

        if (!ctxRef)
            return -1;

        className.vt = VT_NULL;
        path.vt = VT_NULL;

        if (instance == NULL)
            return -1;

        hr = instance->Get(BString("__Class").rep(), 0,
                           &className, NULL, NULL);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("%s():   failed to obtain class name of "
                             "WMI object, rc=%lx", __FUNCTION__, hr);
            return -1;
        }

        hr = instance->Get(BString("__Path").rep(), 0,
                           &path, NULL, NULL);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("%s():   failed to obtain path of WMI object, "
                             "rc=%lx", __FUNCTION__, hr);
            rc = -1;
            goto cleanup;
        }

        hr = ctxRef.getRootWMIConn()->GetObject(className.bstrVal, 0, NULL,
                                                &classObj, NULL);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("%s():   failed to obtain class definition "
                             "of WMI object, rc=%lx", __FUNCTION__, hr);
            rc = -1;
            goto cleanup;
        }

        if (pIn != NULL)
        {
            hr = classObj->GetMethod(BString(methodName).rep(), 0, &pInDef,
                                     NULL);
            if (FAILED(hr))
            {
                PROVIDER_LOG_ERR("%s():   failed to obtain method "
                                 "definition for WMI object, rc=%lx",
                                 __FUNCTION__, hr);
                rc = -1;
                goto cleanup;
            }

            hr = pInDef->SpawnInstance(0, pIn);
            if (FAILED(hr))
            {
                PROVIDER_LOG_ERR("%s():   failed to obtain input "
                                 "parameters instance, rc=%lx",
                                 __FUNCTION__, hr);
                rc = -1;
                goto cleanup;
            }
        }

        value = bstr2cstr(path.bstrVal);
        objPath = String(value);
        delete[] value;

cleanup:
        VariantClear(&className);
        VariantClear(&path);
        if (classObj != NULL)
            classObj->Release();
        if (pInDef != NULL)
            pInDef->Release();

        return rc;
    }

    /// Described in sf_wmi.h.
    int wmiExecMethod(const String &objPath,
                      const String &methodName,
                      IWbemClassObject *pIn = NULL,
                      IWbemClassObject **pOut = NULL)
    {
        HRESULT   hr;
        long int  completionCode;
        int       rc;

        IWbemClassObject *tmpPOut = NULL;

        WMICtxRef ctxRef;

        if (!ctxRef)
            return -1;

        if (pOut != NULL)
            *pOut = NULL;

        CoImpersonateClient();
        hr = ctxRef.getRootWMIConn()->ExecMethod(BString(objPath).rep(),
                                                 BString(methodName).rep(),
                                                 0, NULL, pIn, &tmpPOut,
                                                 NULL);
        CoRevertToSelf();
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("%s():   failed to call %s() "
                             "method, rc=%lx", __FUNCTION__,
                             methodName.c_str(), hr);
            return -1;
        }

        rc = wmiGetIntProp<long int>(tmpPOut, "CompletionCode",
                                     &completionCode);
        if (rc != 0)
        {
            tmpPOut->Release();
            tmpPOut = NULL;
            return -1;
        }
        if (completionCode != 0)
        {
            PROVIDER_LOG_ERR("%s():   %s() method returned wrong "
                             "completion code %ld (0x%lx)",
                             __FUNCTION__, methodName.c_str(),
                             completionCode, completionCode);
            tmpPOut->Release();
            tmpPOut = NULL;
            return -1;
        }

        if (pOut != NULL)
            *pOut = tmpPOut;
        else
            tmpPOut->Release();

        return 0;
    }

    /// Described in sf_wmi.h.
    int updateWbemClassObject(IWbemClassObject **obj)
    {
        String            objPath;
        IWbemClassObject *newObj = NULL;
        int               rc = 0;
        HRESULT           hr;

        WMICtxRef ctxRef;

        if (!ctxRef)
            return -1;

        if (obj == NULL)
            return -1;

        rc = wmiGetStringProp(*obj, "__Path", objPath);
        if (rc != 0)
            return rc;

        hr = ctxRef.getRootWMIConn()->GetObject(
                                        BString(objPath).rep(),
                                        WBEM_FLAG_RETURN_WBEM_COMPLETE,
                                        NULL, &newObj, NULL);
        if (FAILED(hr))
        {
            PROVIDER_LOG_ERR("%s():   failed to get object '%s',"
                             " rc = %ld (0x%lx)",
                             __FUNCTION__, objPath.c_str(), hr, hr);
            return -1;
        }

        (*obj)->Release();
        *obj = newObj;

        return 0;
    }

    /// Described in sf_wmi.h.
    int querySingleObj(IWbemServices *wbemSvc,
                       const char *query, IWbemClassObject *&obj)
    {
        Array<IWbemClassObject *> objs;
        unsigned int              i;
        int                       rc;

        rc = wmiEnumInstancesQuery(wbemSvc, query, objs);
        if (rc != 0)
            return rc;
        if (objs.size() != 1)
        {
            PROVIDER_LOG_ERR("%s():   incorrect number %u of matching "
                             "objects found, query '%s'", __FUNCTION__,
                             objs.size(), query);
            for (i = 0; i < objs.size(); i++)
                objs[i]->Release();
            return -1;
        }

        obj = objs[0];
        return 0;
    }
}
