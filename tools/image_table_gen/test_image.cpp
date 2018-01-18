#include <cstdlib>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "unistd.h"

#include "image.h"

const char *type2str(int type)
{
    switch (type)
    {
        case IMAGE_TYPE_BOOTROM:
            return "FIRMWARE_BOOTROM";
        case IMAGE_TYPE_MCFW:
            return "FIRMWARE_MCFW";
        case IMAGE_TYPE_UEFIROM:
            return "FIRMWARE_UEFIROM";
        default:
            return "UNKNOWN";
    }
}

int main(int argc, const char *argv[])
{
    int fd;
    int rc;

    image_header_t  header;

    if (argc < 2)
        return 1;

    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
        return 1;

    rc = read(fd, &header, sizeof(header));
    if (rc != sizeof(header))
        return 1;

    printf("%s.%u", type2str(header.ih_type), header.ih_subtype);

    close(fd);

    return 0;
}
