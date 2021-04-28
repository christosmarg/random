#include <stdlib.h>

#include <SDL2/SDL.h>

#ifdef _WIN_32
typedef unsigned int	u_int32_t;
typedef unsigned short	u_int16_t;
typedef unsigned char	u_int8_t;
#else
#include <sys/types.h>
#endif /* _WIN_32 */

#define RGB_CALC ((r) * 0x33 << 16 | (g) * 0x33 << 8  | (b) * 0x33)
#define RGB_POS ((r) * 36 + (g) * 6 + (b))
#define PC_POS (pc[3] << 16 | pc[4] << 8 | pc[5])

static u_int32_t palette[0x100];
static u_int8_t mem[0x1000008];
static const u_int8_t keys[16] = {
	SDLK_1, SDLK_2, SDLK_3, SDLK_4,
	SDLK_q, SDLK_w, SDLK_e, SDLK_r,
	SDLK_a, SDLK_s, SDLK_d, SDLK_f,
	SDLK_z, SDLK_x, SDLK_c, SDLK_v
};

static int
evhandle(void)
{
	SDL_Event ev;
	size_t i;
	u_int16_t keybits;
	
	keybits = mem[0] << 8 | mem[1];
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT || ev.key.keysym.sym == SDLK_ESCAPE)
			return 0;
		if (ev.type == SDL_KEYDOWN)
			for (i = 0; i < 16; i++)
				if (ev.key.keysym.sym == keys[i])
					keybits = (keybits & ~(1 << i)) |
					    (ev.type == SDL_KEYDOWN) << i;
	}
	mem[0] = keybits >> 8;
	mem[1] = keybits & 0xFF;
	return 1;
}

static void
cycle(void)
{
	size_t i = 0x10000;
	u_int8_t *pc;
	
	pc = mem + (mem[2] << 16 | mem[3] << 8 | mem[4]);
	while (i--) {
		mem[PC_POS] = mem[pc[0] << 16 | pc[1] << 8 | pc[2]];
		pc = mem + (pc[6] << 16 | pc[7] << 8 | pc[8]);
	}
}

static void
render(SDL_Renderer *ren, SDL_Texture *tex)
{
	u_int32_t pixels[0x10000];
	u_int32_t *out;
	u_int8_t  *in;

	in  = mem + (mem[5] << 16);
	out = pixels;
	for (; out < (pixels + 0x10000); *out++ = palette[*in++])
		;
	SDL_UpdateTexture(tex, 0, pixels,
	    0x100 * SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_BGRA32));
	SDL_RenderClear(ren);
	SDL_RenderCopy(ren, tex, 0, 0);
	SDL_RenderPresent(ren);
}

int
main(int argc, char *argv[])
{
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Texture *tex;
	FILE *fp;
	size_t i;
	int w = 256, h = 256;
	u_int8_t r, g, b;

	if (argc != 2) {
		fprintf(stderr, "usage: %s rom\n", argv[0]);
		return EXIT_FAILURE;
	}
	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "fopen: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	for (i = 0; (mem[i] = fgetc(fp)) != EOF && i < sizeof(mem); i++)
		;
	fclose(fp);

	/* TODO: implement audio */
	win = SDL_CreateWindow("bytepusher", SDL_WINDOWPOS_UNDEFINED,
	    SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, -1, 0);
	tex  = SDL_CreateTexture(ren, SDL_PIXELFORMAT_BGRA32,
	    SDL_TEXTUREACCESS_STATIC, w, h);

	if (!win || !ren || !tex) {
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	for (r = 0; r < 6; r++)
		for (g = 0; g < 6; g++)
			for (b = 0; b < 6; b++)
				palette[RGB_POS] = RGB_CALC;

	for (i = 0xd8; i < 0x100; i++)
		palette[i] = 0x000000;

	for (;;) {
		if (!evhandle())
			break;
		cycle();
		render(ren, tex);
		SDL_Delay(10);
	}

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
