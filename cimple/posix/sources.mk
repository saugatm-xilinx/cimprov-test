libcimpleposix_PURPOSE = target
libcimpleposix_SOURCES = \
    dirent.cpp \
    dlfcn.cpp  \
    getopt.cpp \
    unistd.cpp \
    libgen.cpp \
    pthread.cpp \
    time.cpp

libcimpleposix_DIR = cimple/posix

libcimpleposix_TARGET = libcimpleposix.a

libcimpleposix_INCLUDES = $(libcimpleposix_DIR)
libcimpleposix_CPPFLAGS = $(libcimpleposix_PROVIDE_CPPFLAGS) -DPOSIX_INTERNAL

$(eval $(call component,libcimpleposix,STATIC_LIBRARIES))


