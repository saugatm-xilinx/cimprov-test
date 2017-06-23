/**********************************************************
 * Copyright 2013 VMware, Inc.  All rights reserved.
 **********************************************************/

/* **********************************************************
 * sf_mgmtInterface.c
 * **********************************************************/

#include <vmkapi.h> /* Required for VMKAPI definitions and macros */
#include "sf_mgmtInterface.h"

/*
 * In this file, we'll statically define and initialize the 
 * array of vmk_MgmtCallbackInfo structures that fully describe
 * all of the callbacks that the management interface we're 
 * using could invoke.  The array will include both user and
 * kernel callback descriptions.  In this example, there are
 * two callbacks and thus two vmk_MgmtCallbackInfo structures.
 *
 * We also statically define and initialize the management
 * signature, which includes the callbacks.
 *
 * This file is intended to be compiled for both user-space
 * and kernel-space builds.
 */

vmk_MgmtCallbackInfo mgmtCallbacks[MY_NUM_CALLBACKS] = {
   /*
    * The order of the callbacks in this array does not matter.
    * Here, we're enumerating the kernel space callback first.
    */
   {
      /*
       * The 'location' describes where this callback will run.
       */
      .location = VMK_MGMT_CALLBACK_KERNEL,
      /*
       * The 'callback' is the function that will be invoked.
       * This value may be a "don't care" if this file is being
       * compiled for user-space but the callback is a kernel
       * callback (or vice versa).  mgmtInterface.h conditionally
       * defines this.
       */
      .callback = testKernelCallback,
      /*
       * The 'synchronous' identifier indicates whether the callback
       * will be invoked synchronously from the caller, or whether
       * the callback is queued for later execution.  Notice that
       * only VMK_MGMT_CALLBACK_KERNEL callbacks can be synchronous.
       * Further, if your callback contains VMK_MGMT_PARMTYPE_OUT
       * or VMK_MGMT_PARMTYPE_INOUT parameters, your callback MUST
       * be synchronous (and thus must be a kernel callback).
       */
      .synchronous = 1, /* 0 indicates asynchronous */
      /*
       * 'numParms' indicates the number of non-cookie, non-instanceId parameters
       * that the callback accepts.  In this case, the callback takes
       * two parameters in addition to the cookie and instanceId, so 'numParms' is 2.
       * 'numParms' must be less than or equal to
       * VMK_MGMT_MAX_CALLBACK_PARMS.
       */
      .numParms = 1,
      /*
       * 'parmSizes' is an array indicating the size, in bytes, of each
       * non-cookie and non-instanceId parameter.  The array is in the order of the 
       * parameters that the callback takes (after the instanceId).
       */
      .parmSizes = { sizeof(macAddressType)},  /* the size of macAddressType */ 
      /*
       * 'parmTypes' is an array indicating how memory buffers carrying
       * the contents of the parameters to the callback should be treated.
       * There are three types:
       * VMK_MGMT_PARMTYPE_IN    -- an input parameter to the callback.
       * VMK_MGMT_PARMTYPE_OUT   -- an output parameter from the callback.
       * VMK_MGMT_PARMTYPE_INOUT -- a parameter that is both an input
       *                            to the callback and an output from it.
       * VMK_MGMT_PARMTYPE_IN parameters are copied from the caller into
       * a temporary buffer that is prepared prior to invoking the callback.
       * VMK_MGMT_PARMTYPE_OUT parameters give a scratch buffer as input
       * to the callback but then are copied back to the caller after the
       * callback is completed.
       * VMK_MGMT_PARMTYPE_INOUT parameters are copied from the caller into
       * a temporary buffer before the callback is invoked, then the 
       * callback is invoked, and then the contents of the temporary buffer
       * (presumably modified by the callback) is copied back to the caller.
       * NOTE:  In ALL cases, the contents of the buffers passed to the 
       *        callbacks are invalid after the callback has completed
       *        execution.
       * NOTE:  VMK_MGMT_PARMTYPE_OUT and VMK_MGMT_PARMTYPE_INOUT parameters
       *        require synchronous execution, and thus they can only be
       *        used for VMK_MGMT_CALLBACK_KERNEL callbacks.
       */
      .parmTypes = {  VMK_MGMT_PARMTYPE_INOUT},  /* 
                                                 * macAddressType is both an input and 
                                                 * output parameter
                                                 */
      /*
       * 'callbackId' must be unique among all callback IDs registered with
       * a given Management API signature.  It may not be any of the numbers
       * 0 through VMK_MGMT_RESERVED_CALLBACKS, inclusive.
       */
      .callbackId = TEST_CB_TO_KERNEL,
   },     
};

/*
 * The Management API Signature is used to define the overall signature of the
 * management interface that will be used.  It contains a name, vendor, version,
 * and a description of the callbacks that the interface contains.
 */
vmk_MgmtApiSignature mgmtSig = {
   /*
    * The version specifies the version of this interface.  Only callers that
    * have the same major number are considered compatible.  The VMKAPI 
    * management APIs will be extended to support compatibility shimming in
    * future versions of VMKAPI.  For now, you can use this to express what
    * interfaces are compatible with which callers and callees.
    */
   .version = VMK_REVISION_FROM_NUMBERS(1,0,0,0),
   /*
    * The name is the name of this interface.  The name and vendor,
    * must be globally unique or else
    * initialization will fail.  Only user-space connections from
    * with signatures bearing the same name and vendor will be allowed.
    * That is, vmk_MgmtUserInit will only succeed if there's a matching
    * name and vendor.
    * NOTE:  Your name must be 32 characters or fewer, including NUL.
    */
   .name.string = MY_INTERFACE_NAME,
   /*
    * The vendor is the vendor providing this interface.  The name and
    * vendor, must be globally unique or
    * else initialization will fail.  Only user-space connections from
    * with signatures bearing the same name and vendor will be allowed.
    * That is, vmk_MgmtUserInit will only succeed if there's a matching
    * name and vendor.
    * NOTE:  Your vendor must be 32 characters or fewer, including NUL.
    */
   .vendor.string = MY_INTERFACE_VENDOR,
   /*
    * 'numCallbacks' is the total number of callbacks and thus also
    * specifies the number of elements in the 'callbacks' array.
    */
   .numCallbacks = MY_NUM_CALLBACKS,
   /*
    * 'callbacks' is the array vmk_MgmtCallbackInfo structures that
    * fully describe each individual callback that can be invoked by
    * the interface in this API signature.
    */
   .callbacks = mgmtCallbacks,
};
