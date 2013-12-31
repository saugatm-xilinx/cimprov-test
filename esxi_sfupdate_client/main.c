#include <stdlib.h>
#include <stdio.h>

#include <popt.h>

#ifdef NCURSES_CURSES_H
#include <ncurses/curses.h>
#else
#include <curses.h>
#endif

#include <string.h>
#include <iconv.h>
#include <curl/curl.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#define MAX_PASSWD_LEN 128

#define DEBUG_PRINT(s...) \
    fprintf(stderr, s)

#define MAX_ERR_STR 1024

#define ERROR_MSG(s_...) \
    do {                                                    \
        char str_[MAX_ERR_STR];                             \
        snprintf(str_, MAX_ERR_STR, s_);                    \
        fprintf(stderr, "File %s, Line %d: %s\n",           \
                __FILE__, __LINE__, str_);                  \
    } while (0)

#define ERROR_MSG_PLAIN(s_...) \
    do {                                                    \
        char str_[MAX_ERR_STR];                             \
        snprintf(str_, MAX_ERR_STR, s_);                    \
        fprintf(stderr, "%s\n", str_);                      \
    } while (0)

#define SKIP_TEXT(node_) \
    do {                                            \
        while (node_ != NULL &&                     \
               xmlStrcmp(node_->name,               \
                         BAD_CAST "text") == 0)     \
            node_ = node_->next;                    \
    } while (0)

static char *address = NULL;
static char *cim_host = NULL;
static char *cim_port = NULL;
static int   use_https = 0;
static char *user = NULL;
static char *password = NULL;
static char *cim_namespace = NULL;
static char *fw_url = NULL;
static char *interface_name = NULL;
static int   update_controller = 0;
static int   update_bootrom = 0;
static int   yes = 0;

#define DEF_HTTP_PORT "5988"
#define DEF_HTTPS_PORT "5989"
#define MAX_ADDR_LEN 1024

/** Command line options */
enum {
    OPT_CIMOM_ADDR = 1,
    OPT_CIMOM_HOST,
    OPT_CIMOM_PORT,
    OPT_HTTPS,
    OPT_CIMOM_USER,
    OPT_CIMOM_PASSWORD,
    OPT_PROVIDER_NAMESPACE,
    OPT_FW_URL,
    OPT_IF_NAME,
    OPT_CONTROLLER,
    OPT_BOOTROM,
    OPT_YES,
};

int
parseCmdLine(int argc, const char *argv[])
{
    char *a = NULL;
    char *host = NULL;
    char *port = NULL;
    int   https = 0;
    char *u = NULL;
    char *p = NULL;
    char *ns = NULL;
    char *s = NULL;
    char *url = NULL;
    char *if_name = NULL;
    int   controller = 0;
    int   bootrom = 0;
    int   y = 0;
    char  passwd_buf[MAX_PASSWD_LEN];

    poptContext     optCon;
    int             opt;
    int             rc = 0;

    struct poptOption options_table[] = {
        { "cimom-address", 'a', POPT_ARG_STRING, NULL,
          OPT_CIMOM_ADDR,
          "Address of CIMOM (e.g. https://hostname:5989",
          NULL },

        { "cimom-host", '\0', POPT_ARG_STRING, NULL,
          OPT_CIMOM_HOST,
          "CIMOM host",
          NULL },

        { "cimom-port", '\0', POPT_ARG_STRING, NULL,
          OPT_CIMOM_PORT,
          "CIMOM port",
          NULL },

        { "https", 's', POPT_ARG_NONE, NULL,
          OPT_HTTPS,
          "Use HTTPS to access CIMOM",
          NULL },

        { "cimom-user", 'u', POPT_ARG_STRING, NULL,
          OPT_CIMOM_USER,
          "CIMOM user's name",
          NULL },

        { "cimom-password", 'p', POPT_ARG_STRING, NULL,
          OPT_CIMOM_PASSWORD,
          "CIMOM user's password",
          NULL },

        { "provider-namespace", 'n', POPT_ARG_STRING, NULL,
          OPT_PROVIDER_NAMESPACE,
          "Provider namespace (solarflare/cimv2 by default)",
          NULL },

        { "firmware-url", 'f', POPT_ARG_STRING, NULL,
          OPT_FW_URL,
          "URL of firmware images",
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

            case OPT_CIMOM_HOST:
                host = poptGetOptArg(optCon);
                break;

            case OPT_CIMOM_PORT:
                port = poptGetOptArg(optCon);
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
        free(host);
        free(port);
        free(u);
        free(p);
        free(ns);
        free(url);
        free(if_name);
    }
    else
    {
        address = a;
        cim_host = host;
        cim_port = port;
        use_https = https;
        user = u;
        password = p;
        if (ns != NULL)
        {
            free(cim_namespace);
            cim_namespace = ns;
        }
        fw_url = url;
        interface_name = if_name;
        update_controller = controller;
        update_bootrom = bootrom;
        yes = y;
    }

    return rc;
}

typedef struct xmlCimInstance {
    xmlChar     *class_name;
    xmlNodePtr   namespace_path;
    xmlNodePtr   instance_name;
    xmlNodePtr   instance;

    struct xmlCimInstance *prev;
    struct xmlCimInstance *next;
} xmlCimInstance;

void
freeXmlCimInstance(xmlCimInstance *p)
{
    xmlFree(p->class_name);
    xmlFreeNode(p->namespace_path);
    xmlFreeNode(p->instance_name);
    xmlFreeNode(p->instance);
    free(p);
}

void
sort_inst_list(xmlCimInstance **head,
               int (*f)(xmlCimInstance *, xmlCimInstance *, void *),
               void *arg)
{
    xmlCimInstance *p;
    xmlCimInstance *q;
    int             swapped;

    if (head == NULL)
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

char *str_trim_xml_free(xmlChar *s)
{
    char *result = str_trim((char *)s);
    xmlFree(s);
    return result;
}

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

xmlDocPtr
xmlReqPrepareMethodCall(int imethod, const char *namespace,
                        xmlCimInstance *inst,
                        const char *method_name,
                        xmlNodePtr *methodcall_node)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      simplereq_node = NULL;
    xmlNodePtr      localinstpath_node = NULL;
    xmlNodePtr      inst_name_copy_node = NULL;

    doc = xmlReqPrepareSimpleReq(&simplereq_node);
    *methodcall_node =
        xmlNewChild(simplereq_node, NULL,
                    BAD_CAST (imethod ? "IMETHODCALL" : "METHODCALL"),
                    NULL);
    xmlNewProp(*methodcall_node, BAD_CAST "NAME", BAD_CAST method_name);
    if (imethod)
        fillNamespace(*methodcall_node, namespace);
    else
        addLocalInstPath(*methodcall_node, namespace, inst);

    return doc;
}

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

xmlDocPtr
xmlReqInstallFromURI(const char *namespace, xmlCimInstance *svc,
                     xmlCimInstance *target, const char *url)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      methodcall_node = NULL;
    xmlNodePtr      paramvalue_node = NULL;
    xmlNodePtr      value_node = NULL;

    doc = xmlReqPrepareMethodCall(0, namespace, svc, "InstallFromURI",
                                  &methodcall_node);

    paramvalue_node = xmlNewChild(methodcall_node, NULL,
                                  "PARAMVALUE", NULL);
    xmlNewProp(paramvalue_node, BAD_CAST "NAME",
               BAD_CAST "URI");
    xmlNewProp(paramvalue_node, BAD_CAST "PARAMTYPE",
               BAD_CAST "string");
    value_node = xmlNewChild(paramvalue_node, NULL, BAD_CAST "VALUE",
                             BAD_CAST url);

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

    return doc;
}

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

static char *curl_data = NULL;
static int curl_data_len = 0;

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

typedef struct response_descr {
    char           *method_name;
    int             imethod_called;
    char           *returned_value;
    int             error_returned;
    char           *err_code;
    char           *err_descr;
    xmlCimInstance *inst_list;
    int             inst_count;
} response_descr;

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
    memset(rsp, 0, sizeof(*rsp));
}

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
                              BAD_CAST "RETURNVALUE") == 0)
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

                    response->returned_value =
                        str_trim_xml_free(xmlNodeGetContent(aux));
                }
            }
        }
    }

cleanup:
    xmlFreeDoc(doc);
    return rc;
}

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
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
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

int
install_from_uri(CURL *curl, const char *namespace,
                 xmlCimInstance *svc, xmlCimInstance *target,
                 const char *url,
                 response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqInstallFromURI(namespace, svc, target, url);
    
    return processXmlRequest(curl, doc, "InstallFromURI",
                             response);
}

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
            rc_ = -1;                                                    \
        }                                                               \
    } while (0)

int
update_firmware(CURL *curl, const char *namespace,
                xmlCimInstance *nic_inst,
                int controller, int bootrom,
                const char *firmware_url)
{
    response_descr  svcs_rsp;
    response_descr  log_rsp;
    response_descr  rec_rsp;
    response_descr  call_rsp;
    int             rc = 0;
    int             log_available = 0;
    int             print_err_recs = 0;
    char           *recent_rec_id = NULL;
    char           *svc_name = NULL;

    xmlCimInstance *svc_inst = NULL;
    xmlCimInstance *svc_mcfw_inst = NULL;
    xmlCimInstance *svc_bootrom_inst = NULL;
    xmlCimInstance *log_inst = NULL;
    xmlCimInstance *rec_inst = NULL;

    memset(&svcs_rsp, 0, sizeof(svcs_rsp));
    memset(&log_rsp, 0, sizeof(rec_rsp));
    memset(&rec_rsp, 0, sizeof(rec_rsp));
    memset(&call_rsp, 0, sizeof(call_rsp));

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
                    rc,
                    install_from_uri(curl, namespace,
                                     svc_mcfw_inst,
                                     nic_inst, firmware_url,
                                     &call_rsp),
                    call_rsp,
                    "Call of InstallFromURI() to update "
                    "controller firmware failed");
            if (strcmp(call_rsp.returned_value, "0") != 0)
            {
                ERROR_MSG_PLAIN("InstallFromURI() returned %s when "
                                "trying to update controller firmware",
                                call_rsp.returned_value);
                rc = -1;
                print_err_recs = 1;
            }
            clear_response(&call_rsp);
        }

        if (bootrom)
        {
            CHECK_RESPONSE(
                    rc,
                    install_from_uri(curl, namespace,
                                     svc_bootrom_inst,
                                     nic_inst, firmware_url,
                                     &call_rsp),
                    call_rsp,
                    "Call of InstallFromURI() to update "
                    "BootROM firmware failed");
            if (strcmp(call_rsp.returned_value, "0") != 0)
            {
                ERROR_MSG_PLAIN("InstallFromURI() returned %s when "
                                "trying to update BootROM firmware",
                                call_rsp.returned_value);
                rc = -1;
                print_err_recs = 1;
            }
            else
                printf("Firmware was successfully updated.\n");
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
    clear_response(&svcs_rsp);
    clear_response(&call_rsp);
    clear_response(&log_rsp);
    clear_response(&rec_rsp);
    free(recent_rec_id);
    return rc;
}

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

    if (address == NULL)
    {
        address = calloc(1, MAX_ADDR_LEN);
        snprintf(
            address, MAX_ADDR_LEN, "%s://%s:%s",
            use_https ? "https" : "http",
            cim_host == NULL ? "localhost" : cim_host,
            cim_port == NULL ?
              (use_https ? DEF_HTTPS_PORT : DEF_HTTP_PORT) : cim_port);
    }

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

        if (fw_url != NULL && (update_controller || update_bootrom))
        {
            if (!yes)
            {
                char yes_str[4];
                int  i;

                printf("\nDo you want to update %s firmware on %s? [yes/no]",
                       update_controller && update_bootrom ?
                       "controller and BootROM" :
                            (update_controller ? "controller" : "BootROM"),
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
                                fw_url) < 0)
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

    free(address);
    free(cim_host);
    free(cim_port);
    free(user);
    free(password);
    free(cim_namespace);
    free(fw_url);
    free(interface_name);

    return rc;
}
