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

/*
    Implements logging for CIMPLE.
    Logs can be generated by CIMPLE provider writers using the macros
    in log.h and also by the various adapters using the mechanisms in
    AddapterTracer.h.
    log.cpp writes all logs to a single messages file defined to be in
    a directory defined by an environment variable so that the user
    can place it where required at runtime.
    The log mechanism includes the capability to set maximum size on
    logs and then to either restart the log file or create backup files.
    The runtime configuration parameters for logging include:
       - max number of backup files
       - max log file size
       - log enable/disable
       - log severities to be logged
    The compile time options that affect logging include:
       - Enable adapter traces
       - Enabling the log macros
    The log file modifications are protected by a mutex for multiple threads
    and by a file lock so that multiple processes could write to the
    same log file (The latter is required in the case of pegasus
    out of process logs)
    If there are errors in the config file, the config file cannot be read,
    or the log file created, this code creates entries in an emergency
    log file in a fixed location (/tmp for linux and / for windows). Only
    these emergency LOG_ERR and LOG_DIAG messages go into this file.
    If the user is having problems with getting logs output for some reason
    there is a macro LOG_DIAG that generates output and that can be
    enabled by defining CIMPLE_LOG_DIAG_ENABLE (see below for the
    definition) within this file. Note that
    this is NOT part of the configuration and should be used only if there
    are real questions about log output.
 
    FUTURE TODO:
       - Enable logging by provider or provider module
       - Provide alternate outputs for logging
       - Possibly consider multiple log files by serverity or provider.
       - 
*/

#include <sys/stat.h>
#include <cctype>
#include <pthread.h>
#include "log.h"
#include "File_Lock.h"
#include "Buffer.h"
#include "Datetime.h"
#include "config.h"
#include "options.h"
#include "FileSystem.h"
#include "cimpleio.h"
#include <errno.h>

#ifdef CIMPLE_WINDOWS_MSVC
# include <windows.h>
# include <io.h>
# include <direct.h>
#endif

/***************************************************************************** 
** 
** log.cpp internal compile options
**
*****************************************************************************/ 
// Enable this define to turn on the LOG_DIAG macros. This macro generates
// emergency log outputs for the various steps of log output and should be
// used ONLY to debug issues where the user expects log output but there is
// none.  When enabled it generates entries to the emergency log file for
// many of the steps if reading the config file and initializing the log files.
// Uncommnet the following line to enable the diagnostics.
//#define CIMPLE_LOG_DIAG_ENABLE 1

// This define enables the capability to show log handling errors to an
// emergency log file in those cases where CIMPLE gets errors in the 
// processing of the config file or the generation of logs.  Normally there
// should be nothing in this file unless the log process itself has an
// error.  This also generates output to stdout so that it can be viewed if
// the provider is executing in the foreground.  Normally this define should
// be set.  In earlier versions of CIMPLE is was controlled by CIMPLE_DEBUG
// but with 2.0.22 is was separated from CIMPLE_DEBUG and controlled by the
// following define which is enabled
#define CIMPLE_SHOW_LOG_ERRORS_ENABLE 1

// Define the name for the directory that normally used by CIMPLE including
// for log output.  Also defines the prefix for the configuration file.
#define DEFAULT_CONFIG_FILE_NAME ".cimple"

CIMPLE_NAMESPACE_BEGIN

// Define Environment Variable defining location of CIMPLE_HOME, the
// location where CIMPLE stores and retrieves information. This is set
// from a definitiion in config.h but may be overridded by setters
// in cimple_config.
static String cimple_home_envvar = CIMPLEHOME_ENVVAR ;

// FUTURE static String cimple_home_dir = String::EMPTY;

// When true, macro log calls are executed.  When false, the macro
// calls return immediatly without calling log generation functions
// Further, the vlog function does not execute so even direct log calls
// return without generating logs.  Setting this variable false completely
// disable logging
boolean _log_enabled_state = true;

static File_Lock* _file_lock;
static FILE* _log_file_handle = 0;

// current log level set by info from the config file or the log_set_level
// function. Determines which log calls actually generate log entries.
// Note that the minimum level is FATAL
static Log_Level _level;

// Maximum size of log file.  0 Means ignore the test and is the default
static uint32 _max_log_file_size = 0;

// define the maximum number of backup files that may exist in this
// environment
#define MAXIMUM_NUMBER_BACKUP_FILES 9

// maximum number of backup log files. 0 Means ignore test and is default
// This variable is set by the .cimplerc config file or the CimpleConfig
// set function. It must never be larger than MAXIMUM_NUMBER_BACKUP_FILES
static uint32 _max_log_backup_files = 0;

// defines whether log output has been initialized
static bool _initialized;

// size for log file
static uint32 _current_log_file_size = 0;

// path to config path once started
static char config_path[200];

// full path name of log file including file name.
static char log_file_path[1024];

// log mutex
static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

static const char* _log_level_strings[] =
{
    "FATAL",
    "ERR", 
    "WARN",
    "INFO",
    "DBG",
};

static const size_t _num_strings =
    sizeof(_log_level_strings) / sizeof(_log_level_strings[0]);

//**************************************************************************
// Internal function to output errors  and diagnosticsin Log file handling.
// This should only be called by functions in log.cpp and only through the
// LOG_ERR and LOG_DIAG macros.
#if !defined CIMPLE_SHOW_LOG_ERRORS_ENABLE || !defined CIMPLE_LOG_DIAG_ENABLE

    //CIMPLE_PRINTF_ATTR(3, 4) // generates warning flag
static void  _log_err_output(
    const char* ErrName,
    const char* file,
    size_t line, 
    const char* fmt,
    va_list ap)
{
    Buffer buffer;
    char datetime[Datetime::BUFFER_SIZE];
    {
        Datetime dt = Datetime::now();
        dt.ascii(datetime, true);
        char* dot = strchr(datetime, '.');

        if (dot)
            *dot = '\0';
    }
    buffer.format("%s:%s: %s(%u) : ", datetime, ErrName,
        file, (uint32)line);

    buffer.vformat(fmt, ap);

    if (buffer[buffer.size()-1] != '\n')
        buffer.append('\n');

    printf("%s",buffer.data());

    // Write the data to the emergency log buffer if can open file

#ifdef CIMPLE_WINDOWS_MSVC
    char emer_file_path[1024];
    char * sysroot = getenv("SystemRoot");
    strlcpy(emer_file_path, sysroot, sizeof(emer_file_path));
    strlcat(emer_file_path, "\\Temp\\cimple_log_file_err_messages.log",
        sizeof(emer_file_path));
#else
    const char* emer_file_path = "/tmp/cimple_log_err_messages";
#endif

    FILE * err_file_handle = fopen(emer_file_path, "a");
    if (err_file_handle)
    {
        fwrite(buffer.data(), buffer.size(), 1, err_file_handle);
        fclose(err_file_handle);
    }
}
#endif

//*************************************************************
// The LOG_DIAG function is normally disabled and should be used only
// in case you are having real issues debugging CIMPLE logging (ex.
// do not get any logs).
// Defines a macro (LOG_DIAG) to generate output for diagnosing issues
// in handling of the log files by log.cpp.  This is special
// because it displays operation of the logging functions themselves
// so that log output cannot be used.
// It is provided so that it can be optionally compiled if there
// are issues with the operation of logging.
// It outputs a printf to stdout so that info is only visible
// when provider operating in forground.
// THIS IS USED ONLY BY THE log.cpp module to generate info about
// what is happening in log.cpp
// ex.
// LOG_DIAG(("Cannot create log file %s\n", file_name));
// DO NOT USE THIS FOR ANYTHING EXCEPT TESTING OF ISSUES IN log.cpp
// as it generates lots of output.
//***************************************************************

#ifdef CIMPLE_LOG_DIAG_ENABLE

// struct define frame that contains output info and
// invoke method to execute for LOG_DIAG
struct Log_Diag_Call_Frame
{
    const char* file;
    size_t line;

    // holds line number of call
    Log_Diag_Call_Frame(const char* file_, size_t line_) : 
        file(file_), line(line_) { }
    
    //CIMPLE_PRINTF_ATTR(1, 2) generates error on linux
    inline void invoke(const char* format, ...) 
    {
        va_list ap;
        va_start(ap, format);
        _log_err_output("LOG DIAGNOSTIC",  file, line, format, ap);
        va_end(ap);
    }
};

// define the macro that implements LOG_DIAG behavior
# define LOG_DIAG(ARGS) \
    do \
    { \
        Log_Diag_Call_Frame frame(__FILE__,__LINE__); \
        frame.invoke ARGS ; \
    } \
    while (0)
#else    // CIMPLE_LOG_DIAG NOT Enabled
# define LOG_DIAG(ARGS)
#endif

//*************************************************************
// Define a macro to generate output for errors in creation and 
// handling of the log files.  This is special because the normal
// output is the log and if there are issues with the log there
// is nowhere to output the information.  Only active in debug
// mode and outputs a simple printf so that info is only visible
// when provider operating in forground.
// THIS IS USED ONLY BY THE log.cpp module to generate info about
// errors in log file config, initialization, and handling.
// ex.
// LOG_ERR(("Cannot create log file %s\n", name));
//***************************************************************

#ifdef CIMPLE_SHOW_LOG_ERRORS_ENABLE

// struct define frame that contains output info and
// invoke method to execute for LOG_ERR
struct Log_Err_Call_Frame
{
    const char* file;
    size_t line;

    // holds line number of call
    Log_Err_Call_Frame(const char* file_, size_t line_) : 
        file(file_), line(line_) { }
    
    //CIMPLE_PRINTF_ATTR(1, 2) generates error on linux
    inline void invoke(const char* format, ...) 
    {
        va_list ap;
        va_start(ap, format);
        _log_err_output("LOG FILE ERROR", file, line, format, ap);
        va_end(ap);
    }
};

// define the macro that executes LOG_ERR behavior
# define LOG_ERR(ARGS) \
    do \
    { \
        Log_Err_Call_Frame frame(__FILE__,__LINE__); \
        frame.invoke ARGS ; \
    } \
    while (0)
#else           // CIMPLE_DEBUG not enabled.
# define LOG_ERR(ARGS)
#endif


//*************************************************************
//
// Handlers for reading the configuration file
// 
// ************************************************************
// TODO: This function forces open, and full read of the config file for
// each parameter.  Change to common function so that we only read file 
// once.
static char* _get_opt_value(const char* path, const char* opt)
{
    size_t opt_len = strlen(opt);

    // Open file.

    FILE* is = fopen(path, "r");

    if (!is)
    {
        LOG_ERR(("Cannot Open config file %s", path));
        return 0;
    }

    LOG_DIAG(("Config file %s opened for option %s", path, opt));

    // Scan file looking for option.

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), is) != 0)
    {
        char* p = buffer;
        // trim leading whitespace
        while (*p && isspace(*p))
            p++;

        // skip lines starting with comment character including
        // after leading whitespace
        if (buffer[0] == '#')
            continue;

        // Trim trailing whitespace.
        while (*p)
            p++;

        while (p != buffer && isspace(p[-1]))
            *--p = '\0';

        // Skip blank lines.

        if (buffer[0] == '\0')
            continue;

        // Check for name defined by opt.

        if (strncmp(buffer, opt, opt_len) == 0)
        {
            const char* p = buffer + opt_len;

            while (isspace(*p))
                p++;

            if (*p == '=')
            {
                p++;

                while (isspace(*p))
                    p++;

                fclose(is);
                return strdup(p);
            }
        }
    }
    fclose(is);

    return 0;
}

// Validate the text provided against the possible log level
// string definitions.  Return 0 if valid with the correct enum
// in level or -1 if no match.
// Does a no case match against the level definition strings.
// Returns 0 if txt matches one of the defined log_level strings and
// also the level.
// @return 0 if OK else -1.

static int _validate_log_level(const char * txt, Log_Level& level)
{
    for (size_t i = 0; i < _num_strings; i++)
    {
        if (strcasecmp(_log_level_strings[i], txt) == 0)
        {
            level = Log_Level(i);
            LOG_DIAG(("LOG_LEVEL set to %u %s ",
                level, _log_level_strings[i]));
            return 0;
        }
    }
    level = LL_DBG;
    LOG_ERR(("LOG_LEVEL param \"%s\"invalid. Set to %s.",
             txt, _log_level_strings[level]));
    return -1;   // return invalid
}

// get the log level string definition from the
// config file and if it is valid return 0.  If it is invalid
// return -1. If variable not in file, return 1;
static int _get_log_level_param(const char* path, Log_Level& level)
{
    char* value_str = _get_opt_value(path, "LOG_LEVEL");
    
    // return. variable not in file status
    if (!value_str)
    {
        free(value_str);
        return 1;
    }

    int rtn = _validate_log_level(value_str, level);

    return rtn;
}

// Get a Uint32 value corresponding to the option name "name".  If the
// name is not found return 1.  If error in conversion return -1.
// Good value return 0. 
// @param path - Path to config file
// @param name - Name of parameter
// @param value - Return value and also default if not found.
// @param maxValue - Maximum allowable value for this parameter.
// TODO: Consider possible minimum value also.
static int _get_log_opt_uint32(const char* path,
                               const char * name,
                               uint32& value,
                               uint32 maxValue)
{
    char * value_str = _get_opt_value(path, name);
    if (!value_str)
    {
        LOG_DIAG(("Config Param %s not found. Default %u",
            name, value));
        return 1;
    }
    sint64 temp;
    char* end;

    temp = strtol(value_str, &end, 0);

    if (temp < 0 || (temp > CIMPLE_UINT32_MAX) || (*end != '\0'))
    {
        LOG_ERR(("Config error: %s value %s invalid", name, value_str));
        free(value_str);
        return -1;
    }
    if (temp > maxValue)
    {
        LOG_ERR(("Config error: %s value %lu exceeds max %u",
                  name, temp, maxValue));
        free(value_str);
        return -1;
    }
    value = (uint32)temp;
    LOG_DIAG(("Config set %s to value %u", name, value));
    free(value_str);
    return 0;
}

// Get a boolean option name=value pair. value must be "true" or "false"
// @param path - config file path
// @param name - name of parameter
// @param return value and also default since this function sets the
// value 
static int _get_log_opt_bool(const char* path,
                             const char * name,
                             bool& value)
{
    char * value_str = _get_opt_value(path, name);
    if (!value_str)
    {
        LOG_DIAG(("Config Param %s not found. default %s",
                 name, (value? "true" : "false")));
        free(value_str);
        return 1;
    }
    if (strcasecmp(value_str, "TRUE") == 0)
    {
        value = true;
    }
    else if (strcasecmp(value_str, "FALSE") == 0)
    {
        value = false;
    }
    else
    {
        LOG_ERR(("Config error: %s param value %s invalid",
            name, value_str));
        free(value_str);
        return -1;
    }

    free(value_str);
    LOG_DIAG(("Set boolean %s %s", name,
        (value? "true" : "false")));
    return 0;
}

// get the new value for the max_log_file_size parameter. Any valid Uint32
// is legal for this parameter
static int _get_log_maxLogFileSize_param(const char* path, uint32& rtnValue)
{
    return(_get_log_opt_uint32(path, "MAX_LOG_FILE_SIZE", rtnValue,
        CIMPLE_UINT32_MAX));
}

static int _get_log_maxLogBackupFiles_param(const char* path, uint32& rtnValue)
{
    return(_get_log_opt_uint32(path, "MAX_LOG_BACKUP_FILES", rtnValue,
        MAXIMUM_NUMBER_BACKUP_FILES));
}

static int _get_log_enable_param(const char* path, bool& rtnValue)
{
    return(_get_log_opt_bool(path, "ENABLE_LOGGING", rtnValue));
}

// Read config file for each defined name=value pair.  Any error aborts
// the read. 
// return 0 - good read. 1 values not found, -1 Error but continue
// -2 return permanent error, get out.
int _read_config(char * conf_path)
{
    LOG_DIAG(("Enter _read_config"));
    if (_get_log_level_param(conf_path, _level) == -1)
        return -1;

    if (_get_log_maxLogFileSize_param(conf_path, _max_log_file_size) == -1)
        return -1;

    if (_get_log_maxLogBackupFiles_param(conf_path, _max_log_backup_files) == -1)
        return -1;

    if( _get_log_enable_param(conf_path, _log_enabled_state) == -1)
        return -1;

    LOG_DIAG(("Exit _read_config success"));
    return 0;
}

//*********************************************************************
//
//  Log initialization and roll over functions
//
//********************************************************************

// Initialize the log files using status from the config file if
// it exists. The name of the config file is defined by the
// name variable on input.
static void _initialize(const char* name)
{
    if (_initialized)
    {
        return;
    }
    LOG_DIAG(("Enter log config _initialize() for config file %s", name));
    // Get home path for CIMPLE_HOME.
    // Defined originally in options as define and 
    // as String in this file.
   
    const char* home = getenv(cimple_home_envvar.c_str());

    if (!home)
    {
        LOG_ERR(("No CIMPLE_HOME env var defined. Looking for %s",
            cimple_home_envvar.c_str()));
        return;
    }
    LOG_DIAG(("CIMPLE home %s from env var %s",
        home,cimple_home_envvar.c_str()));

    // Read parameters from .<name>rc configuration file.
    char conf_path[1024];

    sprintf(conf_path, "%s/%src", home, name);

    // WARNING: errors here are not really caught since the only way
    // to display is the log and these represent a log error.
    if (_read_config(conf_path) == -2)
    {
        LOG_ERR(("Config file input error. %s", conf_path));
        return;
    }

    // Create $HOME/.cimple directory.

    char root_path[1024];
    {
        sprintf(root_path, "%s/%s", home, name);
#if defined(CIMPLE_WINDOWS_MSVC) 
        _mkdir(root_path);
#else
        mkdir(root_path, 0777);
#endif
    }

    // Form path to log file:

    {
        strlcpy(log_file_path, root_path, sizeof(log_file_path));
        strlcat(log_file_path, "/messages", sizeof(log_file_path));
    }
    LOG_DIAG(("Log file at %s",log_file_path));

    // Form path to lock file:

    char lock_file_path[1024];
    {
        strlcpy(lock_file_path, root_path, sizeof(log_file_path));
        strlcat(lock_file_path, "/messages.lock", sizeof(lock_file_path));
    }

    // Create file lock object if it does not already exist

    if (!_file_lock)
    {
        _file_lock = new File_Lock(lock_file_path);
    }
    
    // Fail if cannot create lock file.  This is the out if everything
    // fails. And means we do not initialize at all.  Also means we
    // keep trying to initialize.
    // TODO: This should probably mark logging disabled and get out
    // after setting initialize. Otherwise we keep retrying.
    if (!_file_lock->okay())
    {
      delete _file_lock;
      _file_lock = 0;
      LOG_ERR(("Log file lock create error %s", lock_file_path));
      return;
    }

    LOG_DIAG(("Log lock ok file at %s", lock_file_path));

    // initialize _log_file_size by getting size of file and putting
    // it into the variable
    // TODO why are we both setting to zero anc getting from FILE??
    _current_log_file_size = 0;
    FileSystem::get_size(log_file_path, _current_log_file_size);

    LOG_DIAG(("Log file size %u",_current_log_file_size));

    // Open log file for append.
    // The handle should always be zero here or we are reopening 
    // the file which messes up close/rename for windows systems.
    CIMPLE_ASSERT(_log_file_handle == 0);
    _log_file_handle = fopen(log_file_path, "a");

    // If cannot create log file, 
    if (!_log_file_handle)
    {
        delete _file_lock;
        _file_lock = 0;
        LOG_ERR(("Log file open error %s", log_file_path));
        return;
    }
    _initialized = true;
    LOG_DIAG(("log file initialized"));
}

// create a file path name from _log_file_path with the suffix .<number>
String _create_backup_file_name(uint32 number)
{
    String rtn_file_name = string_printf("%s.%u", log_file_path, number);
    return rtn_file_name;
}

bool _delete_log_backup_file(uint32 number)
{
    String delete_file_name = _create_backup_file_name(number);

    if (FileSystem::exists(delete_file_name.c_str()))
    {
        if (!FileSystem::delete_file(delete_file_name.c_str()))
        {
            LOG_ERR(("log backup file %s not deleted\n",
                    delete_file_name.c_str()));
            return false;
        }
    }
    return true;
}
void _log_file_close()
{
    if (!(fclose(_log_file_handle) == 0))
    {
        LOG_ERR(("log file close failed. Error = %s\n",
             strerror(errno)));
    }
    _log_file_handle = 0;
}

// Close log file and delete all current log files.
bool _delete_all_log_files()
{
    LOG_DIAG(("Delete all log files"));
    pthread_mutex_lock(&_mutex);
    bool rtn = true;
    // lock file to prevent other process from touching
    // while we delete.
    if (_file_lock)
    {
        _file_lock->lock();
    }
    // Delete the current file (messages) if it exists
    if (FileSystem::exists(log_file_path))
    {
        _log_file_close();
        if(!FileSystem::delete_file(log_file_path))
        {
            LOG_ERR(("Log file %s not deleted\n", log_file_path));
            pthread_mutex_unlock(&_mutex);
            return false;
        }
    }
    // Delete any backup files up to the max allowed
    for (uint32 i = 1; i < MAXIMUM_NUMBER_BACKUP_FILES; i++)
    {
        String fName = _create_backup_file_name(i);
        if (FileSystem::exists(fName.c_str()))
        {
            if (!_delete_log_backup_file(i))
                rtn = false;
        }
    }
    if (_file_lock)
    {
        _file_lock->unlock();
    }
    pthread_mutex_unlock(&_mutex);
    return rtn;
}

// Rename a backup file from filename.n to filename.(n+1)
// TODO - There should be a cheaper way where we only create a filename
// one time.
bool _rename_log_backup_file(uint32 number)
{
    String from = _create_backup_file_name(number);
    String to = _create_backup_file_name((number+1));

    // NOTE: No error flagged if the from file does not exist.
    if (FileSystem::exists(from.c_str()))
    {
        if (!FileSystem::rename_file(from.c_str(), to.c_str()))
        {
            LOG_ERR(("ERROR: log file rename failed from %s to %s\n",
                   from.c_str(), to.c_str()));
            return false;
        }
    }
    return true;
}
// Called when the log file reaches a predefined length.  Closes the
// file, renames it to *.1, renames all older files, and opens a
// new log file. This function called from vlog and operates under
// the protection of that mutex and filelock. 
void _rollover_log_file()
{
    LOG_DIAG(("Roll over log file. current size = %u",
        _current_log_file_size));
    // close the existing log file
    _log_file_close();

    // rename any existing old log files
    if(_max_log_backup_files != 0)
    {
        // delete the oldest log file if it exists.
        _delete_log_backup_file(_max_log_backup_files);

        // rename all log backup files from name.n to name.n+1
        for (uint32 i = (_max_log_backup_files - 1) ; i > 0 ; i--)
        {
            if (!_rename_log_backup_file(i))
            {
                LOG_ERR(("Renaming log file %s\n", 
                       _create_backup_file_name(i).c_str()));
            }
        }

        // rename current file to filename.1
        String newFileName = _create_backup_file_name(1);

        if(!FileSystem::rename_file(log_file_path, newFileName.c_str()))
        {
           LOG_ERR(("Renaming log file %s to %s. error = %s\n",
                   log_file_path, newFileName.c_str(), strerror(errno)));
        }
    }
    else
        // If maximum log backups == 0, just delete file.
        FileSystem::delete_file(log_file_path);

    // reinitialize log file
    // KS_TODO - need to break up initialize so we just do the essential
    // open log parts here.
    _initialized = false;
    _initialize(DEFAULT_CONFIG_FILE_NAME);
}

// Commented out because not used.
//void open_log(const char* name)
//{
//    // Initialize on the first call.
//
//    if (!_initialized)
//    {
//        _initialize(name);
//    }
//}

//******************************************************************
// 
// log output functions log and vlog).  These should not be used directly
// by CIMPLE providers. Use the macros defined in log.h
// 
//******************************************************************
// This function is the interface to the log output.  It is mutexed and
// both formats the log information and outputs it to the log file.
void vlog(
    Log_Level level, 
    const char* file, 
    size_t line, 
    const char* fmt,
    va_list ap)
{
    pthread_mutex_lock(&_mutex);

    // Initialize from the .cimplerc config file on the first call.

    if (!_initialized)
    {
        _initialize(DEFAULT_CONFIG_FILE_NAME);
    }

    // This is the final check if there are problems with log output
    // enable this and there should be an entry in the emergency log output
    // for every log call. Enable with caution and only to confirm that
    // logs are being called.
#ifdef CIMPLE_LOG_DIAG_ENABLE
    //LOG_DIAG(("vlog called to output log file %s line %u", file, line));
#endif

    // Bail out if initialize failed. This is a permanent test so that
    // all logging is disabled if we cannot initialize the file.

    if (!_log_file_handle)
    {
        pthread_mutex_unlock(&_mutex);
        return;
    }

    // If _log_enabled_state false, disable all logging or
    // log level for this log higher than currently enabled level
    if (!_log_enabled_state  || (level > _level))
    {
        pthread_mutex_unlock(&_mutex);
        return;
    }

    // Format time string:

    char datetime[Datetime::BUFFER_SIZE];
    {
        Datetime dt = Datetime::now();
        dt.ascii(datetime, true);
        char* dot = strchr(datetime, '.');

        if (dot)
            *dot = '\0';
    }

    // Format message prefix:

    Buffer buffer;
    buffer.format("%s %s: %s(%d): ", datetime, _log_level_strings[int(level)], 
        file, (uint32)line);

    // Format message body:

    buffer.vformat(fmt, ap);

    if (buffer[buffer.size()-1] != '\n')
        buffer.append('\n');

    // Test for log file size limits and if exceeded, roll 
    // current file over, to create backup log files
    // If max log file size == 0 ignore the max.

    if ((_max_log_file_size != 0) && 
        (_current_log_file_size > _max_log_file_size))
    {
        // Role the log file to backup file within a file lock.
        _file_lock->lock();
        _rollover_log_file();
        _file_lock->unlock();
    }

    // track log file size by accumulating from buffer
    _current_log_file_size += buffer.size();

    // Write to log file within file lock
    _file_lock->lock();
    fwrite(buffer.data(), buffer.size(), 1, _log_file_handle);
    fflush(_log_file_handle);
    _file_lock->unlock();

    pthread_mutex_unlock(&_mutex);
}

void log(Log_Level level, const char* file, size_t line, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vlog(level, file, line, fmt, ap);
    va_end(ap);
}

//************************************************************************
//
//  Functions to set/get log configuration information.  These should
//  only be used internally by CIMPLE (See CimpleConfig.h for functions
//  to be used by the provider writer.
//
//************************************************************************

// sets the log level based on the input string provided.
// returns 0 if successful or -1 if the string is invalid.
bool log_set_level(String& level)
{
    // try to set the new state and return result
    return(_validate_log_level(level.c_str(), _level));
}

CIMPLE_CIMPLE_LINKAGE
bool log_set_level(Log_Level level)
{
    _level = level;
    return 0;
}

CIMPLE_CIMPLE_LINKAGE
Log_Level log_get_level()
{
    return _level;
}

CIMPLE_CIMPLE_LINKAGE
const char * log_get_level_string()
{
    return _log_level_strings[_level];
}

uint32 log_get_maxFileSize()
{
    return _max_log_file_size;
}

bool log_set_maxlogFileSize(uint32 new_size)
{
    _max_log_file_size = new_size;
    return true;
}

uint32 log_get_maxLogBackupFiles()
{
    return _max_log_backup_files;
}

bool log_set_maxlogBackupFiles(uint32 new_size)
{
    if (new_size <= MAXIMUM_NUMBER_BACKUP_FILES)
    {
        _max_log_backup_files = new_size;
        return true;
    }
    else
        return false;
}

bool log_enable(bool x)
{
    bool rtn = _log_enabled_state;
    _log_enabled_state = x;
    return rtn;
}
bool get_log_enable_state()
{
    return _log_enabled_state;
}

CIMPLE_CIMPLE_LINKAGE
int set_cimple_home_envvar(const char* env_var)
{
    cimple_home_envvar = env_var;
    return 0;
}

const char* get_cimple_home_envvar()
{
    return cimple_home_envvar.c_str();
}

CIMPLE_CIMPLE_LINKAGE
void log_reinitialize()
{
    pthread_mutex_lock(&_mutex);
    if (_file_lock)
    {
        _file_lock->lock();
    }
    if (_log_file_handle)
    {
        _log_file_close();
    }
    _initialized = false;
    _initialize(DEFAULT_CONFIG_FILE_NAME);
    _file_lock->unlock();
    pthread_mutex_unlock(&_mutex);
}

CIMPLE_CIMPLE_LINKAGE
bool log_remove_all_logfiles()
{
    return _delete_all_log_files();
}

bool read_config_file()
{
    // To reread the config file we force complete
    // reinitialization of the log.
    log_reinitialize();
    return true;
}

CIMPLE_NAMESPACE_END
