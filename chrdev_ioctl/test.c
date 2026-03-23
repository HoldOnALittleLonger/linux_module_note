#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/types.h>

int main(int argc, char *argv[])
{
        if (argc < 2) {
                fprintf(stderr, "usage: @this-program <path-to-chrdev-file>\n");
                return -1;
        }

        char buffer[8] = {0};
        
        int cdev_fd = open(argv[1], O_RDONLY);
        if (cdev_fd < 0) {
                fprintf(stderr, "open device file was failed.\n");
                return -1;
        }

        ssize_t ret = read(cdev_fd, buffer, 8);
        if (ret <= 0) {
                fprintf(stderr, "read device file was failed.\n");
                ret = -1;
                goto exit_close;
        }

        __u32 ioctl_number = strtoul(buffer, NULL, 10);
        if (ioctl(cdev_fd, ioctl_number) < 0) {
                fprintf(stderr, "ioctl() failed with number %u .\n", ioctl_number);
                ret = -1;
                goto exit_close;
        }

        fprintf(stdout, "ioctl() succeed.\n");

exit_close:
        close(cdev_fd);
        return 0;
}
