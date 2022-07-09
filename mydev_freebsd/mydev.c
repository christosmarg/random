#include <sys/param.h>
#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/uio.h>

#include "mydev.h"

static d_open_t		mydev_open;
static d_close_t	mydev_close;
static d_read_t		mydev_read;
static d_write_t	mydev_write;
static d_ioctl_t	mydev_ioctl;
static int		mydev_modevent(module_t, int, void *);

static struct cdevsw mydev_cdevsw = {
	.d_name		= "mydev",
	.d_version	= D_VERSION,
	.d_flags	= D_TRACKCLOSE,
	.d_open		= mydev_open,
	.d_close	= mydev_close,
	.d_read		= mydev_read,
	.d_write	= mydev_write,
	.d_ioctl	= mydev_ioctl,
};

static struct cdev *mydev_cdev;

static int
mydev_open(struct cdev *dev, int flags, int devtype, struct thread *td)
{
	uprintf("mydev: device opened\n");

	return (0);
}

static int
mydev_close(struct cdev *dev, int flags, int devtype, struct thread *td)
{
	uprintf("mydev: device closed\n");

	return (0);
}

static int
mydev_read(struct cdev *dev, struct uio *uio, int ioflag)
{
	char buf[32];
	int error = 0;

	strlcpy(buf, "reading from mydev", sizeof(buf));
	error = uiomove(buf, sizeof(buf), uio);

	return (error);
}

static int
mydev_write(struct cdev *dev, struct uio *uio, int ioflag)
{
	int error = 0;

	uprintf("mydev: %s\n", (char *)uio->uio_iov->iov_base);

	return (error);
}

static int
mydev_ioctl(struct cdev *dev, u_long cmd, caddr_t addr, int flags,
    struct thread *td)
{
	foo_t *fp;
	int error = 0;

	switch (cmd) {
	case MYDEVIOC_READ:
		fp = (foo_t *)addr;
		fp->x = 10;
		fp->y = 20;
		break;
	case MYDEVIOC_WRITE:
		fp = (foo_t *)addr;
		uprintf("mydev: x=%d, y=%d\n", fp->x, fp->y);
		break;
	case MYDEVIOC_RDWR:
		fp = (foo_t *)addr;
		fp->x += 5;
		fp->y += 5;
		break;
	default:
		error = ENOTTY;
		break;
	}

	return (error);
}

static int
mydev_modevent(module_t mod, int type, void *arg)
{
	int error = 0;

	switch (type) {
	case MOD_LOAD:
		mydev_cdev = make_dev(&mydev_cdevsw, 0, UID_ROOT, GID_WHEEL,
		    0666, "mydev");
		break;
	case MOD_UNLOAD: /* FALLTHROUGH */
	case MOD_SHUTDOWN:
		destroy_dev(mydev_cdev);
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

DEV_MODULE(mydev, mydev_modevent, NULL);
