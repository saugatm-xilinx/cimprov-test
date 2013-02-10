/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_DeviceConnection_h
#define _cimple_CIM_DeviceConnection_h

#include <cimple/cimple.h>
#include "CIM_Dependency.h"
#include "CIM_LogicalDevice.h"
#include "CIM_LogicalDevice.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_DeviceConnection keys:

/*[1194]*/
class CIM_DeviceConnection : public Instance
{
public:
    // CIM_Dependency features:
    CIM_LogicalDevice* Antecedent;
    CIM_LogicalDevice* Dependent;

    // CIM_DeviceConnection features:
    Property<uint64> NegotiatedSpeed;
    Property<uint32> NegotiatedDataWidth;

    CIMPLE_CLASS(CIM_DeviceConnection)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_DeviceConnection_h */
