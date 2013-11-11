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

#ifndef _cimple_Provider_Handle_h
#define _cimple_Provider_Handle_h

#include "config.h"
#include "Provider.h"
#include "Registration.h"

#include <cimple/log.h>

CIMPLE_NAMESPACE_BEGIN

class CIMPLE_CIMPLE_LINKAGE Provider_Handle
{
public:

    Provider_Handle(const Registration* registration);

    ~Provider_Handle();

    Get_Meta_Class_Status get_meta_class(const Meta_Class*& meta_class);

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const Instance* model,
        Instance*& instance);

    Enum_Instances_Status enum_instances(
        const Instance* model, 
        Enum_Instances_Proc enum_instances_proc,
        void* client_data);

    Create_Instance_Status create_instance(
        Instance* instance);

    Delete_Instance_Status delete_instance(
        const Instance* instance);

    Modify_Instance_Status modify_instance(
        const Instance* model,
        const Instance* instance);

    Invoke_Method_Status invoke_method(
        const Instance* instance,
        const Instance* method);

    Enable_Indications_Status enable_indications(
        Indication_Proc indication_proc, 
        void* client_data);

    Disable_Indications_Status disable_indications();

    Enum_Associator_Names_Status enum_associator_names(
        const Instance* instance,
        const String& result_class,
        const String& role,
        const String& result_role,
        Enum_Associator_Names_Proc proc,
        void* client_data);

    Enum_Associators_Status enum_associators(
        const Instance* instance,
        const String& result_class,
        const String& role,
        const String& result_role,
        Enum_Associators_Proc proc,
        void* client_data);

    Enum_References_Status enum_references(
        const Instance* instance,
        const Instance* model,
        const String& role,
        Enum_References_Proc proc,
        void* client_data);

    Get_Repository_Status get_repository(
        const Meta_Repository*& meta_repository);

    const Registration* registration() const { return _registration; }

protected:

    class FunctionLogger
    {
        String            functionName;
        String            functionParams;
        Datetime          dtStart;
        const Meta_Class* mc;
    public:
        FunctionLogger(Provider_Handle *handle, const char *fn,
                       String params) :
                          functionName(fn), functionParams(params),
                          dtStart(Datetime::now())
        { 
            handle->get_meta_class(mc);
            CIMPLE_DBG(("START Provider_Handle[meta_class='%s']::%s(): %s",
                        mc->name, functionName.c_str(),
                        functionParams.c_str()));
        }

        ~FunctionLogger()
        {
            uint64 timeDiff = Datetime::now().usec() - dtStart.usec();

            CIMPLE_DBG(("END Provider_Handle[meta_class='%s']::%s(): %s; "
                        "ELAPSED %llu usec",
                        mc->name, functionName.c_str(),
                        functionParams.c_str(),
                        static_cast<long long unsigned int>(timeDiff)));
        }
    };

    const Registration* _registration;
    Provider_Proc _proc;
    void* _provider;
};

inline Provider_Handle::Provider_Handle(const Registration* registration) :
    _registration(registration), 
    _proc(_registration->provider_proc), 
    _provider(0)
{
    (Create_Provider_Status)_proc(_registration,
        OPERATION_CREATE_PROVIDER, (void*)&_provider, 0, 0, 0, 0, 0, 0, 0);
}

inline Provider_Handle::~Provider_Handle()
{
    (Destroy_Provider_Status)_proc(_registration,
        OPERATION_DESTROY_PROVIDER, (void*)_provider, 0, 0, 0, 0, 0, 0, 0);
}

inline Get_Meta_Class_Status Provider_Handle::get_meta_class(
    const Meta_Class*& meta_class)
{
    return (Get_Meta_Class_Status)_proc(_registration, 
        OPERATION_GET_META_CLASS, 
        (Meta_Class**)&meta_class, 0, 0, 0, 0, 0, 0, 0);
}

inline Load_Status Provider_Handle::load()
{
    return (Load_Status)_proc(_registration,
        OPERATION_LOAD, (void*)_provider, 0, 0, 0, 0, 0, 0, 0);
}

inline Unload_Status Provider_Handle::unload()
{
    return (Unload_Status)_proc(_registration,
        OPERATION_UNLOAD, (void*)_provider, 0, 0, 0, 0, 0, 0, 0);
}

inline Enum_Instances_Status Provider_Handle::enum_instances(
    const Instance* model,
    Enum_Instances_Proc enum_instances_proc,
    void* client_data)
{
#ifdef CIMPLE_LOG_PROVIDER_OPS
    String            modelPath;
    Buffer            buff;

    instance_to_model_path(model, modelPath);
    buff.format("model='%s'", modelPath.c_str());

    FunctionLogger fl(this, __FUNCTION__, buff.data());
#endif

    return (Enum_Instances_Status)_proc(_registration, OPERATION_ENUM_INSTANCES,
        _provider, (void*)model, (void*)enum_instances_proc, client_data,
        0, 0, 0, 0);
}

inline Create_Instance_Status Provider_Handle::create_instance(
    Instance* instance)
{
    return (Create_Instance_Status)_proc(_registration, 
        OPERATION_CREATE_INSTANCE, 
        _provider, (void*)instance, 0, 0, 0, 0, 0, 0);
}

inline Delete_Instance_Status Provider_Handle::delete_instance(
    const Instance* instance)
{
    return (Delete_Instance_Status)_proc(_registration, 
    OPERATION_DELETE_INSTANCE, 
        (void*)_provider, (void*)instance, 0, 0, 0, 0, 0, 0);
}

inline Modify_Instance_Status Provider_Handle::modify_instance(
    const Instance* model,
    const Instance* instance)
{
    return (Modify_Instance_Status)_proc(_registration, 
        OPERATION_MODIFY_INSTANCE, 
        (void*)_provider, (void*)model, (void*)instance, 0, 0, 0, 0, 0);
}

inline Invoke_Method_Status Provider_Handle::invoke_method(
    const Instance* instance,
    const Instance* method)
{
#ifdef CIMPLE_LOG_PROVIDER_OPS
    String            instancePath;
    String            methodPath;
    Buffer            buff;

    instance_to_model_path(instance, instancePath);
    instance_to_model_path(method, methodPath);
    buff.format("model='%s', method='%s'",
                instancePath.c_str(), methodPath.c_str());

    FunctionLogger fl(this, __FUNCTION__, buff.data());
#endif

    return (Invoke_Method_Status)_proc(_registration, OPERATION_INVOKE_METHOD, 
        (void*)_provider, (void*)instance, (void*)method, 0, 0, 0, 0, 0);
}

inline Enable_Indications_Status Provider_Handle::enable_indications(
    Indication_Proc indication_proc,
    void* client_data)
{
#ifdef CIMPLE_LOG_PROVIDER_OPS
    FunctionLogger fl(this, __FUNCTION__, "none");
#endif

    return (Enable_Indications_Status)_proc(_registration,
        OPERATION_ENABLE_INDICATIONS, (void*)_provider, 
        (void*)indication_proc, client_data, 0, 0, 0, 0, 0);
}

inline Disable_Indications_Status Provider_Handle::disable_indications()
{
#ifdef CIMPLE_LOG_PROVIDER_OPS
    FunctionLogger fl(this, __FUNCTION__, "none");
#endif

    return (Disable_Indications_Status)_proc(_registration,
        OPERATION_DISABLE_INDICATIONS, (void*)_provider, 0, 0, 0, 0, 0, 0, 0);
}

inline Get_Repository_Status Provider_Handle::get_repository(
    const Meta_Repository*& meta_repository)
{
    return (Get_Repository_Status)_proc(_registration,
        OPERATION_GET_REPOSITORY, 
        (void*)&meta_repository, 0, 0, 0, 0, 0, 0, 0);
}

inline Enum_Associators_Status Provider_Handle::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Proc proc,
    void* client_data)
{
#ifdef CIMPLE_LOG_PROVIDER_OPS
    String            instPath;
    Buffer            buff;

    instance_to_model_path(instance, instPath);
    buff.format("instance='%s', result_class='%s', role='%s', "
                "result_role='%s'",
                instPath.c_str(), result_class.c_str(),
                role.c_str(), result_role.c_str());

    FunctionLogger fl(this, __FUNCTION__, buff.data());
#endif

    return (Enum_Associators_Status)_proc(
        _registration,
        OPERATION_ENUM_ASSOCIATORS,
        _provider, 
        (void*)instance, 
        (void*)&result_class, 
        (void*)&role, 
        (void*)&result_role,
        (void*)proc, 
        (void*)client_data,
        0);
}

CIMPLE_NAMESPACE_END

#endif /* _cimple_Provider_Handle_h */
