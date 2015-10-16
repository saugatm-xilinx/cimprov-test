#ifndef SFUPDATE_CIM_CIM_CLIENT_H
#define SFUPDATE_CIM_CIM_CLIENT_H 1

#include "cim_log.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <curl/curl.h>

/**
 * Skip text nodes when enumerating children of an XML node.
 *
 * @param node_   Current child node pointer.
 */
#define SKIP_TEXT(node_) \
    do {                                            \
        while (node_ != NULL &&                     \
               xmlStrcmp(node_->name,               \
                         BAD_CAST "text") == 0)     \
            node_ = node_->next;                    \
    } while (0)

/**< CIM instance description got from response XML */
typedef struct xmlCimInstance {
    xmlChar     *class_name;        /**< Class name */
    xmlNodePtr   namespace_path;    /**< Namespace */
    xmlNodePtr   instance_name;     /**< Instance name (key values) */
    xmlNodePtr   instance;          /**< Instance (values of all
                                         properties) */

    struct xmlCimInstance *prev;    /**< Previous instance in the list */
    struct xmlCimInstance *next;    /**< Next instance in the list */
} xmlCimInstance;

/**
 * Free memory allocated for xmlCimInstance.
 *
 * @param p   xmlCimInstance pointer
 */
extern void freeXmlCimInstance(xmlCimInstance *p);

/**
 * Create a copy of xmlCimInstance.
 *
 * @param p   xmlCimInstance pointer
 */
extern xmlCimInstance * xmlCimInstanceDup(xmlCimInstance *p);

/**
 * Sort a list of CIM instances.
 *
 * @param head    List's head
 * @param f       Comparison function
 * @param arg     Additional argument to be passed to f()
 */
extern void sort_inst_list(xmlCimInstance **head,
                           int (*f)(xmlCimInstance *,
                                    xmlCimInstance *, void *),
                           void *arg);

/**
 * Return property value of a CIM instance.
 *
 * @param p                 CIM instance pointer
 * @param prop_name         Property name
 * @param value       [out] Property value
 * @param type        [out] Property type
 *
 * @return 0 on success, -1 on failure
 */
extern int xmlCimInstGetProp(xmlCimInstance *p,
                             const char *prop_name,
                             xmlChar **value,
                             xmlChar **type);

/**
 * Get property value of a CIM instance and remove spaces
 * from the ends.
 *
 * @param p                 CIM instance pointer
 * @param prop_name         Property name
 * @param value       [out] Property value
 * @param type        [out] Property type
 *
 * @return 0 on success, -1 on failure
 */
extern int xmlCimInstGetPropTrim(xmlCimInstance *p,
                                 const char *prop_name,
                                 char **value,
                                 xmlChar **type);

/**
 * Compare instances by value of the specified property.
 *
 * @param p     CIM instance 1
 * @param q     CIM instance 2
 * @param arg   Property name
 *
 * @return result of strcmp() on the property values on success,
 *         0 on failure
 */
extern int compare_by_prop(xmlCimInstance *p, xmlCimInstance *q,
                           void *arg);

/**
 * Check wether a given property of a CIM instance
 * has a specified value.
 *
 * @param p           CIM instance
 * @param prop_name   Property name
 * @param value       Property value
 *
 * @return 0 on success, -1 on failure
 */
extern int xmlCimInstPropCmp(xmlCimInstance *p,
                             const char *prop_name,
                             const char *value);

/**
 * Construct CIM-XML request down to <SIMPLEREQ> node
 *
 * @return XML document pointer for constructed CIM-XML request
 */
extern xmlDocPtr xmlReqPrepareSimpleReq(xmlNodePtr *simplereq_node);

/**
 * Add local instance path node in a CIM-XML request
 *
 * @param parent      Parent node
 * @param namespace   Namespace
 * @param inst        Instance pointer
 */
extern void addLocalInstPath(xmlNodePtr parent,
                             const char *namespace,
                             xmlCimInstance *inst);

/**
 * Construct CIM-XML method call request.
 *
 * @param imethod                 Whether it is intrinsic or extrinsic
 *                                method call
 * @param namespace               Namespace
 * @param method_name             Method name
 * @param methodcall_node   [out] <METHODCALL> or <IMETHODCALL> node
 *                                pointer
 *
 * @return  XML document pointer of the constructed CIM-XML request
 */
extern xmlDocPtr xmlReqPrepareMethodCall(int imethod,
                                         const char *namespace,
                                         xmlCimInstance *inst,
                                         const char *method_name,
                                         xmlNodePtr *methodcall_node);

/**
 * Add a node for a parameter having "simple" value
 * (i.e. string, uint32, etc).
 *
 * @param parent_node       Where to add an XML node
 * @param param_name        Name of the parameter
 * @param param_type        Type of the parameter
 * @param param_value       Value of the parameter
 */
extern void addSimpleParamValue(xmlNodePtr parent_node,
                                const char *param_name,
                                const char *param_type,
                                const char *param_value);

/**
 * Structure for storing named values (for example,
 * output parameters)
 */
typedef struct named_value {
    char *name;                 /**< Name */
    char *value;                /**< Value */

    struct named_value *prev;   /**< Previous item in the list */
    struct named_value *next;   /**< Next item in the list */
} named_value;

/**
 * Free memory allocated for a named_value.
 *
 * @param val   Memory to be freed
 */
extern void free_named_value(named_value *val);

/**
 * Free memory allocated for a list of named_value's.
 *
 * @param head      List head
 */
extern void free_named_value_list(named_value *head);

/**
 * Get named value by its name from the list of named values.
 *
 * @param list      List of named values
 * @param name      Name
 *
 * @return Value of interest or NULL if not found
 */
extern char *get_named_value(named_value *list, const char *name);

/** Structure for storing parsed CIM-XML response */
typedef struct response_descr {
    char           *method_name;        /**< Intrinsic or extrinsic
                                             method name */
    int             imethod_called;     /**< Whether intrinsic method
                                             was called or not */
    char           *returned_value;     /**< Returned value (for extrinsic
                                             method) */
    named_value    *out_params_list;    /**< List of output parameters
                                             (for extrinsic method) */
    int             out_params_count;   /**< Number of output parameters */
    int             error_returned;     /**< Whether an error was returned
                                             or not */
    char           *err_code;           /**< Error code */
    char           *err_descr;          /**< Error description */
    xmlCimInstance *inst_list;          /**< Returned instances list */
    int             inst_count;         /**< Number of instances in the
                                             list */
} response_descr;

/**
 * Free memory allocated for storing members of a response_descr structure
 *
 * @param rsp   response_descr structure pointer
 */
extern void clear_response(response_descr *rsp);

/**
 * Send CIM-XML request, receive response and parse it
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param doc               XML document pointer of a CIM request
 * @param method_name       Intrinsic or extrinsic method name
 * @param response    [out] response_descr structure to be filled
 *
 * @return 0 on success, -1 on failure
 */
extern int
processXmlRequest(CURL *curl, xmlDocPtr doc,
                  const char *method_name,
                  response_descr *response);

/**
 * Call EnumerateInstances or EnumerateInstanceNames method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param class_name        Class name
 * @param ein               Whether we should call EnumerateInstanceNames()
 *                          or EnumerateInstances() method
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
extern int
enumerate_instances(CURL *curl, const char *namespace,
                    const char *class_name, int ein,
                    response_descr *response);

/**
 * Call Associators() or AssociatorNames() method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param inst              CIM instance for which to find associators
 * @param role              Instance's role in the association
 * @param assoc_class       Association class name
 * @param result_role       Result role in the association
 * @param result_class      Result class name
 * @param an                Whether we should enumerate associator names
 *                          or associators
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
extern int associators(CURL *curl, const char *namespace,
                       xmlCimInstance *inst,
                       const char *role, const char *assoc_class,
                       const char *result_role,
                       const char *result_class,
                       int an, response_descr *response);

/**
 * Call intrinsic or extrinsic CIM method and check response
 *
 * @param rc_   Return value to be set on failure
 * @param f_    Method call
 * @param rsp_  Response to be filled
 * @param msg_  Error message format string
 * @param ...   Error message parameters
 */
#define CHECK_RESPONSE(rc_, f_, rsp_, msg_...) \
    do {                                                                \
        int ret_ = (f_);                                                \
                                                                        \
        if (ret_ < 0 || rsp_.error_returned)                            \
        {                                                               \
            ERROR_MSG(msg_);                                            \
            if (ret_ >= 0)                                              \
                ERROR_MSG("CIM ERROR: code='%s', description='%s'",     \
                          (char *)rsp_.err_code,                        \
                          (char *)rsp_.err_descr);                      \
            rc_ = -1;                                                   \
        }                                                               \
    } while (0)

/**
 * Call extrinsic CIM method and check response and return code
 *
 * @param rc_   Return value to be set on failure
 * @param f_    Method call
 * @param rsp_  Response to be filled
 * @param msg_  Error message format string
 * @param ...   Error message parameters
 */
#define CHECK_RESPONSE_EXT(rc_, f_, rsp_, msg_...) \
    do {                                                                \
        int ret_ = (f_);                                                \
                                                                        \
        if (ret_ < 0 || rsp_.error_returned ||                          \
            strcmp(rsp_.returned_value, "0") != 0)                      \
        {                                                               \
            if (ret_ >= 0)                                              \
            {                                                           \
                if (rsp_.error_returned)                                \
                    ERROR_MSG("CIM ERROR: code='%s', description='%s'", \
                              (char *)rsp_.err_code,                    \
                              (char *)rsp_.err_descr);                  \
                else                                                    \
                    ERROR_MSG("Extrinsic method returned '%s'",         \
                              rsp_.returned_value);                     \
            }                                                           \
            ERROR_MSG(msg_);                                            \
            rc_ = -1;                                                   \
        }                                                               \
    } while (0)

#endif // SFUPDATE_CIM_CIM_CLIENT_H
