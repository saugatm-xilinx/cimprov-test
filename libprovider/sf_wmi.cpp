#include <wbemprov.h>
#include <cimple/wmi/BString.h>
#include <cimple/wmi/utils.h>
#include <sf_wmi.h>

namespace solarflare 
{
    using cimple::String;
    using cimple::BString;
    using cimple::bstr2cstr;
    using cimple::wstr2str;
    using cimple::uint8;
    using cimple::uint16;
    using cimple::Array;

    /// Described in sf_wmi.h.
    IWbemServices *rootWMIConn = NULL;

    /// Described in sf_wmi.h.
    IWbemServices *cimWMIConn = NULL;

    /// Described in sf_wmi.h.
    int wmiEstablishConnNs(const char *ns,
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
            CIMPLE_ERR(("CoCreateInstance() failed, rc = %lx", hr));
            return -1;
        }

        hr = wbemLocator->ConnectServer(BString(ns).rep(),
                                        NULL, NULL, NULL, 0, NULL, NULL,
                                        svc);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("ConnectServer() failed, rc = %lx", hr));
            *svc = NULL;
            wbemLocator->Release();
            return -1;
        }

        wbemLocator->Release();
        return 0;
    }

    /// Described in sf_wmi.h.
    int wmiEstablishConn()
    {
        int rc = 0;
        
        if (rootWMIConn == NULL)
        {
            rc  = wmiEstablishConnNs("\\\\.\\ROOT\\WMI",
                                     &rootWMIConn);
            if (rc != 0)
                return rc;
        }
        if (cimWMIConn == NULL)
        {
            rc = wmiEstablishConnNs("\\\\.\\ROOT\\CIMV2",
                                    &cimWMIConn);
        }
        return rc;
    }

    /// Described in sf_wmi.h.
    void wmiReleaseConn()
    {
        if (rootWMIConn != NULL)
            rootWMIConn->Release();
        rootWMIConn = NULL;

        if (cimWMIConn != NULL)
            cimWMIConn->Release();
        cimWMIConn = NULL;
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
            CIMPLE_ERR(("Failed to obtain value of '%s'", propName));
            return -1;
        }

        if (wbemObjProp.vt != VT_BSTR)
        {
            CIMPLE_ERR(("Wrong variant type 0x%hx", wbemObjProp.vt));
            VariantClear(&wbemObjProp);
            return -1;
        }
        value = bstr2cstr(wbemObjProp.bstrVal);
        VariantClear(&wbemObjProp);

        if (value == NULL)
        {
            CIMPLE_ERR(("%s():   null string obtained", __FUNCTION__));
            return -1;
        }

        str = String(value);
        delete[] value;

        return 0;
    }

    /// Described in sf_wmi.h.
    int wmiGetBoolProp(IWbemClassObject *wbemObj,
                       const char *propName,
                       bool *value)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;

        if (wbemObj == NULL)
            return -1;

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("Failed to obtain value of '%s'", propName));
            return -1;
        }

        if (wbemObjProp.vt == VT_BOOL)
            *value = (wbemObjProp.boolVal ? true : false);
        else
        {
            CIMPLE_ERR(("Wrong variant type 0x%hx", wbemObjProp.vt));
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
                CIMPLE_ERR(("%s():   IEnumWbemClassObject::Next() "
                            "failed with rc=%lx", __FUNCTION__, hr));
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

        if (wbemSvc == NULL && wmiEstablishConn() != 0)
            return -1;
        else if (wbemSvc == NULL)
            wbemSvc = rootWMIConn;

        hr = wbemSvc->CreateInstanceEnum(bstrQuery.rep(),
                                         WBEM_FLAG_SHALLOW, NULL,
                                         &enumWbemObj);

        if (FAILED(hr))
        {
            CIMPLE_ERR(("CreateInstancesEnum() failed, rc = %lx",
                        hr));
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

        if (wbemSvc == NULL && wmiEstablishConn() != 0)
            return -1;
        else if (wbemSvc == NULL)
            wbemSvc = rootWMIConn;

        hr = wbemSvc->ExecQuery(wqlName.rep(),
                                searchBstr.rep(),
                                WBEM_FLAG_FORWARD_ONLY, NULL,
                                &enumWbemObj);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("ExecQuery() failed, rc = %lx", hr));
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

        if (wmiEstablishConn() != 0)
            return -1;

        if (wbemSvc == NULL)
            wbemSvc = cimWMIConn;
        
        hr = wbemSvc->GetObject(BString(objPath).rep(),
                                WBEM_FLAG_RETURN_WBEM_COMPLETE,
                                NULL, obj, NULL);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   IWbemClassObject::GetObject(%s) failed with "
                        "rc = %lx", __FUNCTION__, objPath, hr));
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

        int  rc = 0;

        className.vt = VT_NULL;
        path.vt = VT_NULL;

        if (wmiEstablishConn() != 0)
            return -1;

        if (instance == NULL)
            return -1;

        hr = instance->Get(BString("__Class").rep(), 0,
                           &className, NULL, NULL);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to obtain class name of WMI object, "
                        "rc=%lx", __FUNCTION__, hr));
            return -1;
        }

        hr = instance->Get(BString("__Path").rep(), 0,
                           &path, NULL, NULL);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to obtain path of WMI object, "
                        "rc=%lx", __FUNCTION__, hr));
            rc = -1;
            goto cleanup;
        }

        hr = rootWMIConn->GetObject(className.bstrVal, 0, NULL,
                                    &classObj, NULL);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to obtain class definition "
                        "of WMI object, rc=%lx", __FUNCTION__, hr));
            rc = -1;
            goto cleanup;
        }

        if (pIn != NULL)
        {
            hr = classObj->GetMethod(BString(methodName).rep(), 0, &pInDef,
                                     NULL);
            if (FAILED(hr))
            {
                CIMPLE_ERR(("%s():   failed to obtain method definition "
                            "for WMI object, rc=%lx", __FUNCTION__, hr));
                rc = -1;
                goto cleanup;
            }

            hr = pInDef->SpawnInstance(0, pIn);
            if (FAILED(hr))
            {
                CIMPLE_ERR(("%s():   failed to obtain input "
                            "parameters instance, rc=%lx",
                            __FUNCTION__, hr));
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

        if (pOut != NULL)
            *pOut = NULL;

        CoImpersonateClient();
        hr = rootWMIConn->ExecMethod(BString(objPath).rep(),
                                     BString(methodName).rep(),
                                     0, NULL, pIn, &tmpPOut, NULL);
        CoRevertToSelf();
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to call %s() "
                        "method, rc=%lx", __FUNCTION__,
                        methodName.c_str(), hr));
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
            CIMPLE_ERR(("%s():   %s() method returned wrong completion "
                        "code %ld (0x%lx)", __FUNCTION__,
                        methodName.c_str(), completionCode,
                        completionCode));
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
}
