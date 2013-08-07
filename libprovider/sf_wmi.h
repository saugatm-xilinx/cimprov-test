#ifndef SOLARFLARE_SF_WMI_H
#define SOLARFLARE_SF_WMI_H 1

#include <wbemprov.h>
#include <cimple/wmi/BString.h>
#include <cimple/wmi/utils.h>

/// Auxiliary functions to access WMI objects

namespace solarflare
{
    using cimple::String;
    using cimple::BString;
    using cimple::bstr2cstr;
    using cimple::wstr2str;
    using cimple::uint8;
    using cimple::uint16;
    using cimple::Array;

    ///
    /// WMI connection (with namespace root\WMI).
    ///
    extern IWbemServices *rootWMIConn;

    ///
    /// WMI connection (with namespace root\WMI).
    ///
    extern IWbemServices *cimWMIConn;

    ///
    /// Establish WMI connection with a given namespace.
    ///
    /// @param ns           Namespace
    /// @param svc    [out] Where to save connected IWbemServices
    ///
    /// @return 0 on success, -1 on failure
    ///
    int wmiEstablishConnNs(const char *ns,
                           IWbemServices **svc);

    ///
    /// Establish WMI connection.
    ///
    /// @return 0 on success, -1 on failure
    ///
    int wmiEstablishConn();

    ///
    /// Release WMI connection.
    ///
    void wmiReleaseConn();

    ///
    /// Get string property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    int wmiGetStringProp(IWbemClassObject *wbemObj,
                         const char *propName,
                         String &str);

    ///
    /// Get integer property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    template <class IntType>
    inline int wmiGetIntProp(IWbemClassObject *wbemObj,
                             const char *propName,
                             IntType *value)
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

        if (wbemObjProp.vt == VT_I2)
            *value = wbemObjProp.iVal & 0x0000ffff;
        else if (wbemObjProp.vt == VT_I4)
            *value = wbemObjProp.lVal;
        else if (wbemObjProp.vt == VT_INT)
            *value = wbemObjProp.intVal;
        else if (wbemObjProp.vt == VT_UI2)
            *value = wbemObjProp.uiVal & 0x0000ffff;
        else if (wbemObjProp.vt == VT_UI4)
            *value = wbemObjProp.ulVal;
        else if (wbemObjProp.vt == VT_UINT)
            *value = wbemObjProp.uintVal;
        else if (wbemObjProp.vt == VT_UI1)
            *value = wbemObjProp.bVal;
        else if (wbemObjProp.vt == VT_BSTR)
        {
            char *str_val = NULL;
            char *endptr = NULL;

            str_val = bstr2cstr(wbemObjProp.bstrVal);
            *value = strtoll(str_val, &endptr, 10);
            if (endptr == NULL || *endptr != '\0')
            {
                CIMPLE_ERR(("Failed to convert '%s' string to int",
                            str_val));
                VariantClear(&wbemObjProp);
                delete[] str_val;
                return -1;
            }
            delete[] str_val;
        }
        else
        {
            CIMPLE_ERR(("Wrong variant type 0x%hx", wbemObjProp.vt));
            VariantClear(&wbemObjProp);
            return -1;
        }
        VariantClear(&wbemObjProp);

        return 0;
    }

    ///
    /// Get integer array property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    template <class IntType>
    inline int wmiGetIntArrProp(IWbemClassObject *wbemObj,
                                const char *propName,
                                Array<IntType> &value)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;
        VARTYPE  vt;
        int      dims_number;
        LONG     LBound;
        LONG     UBound;
        LONG     i;

        SAFEARRAY *pArray = NULL;

        value.clear();

        if (wbemObj == NULL)
            return -1;

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("Failed to obtain value of '%s'", propName));
            return -1;
        }

        vt = wbemObjProp.vt;
        if (!(vt & VT_ARRAY))
        {
            CIMPLE_ERR(("%s():   Array flag is not set in "
                        "variant type 0x%hx",
                        __FUNCTION__, wbemObjProp.vt));
            VariantClear(&wbemObjProp);
            return -1;
        }

        vt &= (~VT_ARRAY);
        if (vt != VT_I2 && vt != VT_I4 && vt != VT_INT &&
            vt != VT_UI2 && vt != VT_UI4 && vt != VT_UINT &&
            vt != VT_UI1)
        {
            CIMPLE_ERR(("%s():   variant type 0x%hx is not among "
                        "known integer types",
                        __FUNCTION__, vt));
            VariantClear(&wbemObjProp);
            return -1;
        }

        pArray = V_ARRAY(&wbemObjProp);
        dims_number = SafeArrayGetDim(pArray);
        if (dims_number != 1)
        {
            CIMPLE_ERR(("%s():   wrong number of array dimensions %d",
                        __FUNCTION__, dims_number));
            VariantClear(&wbemObjProp);
            return -1;
        }

        hr = SafeArrayGetLBound(pArray, 1, &LBound);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to obtain lower bound of dimension, "
                        "rc=%lx", __FUNCTION__, hr));
            VariantClear(&wbemObjProp);
            return -1;
        }

        hr = SafeArrayGetUBound(pArray, 1, &UBound);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to obtain upper bound of dimension, "
                        "rc=%lx", __FUNCTION__, hr));
            VariantClear(&wbemObjProp);
            return -1;
        }

        for (i = LBound; i<= UBound; i++)
        {
            long long int el;

            hr = SafeArrayGetElement(pArray, &i,
                                     reinterpret_cast<void *>(&el));
            if (FAILED(hr))
            {
                CIMPLE_ERR(("%s():   failed to get %d element, "
                            "rc=%lx", __FUNCTION__, i, hr));
                VariantClear(&wbemObjProp);
                value.clear();
                return -1;
            }

            if (vt == VT_I2)
                value.append(*(reinterpret_cast<SHORT *>(&el)));
            else if (vt == VT_I4)
                value.append(*(reinterpret_cast<LONG *>(&el)));
            else if (vt == VT_INT)
                value.append(*(reinterpret_cast<INT *>(&el)));
            else if (vt == VT_UI2)
                value.append(*(reinterpret_cast<USHORT *>(&el)));
            else if (vt == VT_UI4)
                value.append(*(reinterpret_cast<ULONG *>(&el)));
            else if (vt == VT_UINT)
                value.append(*(reinterpret_cast<UINT *>(&el)));
            else if (vt == VT_UI1)
                value.append(*(reinterpret_cast<BYTE *>(&el)));
            else
            {
                CIMPLE_ERR(("%s():   failed to get %d element, "
                            "incorrect variant type 0x%hx",
                            __FUNCTION__, i, vt));
                VariantClear(&wbemObjProp);
                value.clear();
                return -1;
            }
        }

        VariantClear(&wbemObjProp);
        return 0;
    }


    ///
    /// Get boolean property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    int wmiGetBoolProp(IWbemClassObject *wbemObj,
                       const char *propName,
                       bool *value);

    /// Iterate over all WMI objects in enumWbemObj and add them into
    /// instances array
    ///
    /// @param enumWbemObj   WMI enumerator
    /// @param instances     Target object holding array
    ///
    /// @return 0 on success or error code
    int wmiCollectInstances(IEnumWbemClassObject     *enumWbemObj,
                            Array<IWbemClassObject *> &instances);

    ///
    /// Enumerate WMI object instances.
    ///
    /// @param wbemSvc            If not NULL, will be used
    ///                           instead of rootWMIConn
    /// @param className          Object class name
    /// @param instances    [out] Where to save obtained instances
    ///
    /// @return 0 on success or error code
    ///
    int wmiEnumInstances(IWbemServices *wbemSvc,
                         const char *className,
                         Array<IWbemClassObject *> &instances);

    ///
    /// Enumerate WMI object instances matching a certain WQL expression.
    ///
    /// @param wbemSvc            If not NULL, will be used
    ///                           instead of rootWMIConn
    /// @param search             WQL search expression
    /// @param instances    [out] Where to save obtained instances
    ///
    /// @return 0 on success or error code
    ///
    int wmiEnumInstancesQuery(IWbemServices *wbemSvc,
                              const char *search,
                              Array<IWbemClassObject *> &instances);

    ///
    /// Get WMI object by a given path.
    ///
    /// @param objPath            Object path
    /// @param obj          [out] Where to save obtained object pointer
    ///
    /// @return 0 on success or error code
    ///
    int wmiGetObject(IWbemServices *wbemSvc, 
                     const char *objPath, IWbemClassObject **obj);

    ///
    /// Get WMI path and input parameters object to call a given method
    /// for a given instance.
    ///
    /// @param instance         WMI object instance pointer
    /// @param methodName       Method name
    /// @param objPath    [out] Instance path to be used for calling method
    /// @param pIn        [out] Input parameters object
    ///
    /// @return 0 on success or error code
    ///
    int wmiPrepareMethodCall(IWbemClassObject *instance,
                             String &methodName,
                             String &objPath,
                             IWbemClassObject **pIn);

    ///
    /// Execute a method on WMI object.
    ///
    /// @param objPath          Instance path to be used for calling method
    /// @param methodName       Method name
    /// @param pIn              Input parameters object
    /// @param pIn        [out] Output parameters object
    ///
    /// @return 0 on success or error code
    ///
    int wmiExecMethod(const String &objPath,
                      const String &methodName,
                      IWbemClassObject *pIn,
                      IWbemClassObject **pOut);

    ///
    /// Update WMI object pointer by replacing with got more
    /// recently by the same path.
    ///
    /// @param obj    WMI object pointer to be updated
    ///
    /// @return 0 on success or -1 on failure
    ///
    int updateWbemClassObject(IWbemClassObject **obj);
}

#endif // SOLARFLARE_SF_PROVIDER_H
