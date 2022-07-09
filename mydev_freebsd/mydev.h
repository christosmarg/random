#ifndef _MYDEV_H_
#define _MYDEV_H_

#include <sys/ioccom.h>

typedef struct {
	int x;
	int y;
} foo_t;

#define MYDEVIOC_READ	_IOR('a', 1, foo_t)
#define MYDEVIOC_WRITE	_IOW('a', 2, foo_t)
#define MYDEVIOC_RDWR	_IOWR('a', 3, foo_t)

#endif /* _MYDEV_H_ */
