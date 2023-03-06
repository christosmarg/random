#include <sys/types.h>
#include <sys/param.h>
#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/malloc.h>
#include <sys/uio.h>

#include "mydev.h"

#define BUFSIZE (1 << 16)

MALLOC_DECLARE(M_MYDEV);
MALLOC_DEFINE(M_MYDEV, "mydev", "test device");

typedef struct {
	char	buf[BUFSIZE + 1];
	size_t	len;
} foo_t;

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
static foo_t *foo;

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
	size_t amnt;
	int v, error = 0;

	/*
	 * Determine how many bytes we have to read. We'll either read the
	 * remaining bytes (uio->uio_resid) or the number of bytes requested by
	 * the caller.
	 */
	v = uio->uio_offset >= foo->len + 1 ? 0 : foo->len + 1 - uio->uio_offset;
	amnt = MIN(uio->uio_resid, v);

	/* Move the bytes from foo->buf to uio. */
	if ((error = uiomove(foo->buf, amnt, uio)) != 0)
		uprintf("uiomove failed\n");

	if (!foo->len)
		uprintf("nothing to read\n");

	return (error);
}

static int
mydev_write(struct cdev *dev, struct uio *uio, int ioflag)
{
	size_t amnt;
	int error = 0;

	/* Do not allow random access. */
	if (uio->uio_offset != 0 && (uio->uio_offset != foo->len))
		return (EINVAL);

	/* We're not appending, reset length. */
	else if (uio->uio_offset == 0)
		foo->len = 0;

	amnt = MIN(uio->uio_resid, (BUFSIZE - foo->len));
	if ((error = uiomove(foo->buf + uio->uio_offset, amnt, uio)) != 0)
		uprintf("uiomove failed");

	foo->len = uio->uio_offset;
	foo->buf[foo->len] = '\0';

	return (error);
}

static int
mydev_ioctl(struct cdev *dev, u_long cmd, caddr_t addr, int flags,
    struct thread *td)
{
	bar_t *bp;
	int error = 0;

	switch (cmd) {
	case MYDEVIOC_READ:
		bp = (bar_t *)addr;
		bp->x = 100;
		bp->y = 300;
		break;
	case MYDEVIOC_WRITE:
		bp = (bar_t *)addr;
		uprintf("/dev/mydev: x=%d, y=%d\n", bp->x, bp->y);
		break;
	case MYDEVIOC_RDWR:
		bp = (bar_t *)addr;
		bp->x += 15;
		bp->y += 30;
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
		foo = malloc(sizeof(foo_t), M_MYDEV, M_WAITOK | M_ZERO);
		foo->buf[0] = '\0';
		foo->len = 0;
		break;
	case MOD_UNLOAD: /* FALLTHROUGH */
	case MOD_SHUTDOWN:
		free(foo, M_MYDEV);
		destroy_dev(mydev_cdev);
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

DEV_MODULE(mydev, mydev_modevent, NULL);
