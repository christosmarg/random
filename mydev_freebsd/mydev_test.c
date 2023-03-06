#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mydev.h"

int
main(int argc, char *argv[])
{
	bar_t bar;
	char buf[BUFSIZ];
	int fd;

	if ((fd = open("/dev/mydev", O_RDWR)) < 0)
		err(1, "open(/dev/mydev)");

	if (ioctl(fd, MYDEVIOC_READ, &bar) != 0)
		err(1, "ioctl(MYDEVIOC_READ)");
	printf("%s: ioctl(MYDEVIOC_READ)\t-> x=%d, y=%d\n",
	    getprogname(), bar.x, bar.y);

	printf("%s: ioctl(MYDEVIOC_WRITE)\t-> ", getprogname());
	fflush(stdout);

	bar.x = 10;
	bar.y = 20;
	if (ioctl(fd, MYDEVIOC_WRITE, &bar) != 0)
		err(1, "ioctl(MYDEVIOC_WRITE)");

	if (ioctl(fd, MYDEVIOC_RDWR, &bar) != 0)
		err(1, "ioctl(MYDEVIOC_RDWR)");
	printf("%s: ioctl(MYDEVIOC_RDWR)\t-> x=%d, y=%d\n",
	    getprogname(), bar.x, bar.y);

	(void)strlcpy(buf, "hello from test program", sizeof(buf));

	printf("%s: write()\t\t\t-> %s\n", getprogname(), buf);
	fflush(stdout);

	if (write(fd, buf, sizeof(buf)) < 0)
		err(1, "write");
	if (read(fd, buf, sizeof(buf)) < 0)
		err(1, "read");
	printf("%s: read()\t\t\t-> %s\n", getprogname(), buf);

	close(fd);

	return (0);
}
