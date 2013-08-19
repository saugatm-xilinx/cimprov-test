#define INITGUID
#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;
#include <wbemprov.h>
#include <wchar.h>
#include <string.h>

typedef enum {
  Job_New = 2,
  Job_Starting = 3,
  Job_Running = 4,
  Job_Suspended = 5,
  Job_ShuttingDown = 6,
  Job_Completed = 7,
  Job_Terminated = 8,
  Job_Killed = 9,
  Job_Exception = 10,
  Job_Service = 11,
  Job_Query_Pending = 12,
  Job_DMTF_Reserved = 13,
  Job_Vendor_Reserved = 32768
} JobState;

#define STATE2STR(state_) \
    case Job_ ## state_: return #state_

const char *jobState2Str(int state)
{
    switch (static_cast<JobState>(state))
    {
        STATE2STR(New);
        STATE2STR(Starting);
        STATE2STR(Running);
        STATE2STR(Suspended);
        STATE2STR(ShuttingDown);
        STATE2STR(Completed);
        STATE2STR(Terminated);
        STATE2STR(Killed);
        STATE2STR(Exception);
        STATE2STR(Service);
        STATE2STR(Query_Pending);
        STATE2STR(DMTF_Reserved);
        STATE2STR(Vendor_Reserved);
   
        default:
            return "UNKNOWN";
    }
}

DEFINE_GUID(IID_IWbemLocator, 0xdc12a687, 0x737f, 0x11cf, 0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24);
DEFINE_GUID(CLSID_WbemLocator, 0x4590f811, 0x1d3a, 0x11d0, 0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24);

#define RELEASE(obj) \
    do {                        \
        if (obj != NULL)        \
        {                       \
            obj->Release();     \
            obj = NULL;         \
        }                       \
    } while (0)

char *HRESULT2STR(HRESULT hr)
{
    static char msg[10000];

    LPTSTR errorText = NULL;
    DWORD  result;

    // Sorry, failed to implement
    msg[0] = '\0';
    return msg;
}


template <class IntType>
static int wmiGetIntProp(VARIANT &wbemObjProp,
                         IntType *value)
{
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
    else
    {
        printf("Wrong variant type 0x%hx\n", wbemObjProp.vt);
        return -1;
    }

    return 0;
}

static int updateWbemClassObject(IWbemServices *svc,
                                 IWbemClassObject **obj)
{
    VARIANT           vtObjPath;
    IWbemClassObject *newObj = NULL;
    int               rc = 0;
    HRESULT           hr;

    if (obj == NULL)
        return -1;

    hr = (*obj)->Get(L"__Path", 0, &vtObjPath, 0, 0);
    if (FAILED(hr))
    {
        printf("ERROR: %s(): failed to get (__Path), "
               "returned strange result: 0x%lx ('%s')\n",
               __FUNCTION__, hr, HRESULT2STR(hr));
        return -1;
    }

    hr = svc->GetObject(vtObjPath.bstrVal,
                        WBEM_FLAG_RETURN_WBEM_COMPLETE,
                        NULL, &newObj, NULL);
    if (FAILED(hr))
    {
        printf("%s(): failed to get object, rc = %ld (0x%lx)\n",
               __FUNCTION__, hr, hr);
        return -1;
    }

    (*obj)->Release();
    *obj = newObj;

    return 0;
}

int main()
{
    HRESULT hr;

    // Create locator:

    IWbemLocator *loc = 0;
    IWbemServices *svc = 0;
    IEnumWbemClassObject* en_diags = 0;
    IWbemClassObject* diagObj = 0;
    ULONG count;
    VARIANT vtProp;
    VARIANT vtDiagPath;
    VARIANT vtDiagName;

    IEnumWbemClassObject *en_nics = NULL;
    IWbemClassObject *nicObj = NULL;
    VARIANT vtNICPath;

    IEnumWbemClassObject *en_jobs = NULL;
    IWbemClassObject *jobObj = NULL;
    VARIANT vtJobName;

    IEnumWbemClassObject *en_recs = NULL;
    IWbemClassObject *recObj = NULL;
    VARIANT vtRecInstID;
    VARIANT vtRecData;

    IWbemClassObject *diagClassObj = NULL;
    IWbemClassObject *pIn = NULL;
    IWbemClassObject *pOut = NULL;
    IWbemClassObject *pInDesc = NULL;
    IWbemClassObject *pOutDesc = NULL;

    BSTR bstr;

    int rc = 0;
    int testCount = 0;

    CoInitialize(NULL);

    // THIS IS REQUIRED!!!
    hr =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

    setbuf(stdout, NULL);

    if (FAILED(hr))
    {
        printf("ERROR: CoInitializeSecurity failed rc=0x%lx ('%s')!\n", hr,
               HRESULT2STR(hr));
        goto cleanup;
    }

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&loc);

    if (FAILED(hr))
    {
        printf("ERROR: CoCreateInstance failed rc=0x%lx ('%s')!\n", hr,
               HRESULT2STR(hr));
        goto cleanup;
    }

    // Create connection:

    bstr = SysAllocStringLen(L"\\\\.\\ROOT\\cimv2",
                             strlen("\\\\.\\ROOT\\cimv2"));
    hr = loc->ConnectServer(
        (OLECHAR *)bstr, NULL, NULL, 0, 0, 0, 0, &svc);
    SysFreeString(bstr);

    if (FAILED(hr))
    {
        printf("ERROR: ConnectServer() failed rc=0x%lx ('%s')!\n", hr,
               HRESULT2STR(hr));
        goto cleanup;
    }

    hr = svc->CreateInstanceEnum(L"SF_DiagnosticTest", WBEM_FLAG_SHALLOW,
                                 NULL, &en_diags);
    if (FAILED(hr))
    {
        printf("ERROR: CreateInstanceEnum() failed for SF_DiagnosticTest, "
               "rc=%lx!\n", hr);
        goto cleanup;
    }

    while (1)
    {
        hr = en_diags->Next(WBEM_INFINITE, 1, &diagObj, &count);
        if (hr != WBEM_S_NO_ERROR && hr != WBEM_S_FALSE)
        {
            printf("ERROR: en_diags->Next() returned strange result: 0x%lx ('%s')\n",
                   hr, HRESULT2STR(hr));
            break;
        }
        if (count != 1)
        {
            if (count != 0)
                printf("ERROR: en_diags->Next() returned count=%lu\n",
                       (long unsigned)count);
            break;
        }

        hr = diagObj->Get(L"__Path", 0, &vtDiagPath, 0, 0);
        if (FAILED(hr))
        {
            printf("ERROR: diagObj->Get(__Path) "
                   "returned strange result: 0x%lx ('%s')\n",
                   hr, HRESULT2STR(hr));
            break;
        }
        wcout << "Test : " << vtDiagPath.bstrVal << endl;

        hr = diagObj->Get(L"Name", 0, &vtDiagName, 0, 0);
        if (FAILED(hr))
        {
            printf("ERROR: diagObj->Get(Name) "
                   "returned strange result: 0x%lx ('%s')\n",
                   hr, HRESULT2STR(hr));
            break;
        }
        wcout << "Name: " << vtDiagName.bstrVal << endl;

        hr = diagObj->Get(L"__Class", 0, &vtProp, 0, 0);
        if (FAILED(hr))
        {
            printf("ERROR: diagObj->Get(__Class) returned "
                   "strange result: 0x%lx ('%s')\n",
                   hr, HRESULT2STR(hr));
            break;
        }

        hr = svc->GetObject(vtProp.bstrVal, 0, NULL,
                            &diagClassObj, NULL);
        VariantClear(&vtProp);
        if (FAILED(hr))
        {
            printf("ERROR: GetObject(SF_DiagnosticTest) returned "
                   "strange result: 0x%lx ('%s')\n",
                   hr, HRESULT2STR(hr));
            break;
        }

        hr = diagClassObj->GetMethod(L"RunDiagnosticService",
                                     0, &pInDesc, &pOutDesc);
        if (FAILED(hr))
        {
            printf("ERROR: GetMethod(RunDiagnosticService) returned "
                   "strange result: 0x%lx ('%s')\n",
                   hr, HRESULT2STR(hr));
            break;
        }

        hr = pInDesc->SpawnInstance(0, &pIn);
        if (FAILED(hr))
        {
            printf("ERROR: SpawnInstance(RunDiagnosticService.pIn) "
                   "returned strange result: 0x%lx ('%s')\n",
                   hr, HRESULT2STR(hr));
            break;
        }

        hr = svc->CreateInstanceEnum(L"SF_ConcreteJob",
                                     WBEM_FLAG_SHALLOW,
                                     NULL, &en_jobs);
        if (FAILED(hr))
        {
            printf("ERROR: CreateInstanceEnum() failed for SF_ConcreteJob "
                   "rc=0x%lx!\n", hr);
            goto cleanup;
        }

        while (1)
        {
            hr = en_jobs->Next(WBEM_INFINITE, 1, &jobObj, &count);

            if (hr != WBEM_S_NO_ERROR && hr != WBEM_S_FALSE)
            {
                printf("ERROR: en_jobs->Next() returned strange "
                       "result: 0x%lx ('%s')\n",
                       hr, HRESULT2STR(hr));
                break;
            }
            if (count != 1)
            {
                if (count != 0)
                    printf("ERROR: en_jobs->Next() returned count=%lu\n",
                           (long unsigned)count);
                break;
            }

            hr = jobObj->Get(L"Name", 0, &vtJobName, 0, 0);
            if (FAILED(hr))
            {
                printf("ERROR: failed to get Name for SF_ConcreteJob "
                       "rc=0x%lx!\n", hr);
                goto cleanup;
            }

            if (wcsstr(vtJobName.bstrVal, vtDiagName.bstrVal) != NULL)
                break;

            VariantClear(&vtJobName);
            RELEASE(jobObj);
        }

        RELEASE(en_jobs);
        if (jobObj == NULL)
        {
            printf("ERROR: corresponding SF_ConcreteJob not found\n");
            goto cleanup;
        }
        else
            wcout << "Related job name: " << vtJobName.bstrVal << endl;
        VariantClear(&vtJobName);

        hr = svc->CreateInstanceEnum(L"SF_NICCard",
                                     WBEM_FLAG_SHALLOW,
                                     NULL, &en_nics);
        if (FAILED(hr))
        {
            printf("ERROR: CreateInstanceEnum() failed for SF_NICCard "
                   "rc=0x%lx!\n", hr);
            goto cleanup;
        }

        while (1)
        {
            hr = en_nics->Next(WBEM_INFINITE, 1, &nicObj, &count);

            if (hr != WBEM_S_NO_ERROR && hr != WBEM_S_FALSE)
            {
                printf("ERROR: en_nics->Next() returned strange "
                       "result: 0x%lx ('%s')\n",
                       hr, HRESULT2STR(hr));
                break;
            }
            if (count != 1)
            {
                if (count != 0)
                    printf("ERROR: en_nics->Next() returned count=%lu\n",
                           (long unsigned)count);
                break;
            }

            hr = nicObj->Get(L"__Relpath", 0, &vtNICPath, 0, 0);
            if (FAILED(hr))
            {
                printf("ERROR: nicObj->Get(__Relpath) failed "
                       "for SF_NICCard rc=0x%lx!\n", hr);
                goto cleanup;
            }
            RELEASE(nicObj);
            wcout << "Call for NIC : " << vtNICPath.bstrVal << endl;

            hr = pIn->Put(L"ManagedElement", 0, &vtNICPath, 0);
            if (FAILED(hr))
            {
                printf("ERROR: pIn->Put(ManagedElement) failed "
                       "rc=0x%lx!\n", hr);
                goto cleanup;
            }
            VariantClear(&vtNICPath);

            hr = svc->ExecMethod(vtDiagPath.bstrVal,
                                 L"RunDiagnosticService",
                                 0, NULL, pIn, &pOut, NULL);
            if (FAILED(hr))
            {
                printf("ERROR: RunDiagnosticService() failed "
                       "rc=0x%lx!\n", hr);
                goto cleanup;
            }

            if (pOut)
            {
                unsigned int RetVal;

                hr = pOut->Get(L"ReturnValue", 0, &vtProp, 0, 0);
                if (FAILED(hr))
                    printf("ERROR: Failed to get ReturnValue for "
                           "RunDiagnosticService(), "
                           "rc=0x%lx!\n", hr);
                else
                {
                    if (wmiGetIntProp<unsigned int>(vtProp, &RetVal) != 0)
                        printf("ERROR: failed to determine return value of "
                               "RunDiagnosticService\n");
                    else
                        printf("RunDiagnosticService() -> %u\n", RetVal);
                    if (RetVal != 0)
                        goto cleanup;
                }
                VariantClear(&vtProp);
            }
            else
                printf("ERROR: failed to determine return value of "
                       "RunDiagnosticService\n");

            while (1)
            {
                VARIANT vtSt;
                int st;

                Sleep(500);
                rc = updateWbemClassObject(svc, &jobObj);
                if (rc != 0)
                {
                    printf("ERROR: failed to update info about current Job\n");
                    goto cleanup;
                }

                hr = jobObj->Get(L"JobState", 0, &vtSt, 0, 0);
                if (FAILED(hr))
                {
                    printf("ERROR: failed get JobState for current Job\n");
                    goto cleanup; 
                }

                rc = wmiGetIntProp<int>(vtSt, &st);
                if (rc != 0)
                {
                    printf("ERROR: failed to convert JobState to int\n");
                    goto cleanup; 
                }

                printf("Job is in state %s\n", jobState2Str(st));
                if (st != Job_Starting &&
                    st != Job_Running &&
                    st != Job_ShuttingDown)
                    break;
            }

            RELEASE(jobObj);

            hr = svc->CreateInstanceEnum(L"SF_DiagnosticCompletionRecord",
                                         WBEM_FLAG_SHALLOW,
                                         NULL, &en_recs);
            if (FAILED(hr))
            {
                printf("ERROR: CreateInstanceEnum() failed for SF_DiagnosticCompletionRecord "
                       "rc=0x%lx!\n", hr);
                goto cleanup;
            }

            while (1)
            {
                hr = en_recs->Next(WBEM_INFINITE, 1, &recObj, &count);

                if (hr != WBEM_S_NO_ERROR && hr != WBEM_S_FALSE)
                {
                    printf("ERROR: en_recs->Next() returned strange "
                           "result: 0x%lx ('%s')\n",
                           hr, HRESULT2STR(hr));
                    break;
                }
                if (count != 1)
                {
                    if (count != 0)
                        printf("ERROR: en_recs->Next() returned count=%lu\n",
                               (long unsigned)count);
                    break;
                }

                hr = recObj->Get(L"InstanceID", 0, &vtRecInstID, 0, 0);
                if (FAILED(hr))
                {
                    printf("ERROR: failed to get InstanceID for SF_DiagnosticCompletionRecord "
                           "rc=0x%lx!\n", hr);
                    goto cleanup;
                }

                if (wcsstr(vtRecInstID.bstrVal, vtDiagName.bstrVal) != NULL)
                    break;

                VariantClear(&vtRecInstID);

                RELEASE(recObj);
            }

            RELEASE(en_recs);
            if (recObj == NULL)
            {
                printf("ERROR: corresponding SF_DiagnosticCompletionRecord not found\n");
                goto cleanup;
            }
            else
                wcout << "Related rec id: " << vtRecInstID.bstrVal << endl;
            VariantClear(&vtRecInstID);

            hr = recObj->Get(L"RecordData", 0, &vtRecData, 0, 0);
            if (FAILED(hr))
            {
                printf("ERROR: failed to get RecordData for SF_DiagnosticCompletionRecord "
                       "rc=0x%lx!\n", hr);
                goto cleanup;
            }

            wcout << "Test result: " << vtRecData.bstrVal << endl;

            VariantClear(&vtRecData);

            RELEASE(recObj);
        }

        RELEASE(nicObj);
        RELEASE(en_nics);
        RELEASE(jobObj);
        RELEASE(diagObj);
        RELEASE(diagClassObj);
        RELEASE(pInDesc);
        RELEASE(pIn);
        RELEASE(pOut);

        VariantClear(&vtDiagPath);
    }

cleanup:
    RELEASE(jobObj);
    RELEASE(en_jobs);
    RELEASE(nicObj);
    RELEASE(en_nics);
    RELEASE(diagObj);
    RELEASE(diagClassObj);
    RELEASE(pInDesc);
    RELEASE(pIn);
    RELEASE(pOut);
    RELEASE(en_diags);
    RELEASE(loc);
    RELEASE(svc);

    CoUninitialize();
    printf("Testing terminated\n");
    return 0;
}
