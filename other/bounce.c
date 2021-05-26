#include <curses.h>
#include <stdio.h>
#include <unistd.h>

struct ball {
	int x;
	int y;
	int vx;
	int vy;
	int xmax;
	int ymax;
};

static void
cursesinit(void)
{
	if (!initscr())
		exit(1);
	cbreak();
	noecho();
	curs_set(0);
}

static void
collision(struct ball *b)
{
	if (b->y < 2 || b->y > b->ymax-1)
		b->vy *= -1.0f;
	if (b->x < 1 || b->x > b->xmax-1)
		b->vx *= -1.0f;
	b->y += b->vy;
	b->x += b->vx;
}

int
main(int argc, char *argv[])
{
	struct ball b = {1, 2, 1, 1, 0, 0};

	cursesinit();
	getmaxyx(stdscr, b.ymax, b.xmax);

	for (;;) {
		erase();
		collision(&b);
		mvaddch(b.y, b.x, 'O');
		mvprintw(0, 0, "(x, y) = (%d, %d)", b.x, b.y);
		mvhline(1, 0, ACS_HLINE, b.xmax);
		refresh();
		usleep(15000);
	}
	endwin();

	return 0;
}
