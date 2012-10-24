// Generated by genprov 2.0.24
#include "SF_LogEntry_Provider.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

SF_LogEntry *SF_LogEntry_Provider::makeReference(const solarflare::Logger& parent,
                                                const solarflare::LogEntry& entry)
{
    Buffer buf;
    SF_LogEntry *l = SF_LogEntry::create(true);
    
    buf.appends(solarflare::System::target.prefix().c_str());
    buf.append(':');
    buf.appends(parent.description());
    buf.append('#');
    buf.append_uint64(entry.id());
    l->InstanceID.set(buf.data());
    return l;
}

bool SF_LogEntry_Provider::Enum::process(const solarflare::LogEntry& entry)
{
    static unsigned const severityMap[] = {
        SF_LogEntry::_PerceivedSeverity::enum_Fatal_NonRecoverable,
        SF_LogEntry::_PerceivedSeverity::enum_Major,
        SF_LogEntry::_PerceivedSeverity::enum_Minor,
        SF_LogEntry::_PerceivedSeverity::enum_Information,
        SF_LogEntry::_PerceivedSeverity::enum_Information,
    };

    char id[17];
    SF_LogEntry *le = makeReference(*owner, entry);
    le->LogInstanceID.set(solarflare::System::target.prefix());
    le->LogInstanceID.value.append(":");
    le->LogInstanceID.value.append(owner->description());
    le->LogName.set(owner->description());
    sprintf(id, "%16.16llx", entry.id());
    le->ElementName.set(id);
    le->RecordID.set(id);
    le->RecordFormat.set("");
    le->RecordData.set(entry.message());
    le->CreationTimeStamp.set(entry.stamp());
    le->PerceivedSeverity.null = false;
    le->PerceivedSeverity.value = severityMap[owner->severity()];
    
    handler->handle(le);
    return true;
}


SF_LogEntry_Provider::SF_LogEntry_Provider()
{
}

SF_LogEntry_Provider::~SF_LogEntry_Provider()
{
}

Load_Status SF_LogEntry_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_LogEntry_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_LogEntry_Provider::get_instance(
    const SF_LogEntry* model,
    SF_LogEntry*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_LogEntry_Provider::enum_instances(
    const SF_LogEntry* model,
    Enum_Instances_Handler<SF_LogEntry>* handler)
{
    for (unsigned i = 0; solarflare::Logger::knownLogs[i] != NULL; i++)
    {
        Enum records(solarflare::Logger::knownLogs[i], handler);
        solarflare::Logger::knownLogs[i]->forAllEntries(records);
    }
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_LogEntry_Provider::create_instance(
    SF_LogEntry* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_LogEntry_Provider::delete_instance(
    const SF_LogEntry* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_LogEntry_Provider::modify_instance(
    const SF_LogEntry* model,
    const SF_LogEntry* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
