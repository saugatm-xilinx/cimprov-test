#include <stdlib.h>
#include <stdio.h>

#include "cim_log.h"
#include "cim_client.h"

#include <string.h>
#include <curl/curl.h>

#include <openssl/evp.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include "curl_aux.h"

/**
 * Free memory allocated for xmlCimInstance.
 *
 * @param p   xmlCimInstance pointer
 */
void
freeXmlCimInstance(xmlCimInstance *p)
{
    if (p == NULL)
        return;
    xmlFree(p->class_name);
    xmlFreeNode(p->namespace_path);
    xmlFreeNode(p->instance_name);
    xmlFreeNode(p->instance);
    free(p);
}

/**
 * Create a copy of xmlCimInstance.
 *
 * @param p   xmlCimInstance pointer
 */
xmlCimInstance *
xmlCimInstanceDup(xmlCimInstance *p)
{
    xmlCimInstance *q;

    if (p == NULL)
        return NULL;

    q = calloc(1, sizeof(xmlCimInstance));
    if (q == NULL)
        return NULL;

    q->class_name = xmlStrdup(p->class_name);
    q->namespace_path = xmlCopyNode(p->namespace_path, 1);
    q->instance_name = xmlCopyNode(p->instance_name, 1);
    q->instance = xmlCopyNode(p->instance, 1);

    return q;
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
static char *
str_trim(char *s)
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
static char *str_trim_xml_free(xmlChar *s)
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
static void
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
static void
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
static xmlDocPtr
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
static xmlDocPtr
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

/**
 * Get named value by its name from the list of named values.
 *
 * @param list      List of named values
 * @param name      Name
 *
 * @return Value of interest or NULL if not found
 */
char *get_named_value(named_value *list, const char *name)
{
    named_value *p;

    for (p = list; p != NULL; p = p->next)
        if (strcmp(p->name, name) == 0)
            return p->value;

    return NULL;
}

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
static int
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
static int
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
    if (res != CURLE_OK)
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
        if (curl_data != NULL)
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


