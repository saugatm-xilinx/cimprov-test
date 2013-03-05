#define CIMPLE_NO_VERSION_SYMBOL
#include <cimple/config.h>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <getopt.h>
#include <util.h>

const char USAGE[] = "\
Usage: %s path [-n array-name]\n\
\n\
This utility converts any file into a character array definition so that\n\
it can be embedded in a C program. This utility allows programs to eliminate\n\
dependencies on external static files. The character array is written to\n\
standard output.\n\
\n";

int main(int argc, char** argv)
{
    const char* arg0 = argv[0];

    // Get args:

    int opt;
    const char* array_name = "array";

    while ((opt = getopt(argc, argv, "n:h")) != -1)
    {
        switch (opt)
        {
            case 'n':
            {
                if (!optarg)
                {
                    fprintf(stderr, "missing argument for -n option\n");
                    exit(1);
                }

                array_name = optarg;
                break;
            }

            case 'h':
                fprintf(stderr, USAGE, arg0);
                exit(1);

            default:
                fprintf(stderr, "Unknown option: -%c\n", opt);
                exit(1);
        }
    }

    // Array name must be a valid C identifier.

    if (!is_c_ident(array_name))
    {
        fprintf(stderr, "Not a valid C array name: %s\n", array_name);
        exit(1);
    }

    // Check usage.

    if (optind + 1 != argc)
    {
        fprintf(stderr, USAGE, arg0);
        exit(1);
    }

    // Open file.

    const char* path = argv[optind];
    FILE* is = fopen(path, "rb");

    if (!is)
    {
        fprintf(stderr, "%s: cannot open %s\n", arg0, path);
        exit(1);
    }

    // Write out C array:

    printf("/* Generated by file2c from \"%s\" */\n", path);
    printf("const unsigned char %s[] = \n", array_name);
    printf("{\n");

    size_t n;
    unsigned char buffer[15];

    while ((n = fread(buffer, 1, sizeof(buffer), is)) > 0)
    {
        printf("    ");

        for (size_t i = 0; i < n; i++)
            printf("0x%02X,", buffer[i]);

        printf("\n");
    }

    printf("    0x00, /* null terminator */\n");

    printf("};\n");
    printf("\n");

    return 0;
}

