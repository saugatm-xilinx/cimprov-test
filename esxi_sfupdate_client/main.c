#include <stdlib.h>
#include <stdio.h>

#include <popt.h>

#ifdef NCURSES_CURSES_H
#include <ncurses/curses.h>
#else
#include <curses.h>
#endif

#include <string.h>
#include <curl/curl.h>

#include <openssl/evp.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "../libprovider/sf_base64.h"

#include <assert.h>

#include "fw_images.h"

/* Maximum password length */
#define MAX_PASSWD_LEN 128

/* Maximum error message length */
#define MAX_ERR_STR 1024

/* Maximum file path length */
#define MAX_FILE_PATH 1024

/**
 * Print error message with file name and line number
 * specified.
 *
 * @param s_    Format string
 * @param ...   Arguments
 */
#define ERROR_MSG(s_...) \
    do {                                                    \
        char str_[MAX_ERR_STR];                             \
        snprintf(str_, MAX_ERR_STR, s_);                    \
        fprintf(stderr, "File %s, Line %d: %s\n",           \
                __FILE__, __LINE__, str_);                  \
    } while (0)

/**
 * Print error message without file name and line number
 * specified.
 *
 * @param s_    Format string
 * @param ...   Arguments
 */
#define ERROR_MSG_PLAIN(s_...) \
    do {                                                    \
        char str_[MAX_ERR_STR];                             \
        snprintf(str_, MAX_ERR_STR, s_);                    \
        fprintf(stderr, "%s\n", str_);                      \
    } while (0)

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

/** CIMOM address */
static char *cim_address = NULL;

/** Whether we should use HTTPS or HTTP */
static int   use_https = 0;

/**
 * If set, we not only show currently installed firmware
 * but also try to update it
 */
static int   do_update = 0;

/** CIMOM user name */
static char *user = NULL;

/** CIMOM user password */
static char *password = NULL;

/** CIM provider namespace */
static char *cim_namespace = NULL;

/** URL of firmware image(s) */
static char *fw_url = NULL;

/** Path to firmware image(s) */
static char *fw_path = NULL;

/**
 * Force firmware update even if version of
 * the firmware image is lower than or the same as
 * already installed.
 */
static int force_update = 0;

/** Network interface name */
static char *interface_name = NULL;

/** Whether we should update controller firmware or not */
static int   update_controller = 0;

/** Whether we should update BootROM firmware or not */
static int   update_bootrom = 0;

/**
 * Whether we can skip asking user for confirmation before firmware update
 * or not
 */
static int   yes = 0;

/* Default HTTP port */
#define DEF_HTTP_PORT "5988"

/* Default HTTPS port */
#define DEF_HTTPS_PORT "5989"

/* Maximum length of a string representing CIMOM address */
#define MAX_ADDR_LEN 1024

/** Command line options */
enum {
    OPT_CIMOM_ADDR = 1,       /**< CIMOM address - use either it or 
                                   HOST/PORT/HTTPS to specify CIMOM */
    OPT_HTTPS,                /**< Use HTTPS */
    OPT_CIMOM_USER,           /**< CIMOM user name */
    OPT_CIMOM_PASSWORD,       /**< CIMOM user password */
    OPT_PROVIDER_NAMESPACE,   /**< CIM provider namespace */
    OPT_FW_URL,               /**< URL of firmware image(s) */
    OPT_FW_PATH,              /**< Path to firmware image(s) */
    OPT_FORCE,                /**< Force firmware update even
                                   if version of the image is
                                   lower than or the same as already
                                   installed */
    OPT_IF_NAME,              /**< Network interface name */
    OPT_CONTROLLER,           /**< Process controller firmware */
    OPT_BOOTROM,              /**< Process BootROM firmware */
    OPT_YES,                  /**< Don't ask user for confirmation when
                                   updating firmware */
    OPT_WRITE,                /**< Perform firmware update */
};

/**
 * Parse command line options.
 *
 * @param argc    Number of arguments
 * @param argv    Array of arguments
 *
 * @return 0 on success, -1 on failure
 */
int
parseCmdLine(int argc, const char *argv[])
{
    char *a = NULL;
    int   https = 0;
    char *u = NULL;
    char *p = NULL;
    char *ns = NULL;
    char *s = NULL;
    char *url = NULL;
    char *path = NULL;
    int   force = 0;
    char *if_name = NULL;
    int   controller = 0;
    int   bootrom = 0;
    int   y = 0;
    int   w = 0;
    char  passwd_buf[MAX_PASSWD_LEN];

    poptContext     optCon;
    int             opt;
    int             rc = 0;

    struct poptOption options_table[] = {
        { "cim-address", 'a', POPT_ARG_STRING, NULL,
          OPT_CIMOM_ADDR,
          "Address of CIM Server (e.g. 'https://hostname:5989' or "
          "'hostname' or 'hostname:5988' - default protocol is HTTP, "
          "default port for HTTP is 5988, default port for HTTPS is "
          "5989)",
          NULL },

        { "https", 's', POPT_ARG_NONE, NULL,
          OPT_HTTPS,
          "Use HTTPS to access CIM Server (has no effect if you "
          "specified protocol in --cim-address parameter)",
          NULL },

        { "cim-user", 'u', POPT_ARG_STRING, NULL,
          OPT_CIMOM_USER,
          "CIM Server user's name",
          NULL },

        { "cim-password", 'p', POPT_ARG_STRING, NULL,
          OPT_CIMOM_PASSWORD,
          "CIM Server user's password",
          NULL },

        { "cim-namespace", 'n', POPT_ARG_STRING, NULL,
          OPT_PROVIDER_NAMESPACE,
          "CIM Provider namespace (solarflare/cimv2 by default)",
          NULL },

        { "interface-name", 'i', POPT_ARG_STRING, NULL,
          OPT_IF_NAME,
          "Interface name (if not specified, firmware for "
          "on interfaces would be processed)",
          NULL },

        { "controller", '\0', POPT_ARG_NONE, NULL,
          OPT_CONTROLLER,
          "Process controller firmware",
          NULL },

        { "bootrom", '\0', POPT_ARG_NONE, NULL,
          OPT_BOOTROM,
          "Process bootrom firmware",
          NULL },

        { "yes", 'y', POPT_ARG_NONE, NULL,
          OPT_YES,
          "Do not ask for confirmation before updating firmware",
          NULL },

        { "write", 'w', POPT_ARG_NONE, NULL,
          OPT_WRITE,
          "Perform firmware update",
          NULL },

        { "firmware-url", 'f', POPT_ARG_STRING, NULL,
          OPT_FW_URL,
          "URL of firmware image(s) to be used instead of internal ones",
          NULL },

        { "firmware-path", '\0', POPT_ARG_STRING, NULL,
          OPT_FW_PATH,
          "Path to firmware image(s) to be used instead of internal ones",
          NULL },

        { "force", '\0', POPT_ARG_NONE, NULL,
          OPT_FORCE,
          "Update firmware even if version of the "
          "image is lower than or the same as already installed",
          NULL },

        POPT_AUTOHELP
        POPT_TABLEEND
    };

    optCon = poptGetContext(NULL, argc, (const char **)argv,
                            options_table, 0);
    while ((opt = poptGetNextOpt(optCon)) >= 0)
    {
        switch (opt)
        {
            case OPT_CIMOM_ADDR:
                a = poptGetOptArg(optCon);
                break;

            case OPT_HTTPS:
                https = 1;
                break;

            case OPT_CIMOM_USER:
                u = poptGetOptArg(optCon);
                break;

            case OPT_CIMOM_PASSWORD:
                p = poptGetOptArg(optCon);
                break;

            case OPT_PROVIDER_NAMESPACE:
                ns = poptGetOptArg(optCon);
                break;

            case OPT_FW_URL:
                url = poptGetOptArg(optCon);
                break;

            case OPT_FW_PATH:
                path = poptGetOptArg(optCon);
                break;

            case OPT_FORCE:
                force = 1;
                break;

            case OPT_IF_NAME:
                if_name = poptGetOptArg(optCon);
                break;

            case OPT_CONTROLLER:
                controller = 1;
                break;

            case OPT_BOOTROM:
                bootrom = 1;
                break;

            case OPT_YES:
                y = 1;
                break;

            case OPT_WRITE:
                w = 1;
                break;

            default:
                ERROR_MSG("Unexpected option number %d", opt);
                rc = -1;
                goto cleanup;
        }
    }

    if (opt != -1)
    {
        ERROR_MSG("%s:%s", poptBadOption(optCon, 0),
                  poptStrerror(opt));
        rc = -1;
        goto cleanup;
    }
    if ((s = (char *)poptGetArg(optCon)) != NULL)
    {
        ERROR_MSG("Unexpected arguments in command line: %s", s);
        rc = -1;
        goto cleanup;
    }

    if (p == NULL)
    {
        initscr();
        noecho();
        printw("Please, enter the password: ");
        getnstr(passwd_buf, MAX_PASSWD_LEN);
        p = strdup(passwd_buf);
        endwin();
    }

cleanup:
    poptFreeContext(optCon);
    free(s);
    if (rc < 0)
    {
        free(a);
        free(u);
        free(p);
        free(ns);
        free(url);
        free(path);
        free(if_name);
    }
    else
    {
        cim_address = a;
        use_https = https;
        user = u;
        password = p;
        if (ns != NULL)
        {
            free(cim_namespace);
            cim_namespace = ns;
        }
        fw_url = url;
        fw_path = path;
        force_update = force;
        interface_name = if_name;
        update_controller = controller;
        update_bootrom = bootrom;
        yes = y;
        do_update = w;
    }

    return rc;
}

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
void
freeXmlCimInstance(xmlCimInstance *p)
{
    xmlFree(p->class_name);
    xmlFreeNode(p->namespace_path);
    xmlFreeNode(p->instance_name);
    xmlFreeNode(p->instance);
    free(p);
}

/**
 * Sort a list of CIM instances.
 *
 * @param head    List's head
 * @param f       Comparison function
 * @param arg     Additional argument to be passed to f()
 */
void
sort_inst_list(xmlCimInstance **head,
               int (*f)(xmlCimInstance *, xmlCimInstance *, void *),
               void *arg)
{
    xmlCimInstance *p;
    xmlCimInstance *q;
    int             swapped;

    if (head == NULL || *head == NULL)
        return;
    do {
        swapped = 0;
        for (p = (*head)->next; p != NULL; p = p->next)
        {
            if (f(p->prev, p, arg) > 0)
            {
                q = p->prev;
                q->next = p->next;
                p->next = q;
                p->prev = q->prev;
                q->prev = p;
                if (p->prev == NULL)
                    *head = p;
                if (p->prev != NULL)
                    p->prev->next = p;
                if (q->next != NULL)
                    q->next->prev = q;
                p = q;
                swapped = 1;
            }
        }
    } while (swapped);
}

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
int
xmlCimInstGetProp(xmlCimInstance *p,
                  const char *prop_name,
                  xmlChar **value,
                  xmlChar **type)
{
    xmlNodePtr  child;
    xmlNodePtr  aux;
    xmlChar    *name;
    int         found = 0;

    if (p->instance_name != NULL)
    {
        child = p->instance_name->children; 
        while (child != NULL && !found)
        {
            if (xmlStrcmp(child->name,
                          BAD_CAST "KEYBINDING") == 0)
            {
                name = xmlGetProp(child, BAD_CAST "NAME");
                if (xmlStrcmp(name, BAD_CAST prop_name) == 0)
                {
                    found = 1;
                    aux = child->children;
                    SKIP_TEXT(aux);
                    if (xmlStrcmp(aux->name, BAD_CAST "KEYVALUE") == 0)
                    {
                        *value = xmlNodeGetContent(aux);
                        if (type != NULL)
                            *type = xmlGetProp(aux, BAD_CAST "VALUETYPE");
                    }
                    else
                        *value = NULL;
                }
                xmlFree(name);
            }
            child = child->next;
        }
    }

    if (p->instance != NULL && !found)
    {
        child = p->instance->children; 
        while (child != NULL && !found)
        {
            if (xmlStrcmp(child->name,
                          BAD_CAST "PROPERTY") == 0)
            {
                name = xmlGetProp(child, BAD_CAST "NAME");
                if (xmlStrcmp(name, BAD_CAST prop_name) == 0)
                {
                    found = 1;

                    if (type != NULL)
                        *type = xmlGetProp(aux, BAD_CAST "TYPE");

                    *value = xmlNodeGetContent(child);
                }
                xmlFree(name);
            }
            child = child->next;
        }
    }

    if (!found)
        return -1;
    return 0;
}

/**
 * For a given string, return a copy with spaces at the
 * begin and the end removed.
 *
 * @param s     String pointer
 *
 * @return  String with spaces removed at the ends
 */
char *str_trim(char *s)
{
    char *result = NULL;
    char *first = NULL;
    int   i;

    if (s == NULL)
        return NULL;

    first = s + strspn(s, " \t\r\n");
    result = strdup(first);
    for (i = strlen(result) - 1; i >= 0; i--)
        if (result[i] == ' ' ||
            result[i] == '\t' ||
            result[i] == '\r' ||
            result[i] == '\n')
            result[i] = '\0';
        else
            break;

    return result;
}

/**
 * Convert xmlChar string to char string with
 * spaces removed from the begin and the end.
 *
 * @param s   xmlChar string
 *
 * @return    char string with spaces removed at the ends
 */
char *str_trim_xml_free(xmlChar *s)
{
    char *result = str_trim((char *)s);
    xmlFree(s);
    return result;
}

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
int
xmlCimInstGetPropTrim(xmlCimInstance *p,
                      const char *prop_name,
                      char **value,
                      xmlChar **type)
{
    xmlChar *xmlVal;

    if (xmlCimInstGetProp(p, prop_name, &xmlVal, type) < 0)
        return -1;

    *value = str_trim((char *)xmlVal);
    xmlFree(xmlVal);

    return 0;
}

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
int
compare_by_prop(xmlCimInstance *p, xmlCimInstance *q, void *arg)
{
    char *p_id = NULL;
    char *q_id = NULL;
    char *prop_name = (char *)arg;
    int   result;

    if (xmlCimInstGetPropTrim(p, prop_name, &p_id, NULL) < 0 ||
        xmlCimInstGetPropTrim(q, prop_name, &q_id, NULL) < 0 ||
        p_id == NULL || q_id == NULL)
    {
        ERROR_MSG("%s(): failed to get '%s' value",
                  __FUNCTION__, prop_name);
        return 0;
    }

    result = strcmp(p_id, q_id);
    free(p_id);
    free(q_id);

    return result;
}

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
int
xmlCimInstPropCmp(xmlCimInstance *p,
                  const char *prop_name,
                  const char *value)
{
    int      rc = 0;
    char    *prop_value;

    if (xmlCimInstGetPropTrim(p, prop_name,
                              &prop_value, NULL) < 0 ||
        prop_value == NULL)
        return -1;

    if (strcmp(prop_value, value) == 0)
        rc = 0;
    else
        rc = -1;

    free(prop_value);
    return rc;
}

/**
 * Fill namespace-describing child nodes in a given XML node.
 *
 * @param parent_node     Parent
 * @param namespace       Namespace
 */
void
fillNamespace(xmlNodePtr parent_node, const char *namespace)
{
    char           *namespace_dup = NULL;
    char           *token = NULL;
    xmlNodePtr      localnspath_node = NULL;
    xmlNodePtr      namespace_node = NULL;

    namespace_dup = strdup(namespace);

    localnspath_node = xmlNewChild(parent_node, NULL,
                                   BAD_CAST "LOCALNAMESPACEPATH", NULL);
    token = strtok(namespace_dup, "/");
    while (token != NULL)
    {
        namespace_node = xmlNewChild(localnspath_node, NULL,
                                     BAD_CAST "NAMESPACE", NULL);
        xmlNewProp(namespace_node, BAD_CAST "NAME", BAD_CAST token);
        token = strtok(NULL, "/");
    }
    free(namespace_dup);
}

/**
 * Construct CIM-XML request down to <SIMPLEREQ> node
 *
 * @return XML document pointer for constructed CIM-XML request
 */
xmlDocPtr
xmlReqPrepareSimpleReq(xmlNodePtr *simplereq_node)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      cim_node = NULL;
    xmlNodePtr      message_node = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");

    cim_node = xmlNewNode(NULL, BAD_CAST "CIM");
    xmlNewProp(cim_node, BAD_CAST "CIMVERSION", BAD_CAST "2.0");
    xmlNewProp(cim_node, BAD_CAST "DTDVERSION", BAD_CAST "2.0");
    xmlDocSetRootElement(doc, cim_node);

    message_node = xmlNewChild(cim_node, NULL, BAD_CAST "MESSAGE", NULL);
    xmlNewProp(message_node, BAD_CAST "ID", BAD_CAST "1001");
    xmlNewProp(message_node, BAD_CAST "PROTOCOLVERSION", BAD_CAST "1.0");

    *simplereq_node = xmlNewChild(message_node, NULL,
                                  BAD_CAST "SIMPLEREQ", NULL);

    return doc;
}

/**
 * Add local instance path node in a CIM-XML request
 *
 * @param parent      Parent node
 * @param namespace   Namespace
 * @param inst        Instance pointer
 */
void
addLocalInstPath(xmlNodePtr parent, const char *namespace,
                 xmlCimInstance *inst)
{
    xmlNodePtr localinstpath_node;
    xmlNodePtr inst_name_copy_node;

    localinstpath_node = xmlNewChild(parent, NULL,
                                     BAD_CAST "LOCALINSTANCEPATH",
                                     NULL);
    fillNamespace(localinstpath_node, namespace);
    inst_name_copy_node = xmlCopyNode(inst->instance_name, 1);
    xmlAddChild(localinstpath_node, inst_name_copy_node);
}

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
xmlDocPtr
xmlReqPrepareMethodCall(int imethod, const char *namespace,
                        xmlCimInstance *inst,
                        const char *method_name,
                        xmlNodePtr *methodcall_node)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      methodcall_node_aux = NULL;
    xmlNodePtr      simplereq_node = NULL;
    xmlNodePtr      localinstpath_node = NULL;
    xmlNodePtr      inst_name_copy_node = NULL;

    doc = xmlReqPrepareSimpleReq(&simplereq_node);
    methodcall_node_aux =
        xmlNewChild(simplereq_node, NULL,
                    BAD_CAST (imethod ? "IMETHODCALL" : "METHODCALL"),
                    NULL);
    xmlNewProp(methodcall_node_aux, BAD_CAST "NAME", BAD_CAST method_name);
    if (imethod)
        fillNamespace(methodcall_node_aux, namespace);
    else
        addLocalInstPath(methodcall_node_aux, namespace, inst);

    if (methodcall_node != NULL)
        *methodcall_node = methodcall_node_aux;

    return doc;
}

/**
 * Add a node storing some value in a CIM-XML request
 *
 * @param parent            Parent node pointer
 * @param name              Child node name
 * @param name_attr         Value of NAME attribute for the child node
 * @param value_node_name   Name of a value node to
 *                          be inserted in the child node
 * @param value_attr_name   Name of an attribute storing the value
 *                          (if value is not stored as a text node)
 * @param value             Value to be stored
 */
void
addNodeWithValue(xmlNodePtr parent,
                 const char *name,
                 const char *name_attr,
                 const char *value_node_name,
                 const char *value_attr_name,
                 const char *value)
{
    xmlNodePtr child_node;
    xmlNodePtr value_node;

    child_node = xmlNewChild(parent, NULL,
                             BAD_CAST name, NULL);
    if (name_attr != NULL)
        xmlNewProp(child_node, BAD_CAST "NAME",
                   BAD_CAST name_attr);

    if (value_attr_name == NULL)
        value_node = xmlNewChild(child_node, NULL,
                                 BAD_CAST value_node_name,
                                 BAD_CAST value);
    else
    {
        value_node = xmlNewChild(child_node, NULL,
                                 BAD_CAST value_node_name,
                                 NULL);
        xmlNewProp(value_node, BAD_CAST value_attr_name,
                   BAD_CAST value);
    }

}

/**
 * Add a node for a parameter having "simple" value
 * (i.e. string, uint32, etc).
 *
 * @param parent_node       Where to add an XML node
 * @param param_name        Name of the parameter
 * @param param_type        Type of the parameter
 * @param param_value       Value of the parameter
 */
void addSimpleParamValue(xmlNodePtr parent_node,
                         const char *param_name,
                         const char *param_type,
                         const char *param_value)
{
   xmlNodePtr paramvalue_node;
   xmlNodePtr value_node;

   paramvalue_node = xmlNewChild(parent_node, NULL,
                                 "PARAMVALUE", NULL);
   xmlNewProp(paramvalue_node, BAD_CAST "NAME",
              BAD_CAST param_name);
   xmlNewProp(paramvalue_node, BAD_CAST "PARAMTYPE",
              BAD_CAST param_type);
   value_node = xmlNewChild(paramvalue_node, NULL, BAD_CAST "VALUE",
                            BAD_CAST param_value);
}

/**
 * Construct CIM-XML request for calling InstallFromURI() method
 *
 * @param namespace     Namespace
 * @param svc           SF_SoftwareInstallationService instance pointer
 * @param target        Target parameter (may be instance of SF_NICCard or
 *                      NULL)
 * @param force         Update firmware even if verion of the image is
 *                      the same or earlier than already installed
 * @param base64_hash   SHA-1 hash of firmware image, encoded in Base64
 * @param url           Firmware image(s) URL
 *
 * @return XML document pointer for the constructed CIM-XML request
 */
xmlDocPtr
xmlReqInstallFromURI(const char *namespace, xmlCimInstance *svc,
                     xmlCimInstance *target, const char *url,
                     int force, const char *base64_hash)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      methodcall_node = NULL;
    xmlNodePtr      paramvalue_node = NULL;
    xmlNodePtr      value_arr_node = NULL;
    xmlNodePtr      value_node = NULL;

    doc = xmlReqPrepareMethodCall(0, namespace, svc, "InstallFromURI",
                                  &methodcall_node);

    addSimpleParamValue(methodcall_node, "URI", "string", url);

    if (target != NULL)
    {
        paramvalue_node = xmlNewChild(methodcall_node, NULL,
                                      "PARAMVALUE", NULL);
        xmlNewProp(paramvalue_node, BAD_CAST "NAME",
                   BAD_CAST "Target");
        xmlNewProp(paramvalue_node, BAD_CAST "PARAMTYPE",
                   BAD_CAST "reference");
   
        value_node = xmlNewChild(paramvalue_node, NULL,
                                 BAD_CAST "VALUE.REFERENCE",
                                 NULL);
        addLocalInstPath(value_node, namespace, target);
    }

    if (force || base64_hash != NULL)
    {
        paramvalue_node = xmlNewChild(methodcall_node, NULL,
                                      "PARAMVALUE", NULL);
        xmlNewProp(paramvalue_node, BAD_CAST "NAME",
                   BAD_CAST "InstallOptions");
        xmlNewProp(paramvalue_node, BAD_CAST "PARAMTYPE",
                   BAD_CAST "uint16");
        value_arr_node = xmlNewChild(paramvalue_node, NULL,
                                     BAD_CAST "VALUE.ARRAY",
                                     NULL);
        if (base64_hash != NULL)
            value_node = xmlNewChild(value_arr_node, NULL,
                                     BAD_CAST "VALUE",
                                     BAD_CAST "32768");
        if (force)
            value_node = xmlNewChild(value_arr_node, NULL,
                                     BAD_CAST "VALUE",
                                     BAD_CAST "3");

        paramvalue_node = xmlNewChild(methodcall_node, NULL,
                                      "PARAMVALUE", NULL);
        xmlNewProp(paramvalue_node, BAD_CAST "NAME",
                   BAD_CAST "InstallOptionsValues");
        xmlNewProp(paramvalue_node, BAD_CAST "PARAMTYPE",
                   BAD_CAST "string");
        value_arr_node = xmlNewChild(paramvalue_node, NULL,
                                     BAD_CAST "VALUE.ARRAY",
                                     NULL);
        if (base64_hash != NULL)
            value_node = xmlNewChild(value_arr_node, NULL,
                                     BAD_CAST "VALUE",
                                     BAD_CAST base64_hash);
        if (force)
            value_node = xmlNewChild(value_arr_node, NULL,
                                     BAD_CAST "VALUE",
                                     /*
                                      * For some reason sfcb does not
                                      * digest <VALUE/>
                                      */
                                     BAD_CAST "x");
    }

    return doc;
}

/**
 * Construct CIM-XML request for calling GetRequiredFwImageName() method
 *
 * @param namespace     Namespace
 * @param svc           SF_SoftwareInstallationService instance pointer
 * @param target        Target parameter (should be instance of SF_NICCard)
 *
 * @return XML document pointer for the constructed CIM-XML request
 */
xmlDocPtr
xmlReqGetRequiredFwImageName(const char *namespace,
                             xmlCimInstance *svc,
                             xmlCimInstance *target)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      methodcall_node = NULL;
    xmlNodePtr      paramvalue_node = NULL;
    xmlNodePtr      value_node = NULL;

    doc = xmlReqPrepareMethodCall(0, namespace, svc,
                                  "GetRequiredFwImageName",
                                  &methodcall_node);

    paramvalue_node = xmlNewChild(methodcall_node, NULL,
                                  "PARAMVALUE", NULL);
    xmlNewProp(paramvalue_node, BAD_CAST "NAME",
               BAD_CAST "Target");
    xmlNewProp(paramvalue_node, BAD_CAST "PARAMTYPE",
               BAD_CAST "reference");

    value_node = xmlNewChild(paramvalue_node, NULL,
                             BAD_CAST "VALUE.REFERENCE",
                             NULL);
    addLocalInstPath(value_node, namespace, target);

    return doc;
}

/**
 * Construct CIM-XML request for calling SendFwImageData() method
 *
 * @param namespace     Namespace
 * @param svc           SF_SoftwareInstallationService instance pointer
 * @param file_name     Name of temporary file in which data
 *                      would be stored
 * @param base64        Data from firmware image encoded in Base64
 *
 * @return XML document pointer for the constructed CIM-XML request
 */
xmlDocPtr
xmlReqSendFwImageData(const char *namespace, xmlCimInstance *svc,
                      const char *file_name, const char *base64)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      methodcall_node = NULL;
    xmlNodePtr      paramvalue_node = NULL;
    xmlNodePtr      value_node = NULL;

    doc = xmlReqPrepareMethodCall(0, namespace, svc,
                                  "SendFwImageData",
                                  &methodcall_node);

    addSimpleParamValue(methodcall_node, "file_name", "string",
                        file_name);
    addSimpleParamValue(methodcall_node, "base64", "string",
                        base64);

    return doc;
}

/**
 * Construct CIM-XML request for calling RemoveFwImage() method
 *
 * @param namespace     Namespace
 * @param svc           SF_SoftwareInstallationService instance pointer
 * @param file_name     Name of temporary file in which data
 *                      would be stored
 *
 * @return XML document pointer for the constructed CIM-XML request
 */
xmlDocPtr
xmlReqRemoveFwImage(const char *namespace, xmlCimInstance *svc,
                    const char *file_name)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      methodcall_node = NULL;
    xmlNodePtr      paramvalue_node = NULL;
    xmlNodePtr      value_node = NULL;

    doc = xmlReqPrepareMethodCall(0, namespace, svc,
                                  "RemoveFwImage",
                                  &methodcall_node);

    addSimpleParamValue(methodcall_node, "file_name", "string",
                        file_name);

    return doc;
}

/**
 * Construct CIM-XML request for
 * EnumerateInstances()/EnumerateInstanceNames() intrinsic method
 *
 * @param namespace     Namespace
 * @param class_name    Class name
 * @param ein           Whether we should enumerate instance names or
 *                      instances
 *
 * @return XML document pointer of the constructed CIM-XML request
 */
xmlDocPtr
xmlReqEnumerateInstances(const char *namespace, const char *class_name,
                         int ein)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      imethodcall_node = NULL;
    xmlNodePtr      iparamvalue_node = NULL;
    xmlNodePtr      classname_node = NULL;
    char           *method_name = NULL;

    if (ein)
        method_name = "EnumerateInstanceNames";
    else
        method_name = "EnumerateInstances";

    doc = xmlReqPrepareMethodCall(1, namespace, NULL, method_name,
                                  &imethodcall_node);

    iparamvalue_node = xmlNewChild(imethodcall_node, NULL,
                                   BAD_CAST "IPARAMVALUE", NULL);
    xmlNewProp(iparamvalue_node, BAD_CAST "NAME", BAD_CAST "ClassName");

    classname_node = xmlNewChild(iparamvalue_node, NULL,
                                 BAD_CAST "CLASSNAME", NULL);
    xmlNewProp(classname_node, BAD_CAST "NAME", BAD_CAST class_name);

    return doc;
}

/**
 * Construct CIM-XML request for Associators()/AssociatorNames()
 * intrinsic method
 *
 * @param namespace     Namespace
 * @param inst          CIM instance for which to find associators
 * @param role          Instance's role in the association
 * @param assoc_class   Association class name
 * @param result_role   Result role in the association
 * @param result_class  Result class name
 * @param an            Whether we should enumerate associator names
 *                      or associators
 *
 * @return XML document pointer of the constructed CIM-XML request
 */
xmlDocPtr
xmlReqAssociators(const char *namespace, xmlCimInstance *inst,
                  const char *role, const char *assoc_class,
                  const char *result_role, const char *result_class,
                  int an)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      imethodcall_node = NULL;
    xmlNodePtr      iparamvalue_node = NULL;
    xmlNodePtr      inst_name_copy_node = NULL;
    char           *method_name = NULL;

    if (an)
        method_name = "AssociatorNames";
    else
        method_name = "Associators";

    doc = xmlReqPrepareMethodCall(1, namespace, NULL, method_name,
                                  &imethodcall_node);

    if (role != NULL)
        addNodeWithValue(imethodcall_node, "IPARAMVALUE",
                         "Role", "VALUE", NULL, role);

    if (assoc_class != NULL)
        addNodeWithValue(imethodcall_node, "IPARAMVALUE",
                         "AssocClass", "CLASSNAME",
                         "NAME", assoc_class);

    if (result_role != NULL)
        addNodeWithValue(imethodcall_node, "IPARAMVALUE",
                         "ResultRole", "VALUE", NULL, result_role);

    if (result_class != NULL)
        addNodeWithValue(imethodcall_node, "IPARAMVALUE",
                         "ResultClass", "CLASSNAME",
                         "NAME", result_class);

    iparamvalue_node = xmlNewChild(imethodcall_node, NULL,
                                   BAD_CAST "IPARAMVALUE", NULL);
    xmlNewProp(iparamvalue_node, BAD_CAST "NAME", BAD_CAST "ObjectName");

    inst_name_copy_node = xmlCopyNode(inst->instance_name, 1);
    xmlAddChild(iparamvalue_node, inst_name_copy_node);

    return doc;
}

/** Where to store data received from CURL */
static char *curl_data = NULL;
/** Received data length */
static int curl_data_len = 0;

/**
 * Data receiving callback for CURL
 *
 * @param data    Received data pointer
 * @param size    Data member size
 * @param count   Number of members
 * @param ptr     Unused
 *
 * @return Number of bytes stored successfully
 */
size_t curl_write(void *data, size_t size, size_t count, void *ptr)
{
    int nbytes = size * count;

    char *p;
    int   new_len;

    new_len = curl_data_len + nbytes;
    p = realloc(curl_data, new_len);
    if (p == NULL)
    {
        ERROR_MSG("%s(): out of memory", __FUNCTION__);
        return 0;
    }

    curl_data = p;
    memcpy(curl_data + curl_data_len, data, nbytes);
    curl_data_len += nbytes;

    return nbytes;
}

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
void free_named_value(named_value *val)
{
    free(val->name);
    free(val->value);
    free(val);
}

/**
 * Free memory allocated for a list of named_value's.
 *
 * @param head      List head
 */
void free_named_value_list(named_value *head)
{
    named_value *p;
    named_value *q;

    p = head;
    while (p != NULL)
    {
        q = p;
        p = p->next;
        free_named_value(q);
    }
}

char *get_named_value(named_value *list, const char *name)
{
    named_value *p;

    for (p = list; p != NULL; p = p->next)
        if (strcmp(p->name, name) == 0)
            return p->value;

    return NULL;
}

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
void
clear_response(response_descr *rsp)
{
    xmlCimInstance *p, *q;
    free(rsp->method_name);
    free(rsp->returned_value);
    free(rsp->err_code);
    free(rsp->err_descr);

    p = rsp->inst_list;
    while (p != NULL)
    {
        q = p;
        p = p->next;
        freeXmlCimInstance(q);
    }

    free_named_value_list(rsp->out_params_list);
    memset(rsp, 0, sizeof(*rsp));
}

/**
 * Parse XML representation of a CIM instance
 *
 * @param inst    [out] CIM instance pointer
 * @param parent        Node storing CIM instance XML representation
 *
 * @return 0 on success, -1 on failure
 */
int
xmlParseInstance(xmlCimInstance *inst, xmlNodePtr parent)
{
    xmlNodePtr child;

    child = parent->children;

    while (child != NULL)
    {
        if (xmlStrcmp(child->name, BAD_CAST "text") == 0)
        {
            child = child->next;
            continue;
        }
        else if (xmlStrcmp(child->name, BAD_CAST "INSTANCEPATH") == 0)
        {
            if (xmlParseInstance(inst, child) < 0)
                return -1;
        }
        else if (xmlStrcmp(child->name, BAD_CAST "NAMESPACEPATH") == 0)
        {
            if (inst->namespace_path != NULL)
            {
                ERROR_MSG("Duplicating <NAMESPACEPATH> node "
                          "encountered");
                return -1;
            }
            inst->namespace_path = xmlCopyNode(child, 1);
        }
        else if (xmlStrcmp(child->name, BAD_CAST "INSTANCENAME") == 0)
        {
            if (inst->instance_name != NULL)
            {
                ERROR_MSG("Duplicating <INSTANCENAME> node "
                          "encountered");
                return -1;
            }
            inst->instance_name = xmlCopyNode(child, 1);
        }
        else if (xmlStrcmp(child->name, BAD_CAST "INSTANCE") == 0)
        {
            if (inst->instance != NULL)
            {
                ERROR_MSG("Duplicating <INSTANCE> node "
                          "encountered");
                return -1;
            }
            inst->instance = xmlCopyNode(child, 1);
        }
        else
        {
            ERROR_MSG("Unexpected node <%s> encountered",
                      (char *)child->name);
            return -1;
        }

        child = child->next;
    }
}

/**
 * Parse CIM-XML response
 *
 * @param data                  Data received from CURL
 * @param data_len              Data length
 * @param response_descr  [out] response_descr structure to be filled
 *
 * @return 0 on success, -1 on failure
 */
int
xmlParseCimResponse(const char *data,
                    int data_len,
                    response_descr *response)
{
    xmlDocPtr       doc;
    xmlNodePtr      child;
    xmlNodePtr      aux;
    xmlCimInstance *inst;
    int             rc = 0;

    memset(response, 0, sizeof(*response));

    doc = xmlReadMemory(data, data_len, NULL,
                        "UTF-8", 0);
    if (doc == NULL)
    {
        ERROR_MSG("Failed to parse CIM server response XML");
        rc = -1;
        goto cleanup;
    }

    child = xmlDocGetRootElement(doc);
    if (xmlStrcmp(child->name, BAD_CAST "CIM") != 0)
    {
        ERROR_MSG("Response have wrong root element %s",
              child->name);
        rc = -1;
        goto cleanup;
    }

    child = child->children;
    SKIP_TEXT(child);
    if (child == NULL || xmlStrcmp(child->name,
                                   BAD_CAST "MESSAGE") != 0)
    {
        ERROR_MSG("Failed to find <MESSAGE> node");
        rc = -1;
        goto cleanup;
    }

    child = child->children;
    SKIP_TEXT(child);
    if (child == NULL || xmlStrcmp(child->name,
                                   BAD_CAST "SIMPLERSP") != 0)
    {
        ERROR_MSG("Failed to find <SIMPLERSP> node");
        rc = -1;
        goto cleanup;
    }

    child = child->children;
    SKIP_TEXT(child);
    if (child == NULL)
    {
        ERROR_MSG("Empty <SIMPLERSP> node encountered");
        rc = -1;
        goto cleanup;
    }

    if (xmlStrcmp(child->name, BAD_CAST "IMETHODRESPONSE") == 0)
        response->imethod_called = 1; 
    else if (xmlStrcmp(child->name, BAD_CAST "METHODRESPONSE") != 0)
    {
        ERROR_MSG("Inappropriate child of <SIMPLERSP> encountered");
        rc = -1;
        goto cleanup;
    }
    response->method_name =
        str_trim_xml_free(xmlGetProp(child, BAD_CAST "NAME"));

    child = child->children;
    SKIP_TEXT(child);
    if (child == NULL)
    {
        ERROR_MSG("Empty method response node encountered");
        rc = -1;
        goto cleanup;
    }

    if (xmlStrcmp(child->name, BAD_CAST "ERROR") == 0)
    {
        response->error_returned = 1;
        response->err_code = 
            str_trim_xml_free(xmlGetProp(child, BAD_CAST "CODE"));
        response->err_descr =
            str_trim_xml_free(xmlGetProp(child, BAD_CAST "DESCRIPTION"));
    }
    else
    {
        if (response->imethod_called)
        {
            if (child == NULL ||
                xmlStrcmp(child->name, BAD_CAST "IRETURNVALUE") != 0)
            {
                ERROR_MSG("Failed to find IRETURNVALUE node.");
                rc = -1;
                goto cleanup;
            }

            child = child->children;
            SKIP_TEXT(child);

            while (child != NULL)
            {
                inst = calloc(1, sizeof(*inst));
                if (xmlStrcmp(child->name, BAD_CAST "INSTANCENAME") == 0) 
                    inst->instance_name = xmlCopyNode(child, 1);
                else if (xmlStrcmp(child->name,
                                   BAD_CAST "VALUE.NAMEDINSTANCE") == 0 ||
                         xmlStrcmp(child->name,
                                   BAD_CAST "VALUE.OBJECTWITHPATH") == 0 ||
                         xmlStrcmp(child->name,
                                   BAD_CAST "OBJECTPATH") == 0)
                {
                    if (xmlParseInstance(inst, child) < 0)
                    {
                        freeXmlCimInstance(inst);
                        rc = -1;
                        goto cleanup;
                    }
                }
                else
                {
                    ERROR_MSG("Unexpected node %s encountered",
                              (char *)child->name);
                    freeXmlCimInstance(inst);
                    rc = -1;
                    goto cleanup;
                }
                if (inst->instance_name == NULL)
                {
                    ERROR_MSG("<INSTANCENAME> was not found");
                    freeXmlCimInstance(inst);
                    rc = -1;
                    goto cleanup;
                }
                inst->class_name = xmlGetProp(inst->instance_name,
                                              BAD_CAST "CLASSNAME");
                if (inst->class_name == NULL)
                {
                    ERROR_MSG("Failed to determine class name");
                    freeXmlCimInstance(inst);
                    rc = -1;
                    goto cleanup;
                }
                inst->next = response->inst_list;
                if (response->inst_list != NULL)
                    response->inst_list->prev = inst;
                response->inst_list = inst;
                response->inst_count++;

                child = child->next;
                SKIP_TEXT(child);
            }
        }
        else
        {
            for ( ; child != NULL; child = child->next)
            {
                if (xmlStrcmp(child->name,
                              BAD_CAST "RETURNVALUE") == 0 ||
                    xmlStrcmp(child->name,
                              BAD_CAST "PARAMVALUE") == 0)
                {
                    aux = child->children;
                    SKIP_TEXT(aux);
                    if (xmlStrcmp(aux->name, BAD_CAST "VALUE") != 0)
                    {
                        ERROR_MSG("Tag '%s' was encountered unexpectedly",
                                  (char *)aux->name);
                        rc = -1;
                        goto cleanup;
                    }

                    if (xmlStrcmp(child->name,
                                  BAD_CAST "RETURNVALUE") == 0)
                        response->returned_value =
                            str_trim_xml_free(xmlNodeGetContent(aux));
                    else
                    {
                        named_value *val = calloc(1, sizeof(named_value));

                        val->name =
                            str_trim_xml_free(
                                     xmlGetProp(child, BAD_CAST "NAME"));
                        val->value =
                            str_trim_xml_free(xmlNodeGetContent(aux));

                        val->prev = NULL;
                        val->next = response->out_params_list;
                        if (response->out_params_list != NULL)
                            response->out_params_list->prev = val;
                        response->out_params_list = val;
                        response->out_params_count++;
                    }
                }
            }
        }
    }

cleanup:
    xmlFreeDoc(doc);
    return rc;
}

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
int
processXmlRequest(CURL *curl, xmlDocPtr doc, const char *method_name,
                  response_descr *response)
{
#define MAX_HEADER_LEN 1024

    xmlChar        *data;
    int             data_len;
    CURLcode        res;
    char            method_header[MAX_HEADER_LEN];

    struct curl_slist *headers = NULL;

    snprintf(method_header, MAX_HEADER_LEN, "CIMMethod: %s", method_name);
    headers = curl_slist_append(headers, "Content-Type: application/xml; "
                                "charset=\"utf-8\"");
    headers = curl_slist_append(headers, "CIMOperation: MethodCall");
    headers = curl_slist_append(headers, method_header);
    headers = curl_slist_append(headers, "CIMObject: root/solarflare");
    headers = curl_slist_append(headers, "Expect:");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    xmlDocDumpFormatMemoryEnc(doc, &data, &data_len, "UTF-8", 0);
    xmlFreeDoc(doc);

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (char *)data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_len);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

    free(curl_data);
    curl_data = NULL;
    curl_data_len = 0;

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
          ERROR_MSG("curl_easy_perform() failed: %s",
                    curl_easy_strerror(res));
          xmlFree(data);
          curl_slist_free_all(headers);
          return -1;
    }
    xmlFree(data);
    curl_slist_free_all(headers);

    if (xmlParseCimResponse(curl_data, curl_data_len, response) < 0)
    {
        ERROR_MSG("Failed to parse CIM response");
        printf("%s\n", curl_data);
        return -1;
    }

    return 0;
}

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
int
enumerate_instances(CURL *curl, const char *namespace,
                    const char *class_name, int ein,
                    response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqEnumerateInstances(namespace, class_name, ein);
    
    return processXmlRequest(curl, doc, (ein ? "EnumerateInstanceNames" : 
                                               "EnumerateInstances"),
                             response);
}

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
int
associators(CURL *curl, const char *namespace, xmlCimInstance *inst,
            const char *role, const char *assoc_class,
            const char *result_role, const char *result_class,
            int an, response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqAssociators(namespace, inst, role, assoc_class,
                            result_role, result_class, an);
    
    return processXmlRequest(curl, doc, (an ? "AssociatorNames" : 
                                              "Associators"),
                             response);
}

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

/**
 * Call InstallFromURI() extrinsic method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param target            Target parameter (may be instance of SF_NICCard
 *                          or NULL)
 * @param url               Firmware image(s) URL
 * @param force             Update firmware even if verion of the image is
 *                          the same or earlier than already installed
 * @param base64_hash       SHA-1 hash of firmware image, encoded in Base64
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
int
call_install_from_uri(CURL *curl, const char *namespace,
                      xmlCimInstance *svc, xmlCimInstance *target,
                      const char *url,
                      int force,
                      const char *base64_hash,
                      response_descr *response)
{
    xmlDocPtr doc;

    char       *svc_name = NULL;
    const char *firmware_type = NULL;
    int         rc = 0;

    xmlCimInstance *port_inst = NULL;

    response_descr assoc_cntr_rsp;
    response_descr assoc_ports_rsp;

    memset(&assoc_cntr_rsp, 0, sizeof(assoc_cntr_rsp));
    memset(&assoc_ports_rsp, 0, sizeof(assoc_ports_rsp));

    if (xmlCimInstGetPropTrim(svc, "Name",
                              &svc_name, NULL) < 0 ||
        svc_name == NULL)
    {
        ERROR_MSG("Failed to get Name attribute "
                  "of SF_SoftwareInstallationService instance");
        return -1;
    }

    if (strcmp(svc_name, "BootROM") == 0)
        firmware_type = "BootROM";
    else
        firmware_type = "Controller";

    printf("Updating %s firmware for ", firmware_type);
    if (target == NULL)
        printf("all interfaces");
    else
    {
        CHECK_RESPONSE(
            rc,
            associators(curl, cim_namespace, target,
                        "Antecedent", "SF_CardRealizesController",
                        NULL, "SF_PortController",
                        0, &assoc_cntr_rsp),
            assoc_cntr_rsp,
            "Failed to find associated SF_PortController instance");
        if (rc < 0)
            goto cleanup;
        if (assoc_cntr_rsp.inst_count != 1)
        {
                ERROR_MSG("%d instead of just one associated "
                          "SF_PortController instances were found",
                          assoc_cntr_rsp.inst_count);
                rc = -1;
                goto cleanup;
        }

        CHECK_RESPONSE(
            rc,
            associators(curl, cim_namespace, assoc_cntr_rsp.inst_list,
                        "Antecedent", "SF_ControlledBy",
                        NULL, "SF_EthernetPort",
                        0, &assoc_ports_rsp),
            assoc_ports_rsp,
            "Failed to find associated SF_PortController instances");
        if (rc < 0)
            goto cleanup;

        for (port_inst = assoc_ports_rsp.inst_list;
             port_inst != NULL; port_inst = port_inst->next)
        {
            char *device_id = NULL;

            if (xmlCimInstGetPropTrim(port_inst, "DeviceID",
                                      &device_id, NULL) < 0 ||
                device_id == NULL)
            {
                ERROR_MSG("Failed to get DeviceID attribute "
                          "of SF_EthernetPort instance");
                rc = -1;
                goto cleanup;
            }

            if (port_inst->prev == NULL)
                printf("%s", device_id);
            else
                printf(", %s", device_id);

            free(device_id);
        }
    }
    printf("...\n");

    doc = xmlReqInstallFromURI(namespace, svc, target, url,
                               force, base64_hash);
    
    rc = processXmlRequest(curl, doc, "InstallFromURI",
                           response);

cleanup:

    clear_response(&assoc_cntr_rsp);
    clear_response(&assoc_ports_rsp);
    free(svc_name);
    return rc;
}

/**
 * Call GetRequiredFwImageName() extrinsic method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param target            Target parameter (may be instance of SF_NICCard
 *                          or NULL)
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
int
call_get_required_fw_image_name(CURL *curl, const char *namespace,
                                xmlCimInstance *svc, xmlCimInstance *target,
                                response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqGetRequiredFwImageName(namespace, svc, target);
    
    return processXmlRequest(curl, doc, "GetRequiredFwImageName",
                             response);
}

/**
 * Call StartFwImageSend() extrinsic method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
int
call_start_fw_image_send(CURL *curl, const char *namespace,
                         xmlCimInstance *svc,
                         response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqPrepareMethodCall(0, namespace, svc,
                                  "StartFwImageSend",
                                  NULL);
    
    return processXmlRequest(curl, doc, "StartFwImageSend",
                             response);
}

/**
 * Call SendFwImageData() extrinsic method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param file_name         Temporary file name where to store image data
 * @param base64            Firmware image data encoded in Base64
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
int
call_send_fw_image_data(CURL *curl, const char *namespace,
                        xmlCimInstance *svc,
                        const char *file_name,
                        const char *base64,
                        response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqSendFwImageData(namespace, svc, file_name, base64);
    
    return processXmlRequest(curl, doc, "SendFwImageData",
                             response);
}

/**
 * Call RemoveFwImage() extrinsic method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param file_name         Temporary file name where image was stored
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
int
call_remove_fw_image(CURL *curl, const char *namespace,
                     xmlCimInstance *svc,
                     const char *file_name,
                     response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqRemoveFwImage(namespace, svc, file_name);
    
    return processXmlRequest(curl, doc, "RemoveFwImage",
                             response);
}

/**
 * Install firmware from an image stored locally on a host
 * where this program is run.
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param target            Target parameter (may be instance of SF_NICCard
 *                          or NULL)
 * @param path              Firmware image(s) local path
 * @param force             Update firmware even if verion of the image is
 *                          the same or earlier than already installed
 * @param unused1           Unused parameter (for compatibility with
 *                          call_install_from_uri() function signature)
 * @param unused2           Another unused parameter for the same reason
 *
 * @return 0 on success, -1 on failure
 */
int
install_from_local_path(CURL *curl, const char *namespace,
                        xmlCimInstance *svc, xmlCimInstance *target,
                        const char *path,
                        int force,
                        const char *unused1,
                        response_descr *unused2)
{
#define MAX_PATH_LEN 1024
#define CHUNK_LEN 100000

    int             rc = 0;
    int             rc_rsp = 0;
    xmlCimInstance  aux_inst;
    xmlCimInstance *targets_list = NULL;
    xmlCimInstance *p;
    response_descr  nics_rsp;
    response_descr  call_rsp;
    char           *svc_name = NULL;

    int             not_full_path = 0;
    char            full_path[MAX_PATH_LEN];
    char            full_path_prev[MAX_PATH_LEN];
    char           *tmp_file_name = NULL;
    char            url[MAX_FILE_PATH];

    EVP_MD_CTX      mdctx;
    char            md_hash[EVP_MAX_MD_SIZE];
    int             md_len;

    size_t  enc_size;
    char   *encoded = NULL;

    char            img_chunk[CHUNK_LEN];
    size_t          read_len;

    memset(&nics_rsp, 0, sizeof(nics_rsp));
    memset(&call_rsp, 0, sizeof(call_rsp));

    full_path[0] = '\0';
    full_path_prev[0] = '\0';

    if (strstr(path, ".dat") != path + (strlen(path) - 4))
        not_full_path = 1;

    if (target != NULL)
    {
        memcpy(&aux_inst, target, sizeof(aux_inst));
        aux_inst.prev = NULL;
        aux_inst.next = NULL;
        targets_list = &aux_inst;
    }
    else if (!not_full_path)
            targets_list = NULL;
    else
    {
        CHECK_RESPONSE(
            rc,
            enumerate_instances(curl, namespace,
                                "SF_NICCard", 0,
                                &nics_rsp),
            nics_rsp,
            "Failed to enumerate SF_NICCard instances");
        if (rc < 0)
            goto cleanup;

        targets_list = nics_rsp.inst_list;
        if (targets_list == NULL)
        {
            ERROR_MSG_PLAIN("No SF_NICCard instances were found");
            goto cleanup;
        }
    }

    if (xmlCimInstGetPropTrim(svc, "Name",
                              &svc_name, NULL) < 0)
    {
        ERROR_MSG("Failed to get Name attribute "
                  "of SF_SoftwareInstallationService instance");
        rc = -1;
        goto cleanup;
    }

    p = targets_list;

    do {
        strncpy(full_path_prev, full_path, MAX_PATH_LEN);
        if (not_full_path)
        {
            char *tag;
            char *name;
            char *subdir = NULL;

            if (xmlCimInstGetPropTrim(p, "Tag",
                                      &tag, NULL) < 0)
            {
                ERROR_MSG("Failed to get Tag attribute "
                          "of SF_NICCard instance");
                rc = -1;
                goto next_target;
            }


            /* FIXME: should we just skip NIC for which
             * it is impossible to get required firmware
             * name? */
            assert(p != NULL);
            CHECK_RESPONSE_EXT(
                rc_rsp,
                call_get_required_fw_image_name(curl, namespace,
                                                svc, p,
                                                &call_rsp),
                call_rsp,
                "Failed to get required firmware image name for "
                "SF_NICCard.Tag='%s'", tag);

            if (rc_rsp < 0)
            {
                free(tag);
                rc = -1;
                goto next_target;
            }

            name = get_named_value(call_rsp.out_params_list,
                                   "name");
            if (name == NULL || strlen(name) == 0)
            {
                ERROR_MSG_PLAIN("Failed to get required firmware "
                                "image name for SF_NICCard.Tag='%s'",
                                tag);
                free(tag);
                rc = -1;
                goto next_target;
            }

            if (strcmp(svc_name, "BootROM") == 0)
                subdir = "bootrom";
            else
                subdir = "mcfw";
#ifdef _WIN32
            snprintf(full_path, MAX_PATH_LEN, "%s\\image\\%s\\%s",
                     path, subdir, name);
#else
            snprintf(full_path, MAX_PATH_LEN, "%s/image/%s/%s",
                     path, subdir, name);
#endif
            free(tag);
            clear_response(&call_rsp);
        }
        else
            snprintf(full_path, MAX_PATH_LEN, "%s", path);

        if (strcmp(full_path, full_path_prev) != 0)
        {
            FILE *f;

            if (tmp_file_name != NULL)
            {
                CHECK_RESPONSE_EXT(
                    rc_rsp,
                    call_remove_fw_image(curl, namespace,
                                         svc, tmp_file_name,
                                         &call_rsp),
                    call_rsp,
                    "Failed to remove temporary file '%s'",
                    tmp_file_name);

                if (rc_rsp < 0)
                {
                    free(tmp_file_name);
                    rc = -1;
                    goto cleanup;
                }

                free(tmp_file_name);
                tmp_file_name = NULL;
                clear_response(&call_rsp);
            }

            CHECK_RESPONSE_EXT(
                rc_rsp,
                call_start_fw_image_send(curl, namespace,
                                         svc, &call_rsp),
                call_rsp,
                "Failed to create temporary file for "
                "firmware image transfer");
            if (rc_rsp < 0)
            {
                rc = -1;
                goto cleanup;
            }

            tmp_file_name = get_named_value(call_rsp.out_params_list,
                                            "file_name");
            if (tmp_file_name == NULL || strlen(tmp_file_name) == 0)
            {
                ERROR_MSG_PLAIN("Failed to get temporary file name "
                                "for firmware transfer");
                rc = -1;
                goto cleanup;
            }

            tmp_file_name = strdup(tmp_file_name);
            clear_response(&call_rsp);

            f = fopen(full_path, "rb");
            if (f == NULL)
            {
                ERROR_MSG_PLAIN("Failed to open file '%s' for reading",
                                full_path);
                rc = -1;
                goto cleanup;
            }

            EVP_MD_CTX_init(&mdctx);
            if (!EVP_DigestInit_ex(&mdctx, EVP_sha1(), NULL))
            {
                ERROR_MSG("EVP_DigestInit_ex() failed");
                fclose(f);
                rc = -1;
                goto cleanup;
            }
            while (!feof(f))
            {
                int ferr;

                read_len = fread(img_chunk, 1, CHUNK_LEN, f); 
                ferr = ferror(f);
                if (ferr != 0)
                {
                    ERROR_MSG("Read error occured when reading from '%s'",
                              full_path);
                    rc = -1;
                    fclose(f);
                    goto cleanup;
                }

                if (read_len > 0)
                {
                    enc_size = base64_enc_size(read_len);

                    if (!EVP_DigestUpdate(&mdctx, img_chunk, read_len))
                    {
                        ERROR_MSG("EVP_DigestUpdate() failed");
                        rc = -1;
                        fclose(f);
                        goto cleanup;
                    }

                    encoded = calloc(enc_size, 1);
                    if (encoded == NULL)
                    {
                        ERROR_MSG("Failed to allocate memory for Base64 "
                                  "string");
                        rc = -1;
                        fclose(f);
                        goto cleanup;
                    }

                    base64_encode(encoded, img_chunk, read_len);
                    
                    CHECK_RESPONSE_EXT(
                        rc_rsp,
                        call_send_fw_image_data(curl, namespace,
                                                svc,
                                                tmp_file_name, encoded,
                                                &call_rsp),
                        call_rsp,
                        "Failed to send firmware image data");
                    if (rc_rsp < 0)
                    {
                        rc = -1;
                        free(encoded);
                        fclose(f);
                        goto cleanup;
                    }
                    clear_response(&call_rsp);
                    free(encoded);
                }
            }
            fclose(f);
            if (!EVP_DigestFinal_ex(&mdctx, md_hash, &md_len))
            {
                ERROR_MSG("Failed to compute SHA1 hash of "
                          "firmware image");
                rc = -1;
                goto cleanup;
            }
            if (!EVP_MD_CTX_cleanup(&mdctx))
            {
                ERROR_MSG("EVP_MD_CTX_cleanup() failed");
                rc = -1;
                goto cleanup;
            }
        }
 
        enc_size = base64_enc_size(md_len);
        encoded = calloc(enc_size, 1);
        if (encoded == NULL)
        {
            ERROR_MSG("Failed to allocate memory for Base64 "
                      "string");
            rc = -1;
            goto cleanup;
        }
        base64_encode(encoded, md_hash, md_len);

        snprintf(url, MAX_FILE_PATH, "file://%s",
                 tmp_file_name);

        CHECK_RESPONSE_EXT(
            rc_rsp,
            call_install_from_uri(curl, namespace,
                                  svc, p,
                                  url, force, encoded,
                                  &call_rsp),
            call_rsp,
            "InstallFromURI() failed");
        if (rc_rsp >= 0 && strcmp(call_rsp.returned_value, "0") != 0)
        {
            ERROR_MSG_PLAIN("InstallFromURI() returned %s when "
                            "trying to update firmware",
                            call_rsp.returned_value);
            rc = -1;
            free(encoded);
            goto cleanup;
        }
        else if (rc_rsp < 0)
        {
            rc = -1;
            free(encoded);
            goto cleanup;
        }
        free(encoded);

next_target:
        clear_response(&call_rsp);
        if (p != NULL)
            p = p->next;
    } while (p != NULL);

cleanup:

    if (tmp_file_name != NULL)
    {
        CHECK_RESPONSE_EXT(
            rc_rsp,
            call_remove_fw_image(curl, namespace,
                                 svc, tmp_file_name,
                                 &call_rsp),
            call_rsp,
            "Failed to remove temporary file '%s'",
            tmp_file_name);

        if (rc_rsp < 0)
            rc = -1;

        free(tmp_file_name);
        tmp_file_name = NULL;
    }

    free(svc_name);
    clear_response(&nics_rsp);
    clear_response(&call_rsp);
    return rc;
}

/**
 * Firmware install function pointer type
 */
typedef int (*firmware_install_f)(
                    CURL *, const char *,
                    xmlCimInstance *, xmlCimInstance *,
                    const char *, int, const char *,
                    response_descr *);

/**
 * Update firmware
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Provider namespace
 * @param nic_inst          SF_NICCard instance as target parameter
 *                          (or NULL)
 * @param controller        Whether to update controller firmware or not
 * @param bootrom           Whether to update BootROM firmware or not
 * @param firmware_source   Firmware image(s) URL
 * @param url_specified     Firmware URL was specified (not local path)
 * @param force             --force option was specified
 *
 * @return 0 on success, -1 on failure
 */
int
update_firmware(CURL *curl, const char *namespace,
                xmlCimInstance *nic_inst,
                int controller, int bootrom,
                const char *firmware_source,
                int url_specified,
                int force)
{
    response_descr  svcs_rsp;
    response_descr  log_rsp;
    response_descr  rec_rsp;
    response_descr  call_rsp;
    int             rc = 0;
    int             rc_rsp = 0;
    int             log_available = 0;
    int             print_err_recs = 0;
    char           *recent_rec_id = NULL;
    char           *svc_name = NULL;

    firmware_install_f  func_install;

    xmlCimInstance *svc_inst = NULL;
    xmlCimInstance *svc_mcfw_inst = NULL;
    xmlCimInstance *svc_bootrom_inst = NULL;
    xmlCimInstance *log_inst = NULL;
    xmlCimInstance *rec_inst = NULL;

    memset(&svcs_rsp, 0, sizeof(svcs_rsp));
    memset(&log_rsp, 0, sizeof(rec_rsp));
    memset(&rec_rsp, 0, sizeof(rec_rsp));
    memset(&call_rsp, 0, sizeof(call_rsp));

    if (url_specified)
        func_install = call_install_from_uri;
    else
        func_install = install_from_local_path;

    printf("\nUpdating firmware...\n");

    CHECK_RESPONSE(
        rc,
        enumerate_instances(curl, namespace,
                            "SF_SoftwareInstallationService", 0,
                            &svcs_rsp),
        svcs_rsp,
        "Failed to enumerate SF_SoftwareInstallationService instances");
    if (rc < 0)
        goto cleanup;

    for (svc_inst = svcs_rsp.inst_list; svc_inst != NULL;
         svc_inst = svc_inst->next)
    {
        char *name;

        if (xmlCimInstGetPropTrim(svc_inst, "Name", &name, NULL) < 0)
        {
            ERROR_MSG("Failed to get Name property of "
                      "SF_SoftwareInstallationService");
            rc = -1;
            goto cleanup;
        }

        if (strcmp(name, "Firmware") == 0)
            svc_mcfw_inst = svc_inst;
        else if (strcmp(name, "BootROM") == 0)
            svc_bootrom_inst = svc_inst;
    }

    if ((svc_mcfw_inst == NULL && controller) ||
        (svc_bootrom_inst == NULL && bootrom))
    {
        ERROR_MSG("Failed to find appropriate "
                  "SF_SoftwareInstallationService instance");
        rc = -1;
    }
    else
    {
        CHECK_RESPONSE(
            rc,
            enumerate_instances(curl, namespace,
                                "SF_ProviderLog", 0,
                                &log_rsp),
            log_rsp,
            "Failed to enumerate SF_ProviderLog instances");
        if (rc >= 0)
        {
            for (log_inst = log_rsp.inst_list; log_inst != NULL;
                 log_inst = log_inst->next)
            {
                char *log_descr = NULL;
            
                if (xmlCimInstGetPropTrim(log_inst, "Description",
                                          &log_descr, NULL) >= 0 &&
                    log_descr != NULL &&
                    strcmp(log_descr, "Error log") == 0)
                    break;
            }

            if (log_inst == NULL)
            {
                ERROR_MSG_PLAIN("Error log was not found");
                rc = -1;
            }
            else
            {
                CHECK_RESPONSE(
                    rc,
                    associators(curl, namespace, log_inst, "Log",
                                "SF_LogManagesRecord", NULL, NULL, 0,
                                &rec_rsp),
                    rec_rsp,
                    "Failed to find associated SF_LogEntry instances");
                if (rc >= 0)
                {
                    sort_inst_list(&rec_rsp.inst_list, compare_by_prop,
                                   "RecordID");
                    for (rec_inst = rec_rsp.inst_list;
                         rec_inst != NULL && rec_inst->next != NULL;
                         rec_inst = rec_inst->next);

                    if (rec_inst != NULL &&
                        xmlCimInstGetPropTrim(rec_inst, "RecordID",
                                              &recent_rec_id, NULL) >= 0 &&
                        recent_rec_id != NULL)
                        log_available = 1;
                    else if (rec_inst == NULL)
                        log_available = 1;
                }
            }

            clear_response(&rec_rsp);
        }

        if (controller)
        {
            CHECK_RESPONSE(
                    rc_rsp,
                    func_install(curl, namespace,
                                 svc_mcfw_inst,
                                 nic_inst, firmware_source,
                                 force, NULL, &call_rsp),
                    call_rsp,
                    "Attempt to update "
                    "Controller firmware failed");
            if (rc_rsp >= 0 && func_install != install_from_local_path &&
                strcmp(call_rsp.returned_value, "0") != 0)
            {
                ERROR_MSG_PLAIN("InstallFromURI() returned %s when "
                                "trying to update Controller firmware",
                                call_rsp.returned_value);
                rc = -1;
                print_err_recs = 1;
            }
            else if (rc_rsp < 0)
            {
                rc = -1;
                print_err_recs = 1;
            }
            clear_response(&call_rsp);
        }

        if (bootrom)
        {
            CHECK_RESPONSE(
                    rc_rsp,
                    func_install(curl, namespace,
                                 svc_bootrom_inst,
                                 nic_inst, firmware_source,
                                 force, NULL, &call_rsp),
                    call_rsp,
                    "Attempt to update "
                    "BootROM firmware failed");
            if (rc_rsp >= 0 && func_install != install_from_local_path &&
                strcmp(call_rsp.returned_value, "0") != 0)
            {
                ERROR_MSG_PLAIN("InstallFromURI() returned %s when "
                                "trying to update BootROM firmware",
                                call_rsp.returned_value);
                rc = -1;
                print_err_recs = 1;
            }
            else if (rc_rsp < 0)
            {
                rc = -1;
                print_err_recs = 1;
            }
            clear_response(&call_rsp);
        }

        if (print_err_recs && log_available)
        {
            int saved_rc = 0;

            ERROR_MSG_PLAIN("\nProvider log"); 
            CHECK_RESPONSE(
                saved_rc,
                associators(curl, namespace, log_inst, "Log",
                            "SF_LogManagesRecord", NULL, NULL, 0,
                            &rec_rsp),
                rec_rsp,
                "Failed to find associated SF_LogEntry instances");
            if (saved_rc >= 0)
            {
                sort_inst_list(&rec_rsp.inst_list, compare_by_prop,
                               "RecordID");
                for (rec_inst = rec_rsp.inst_list;
                     rec_inst != NULL;
                     rec_inst = rec_inst->next)
                {
                    char *rec_id = NULL;
                    char *rec_data = NULL;

                    if (xmlCimInstGetPropTrim(rec_inst, "RecordID",
                                              &rec_id, NULL) < 0 ||
                        rec_id == NULL)
                    {
                        ERROR_MSG("Failed to obtain RecordID value");
                        rc = -1;
                        goto cleanup;
                    }

                    if (recent_rec_id == NULL ||
                        strcmp(rec_id, recent_rec_id) > 0)
                    {
                        if (xmlCimInstGetPropTrim(rec_inst, "RecordData",
                                                  &rec_data, NULL) < 0 ||
                            rec_id == NULL)
                        {
                            ERROR_MSG("Failed to obtain "
                                      "RecordID value");
                            rc = -1;
                            free(rec_id);
                            goto cleanup;
                        }
                        ERROR_MSG_PLAIN(rec_data);
                        free(rec_data);
                    }
                    free(rec_id);
                }

                clear_response(&rec_rsp);
                clear_response(&log_rsp);
            }
            else
            {
                ERROR_MSG("Failed to obtain provider log");
                rc = -1;
            }
            ERROR_MSG_PLAIN(""); 
        }
    }

cleanup:

    if (rc >= 0)
        printf("Firmware was successfully updated!\n");
    clear_response(&svcs_rsp);
    clear_response(&call_rsp);
    clear_response(&log_rsp);
    clear_response(&rec_rsp);
    free(recent_rec_id);
    return rc;
}

/**
 * Program entry function
 *
 * @param argc    Command line arguments' count
 * @param argv    Command line arguments
 * 
 * @return 0 on success, non-zero on failure
 */
int
main(int argc, const char *argv[])
{
    char  passwd_buf[MAX_PASSWD_LEN];
    char *s = NULL;
    int   rc = 0;
    int   if_found = 0;

    xmlCimInstance *port_inst = NULL;
    xmlCimInstance *controller_inst = NULL;
    xmlCimInstance *sw_inst = NULL;
    xmlCimInstance *nic_inst = NULL;

    CURL        *curl = NULL;
    CURLcode     res;

    char    address[MAX_ADDR_LEN];
    int     add_proto = 1;
    int     add_port = 1;
    int     i;

    cim_namespace = strdup("solarflare/cimv2");

    response_descr ei_ports_rsp;
    response_descr assoc_cntr_rsp;
    response_descr assoc_sw_rsp;
    response_descr assoc_nic_rsp;

    memset(&ei_ports_rsp, 0, sizeof(ei_ports_rsp));
    memset(&assoc_cntr_rsp, 0, sizeof(assoc_cntr_rsp));
    memset(&assoc_sw_rsp, 0, sizeof(assoc_sw_rsp));
    memset(&assoc_nic_rsp, 0, sizeof(assoc_nic_rsp));

    if (parseCmdLine(argc, argv) < 0)
        exit(2);

    if (cim_address == NULL)
    {
        ERROR_MSG_PLAIN("Please, specify an address of CIM Server");
        rc = -1;
        goto cleanup;
    }

    for (i = 0; i < strlen(cim_address); i++)
    {
        if (strncmp(cim_address + i, "://", 3) == 0)
        {
            add_proto = 0;
            break;
        }

        if (!((cim_address[i] >= 'A' && cim_address[i] <= 'Z') ||
            (cim_address[i] >= 'a' && cim_address[i] <= 'z') ||
            (cim_address[i] >= '0' && cim_address[i] <= '9')))
            break;
    }

    for (i = strlen(cim_address) - 1; i >= 0; i--)
    {
        if (cim_address[i] == ':')
        {
            add_port = 0;
            break;
        }

        if (!(cim_address[i] >= '0' && cim_address[i] <= '9'))
            break;
    }

    snprintf(
        address, MAX_ADDR_LEN, "%s%s%s%s",
        add_proto ? (use_https ? "https://" : "http://") : "",
        cim_address,
        add_port ? ":" : "",
        add_port ? (use_https ? DEF_HTTPS_PORT : DEF_HTTP_PORT) : "");

    if (!update_controller && !update_bootrom)
    {
        update_controller = 1;
        update_bootrom = 1;
    }
    setbuf(stderr, NULL);
    setbuf(stdout, NULL);
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, address);
        curl_easy_setopt(curl, CURLOPT_USERNAME, user);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        CHECK_RESPONSE(
            rc,
            enumerate_instances(curl, cim_namespace,
                                "SF_EthernetPort", 0,
                                &ei_ports_rsp),
            ei_ports_rsp,
            "Failed to enumerate SF_EthernetPort instances");
        if (rc < 0)
            goto cleanup;

        sort_inst_list(&ei_ports_rsp.inst_list, compare_by_prop,
                       "DeviceID");

        for (port_inst = ei_ports_rsp.inst_list; port_inst != NULL;
             port_inst = port_inst->next)
        {
            char *dev_id;
            char *perm_addr;
            char *nic_model;

            if (xmlCimInstGetPropTrim(port_inst, "DeviceID",
                                      &dev_id, NULL) < 0)
            {
                ERROR_MSG("Failed to get DeviceID property value");
                rc = -1;
                goto cleanup;
            }

            if (interface_name != NULL &&
                strcmp(interface_name, dev_id) != 0)
            {
                free(dev_id);
                continue;
            }
            if_found = 1;

            if (xmlCimInstGetPropTrim(port_inst, "PermanentAddress",
                                      &perm_addr, NULL) < 0)
            {
                ERROR_MSG("Failed to get PermanentAddress property value");
                rc = -1;
                free(dev_id);
                goto cleanup;
            }

            printf("%s - MAC: %s\n", (char *)dev_id, (char *)perm_addr);
            free(perm_addr);
            free(dev_id);

            CHECK_RESPONSE(
                rc,
                associators(curl, cim_namespace, port_inst, "Dependent",
                            "SF_ControlledBy", NULL, NULL, 1,
                            &assoc_cntr_rsp),
                assoc_cntr_rsp,
                "Failed to find associated SF_PortController instance");
            if (rc < 0)
                goto cleanup;
            if (assoc_cntr_rsp.inst_count != 1)
            {
                ERROR_MSG("%d instead of just one associated "
                          "SF_PortController instances were found",
                          assoc_cntr_rsp.inst_count);
                rc = -1;
                goto cleanup;
            }
            controller_inst = assoc_cntr_rsp.inst_list;

            CHECK_RESPONSE(
                rc,
                associators(curl, cim_namespace, controller_inst, "Dependent",
                            "SF_ControllerSoftwareIdentity", NULL, NULL, 0,
                            &assoc_sw_rsp),
                assoc_sw_rsp,
                "Failed to find associated SF_SoftwareIdentity instances");
            if (rc < 0)
                goto cleanup;

            CHECK_RESPONSE(
                rc,
                associators(curl, cim_namespace, controller_inst, "Dependent",
                            "SF_CardRealizesController", NULL, "SF_NICCard",
                            0, &assoc_nic_rsp),
                assoc_nic_rsp,
                "Failed to find associated SF_NICCard instance");
            if (rc < 0)
                goto cleanup;
            if (assoc_nic_rsp.inst_count != 1)
            {
                    ERROR_MSG("%d instead of just one associated "
                              "SF_PortController instances were found",
                              assoc_nic_rsp.inst_count);
                    rc = -1;
                    goto cleanup;
            }

            nic_inst = assoc_nic_rsp.inst_list;

            if (xmlCimInstGetPropTrim(nic_inst, "Model",
                                      &nic_model, NULL) < 0 ||
                nic_model == NULL)
            {
                ERROR_MSG("Failed to get Model property value "
                          "for SF_NICCard instance");
                rc = -1;
                goto cleanup;
            }

            printf("NIC model: %s\n", nic_model);
            free(nic_model);

            if (!if_found && interface_name != NULL)
            {
                clear_response(&assoc_cntr_rsp);
                controller_inst = NULL;
                clear_response(&assoc_nic_rsp);
                nic_inst = NULL;
            }

            for (sw_inst = assoc_sw_rsp.inst_list; sw_inst != NULL;
                 sw_inst = sw_inst->next)
            {
                char *description = NULL; 
                char *version = NULL;

                if (xmlCimInstGetPropTrim(sw_inst, "Description",
                                          &description, NULL) < 0)
                {
                    ERROR_MSG("Failed to get Description property "
                              "value of SF_SoftwareIdentity instance");
                    rc = -1;
                    goto cleanup;
                }

                if (xmlCimInstGetPropTrim(sw_inst, "VersionString",
                                          &version, NULL) < 0)
                {
                    ERROR_MSG("Failed to get VersionString property "
                              "value of SF_SoftwareIdentity instance");
                    rc = -1;
                    free(description);
                    goto cleanup;
                }

                if (strcmp(description, "NIC MC Firmware") == 0 &&
                    update_controller)
                {
                    printf("Controller version: %s\n", version); 
                }
                else if (strcmp(description, "NIC BootROM") == 0 &&
                         update_bootrom)
                {
                    printf("BootROM version:    %s\n", version); 
                }

                free(description);
                free(version);
            }

            clear_response(&assoc_sw_rsp);

            if (if_found && interface_name != NULL)
                break;
        }

        clear_response(&ei_ports_rsp);

        if (!if_found)
        {
            ERROR_MSG("No interface named '%s' was found",
                      interface_name);
            rc = -1;
            goto cleanup;
        }

        if (do_update && (fw_url != NULL || fw_path != NULL)
            && (update_controller || update_bootrom))
        {
            if (!yes)
            {
                char yes_str[4];
                int  i;

                printf("\nDo you want to update %s firmware on %s? [yes/no]",
                       update_controller && update_bootrom ?
                       "controller and BootROM" :
                            (update_controller ? "Controller" : "BootROM"),
                        interface_name == NULL ? "all NICs" : interface_name);
                fgets(yes_str, sizeof(yes_str), stdin);
                for (i = 0; i < sizeof(yes_str); i++)
                    if (yes_str[i] == '\n' || yes_str[i] == '\r')
                        yes_str[i] = '\0';

                if (strncasecmp(yes_str, "yes", sizeof(yes_str)) == 0 ||
                    strncasecmp(yes_str, "y", sizeof(yes_str)) == 0)
                    yes = 1;
            }

            if (yes &&
                update_firmware(curl, cim_namespace,
                                interface_name == NULL ? NULL : nic_inst,
                                update_controller, update_bootrom,
                                fw_url == NULL ? fw_path : fw_url,
                                fw_url == NULL ? 0 : 1,
                                force_update) < 0)
            {
                ERROR_MSG_PLAIN("Problems encountered when trying to "
                                "update firmware");
                rc = -1;
                goto cleanup;
            }
        }

        clear_response(&assoc_nic_rsp);
        nic_inst = NULL;

        clear_response(&assoc_cntr_rsp);
        controller_inst = NULL;

        curl_easy_cleanup(curl);
        curl = NULL;
    }

cleanup:
    if (curl != NULL)
        curl_easy_cleanup(curl);
    curl_global_cleanup();
    free(curl_data);

    clear_response(&ei_ports_rsp);
    clear_response(&assoc_cntr_rsp);
    clear_response(&assoc_sw_rsp);
    clear_response(&assoc_nic_rsp);

    free(user);
    free(password);
    free(cim_namespace);
    free(fw_url);
    free(fw_path);
    free(interface_name);

    return rc;
}
