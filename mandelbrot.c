#include <math.h>
#include <stdio.h>

#define IMGW 1920
#define IMGH 1080
#define MAXN 1024
#define MINR (-1.5)
#define MAXR 0.7
#define MINI (-1.0)
#define MAXI 1.0

#define TOREAL(x) ((x) * ((MAXR - MINR) / IMGW) + MINR)
#define TOIMGN(y) ((y) * ((MAXI - MINI) / IMGH) + MINI)

static int
fmandelbrot(double cr, double ci)
{
	double zr = 0.0, zi = 0.0, tmp;
	int i = 0;

	for (; i < MAXN && (zr * zr + zi * zi) < 4.0; i++) {
		tmp = zr * zr - zi * zi + cr;
		zi = 2.0 * zr * zi + ci;
		zr = tmp;
	}

	return (i);
}

int
main(int argc, char *argv[])
{
	double cr, ci;
	int x, y, n, r, g, b;

	printf("P3\n%d %d\n256\n", IMGW, IMGH);
	for (y = 0; y < IMGH; y++) {
		for (x = 0; x < IMGW; x++) {
			cr = TOREAL(x);
			ci = TOIMGN(y);
			n = fmandelbrot(cr, ci);
			r = (n % 256);
			g = ((int)(n * sinf(M_PI)) % 256);
			b = ((n * 3) % 256);
			printf("%d %d %d ", r, g, b);
		}
		printf("\n");
	}

	return (0);
}
