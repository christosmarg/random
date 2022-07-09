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
	foo_t foo;
	char buf[32];
	int dev;

	if ((dev = open("/dev/mydev", O_RDWR)) < 0)
		err(1, "open(/dev/mydev)");

	if (ioctl(dev, MYDEVIOC_READ, &foo) != 0)
		err(1, "ioctl(MYDEVIOC_READ)");
	printf("%s: x=%d, y=%d\n", getprogname(), foo.x, foo.y);

	foo.x = 1;
	foo.y = 2;
	if (ioctl(dev, MYDEVIOC_WRITE, &foo) != 0)
		err(1, "ioctl(MYDEVIOC_WRITE)");

	if (ioctl(dev, MYDEVIOC_RDWR, &foo) != 0)
		err(1, "ioctl(MYDEVIOC_RDWR)");
	printf("%s: x=%d, y=%d\n", getprogname(), foo.x, foo.y);

	if (read(dev, buf, sizeof(buf)) < 0)
		err(1, "read");
	printf("%s: %s\n", getprogname(), buf);

	strlcpy(buf, "writing to mydev", sizeof(buf));
	if (write(dev, buf, sizeof(buf)) < 0)
		err(1, "write");

	close(dev);

	return (0);
}
