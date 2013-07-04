// The embedded instance functionality is enabled in config.options
#define CIMPLE_ENABLE_EMBEDDED_INSTANCES


// Define the name of the env var used for runtime home
#ifndef __WIN32__
#define CIMPLEHOME_ENVVAR "CIMPLE_HOME"
#else
#define CIMPLEHOME_ENVVAR "SystemRoot"
#endif
// END_OF_FILE
