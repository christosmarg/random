#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define IMGW 1920
#define IMGH 1080
#define MAXN 1024
#define MINR (-1.5)
#define MAXR 0.7
#define MINI (-1.0)
#define MAXI 1.0

#define TOREAL(x, imgw, minr, maxr) (x * ((maxr - minr) / imgw) + minr)
#define TOIMGN(y, imgh, mini, maxi) (y * ((maxi - mini) / imgh) + mini)

static int
fmandelbrot(double cr, double ci)
{
	int i = 0;
	double zr = 0.0, zi = 0.0;
	double tmp;

	for ( ; i < MAXN && (zr * zr + zi * zi) < 4.0; i++) {
		tmp = zr * zr - zi * zi + cr;
		zi = 2.0 * zr * zi + ci;
		zr = tmp;
	}
	return i;
}

int
main(int argc, char *argv[])
{
	FILE *fp;
	int x, y, n, r, g, b;
	double cr, ci;

	if ((fp = fopen("mandelbrot_output.ppm", "w")) == NULL) {
		fprintf(stderr, "Cannot open output file. Exiting...");
		return EXIT_FAILURE;
	}
	fprintf(fp, "P3\n%d %d\n256\n", IMGW, IMGH);

	for (y = 0; y < IMGH; y++) {
		for (x = 0; x < IMGW; x++) {
			cr = TOREAL(x, IMGW, MINR, MAXR);
			ci = TOIMGN(y, IMGH, MINI, MAXI);
			n = fmandelbrot(cr, ci);
			r = (n % 256);
			g = ((int)(n * sinf(M_PI)) % 256);
			b = ((n * 3) % 256);
			fprintf(fp, "%d %d %d ", r, g, b);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	return 0;
}
