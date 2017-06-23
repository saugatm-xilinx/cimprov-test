
/* **********************************************************
 * mgmtInterface.h
 * **********************************************************/

#ifndef SOLARFLARE_SF_MGMTINTERFACE_H__
#define SOLARFLARE_SF_MGMTINTERFACE_H__ 1

#include <vmkapi.h> /* Required for VMKAPI type definitions */
#define TEST_CB_TO_KERNEL (VMK_MGMT_RESERVED_CALLBACKS + 1)


/*
 * The total number of callbacks (kernel).
 */
#define MY_NUM_CALLBACKS 1

/*
 * The name used to describe this interface.
 */
#define MY_INTERFACE_NAME   "macAddressCallback"
/*
 * The vendor for this interface.
 */
#define MY_INTERFACE_VENDOR "solarflare"
/*
 * You should place type definitions that are shared between
 * user and kernel-space (such as those passed between user
 * and kernel-space) in this file.
 */
#define MGMT_PARM_STRLEN 128
#define MGMT_MAC_ADDRLEN  6

/*
 * MAC Address type datastructure contains the 6 byte MAC Address
 * Used to send data from Kernel to User space
 */
typedef struct _macAddressType {

   vmk_uint8  macAddress[MGMT_MAC_ADDRLEN];

} __attribute__((__packed__)) macAddressType;

/*
 * Here, we conditionally define the prototypes for the
 * callback functions that will be invoked by the VMKAPI
 * management APIs.
 */
#ifdef VMKERNEL
/*
 * These are the definitions of prototypes as viewed from
 * kernel-facing code.  Kernel callbacks have their prototypes 
 * defined.  User callbacks, in this section, will be 
 * #define'd as NULL, since their callback pointer has no
 * meaning from the kernel perspective.
 *
 * All callbacks must return an integer, and must take two
 * metadata parameters.  User callbacks take two vmk_uint64
 * paramaters as the first two arguments.  The first argument is
 * the cookie value, and the second is an instance ID from which
 * the callback originated in the kernel.  Kernel callbacks take
 * a vmk_MgmtCookies pointer as the first parameter and a
 * vmk_MgmtEnvelope pointer as the second parameter.  The cookies
 * structure contains handle-wide and session-specific cookie
 * values.  The envelope structure contains a session ID (indicating
 * which session the callback request originated from) and an
 * instance ID (indicating which specific instance, if any, this
 * callback is destined for).  When not addressing specific instances
 * or tracking instance-specific callback invocations, simply use
 * VMK_MGMT_NO_INSTANCE_ID for this parameter.  Regarding session IDs,
 * kernel management handles support simultaneous access by user-space
 * applications, thus the callbacks convey more information about which
 * session the callback invocation is associated with.  The return type merely
 * indicates that the callback ran to completion without
 * error - it does not indicate the semantic success or failure
 * of the operation.  The cookie argument passed to the callback
 * is the same value that was given as the 'cookie' parameter
 * during initialization.  Thus kernel callbacks get a handle cookie
 * provided to vmk_MgmtInit() (in addition to the session-specific cookie
 * that a kernel module may set in its session-announcement function), and
 * provided to vmk_MgmtUserInit().  The instanceId corresponds
 * to the specific instance that this callback is targeted to
 * (if it's a kernel-located callback) or the instance from
 * which the callback originates (if it's a user-located callback).
 *
 */
#define testUserCallback NULL

/*
 * As with the previous samples, the semantics for the buffers
 * that carry the data for parameters ("flagFromUser" and
 * "mgmtParm") are determined by the parameter type and whether
 * the callback is synchronous.  In this case, you'll see that
 * flagFromUser is an input parameter, meaning that changes to
 * that parameter in the callback aren't reflected back to the 
 * caller.  mgmtParm is an input-output parameter, which means
 * that changes to it are copied back to the user-space caller
 * at the completion of the call.
 */
int testKernelCallback(vmk_MgmtCookies *cookies,
                       vmk_MgmtEnvelope *envelope,
                       macAddressType *macAddr);
#else 
/*
 * This section is where callback definitions, as visible to
 * user-space, go.  In this example, there are is one user-run 
 * callback: testUserCallback.
 *
 * This callback takes two payload parameters: eventCount 
 * and statParm.  The semantics of the buffers used for 
 * eventCount and statParm are determined by the individual
 * parameter type, as specified in the vmk_MgmtCallbackInfo
 * corresponding to this callback.  In the case of user-running
 * callbacks (which this is), all callbacks are asynchronous
 * and therefore all parameters can only be of the type
 * VMK_MGMT_PARMTYPE_IN.  This means that any changes by 
 * testUserCallback to the buffers used for those parameters
 * will not be reflected in the kernel. 
 */
	#define testUserCallback NULL
 /*
  * Kernel-run callbacks are defined as NULL for the user-
  * compiled portion of the interface.
  */
 #define testKernelCallback NULL
#endif /* VMKERNEL */ 

#endif /* __MGMTINTERFACE_H__ */
