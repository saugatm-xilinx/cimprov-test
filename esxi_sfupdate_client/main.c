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
#include <libxml/xmlstring.h>

#include "sf_base64.h"

#include <assert.h>

#include "fw_images.h"

#include "cim_client.h"
#include "cim_log.h"

#include "curl_aux.h"
#include "common/efx.h"

extern fw_img_descr firmware_images[];
extern unsigned int fw_images_count;

static fw_img_descr *downloaded_fw_imgs = NULL;
static unsigned int  downloaded_count = 0;
static unsigned int  downloaded_max_count = 0;

/* Maximum file path length */
#define MAX_PATH_LEN 1024

/* Maximum password length */
#define MAX_PASSWD_LEN 128

/* Maximum NIC tag length */
#define MAX_NIC_TAG_LEN 1024

/* Maximum version string length */
#define MAX_VER_LEN 256

/* Maximum image name length */
#define MAX_IMG_NAME_LEN 256

#define HTTP_PROTO "http://"
#define HTTPS_PROTO "https://"
#define FILE_PROTO "file://"


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

/** Whether we should update UEFIROM firmware or not */
static int   update_uefirom = 0;

/** Whether we should update SUCFW firmware or not */
static int   update_sucfw = 0;

/**
 * Whether we can skip asking user for confirmation before firmware update
 * or not
 */
static int   yes = 0;

/**
 * Do not try to download firmware image from an URL
 * specified on a host where this tool is run.
 */
static int   fw_url_no_local_access = 0;

/**
 * Do not pass URL to CIM provider but transfer
 * downloaded firmware images via CIM private methods
 * instead.
 */
static int   fw_url_use_cim_transf = 0;

/**
 * Tool version
 * The version number is same as cimprovider version
 * It gets update by scripts/createtag script
 */
static char* tool_version = "v2.1.0.24";

/* Default HTTP port */
#define DEF_HTTP_PORT "5988"

/* Default HTTPS port */
#define DEF_HTTPS_PORT "5989"

/* Maximum length of a string representing CIMOM address */
#define MAX_ADDR_LEN 1024

#define SVC_BOOTROM_NAME "BootROM"
#define SVC_MCFW_NAME "Firmware"
#define SVC_UEFIROM_NAME "UEFIROM"
#define SVC_SUCFW_NAME "SUCFW"

/** Command line options */
enum {
    OPT_CIMOM_ADDR = 1,         /**< CIMOM address - use either it or 
                                     HOST/PORT/HTTPS to specify CIMOM */
    OPT_HTTPS,                  /**< Use HTTPS */
    OPT_CIMOM_USER,             /**< CIMOM user name */
    OPT_CIMOM_PASSWORD,         /**< CIMOM user password */
    OPT_PROVIDER_NAMESPACE,     /**< CIM provider namespace */
    OPT_FW_URL,                 /**< URL of firmware image(s) */
    OPT_FW_PATH,                /**< Path to firmware image(s) */
    OPT_FORCE,                  /**< Force firmware update even
                                     if version of the image is
                                     lower than or the same as already
                                     installed */
    OPT_IF_NAME,                /**< Network interface name */
    OPT_CONTROLLER,             /**< Process controller firmware */
    OPT_BOOTROM,                /**< Process BootROM firmware */
    OPT_UEFIROM,                /**< Process UEFIROM firmware */
    OPT_SUCFW,                  /**< Process SUC firmware */
    OPT_YES,                    /**< Don't ask user for confirmation when
                                     updating firmware */
    OPT_WRITE,                  /**< Perform firmware update */
    OPT_FW_URL_NO_LOCAL_ACCESS, /**< Don't try to download firmware
                                     image on a host where this tool
                                     is run */
    OPT_FW_URL_USE_CIM_TRANSF,  /**< Do not pass URL to CIM provider
                                     but transfer donwloaded firmare
                                     images to it via private CIM methods */
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
    int   uefirom = 0;
    int   sucfw = 0;
    int   y = 0;
    int   w = 0;
    int   url_no_local_access = 0;
    int   url_use_cim_transf = 0;
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
          "Process Controller firmware",
          NULL },

        { "bootrom", '\0', POPT_ARG_NONE, NULL,
          OPT_BOOTROM,
          "Process BootROM firmware",
          NULL },

        { "uefirom", '\0', POPT_ARG_NONE, NULL,
          OPT_UEFIROM,
          "Process UEFIROM firmware",
          NULL },

        { "sucfw", '\0', POPT_ARG_NONE, NULL,
          OPT_SUCFW,
          "Process SUC firmware",
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

        { "firmware-url-no-local-access", '\0', POPT_ARG_NONE, NULL,
          OPT_FW_URL_NO_LOCAL_ACCESS,
          "Do not try to access firmware images from an URL specified "
          "from this tool, just pass URL to CIM provider. Version "
          "checks will be disabled; --fw-url-use-cim-transfer "
          "cannot be used together with this option",
          NULL },

        { "firmware-url-use-cim-transfer", '\0', POPT_ARG_NONE, NULL,
          OPT_FW_URL_USE_CIM_TRANSF,
          "Do not pass firmware URL to CIM provider but transfer "
          "downloaded firmware images via private CIM methods. May "
          "be useful if you have issues with ESXi firewall or if "
          "URL specified is not available on the ESXi target host",
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

            case OPT_UEFIROM:
                uefirom = 1;
                break;

            case OPT_SUCFW:
                sucfw = 1;
                break;

            case OPT_YES:
                y = 1;
                break;

            case OPT_WRITE:
                w = 1;
                break;

            case OPT_FW_URL_NO_LOCAL_ACCESS:
                url_no_local_access = 1;
                break;

            case OPT_FW_URL_USE_CIM_TRANSF:
                url_use_cim_transf = 1;
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
        ERROR_MSG_PLAIN("Unexpected arguments in command line: %s", s);
        rc = -1;
        goto cleanup;
    }

    if (a == NULL || strlen(a) == 0)
    {
        ERROR_MSG_PLAIN("Address of CIM server was not specified "
                        "(--cim-address=?)");
        rc = -1;
        goto cleanup;
    }
    if (url_no_local_access && url_use_cim_transf)
    {
        ERROR_MSG_PLAIN("Options --fw-url-no-local-access and "
                        "--fw-url-use-cim-transfer cannot be used "
                        "simultaneously");
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
        if (u == NULL || strlen(u) == 0)
            user = strdup("root");
        else
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
        update_uefirom = uefirom;
        update_sucfw = sucfw;
        yes = y;
        do_update = w;
        fw_url_no_local_access = url_no_local_access;
        fw_url_use_cim_transf = url_use_cim_transf;
    }

    return rc;
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

    addSimpleParamValue(methodcall_node, "FileName", "string",
                        file_name);
    addSimpleParamValue(methodcall_node, "Base64Str", "string",
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

    doc = xmlReqPrepareMethodCall(0, namespace, svc,
                                  "RemoveFwImage",
                                  &methodcall_node);

    addSimpleParamValue(methodcall_node, "FileName", "string",
                        file_name);

    return doc;
}

/**
 * Construct CIM-XML request for calling GetLocalFwImageVersion() method
 *
 * @param namespace     Namespace
 * @param svc           SF_SoftwareInstallationService instance pointer
 * @param target        Target parameter (should be instance of SF_NICCard)
 * @param file_name     Name of temporary file in which data
 *                      would be stored
 *
 * @return XML document pointer for the constructed CIM-XML request
 */
xmlDocPtr
xmlReqGetLocalFwImageVersion(const char *namespace, xmlCimInstance *svc,
                             xmlCimInstance *target, const char *file_name)
{
    xmlDocPtr       doc = NULL;
    xmlNodePtr      methodcall_node = NULL;
    xmlNodePtr      paramvalue_node = NULL;
    xmlNodePtr      value_node = NULL;

    doc = xmlReqPrepareMethodCall(0, namespace, svc,
                                  "GetLocalFwImageVersion",
                                  &methodcall_node);

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

    addSimpleParamValue(methodcall_node, "FileName", "string",
                        file_name);

    return doc;
}


/**
 * Call InstallFromURI() extrinsic method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param target            Target parameter (may be instance of SF_NICCard
 *                          or NULL)
 * @param url               Firmware image(s) URL
 * @param unused            Unused parameter (for compatibility with
 *                          install_from_local_source() signature)
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
                      int unused,
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

    if (strcmp(svc_name, SVC_BOOTROM_NAME) == 0)
        firmware_type = "BootROM";
    else if (strcmp(svc_name, SVC_MCFW_NAME) == 0)
        firmware_type = "Controller";
    else if (strcmp(svc_name, SVC_UEFIROM_NAME) == 0)
        firmware_type = "UEFIROM";
    else
        firmware_type = "SUCFW";

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
 * Call GetLocalFwImageVersion() extrinsic method
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Namespace
 * @param svc               SF_SoftwareInstallationService instance pointer
 * @param target            Target parameter (should be instance of SF_NICCard)
 * @param file_name         Temporary file name where image was stored
 * @param response    [out] Parsed CIM-XML response
 *
 * @return 0 on success, -1 on failure
 */
int
call_get_local_fw_image_version(CURL *curl, const char *namespace,
                                xmlCimInstance *svc,
                                xmlCimInstance *target,
                                const char *file_name,
                                response_descr *response)
{
    xmlDocPtr doc;

    doc = xmlReqGetLocalFwImageVersion(namespace, svc, target, file_name);
    
    return processXmlRequest(curl, doc, "GetLocalFwImageVersion",
                             response);
}

/**
 * Get default firmware subdir.
 *
 * @param svc_name      Name of SF_SoftwareInstallationService class
 *                      instance
 *
 * @return Subdir's name on success or NULL on error
 */
const char *
getFwSubdirName(const char *svc_name)
{
    const char *subdir = NULL;

    if (strcmp(svc_name, SVC_BOOTROM_NAME) == 0)
        subdir = "bootrom";
    else if (strcmp(svc_name, SVC_MCFW_NAME) == 0)
        subdir = "mcfw";
    else if (strcmp(svc_name, SVC_UEFIROM_NAME) == 0)
        subdir = "uefirom";
    else if (strcmp(svc_name, SVC_SUCFW_NAME) == 0)
        subdir = "sucfw";

    return subdir;
}

/**
 * Complete firmare image path if it is a path to directory rather
 * than to concrete image.
 *
 * @param fw_source           Path to be completed
 * @param svc_name            Name of SF_SoftwareInstallationService
 *                            class instance
 * @param url_specified       Whether a given path is URL or nor
 * @param img_def_name        Default firmware image name
 * @param full_path     [out] Path to firmware image (completed if
 *                            it was necessary)
 *
 * @return 0 on success, -1 on failure
 */
static int
pathCompletion(const char *fw_source, const char *svc_name,
               int url_specified,
               const char *img_def_name, char *full_path)
{
    if (strstr(fw_source, ".dat") !=
        fw_source + (strlen(fw_source) - 4))
    {
        const char *add_delimeter = "";
        const char *subdir = NULL;

        subdir = getFwSubdirName(svc_name);
        if (subdir == NULL)
        {
            ERROR_MSG("'%s' service name is not supported",
                      svc_name);
            return -1;
        }

#ifdef _WIN32
        if (!url_specified && strlen(fw_source) > 0 &&
            fw_source[strlen(fw_source) - 1] != '\\')
            add_delimeter = "\\";
        else if (url_specified && strlen(fw_source) > 0 &&
                 fw_source[strlen(fw_source) - 1] != '/')
            add_delimeter = "/";
#else
        if (!strlen(fw_source) > 0 &&
            fw_source[strlen(fw_source) - 1] != '/')
            add_delimeter = "/";
#endif

#ifdef _WIN32
        if (!url_specified)
            snprintf(full_path, MAX_PATH_LEN, "%s%simage\\%s\\%s",
                     fw_source, add_delimeter, subdir, img_def_name);
        else
            snprintf(full_path, MAX_PATH_LEN, "%s%simage/%s/%s",
                     fw_source, add_delimeter, subdir, img_def_name);
#else
        snprintf(full_path, MAX_PATH_LEN, "%s%simage/%s/%s",
                 fw_source, add_delimeter, subdir, img_def_name);
#endif
    }
    else
        snprintf(full_path, MAX_PATH_LEN, "%s", fw_source);

    return 0;
}

/**
 * Checking whether current version is lower than
 * new one.
 *
 * @param cur_version       Current version
 * @param new_ver_a         First number of the new version
 * @param new_ver_b         Second number of the new version
 * @param new_ver_c         Third number of the new version
 * @param new_ver_d         Fourth number of the new version
 *
 * @return 1 if current version is lower, 0 otherwise
 */
int
checkVersion(const char *cur_version,
             int new_ver_a, int new_ver_b,
             int new_ver_c, int new_ver_d)
{
    int old_ver_a = 0;
    int old_ver_b = 0;
    int old_ver_c = 0;
    int old_ver_d = 0;

    sscanf(cur_version, "%d.%d.%d.%d", 
           &old_ver_a, &old_ver_b, &old_ver_c, &old_ver_d);

    if (old_ver_a < new_ver_a)
        return 1;
    else if (old_ver_a == new_ver_a &&
             old_ver_b < new_ver_b)
        return 1;
    else if (old_ver_a == new_ver_a &&
             old_ver_b == new_ver_b &&
             old_ver_c < new_ver_c)
        return 1;
    else if (old_ver_a == new_ver_a &&
             old_ver_b == new_ver_b &&
             old_ver_c == new_ver_c &&
             old_ver_d < new_ver_d)
        return 1;

    return 0;
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
 * @param path              Firmware image(s) path
 * @param url_specified     Whether path is URL or not
 * @param force             Update firmware even if verion of the image is
 *                          the same or earlier than already installed
 * @param unused1           Unused parameter (for compatibility with
 *                          call_install_from_uri() function signature)
 * @param unused2           Another unused parameter for the same reason
 *
 * @return 0 on success, -1 on failure
 */
int
install_from_local_source(CURL *curl, const char *namespace,
                          xmlCimInstance *svc, xmlCimInstance *target,
                          const char *path,
                          int url_specified,
                          int force,
                          const char *unused1,
                          response_descr *unused2)
{
#define CHUNK_LEN 100000

    int                 rc = 0;
    int                 rc_rsp = 0;
    xmlCimInstance      aux_inst;
    xmlCimInstance     *targets_list = NULL;
    xmlCimInstance     *p;
    response_descr      nics_rsp;
    response_descr      call_rsp;
    char               *svc_name = NULL;

    int                 not_full_path = 0;
    char                full_path[MAX_PATH_LEN];
    char                full_path_prev[MAX_PATH_LEN];
    int                 img_idx = -1;
    int                 img_idx_prev = -1;
    char               *tmp_file_name = NULL;
    char                url[MAX_PATH_LEN];

    EVP_MD_CTX          mdctx;
    char                md_hash[EVP_MAX_MD_SIZE];
    int                 md_len;

    size_t              enc_size;
    char               *encoded = NULL;

    char                img_chunk[CHUNK_LEN];
    size_t              read_len;
    size_t              was_sent;

    fw_img_descr       *imgs_array = NULL;
    unsigned int        imgs_count = 0;

    int                  use_local_path = 0;

    efx_image_header_t   update_img_header;
    int                  no_cur_ver = 0;
    int                  no_img_ver = 0;
    char                 cur_version[MAX_VER_LEN];

    memset(&nics_rsp, 0, sizeof(nics_rsp));
    memset(&call_rsp, 0, sizeof(call_rsp));

    if (path != NULL && !url_specified)
        use_local_path = 1;

    if (url_specified)
    {
        imgs_array = downloaded_fw_imgs;
        imgs_count = downloaded_count;
    }
    else
    {
        imgs_array = firmware_images;
        imgs_count = fw_images_count;
    }

    full_path[0] = '\0';
    full_path_prev[0] = '\0';

    if (path == NULL ||
        strstr(path, ".dat") != path + (strlen(path) - 4))
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
        img_idx_prev = img_idx;
        strncpy(full_path_prev, full_path, MAX_PATH_LEN);

        if (p != NULL)
        {
            char *ver = NULL;

            if (strcmp(svc_name, SVC_SUCFW_NAME) != 0)
                CHECK_RESPONSE_EXT(
                    rc_rsp,
                    call_get_required_fw_image_name(curl, namespace,
                                                    svc, p,
                                                    &call_rsp),
                    call_rsp,
                    "Failed to get required firmware image name");
            else {
                // for sucfw, Firmare type might not be supported on board
                // so call to get firmware image might fail
                // and we dont want to print any error message in that case
                rc_rsp = call_get_required_fw_image_name(curl, namespace,
                                        svc, p,
                                        &call_rsp);
                if (call_rsp.error_returned ||
                    strcmp(call_rsp.returned_value, "0") != 0)
                        goto next_target;
            }

            if (rc_rsp < 0)
            {
                rc = -1;
                goto next_target;
            }

            ver = get_named_value(call_rsp.out_params_list,
                                  "CurrentVersion");
            if (ver == NULL || strlen(ver) == 0)
            {
                ERROR_MSG_PLAIN("Failed to get current firmware "
                                "version");
                no_cur_ver = 1;
            }
            else
            {
                strncpy(cur_version, ver, MAX_VER_LEN);
                no_cur_ver = 0;
            }
        }

        if (not_full_path)
        {
            assert(p != NULL);

            if (use_local_path)
            {
                char *name;

                name = get_named_value(call_rsp.out_params_list,
                                       "Name");
                if (name == NULL || strlen(name) == 0)
                {
                    ERROR_MSG_PLAIN("Failed to get required firmware "
                                    "image name");
                    rc = -1;
                    goto next_target;
                }

                if (pathCompletion(path, svc_name, 0, name,
                                   full_path) < 0)
                {
                    ERROR_MSG("Failed to complete path '%s'",
                              path);
                    rc = -1;
                    goto next_target;
                }
            }
            else
            {
                char *type;
                char *subtype;
 
                unsigned int   int_type;
                unsigned int   int_subtype;
                unsigned int   i;
                efx_image_info_t  imageInfo;

                type = get_named_value(call_rsp.out_params_list,
                                       "Type");
                subtype = get_named_value(call_rsp.out_params_list,
                                          "Subtype");
                if (type == NULL || strlen(type) == 0 ||
                    subtype == NULL || strlen(subtype) == 0)
                {
                    ERROR_MSG_PLAIN("Failed to get required firmware "
                                    "image type/subtype");
                    rc = -1;
                    goto next_target;
                }
 
                int_type = strtol(type, NULL, 10);
                int_subtype = strtol(subtype, NULL, 10);

                for (i = 0; i < imgs_count; i++)
                {
                    rc = efx_check_reflash_image(imgs_array[i].data, imgs_array[i].size, &imageInfo);
                    if (rc != 0 )
                    {
                        ERROR_MSG_PLAIN("Error in checking firmware image");
                        continue;
                    }
                    efx_image_header_t *header = imageInfo.eii_headerp;
                    if (header->eih_type == int_type &&
                        header->eih_subtype == int_subtype)
                        break;
                }

                if (i < imgs_count)
                    img_idx = i;
                else
                    /** Skip not found image */
                    goto next_target;
            }

            clear_response(&call_rsp);
        }
        else
        {
            snprintf(full_path, MAX_PATH_LEN, "%s", path);
            if (url_specified && !not_full_path)
            {
                assert(imgs_count == 1);
                img_idx = 0;
            }
        }

        if ((!use_local_path && img_idx != img_idx_prev) ||
            (use_local_path && strcmp(full_path, full_path_prev) != 0))
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
                                            "FileName");
            if (tmp_file_name == NULL || strlen(tmp_file_name) == 0)
            {
                ERROR_MSG_PLAIN("Failed to get temporary file name "
                                "for firmware transfer");
                rc = -1;
                goto cleanup;
            }

            tmp_file_name = strdup(tmp_file_name);
            clear_response(&call_rsp);

            if (use_local_path)
            {
                f = fopen(full_path, "rb");
                if (f == NULL)
                    /* Skip not found firmware image */
                    goto next_target;
            }

            EVP_MD_CTX_init(&mdctx);
            if (!EVP_DigestInit_ex(&mdctx, EVP_sha1(), NULL))
            {
                ERROR_MSG("EVP_DigestInit_ex() failed");
                if (use_local_path)
                    fclose(f);
                rc = -1;
                goto cleanup;
            }

            was_sent = 0;
            do {
                int ferr;
                if (use_local_path)
                {
                    read_len = fread(img_chunk, 1, CHUNK_LEN, f); 
                    ferr = ferror(f);
                    if (ferr != 0)
                    {
                        ERROR_MSG("Read error occured when reading "
                                  "from '%s'",
                                  full_path);
                        rc = -1;
                        fclose(f);
                        goto cleanup;
                    }
                }
                else
                {
                    read_len = imgs_array[img_idx].size - was_sent;
                    if (read_len > CHUNK_LEN)
                        read_len = CHUNK_LEN;
                    memcpy(img_chunk,
                           imgs_array[img_idx].data + was_sent,
                           read_len);
                }

                if (read_len > 0)
                {
                    if (was_sent == 0)
                    {
                        if (read_len < sizeof(update_img_header))
                        {
                            ERROR_MSG("Unexpectedly small first chunk "
                                      "of image was read, so version was "
                                      "not determined");
                            no_img_ver = 1;
                        }
                        else
                            no_img_ver = 0;
                    }

                    enc_size = base64_enc_size(read_len);

                    if (!EVP_DigestUpdate(&mdctx, img_chunk, read_len))
                    {
                        ERROR_MSG("EVP_DigestUpdate() failed");
                        rc = -1;
                        if (use_local_path)
                            fclose(f);
                        goto cleanup;
                    }

                    encoded = calloc(enc_size, 1);
                    if (encoded == NULL)
                    {
                        ERROR_MSG("Failed to allocate memory for Base64 "
                                  "string");
                        rc = -1;
                        if (use_local_path)
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
                        if (use_local_path)
                            fclose(f);
                        goto cleanup;
                    }
                    clear_response(&call_rsp);
                    free(encoded);

                    was_sent += read_len;
                }
            } while ((use_local_path && !feof(f)) ||
                     (!use_local_path && read_len != 0));

            if (use_local_path)
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

        if (!(no_cur_ver || no_img_ver || force))
        {
            /*
             * Skip firmware image with lower version unless
             * force is specified.
             */
            efx_image_info_t  imageInfo;
            if (use_local_path)
            {
                if (rc = getImageHeader(full_path, &update_img_header) != 0)
                {
                    ERROR_MSG_PLAIN("%s: Error in reading firmware image", __FUNCTION__);
                    rc = -1;
                    goto cleanup;
                }
            }
            else
            {
                rc = efx_check_reflash_image(imgs_array[img_idx].data, imgs_array[img_idx].size, &imageInfo);
                if (rc != 0)
                {
                    ERROR_MSG_PLAIN("Error in checking firmware image");
                    rc = -1;
                    goto cleanup;
                }
                memcpy(&update_img_header,
                       imageInfo.eii_headerp, sizeof(update_img_header));
            }

            if (!checkVersion(cur_version,
                              update_img_header.eih_code_version_a,
                              update_img_header.eih_code_version_b,
                              update_img_header.eih_code_version_c,
                              update_img_header.eih_code_version_d))
                goto next_target;
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

        snprintf(url, MAX_PATH_LEN, "file://%s",
                 tmp_file_name);

        CHECK_RESPONSE_EXT(
            rc_rsp,
            call_install_from_uri(curl, namespace,
                                  svc, p,
                                  url, 1, force, encoded,
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
        rc_rsp = 0;
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
                    const char *, int, int, const char *,
                    response_descr *);

/**
 * Get SF_SofwareInstallationService instances responsible for
 * updating BootROM, Controller, UEFIROM and SUC firmware.
 *
 * @param curl                     CURL pointer returned by curl_easy_init()
 * @param namespace                Provider namespace
 * @param svc_mcfw_inst      [out] Service instance responsible for
 *                                 Controller firmware update
 * @param svc_bootrom_inst   [out] Service instance responsible for
 *                                 Controller firmware update
 * @param svc_uefirom_inst   [out] Service instance responsible for
 *                                 UEFIROM firmware update
 * @param svc_sucfw_inst     [out] Service instance responsible for
 *                                 SUC firmware update
 *
 * @return 0 on success, -1 on failure
 */
static int
getInstServices(CURL *curl, const char *namespace,
                xmlCimInstance **svc_mcfw_inst,
                xmlCimInstance **svc_bootrom_inst,
                xmlCimInstance **svc_uefirom_inst,
                xmlCimInstance **svc_sucfw_inst)
{
    response_descr  svcs_rsp;
    xmlCimInstance *svc_inst = NULL;
    int             rc = 0;

    memset(&svcs_rsp, 0, sizeof(svcs_rsp));

    CHECK_RESPONSE(
        rc,
        enumerate_instances(curl, namespace,
                            "SF_SoftwareInstallationService", 0,
                            &svcs_rsp),
        svcs_rsp,
        "Failed to enumerate SF_SoftwareInstallationService instances");
    if (rc < 0)
        return -1;

    for (svc_inst = svcs_rsp.inst_list; svc_inst != NULL;
         svc_inst = svc_inst->next)
    {
        char *name = NULL;

        if (xmlCimInstGetPropTrim(svc_inst, "Name", &name, NULL) < 0 ||
            name == NULL)
        {
            ERROR_MSG("Failed to get Name property of "
                      "SF_SoftwareInstallationService");
            clear_response(&svcs_rsp);
            return -1;
        }

        if (strcmp(name, SVC_MCFW_NAME) == 0 &&
            svc_mcfw_inst != NULL)
            *svc_mcfw_inst = svc_inst;
        else if (strcmp(name, SVC_BOOTROM_NAME) == 0 &&
                 svc_bootrom_inst != NULL)
            *svc_bootrom_inst = svc_inst;
        else if (strcmp(name, SVC_UEFIROM_NAME) == 0 &&
                 svc_uefirom_inst != NULL)
            *svc_uefirom_inst = svc_inst;
        else if (strcmp(name, SVC_SUCFW_NAME) == 0 &&
                 svc_sucfw_inst != NULL)
            *svc_sucfw_inst = svc_inst;

        free(name);
    }

    if (svc_mcfw_inst != NULL)
        *svc_mcfw_inst = xmlCimInstanceDup(*svc_mcfw_inst);
    if (svc_bootrom_inst != NULL)
        *svc_bootrom_inst = xmlCimInstanceDup(*svc_bootrom_inst);
    if (svc_uefirom_inst != NULL)
        *svc_uefirom_inst = xmlCimInstanceDup(*svc_uefirom_inst);
    if (svc_sucfw_inst != NULL)
        *svc_sucfw_inst = xmlCimInstanceDup(*svc_sucfw_inst);

    clear_response(&svcs_rsp);
    return 0;
}

/**
 * Update firmware
 *
 * @param curl              CURL pointer returned by curl_easy_init()
 * @param namespace         Provider namespace
 * @param nic_inst          SF_NICCard instance as target parameter
 *                          (or NULL)
 * @param controller        Whether to update controller firmware or not
 * @param bootrom           Whether to update BootROM firmware or not
 * @param uefirom           Whether to update UEFIROM firmware or not
 * @param sucfw             Whether to update SUC firmware or not
 * @param firmware_source   Firmware image(s) URL
 * @param url_specified     Firmware URL was specified (not local path)
 * @param force             --force option was specified
 *
 * @return 0 on success, -1 on failure
 */
int
update_firmware(CURL *curl, const char *namespace,
                xmlCimInstance *nic_inst,
                int controller, int bootrom, int uefirom,
                int sucfw, const char *firmware_source,
                int url_specified, int force)
{
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

    xmlCimInstance *svc_mcfw_inst = NULL;
    xmlCimInstance *svc_bootrom_inst = NULL;
    xmlCimInstance *svc_uefirom_inst = NULL;
    xmlCimInstance *svc_sucfw_inst = NULL;
    xmlCimInstance *log_inst = NULL;
    xmlCimInstance *rec_inst = NULL;

    memset(&log_rsp, 0, sizeof(rec_rsp));
    memset(&rec_rsp, 0, sizeof(rec_rsp));
    memset(&call_rsp, 0, sizeof(call_rsp));

    if (url_specified && firmware_source != NULL &&
        (fw_url_no_local_access ||
         strncasecmp(firmware_source, FILE_PROTO,
                     strlen(FILE_PROTO)) == 0) &&
         !fw_url_use_cim_transf)
        func_install = call_install_from_uri;
    else
        func_install = install_from_local_source;

    printf("\nUpdating firmware...\n");

    if ((rc = getInstServices(curl, namespace,
                              &svc_mcfw_inst,
                              &svc_bootrom_inst,
                              &svc_uefirom_inst,
                              &svc_sucfw_inst)) != 0)
        goto cleanup;

    if ((svc_mcfw_inst == NULL && controller) ||
        (svc_bootrom_inst == NULL && bootrom) ||
        (svc_uefirom_inst == NULL && uefirom) ||
        (svc_sucfw_inst == NULL && sucfw))
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
                                 url_specified,
                                 force, NULL, &call_rsp),
                    call_rsp,
                    "Attempt to update "
                    "Controller firmware failed");
            if (rc_rsp >= 0 && func_install != install_from_local_source &&
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
                                 url_specified,
                                 force, NULL, &call_rsp),
                    call_rsp,
                    "Attempt to update "
                    "BootROM firmware failed");
            if (rc_rsp >= 0 && func_install != install_from_local_source &&
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

        if (uefirom)
        {
            CHECK_RESPONSE(
                    rc_rsp,
                    func_install(curl, namespace,
                                 svc_uefirom_inst,
                                 nic_inst, firmware_source,
                                 url_specified,
                                 force, NULL, &call_rsp),
                    call_rsp,
                    "Attempt to update "
                    "UEFIROM firmware failed");
            if (rc_rsp >= 0 && func_install != install_from_local_source &&
                strcmp(call_rsp.returned_value, "0") != 0)
            {
                ERROR_MSG_PLAIN("InstallFromURI() returned %s when "
                                "trying to update UEFIROM firmware",
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

        if (sucfw)
        {
            CHECK_RESPONSE(
                    rc_rsp,
                    func_install(curl, namespace,
                                 svc_sucfw_inst,
                                 nic_inst, firmware_source,
                                 url_specified,
                                 force, NULL, &call_rsp),
                    call_rsp,
                    "Attempt to update "
                    "SUCFW firmware failed");
            if (rc_rsp >= 0 && func_install != install_from_local_source &&
                strcmp(call_rsp.returned_value, "0") != 0)
            {
                ERROR_MSG_PLAIN("InstallFromURI() returned %s when "
                                "trying to update SUCFW firmware",
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
    freeXmlCimInstance(svc_mcfw_inst);
    freeXmlCimInstance(svc_bootrom_inst);
    freeXmlCimInstance(svc_uefirom_inst);
    freeXmlCimInstance(svc_sucfw_inst);
    clear_response(&call_rsp);
    clear_response(&log_rsp);
    clear_response(&rec_rsp);
    free(recent_rec_id);
    return rc;
}

/**
 * Find available update for a given firmware installation
 * service and target NIC.
 *
 * @param curl                   CURL handle used to communicate with
 *                               CIM server
 * @param namespace              Our provider's namespace
 * @param svc_inst               SF_SoftwareInstallationService instance
 * @param nic_inst               SF_NICCard instance
 * @param firmware_source        Firmware image(s) source
 * @param url_specified          Is firmware_source an URL address
 * @param applicable_found [out] Whether applicable firmware image
 *                               for update was found
 * @param ver_a                  The first number of applicable image
 *                               version
 * @param ver_b                  The second number of applicable image
 *                               version
 * @param ver_c                  The third number of applicable image
 *                               version
 * @param ver_d                  The fourth number of applicable image
 *                               version
 *
 * @return 0 on success, -1 on failure
 */
static int
findAvailableUpdate(CURL *curl, const char *namespace,
                    xmlCimInstance *svc_inst,
                    xmlCimInstance *nic_inst,
                    const char *firmware_source,
                    int url_specified,
                    int *applicable_found,
                    int *ver_a, int *ver_b,
                    int *ver_c, int *ver_d,
                    char *req_img_name)
{
    response_descr      call_rsp;
    response_descr      call_rsp_aux;
    int                 rc_rsp = 0;
    int                 rc = 0;
    char               *svc_name = NULL;
    char               *img_name = NULL;
    const char         *subdir = NULL;
    char               *img_type_str = NULL;
    char               *img_subtype_str = NULL;
    unsigned int        img_type = 0;
    unsigned int        img_subtype = 0;
    efx_image_header_t *header = NULL;
    int                 i;
    efx_image_info_t    imageInfo;

    memset(&call_rsp, 0, sizeof(call_rsp));
    memset(&call_rsp_aux, 0, sizeof(call_rsp_aux));

    *applicable_found = 0;

    if (xmlCimInstGetPropTrim(svc_inst, "Name",
                              &svc_name, NULL) < 0 ||
        svc_name == NULL)
    {
        ERROR_MSG("Failed to get Name attribute "
                  "of SF_SoftwareInstallationService instance");
        return -1;
    }
    if (!strcmp(svc_name, SVC_SUCFW_NAME) == 0)
        CHECK_RESPONSE_EXT(
            rc_rsp,
            call_get_required_fw_image_name(curl, namespace,
                                            svc_inst, nic_inst,
                                            &call_rsp),
            call_rsp,
            "Failed to get required firmware image name");
    else {
         // for sucfw, Firmare type might not be supported on board
         // so call to get firmware image might fail
         // and we dont want to print any error message in that case
        rc = call_get_required_fw_image_name(curl, namespace,
                                        svc_inst, nic_inst,
                                        &call_rsp);
        if (rc < 0 || call_rsp.error_returned ||
            strcmp(call_rsp.returned_value, "0") != 0)
                rc_rsp = -1;
    }
    if (rc_rsp < 0)
    {
        rc = -1;
        goto cleanup;
    }

    img_name = get_named_value(call_rsp.out_params_list,
                               "Name");
    img_type_str = get_named_value(call_rsp.out_params_list,
                                   "Type");
    img_subtype_str = get_named_value(call_rsp.out_params_list,
                                      "Subtype");

    if (img_name == NULL || img_type_str == NULL ||
        img_subtype_str == NULL)
    {
        ERROR_MSG("Failed to get information about required "
                  "firmware image");
        rc = -1;
        goto cleanup;
    }

    img_type = strtol(img_type_str, NULL, 10);
    img_subtype = strtol(img_subtype_str, NULL, 10);
    subdir = getFwSubdirName(svc_name);
    if (subdir != NULL && strlen(img_name) != 0 &&
        req_img_name != NULL)
        snprintf(req_img_name, MAX_IMG_NAME_LEN,
                 "%s/%s", subdir, img_name);

    if (firmware_source != NULL &&
        url_specified &&
        strncasecmp(firmware_source, FILE_PROTO,
                    strlen(FILE_PROTO)) == 0 &&
        !fw_url_use_cim_transf)
    {
        char *applicable = NULL;
        char *version = NULL;

        CHECK_RESPONSE_EXT(
            rc_rsp,
            call_get_local_fw_image_version(curl, namespace,
                                            svc_inst, nic_inst,
                                            firmware_source,
                                            &call_rsp_aux),
            call_rsp_aux,
            "Failed to get local firmware image version");
        if (rc_rsp < 0)
        {
            rc = -1;
            goto cleanup;
        }
   
        applicable = get_named_value(call_rsp_aux.out_params_list,
                                     "ApplicableFound");
        version = get_named_value(call_rsp_aux.out_params_list,
                                  "Version");
        if (applicable == NULL || version == NULL)
        {
            ERROR_MSG("Failed to get information about local "
                      "firmware image");
            rc = -1;
            goto cleanup;
        }

        if (strcasecmp(applicable, "FALSE") == 0)
            *applicable_found = 0;
        else
        {
            *applicable_found = 1;
            sscanf(version, "%d.%d.%d.%d",
                   ver_a, ver_b, ver_c, ver_d);
        }
    }
    else
    {
        if (firmware_source == NULL)
        {
            for (i = 0; i < fw_images_count; i++)
            {
                rc = efx_check_reflash_image(firmware_images[i].data, firmware_images[i].size, &imageInfo);
                if (rc != 0 )
                {
                    ERROR_MSG("Error in checking firmware image");
                    continue;
                }
                header = imageInfo.eii_headerp;
                if (header->eih_type == img_type &&
                    header->eih_subtype == img_subtype)
                    break;
            }

            if (i >= fw_images_count)
                *applicable_found = 0;
            else
            {
                *applicable_found = 1;
                *ver_a = header->eih_code_version_a;
                *ver_b = header->eih_code_version_b;
                *ver_c = header->eih_code_version_c;
                *ver_d = header->eih_code_version_d;
            }
        }
        else
        {
            char    full_path[MAX_PATH_LEN];

            if (pathCompletion(firmware_source, svc_name,
                               url_specified, img_name,
                               full_path) < 0)
            {
                ERROR_MSG("Failed to complete path '%s'",
                          firmware_source);
                rc = -1;
                goto cleanup;
            }

            if (url_specified)
            {
                CURL           *curl_fw = NULL;
                CURLcode        curl_rc;

                curl_fw = curl_easy_init();
                if (curl_fw == NULL)
                {
                    ERROR_MSG_PLAIN("%s(): failed to initialize CURL",
                                    __FUNCTION__);
                    rc = -1;
                    goto cleanup;
                }

                curl_easy_setopt(curl_fw, CURLOPT_URL, full_path);
                curl_easy_setopt(curl_fw, CURLOPT_WRITEFUNCTION,
                                 curl_write);
                curl_easy_setopt(curl_fw, CURLOPT_WRITEDATA, NULL);
                curl_easy_setopt(curl_fw, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl_fw, CURLOPT_SSL_VERIFYHOST, 0L);

                free(curl_data);
                curl_data = NULL;
                curl_data_len = 0;

                curl_rc = curl_easy_perform(curl_fw);
                if (curl_rc != CURLE_OK)
                {
                    if (curl_rc == CURLE_TFTP_NOTFOUND ||
                        curl_rc == CURLE_REMOTE_FILE_NOT_FOUND)
                        *applicable_found = 0;
                    else
                    {
                        ERROR_MSG("curl_easy_perform() failed: %s",
                                  curl_easy_strerror(curl_rc));
                        rc = -1;
                    }
                    curl_easy_cleanup(curl_fw);
                    goto cleanup;
                }
                if (strncasecmp(full_path, HTTP_PROTO,
                                strlen(HTTP_PROTO)) == 0 ||
                    strncasecmp(full_path, HTTPS_PROTO,
                                strlen(HTTPS_PROTO)) == 0)
                {
                    long int       http_code = 0;

                    curl_rc = curl_easy_getinfo(curl_fw,
                                                CURLINFO_RESPONSE_CODE,
                                                &http_code);
                    if (curl_rc != CURLE_OK)
                    {
                        ERROR_MSG(
                            "curl_easy_getinfo(CURLINFO_RESPONSE_CODE)"
                            " failed: %s",
                            curl_easy_strerror(curl_rc));
                        rc = -1;
                        curl_easy_cleanup(curl_fw);
                        goto cleanup;
                    }
                    if (http_code >= 400)
                    {
                        if (http_code == 404)
                            *applicable_found = 0;
                        else
                        {
                            ERROR_MSG("Trying to obtain '%s' resulted "
                                      "in %ld HTTP status code",
                                      full_path, http_code);
                            rc = -1;
                        }
                        curl_easy_cleanup(curl_fw);
                        goto cleanup;
                    }

                }
                curl_easy_cleanup(curl_fw);

                if (downloaded_count >= downloaded_max_count)
                {
                    fw_img_descr    *imgs;
                    unsigned int     new_size;

                    new_size = (downloaded_max_count + 1) * 2;

                    imgs = realloc(downloaded_fw_imgs,
                                   new_size * sizeof(fw_img_descr));
                    if (imgs == NULL)
                    {
                        ERROR_MSG("Failed to reallocate array "
                                  "of downloaded images");
                        rc = -1;
                        goto cleanup;
                    }
                    downloaded_fw_imgs = imgs;
                    downloaded_max_count = new_size;
                }

                downloaded_fw_imgs[downloaded_count].data =
                                                   calloc(1, curl_data_len);
                if (downloaded_fw_imgs[downloaded_count].data == NULL)
                {
                    ERROR_MSG("Failed to allocate memory for downloaded "
                              "image");
                    rc = -1;
                    goto cleanup;
                }
     
                memcpy(downloaded_fw_imgs[downloaded_count].data,
                       curl_data, curl_data_len);
                downloaded_fw_imgs[downloaded_count].size = curl_data_len;

                rc = efx_check_reflash_image(
                       downloaded_fw_imgs[downloaded_count].data,
                       downloaded_fw_imgs[downloaded_count].size,
                       &imageInfo);
                if (rc != 0 )
                {
                    ERROR_MSG("Error in checking firmware image");
                    *applicable_found = 0;
                    free(downloaded_fw_imgs[downloaded_count].data);
                    downloaded_fw_imgs[downloaded_count].size = 0;
                    rc = -1;
                    goto cleanup;
                }
                header = imageInfo.eii_headerp;

                if (header->eih_type != img_type ||
                    header->eih_subtype != img_subtype)
                {
                    *applicable_found = 0;
                    free(downloaded_fw_imgs[downloaded_count].data);
                    downloaded_fw_imgs[downloaded_count].size = 0;
                }
                else
                {
                    *ver_a = header->eih_code_version_a;
                    *ver_b = header->eih_code_version_b;
                    *ver_c = header->eih_code_version_c;
                    *ver_d = header->eih_code_version_d;
                    *applicable_found = 1;

                    downloaded_count++;
                }
            }
            else
            {
                efx_image_header_t header_aux;
                if (getImageHeader(full_path, &header_aux) != 0)
                {
                    ERROR_MSG_PLAIN("%s: Error in reading firmware image", __FUNCTION__);
                    rc = -1;
                    *applicable_found = 0;
                    goto cleanup;
                }

                if (header_aux.eih_type != img_type ||
                    header_aux.eih_subtype != img_subtype)
                    *applicable_found = 0;
                else
                {
                    *applicable_found = 1;
                    *ver_a = header_aux.eih_code_version_a;
                    *ver_b = header_aux.eih_code_version_b;
                    *ver_c = header_aux.eih_code_version_c;
                    *ver_d = header_aux.eih_code_version_d;
                }
            }
        }
    }

cleanup:

    clear_response(&call_rsp_aux);
    clear_response(&call_rsp);
    free(svc_name);
    return rc;
}


/**
 * Read firmware image header from file
 *
 * @param fileName  Image File to read
 * @param imageHdr  Pointer to efx_image_header_t
 *
 * @return 0 on success, non-zero on failure
 */
int getImageHeader(char *fileName, efx_image_header_t *imageHdr)
{
       FILE *f;
       size_t fileSize = 0;
       char   *pBuffer = NULL;
       int rc = -1;
       efx_image_info_t imageInfo;
       f = fopen(fileName, "rb");
       if (f == NULL)
       {
           ERROR_MSG("%s: File open failed", __FUNCTION__);
           goto cleanup;
       }
       if (fseek(f, 0, SEEK_END) != 0)
       {
           ERROR_MSG("%s: Seeking file end failed", __FUNCTION__);
           goto cleanup;
       }
       if ((fileSize = ftell(f)) == -1)
       {
           ERROR_MSG("%s: Getting file size failed", __FUNCTION__);
           goto cleanup;
       }
       rewind(f);

       pBuffer = (char*) calloc(fileSize, sizeof(char));
       if (pBuffer == NULL)
       {
           ERROR_MSG("%s: Memory allocation failed", __FUNCTION__);
           goto cleanup;
       }
       if (fread(pBuffer, 1, fileSize, f) != fileSize)
       {
           ERROR_MSG("%s: Failed to read firmware image", __FUNCTION__);
           goto cleanup;
       }
       rc = efx_check_reflash_image(pBuffer, fileSize, &imageInfo);
       if (rc != 0 )
       {
           ERROR_MSG("%s: Error in reading firmware image", __FUNCTION__);
           goto cleanup;
       }
       memcpy(imageHdr, imageInfo.eii_headerp, sizeof(efx_image_header_t));

       rc = 0;
cleanup:
   if (f != NULL)
       fclose(f);
   if(pBuffer != NULL)
       free(pBuffer);
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
    xmlCimInstance *svc_mcfw_inst = NULL;
    xmlCimInstance *svc_bootrom_inst = NULL;
    xmlCimInstance *svc_uefirom_inst = NULL;
    xmlCimInstance *svc_sucfw_inst = NULL;

    int controller_applicable_found = 0;
    int controller_ver_a = 0;
    int controller_ver_b = 0;
    int controller_ver_c = 0;
    int controller_ver_d = 0;
    char controller_exp_img_name[MAX_IMG_NAME_LEN];
    int bootrom_applicable_found = 0;
    int bootrom_ver_a = 0;
    int bootrom_ver_b = 0;
    int bootrom_ver_c = 0;
    int bootrom_ver_d = 0;
    char bootrom_exp_img_name[MAX_IMG_NAME_LEN];
    int uefirom_applicable_found = 0;
    int uefirom_ver_a = 0;
    int uefirom_ver_b = 0;
    int uefirom_ver_c = 0;
    int uefirom_ver_d = 0;
    char uefirom_exp_img_name[MAX_IMG_NAME_LEN];
    int sucfw_applicable_found = 0;
    int sucfw_ver_a = 0;
    int sucfw_ver_b = 0;
    int sucfw_ver_c = 0;
    int sucfw_ver_d = 0;
    int sucfw_valid = 0;
    char sucfw_exp_img_name[MAX_IMG_NAME_LEN];
    int ver_check;

    char nic_tag_prev[MAX_NIC_TAG_LEN] = "";
    int  have_applicable_imgs = 0;
    int  have_applicable_bootrom = 0;
    int  have_applicable_controller = 0;
    int  have_applicable_uefirom = 0;
    int  have_applicable_sucfw = 0;

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

    printf("sfupdate_esxi version %s\n", tool_version);

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
        add_proto ? (use_https ? HTTPS_PROTO : HTTP_PROTO) : "",
        cim_address,
        add_port ? ":" : "",
        add_port ? (use_https ? DEF_HTTPS_PORT : DEF_HTTP_PORT) : "");

    if (!update_controller && !update_bootrom &&
        !update_uefirom && !update_sucfw)
    {
        update_controller = 1;
        update_bootrom = 1;
        update_uefirom = 1;
        update_sucfw = 1;
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

        if ((rc = getInstServices(curl, cim_namespace,
                                  &svc_mcfw_inst,
                                  &svc_bootrom_inst,
                                  &svc_uefirom_inst,
                                  &svc_sucfw_inst)) != 0)
            goto cleanup;

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
            char *nic_tag;

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

            if (xmlCimInstGetPropTrim(nic_inst, "Model",
                                      &nic_tag, NULL) < 0 ||
                nic_tag == NULL)
            {
                ERROR_MSG("Failed to get Tag property value "
                          "for SF_NICCard instance");
                rc = -1;
                goto cleanup;
            }

            if (strcmp(nic_tag, nic_tag_prev) != 0 &&
                !(fw_url != NULL && fw_url_no_local_access))
            {
                controller_exp_img_name[0] = '\0';
                if (svc_mcfw_inst != NULL)
                    findAvailableUpdate(curl, cim_namespace,
                                        svc_mcfw_inst,
                                        nic_inst,
                                        fw_url == NULL ? fw_path : fw_url,
                                        fw_url == NULL ? 0 : 1,
                                        &controller_applicable_found,
                                        &controller_ver_a,
                                        &controller_ver_b,
                                        &controller_ver_c,
                                        &controller_ver_d,
                                        controller_exp_img_name);

                bootrom_exp_img_name[0] = '\0';
                if (svc_bootrom_inst != NULL)
                    findAvailableUpdate(curl, cim_namespace,
                                        svc_bootrom_inst,
                                        nic_inst,
                                        fw_url == NULL ? fw_path : fw_url,
                                        fw_url == NULL ? 0 : 1,
                                        &bootrom_applicable_found,
                                        &bootrom_ver_a,
                                        &bootrom_ver_b,
                                        &bootrom_ver_c,
                                        &bootrom_ver_d,
                                        bootrom_exp_img_name);
                uefirom_exp_img_name[0] = '\0';
                if (svc_uefirom_inst != NULL)
                    findAvailableUpdate(curl, cim_namespace,
                                        svc_uefirom_inst,
                                        nic_inst,
                                        fw_url == NULL ? fw_path : fw_url,
                                        fw_url == NULL ? 0 : 1,
                                        &uefirom_applicable_found,
                                        &uefirom_ver_a,
                                        &uefirom_ver_b,
                                        &uefirom_ver_c,
                                        &uefirom_ver_d,
                                        uefirom_exp_img_name);
                sucfw_exp_img_name[0] = '\0';
                if (svc_sucfw_inst != NULL)
                    sucfw_valid = findAvailableUpdate(curl, cim_namespace,
                                        svc_sucfw_inst,
                                        nic_inst,
                                        fw_url == NULL ? fw_path : fw_url,
                                        fw_url == NULL ? 0 : 1,
                                        &sucfw_applicable_found,
                                        &sucfw_ver_a,
                                        &sucfw_ver_b,
                                        &sucfw_ver_c,
                                        &sucfw_ver_d,
                                        sucfw_exp_img_name);
            }

            snprintf(nic_tag_prev, MAX_NIC_TAG_LEN, "%s",
                     nic_tag);
            free(nic_tag);

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
                    if (!(fw_url != NULL && fw_url_no_local_access))
                    {
                        ver_check = checkVersion(version,
                                                 controller_ver_a,
                                                 controller_ver_b,
                                                 controller_ver_c,
                                                 controller_ver_d);
                        if (controller_applicable_found)
                            printf(
                                "    Available update: %d.%d.%d.%d%s\n",
                                controller_ver_a,
                                controller_ver_b,
                                controller_ver_c,
                                controller_ver_d,
                                ver_check == 0 && !force_update?
                                   " (won't be applied without --force)" :
                                   "");
                        else if (strlen(controller_exp_img_name) == 0)
                            printf("    No update available\n");
                        else
                            printf("    No update available "
                                   "(you may look for %s)\n",
                                   controller_exp_img_name);

                        if (controller_applicable_found &&
                            (ver_check == 1 || force_update))
                        {
                            have_applicable_imgs = 1;
                            have_applicable_controller = 1;
                        }
                    }
                }
                else if (strcmp(description, "NIC BootROM") == 0 &&
                         update_bootrom)
                {
                    printf("BootROM version: %s\n", version); 
                    if (!(fw_url != NULL && fw_url_no_local_access))
                    {
                        ver_check = checkVersion(version,
                                                 bootrom_ver_a,
                                                 bootrom_ver_b,
                                                 bootrom_ver_c,
                                                 bootrom_ver_d);
                        if (bootrom_applicable_found)
                            printf(
                                "    Available update: %d.%d.%d.%d%s\n",
                                bootrom_ver_a,
                                bootrom_ver_b,
                                bootrom_ver_c,
                                bootrom_ver_d,
                                ver_check == 0 && !force_update?
                                   " (won't be applied without --force)" :
                                   "");
                        else if (strlen(bootrom_exp_img_name) == 0)
                            printf("    No update available\n");
                        else
                            printf("    No update available "
                                   "(you may look for %s)\n",
                                   bootrom_exp_img_name);

                        if (bootrom_applicable_found &&
                            (ver_check == 1 || force_update))
                        {
                            have_applicable_imgs = 1;
                            have_applicable_bootrom = 1;
                        }
                    }
                }
                else if (strcmp(description, "NIC UEFIROM") == 0 &&
                         update_uefirom)
                {
                    printf("UEFIROM version: %s\n", version);
                    if (!(fw_url != NULL && fw_url_no_local_access))
                    {
                        ver_check = checkVersion(version,
                                                 uefirom_ver_a,
                                                 uefirom_ver_b,
                                                 uefirom_ver_c,
                                                 uefirom_ver_d);
                        if (uefirom_applicable_found)
                            printf(
                                "    Available update: %d.%d.%d.%d%s\n",
                                uefirom_ver_a,
                                uefirom_ver_b,
                                uefirom_ver_c,
                                uefirom_ver_d,
                                ver_check == 0 && !force_update?
                                   " (won't be applied without --force)" :
                                   "");
                        else if (strlen(uefirom_exp_img_name) == 0)
                            printf("    No update available\n");
                        else
                            printf("    No update available "
                                   "(you may look for %s)\n",
                                   uefirom_exp_img_name);

                        if (uefirom_applicable_found &&
                            (ver_check == 1 || force_update))
                        {
                            have_applicable_imgs = 1;
                            have_applicable_uefirom = 1;
                        }
                    }
                }
                else if (strcmp(description, "NIC SUCFW") == 0 &&
                         update_sucfw )
                {
                    if (sucfw_valid < 0 ) {
                        printf("SUCFW Not Applicable\n");
                        continue;
                    }

                    printf("SUCFW version: %s\n", version);
                    if (!(fw_url != NULL && fw_url_no_local_access))
                    {
                        ver_check = checkVersion(version,
                                                 sucfw_ver_a,
                                                 sucfw_ver_b,
                                                 sucfw_ver_c,
                                                 sucfw_ver_d);
                        if (sucfw_applicable_found)
                            printf(
                                "    Available update: %d.%d.%d.%d%s\n",
                                sucfw_ver_a,
                                sucfw_ver_b,
                                sucfw_ver_c,
                                sucfw_ver_d,
                                ver_check == 0 && !force_update?
                                   " (won't be applied without --force)" :
                                   "");
                        else if (strlen(sucfw_exp_img_name) == 0)
                            printf("    No update available\n");
                        else
                            printf("    No update available "
                                   "(you may look for %s)\n",
                                   sucfw_exp_img_name);

                        if (sucfw_applicable_found &&
                            (ver_check == 1 || force_update))
                        {
                            have_applicable_imgs = 1;
                            have_applicable_sucfw = 1;
                        }
                    }
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
            ERROR_MSG("\nNo interface named '%s' was found",
                      interface_name);
            rc = -1;
            goto cleanup;
        }

        if (fw_url != NULL && fw_url_no_local_access)
        /* If we could not examine firmware images,
         * we cannot know whether there was no available
         * ones or not */
            have_applicable_imgs = 1;
        else
        {
            update_controller = (update_controller &&
                                 have_applicable_controller);
            update_bootrom = (update_bootrom &&
                              have_applicable_bootrom);
            update_uefirom = (update_uefirom &&
                              have_applicable_uefirom);
            update_sucfw = (update_sucfw &&
                              have_applicable_sucfw);
        }

        if (!have_applicable_imgs)
            printf("\nThere is no firmware images which can be "
                   "used for update\n");
        if (do_update && have_applicable_imgs &&
            (update_controller || update_bootrom || update_uefirom || update_sucfw))
        {
            if (!yes)
            {
                char yes_str[4];
                char string[40];
                char *arr[] ={"SUCFW", "UEFIROM", "BOOTROM", "Controller"};

                int fw_bmap = update_controller<<3 | update_bootrom <<2
                              | update_uefirom <<1 | update_sucfw;
                int i = 3;
                string[0] = '\0';
                while (fw_bmap) {
                    if (fw_bmap & 1<<i) {
                        fw_bmap = fw_bmap ^ 1<<i;
                        if (!strlen(string))
                            strcat(string, arr[i]);
                        else {
                            if (fw_bmap > 0)
                                strcat(string, ", ");
                            else
                                strcat(string, " and ");
                            strcat(string, arr[i]);
                        }
                    }
                    i--;
                }

                printf("\nDo you want to update %s firmware on %s? [yes/no]",
                            string,
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
                                update_uefirom, update_sucfw,
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

    freeXmlCimInstance(svc_mcfw_inst);
    freeXmlCimInstance(svc_bootrom_inst);
    freeXmlCimInstance(svc_uefirom_inst);
    freeXmlCimInstance(svc_sucfw_inst);

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

    for (i = 0; i < downloaded_count; i++)
        free(downloaded_fw_imgs[i].data);
    free(downloaded_fw_imgs);

    return rc;
}
