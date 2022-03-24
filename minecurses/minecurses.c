#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <curses.h>

#include "defs.h"

/* TODO:
 * - play again
 * - win fix
 * - first open
 */

struct minecurses {
	WINDOW *gw;
	char **dispboard;
	char **mineboard;
	int x;
	int y;
	int xmax;
	int ymax;
	int wxmax;
	int wymax;
	int rows;
	int cols;
	int nmines;
	int ndef;
	int move;
	int gameover;
};

static void cursesinit(struct minecurses *);
static void gwinit(struct minecurses *);
static void gamereset(struct minecurses *);
static void gamestart(struct minecurses *);
static int valset(int, int, const char *, int, int);
static int adjcount(const struct minecurses *, int, int);
static void boardsdealloc(struct minecurses *);
static void cellreveal(const struct minecurses *);
static void boardprint(const struct minecurses *);
static void menuopts(void);
static void endscreen(struct minecurses *, int);
static void *emalloc(size_t);

static void
cursesinit(struct minecurses *m)
{
	if (!initscr())
		errx(1, "initscr");
	noecho();
	cbreak();

	m->xmax = getmaxx(stdscr);
	m->ymax = getmaxy(stdscr);
}

static void
gwinit(struct minecurses *m)
{
	int wr, wc, wy, wx;
	
	wr = SCRSPACE_Y(m->rows);
	wc = SCRSPACE_X(m->cols);
	wy = CENTER(m->ymax, wr);
	wx = CENTER(m->xmax, wc);
	if ((m->gw = newwin(wr, wc, wy, wx)) == NULL)
		errx(1, "newwin");

	m->wxmax = getmaxx(m->gw);
	m->wymax = getmaxy(m->gw);
}

static void
gamereset(struct minecurses *m)
{
	size_t i, j, r, c;

	echo();
	m->cols = valset(m->ymax, 4, MSG_COLS, 5, ARRSPACE_X(m->xmax) - 2);
	m->rows = valset(m->ymax, 3, MSG_ROWS, 5, ARRSPACE_Y(m->ymax) - 3);
	m->nmines = valset(m->ymax, 2, MSG_MINES, 1, m->rows * m->cols - 15);
	m->x = m->y = 0;
	m->ndef = 0;
	m->gameover = 0;
	noecho();

	gwinit(m);
	menuopts();
	/* allocate memory for the boards */
	m->dispboard = emalloc(m->rows * sizeof(char *));
	m->mineboard = emalloc(m->rows * sizeof(char *));
	for (i = 0; i < m->rows; i++) {
		m->dispboard[i] = emalloc(m->cols);
		m->mineboard[i] = emalloc(m->cols);
	}

	/* place mines */
	srand(time(NULL));
	for (i = 0; i < m->nmines; i++) {
		r = rand() % m->rows;
		c = rand() % m->cols;
		m->mineboard[r][c] = CELL_MINE;
	}

	/* TODO: do it in one loop */
	/* add numbers */
	for (i = 0; i < m->rows; i++) {
		for (j = 0; j < m->cols; j++) {
			if (!IS_MINE(m, i, j))
				m->mineboard[i][j] = adjcount(m, i, j) + '0';
			/* in the meantime, initialize dispboard */
			m->dispboard[i][j] = CELL_BLANK;
		}
	}

	/* fill spaces */
	for (i = 0; i < m->rows; i++)
		for (j = 0; j < m->cols; j++)
			if (!IS_MINE(m, i, j) &&  m->mineboard[i][j] == '0')
				m->mineboard[i][j] = '-';
}

#define bx m->x
#define by m->y

static void
gamestart(struct minecurses *m)
{
	static int y = 1, x = 2;

	for (;;) {
		werase(m->gw);
		erase();

		boardprint(m);
		CURS_UPDATE(m, y, x);
		bx = ARRSPACE_X(x);
		by = ARRSPACE_Y(y);

		/* session info */
		mvprintw(0, 0, MSG_CURPOS, bx, by);
		mvprintw(0, CENTER(m->xmax, strlen(MSG_NDEFUSED) - 2),
		    MSG_NDEFUSED, m->ndef, m->nmines);
		mvprintw(0, m->xmax - strlen(OPT_CTRLS), OPT_CTRLS);

		refresh();
		wrefresh(m->gw);

		/* handle moves */
		switch (m->move = wgetch(m->gw)) {
		case MOVE_UP:
			if (--y < 1)
				y = 1;
			break;
		case MOVE_DOWN:
			if (++y > m->wxmax - 2)
				y = m->wymax - 2;
			break;
		case MOVE_LEFT:
			x -= 3;
			if (x < 2)
				x = 2;
			break;
		case MOVE_RIGHT:
			x += 3;
			if (x > m->wxmax - 3)
				x = m->wxmax - 3;
			break;
		case MOVE_ENTER: /* FALLTHROUGH */
		case MOVE_OPEN_CELL:
			m->dispboard[by][bx] = m->mineboard[by][bx];
			m->gameover = IS_MINE(m, by, bx);
			cellreveal(m);
			break;
		case MOVE_FLAG_CELL:
			if (IS_FLAGGED(m, by, bx))
				m->dispboard[by][bx] = CELL_BLANK;
			else if (!IS_FLAGGED(m, by, bx) && !IS_BLANK(m, by, bx))
				break;
			else
				m->dispboard[by][bx] = CELL_FLAGGED;
			cellreveal(m);
			break;
		case MOVE_DEFUSE_CELL:
			if (IS_FLAGGED(m, by, bx) &&  IS_MINE(m, by, bx)) {
				m->ndef++;
				m->dispboard[by][bx] = MINE_DEFUSED;
				m->mineboard[by][bx] = MINE_DEFUSED;
				cellreveal(m);
			} else if (IS_FLAGGED(m, by, bx) && !IS_MINE(m, by, bx))
				m->gameover = 1;	      
			break;
		case MOVE_OPEN_MENU:
			menuopts();
			boardprint(m);
			break;
		}

		if (OUT_OF_BOUNDS(m, by, bx)
		||  m->gameover
		||  m->ndef == m->nmines
		||  m->move == MOVE_QUIT)
			break;
	}

	if (m->gameover)
		endscreen(m, GAME_LOST);
	else if (m->ndef == m->nmines)
		endscreen(m, GAME_WON);
}

#undef bx
#undef by

static int
valset(int ymax, int offy, const char *msg, int min, int max)
{
	int val;

	do {
		mvprintw(ymax - offy, 1, msg, min, max);
		scanw("%d", &val);
	} while (val < min || val > max);

	return (val);
}

static int
adjcount(const struct minecurses *m, int r, int c)
{
	int n = 0;

	if (!OUT_OF_BOUNDS(m, r, c-1)	&& IS_MINE(m, r, c-1))	 n++; // north
	if (!OUT_OF_BOUNDS(m, r, c+1)	&& IS_MINE(m, r, c+1))	 n++; // south
	if (!OUT_OF_BOUNDS(m, r+1, c)	&& IS_MINE(m, r+1, c))	 n++; // east
	if (!OUT_OF_BOUNDS(m, r-1, c)	&& IS_MINE(m, r-1, c))	 n++; // west
	if (!OUT_OF_BOUNDS(m, r+1, c-1) && IS_MINE(m, r+1, c-1)) n++; // north-east
	if (!OUT_OF_BOUNDS(m, r-1, c-1) && IS_MINE(m, r-1, c-1)) n++; // north-west
	if (!OUT_OF_BOUNDS(m, r+1, c+1) && IS_MINE(m, r+1, c+1)) n++; // south-east
	if (!OUT_OF_BOUNDS(m, r-1, c+1) && IS_MINE(m, r-1, c+1)) n++; // south-west

	return (n);
}

static void
boardsdealloc(struct minecurses *m)
{
	size_t i = 0;

	if (m->dispboard && m->mineboard) {
		for (; i < m->rows; i++) {
			free(m->dispboard[i]);
			free(m->mineboard[i]);
		}
		free(m->dispboard);
		free(m->mineboard);
	}
}

static void
cellreveal(const struct minecurses *m)
{
	mvwaddch(m->gw, m->y + 1, SCRSPACE_X(m->x), m->dispboard[m->y][m->x]);
}

static void
boardprint(const struct minecurses *m)
{    
	size_t i, j, x, y;

	wattroff(m->gw, A_BOLD);
	box(m->gw, 0, 0);
	for (i = 1; i <= m->rows; i++) {
		wmove(m->gw, i, 1);
		for (j = 0; j < m->cols; j++)
			waddstr(m->gw, GRID_BOX);
	}
	wattron(m->gw, A_BOLD);
	for (i = 0, y = 1; i < m->rows; i++, y++)
		for (j = 0, x = 2; j < m->cols; j++, x += 3)
			mvwaddch(m->gw, y, x, m->dispboard[i][j]);
	wattroff(m->gw, A_BOLD);
}

static void
menuopts(void)
{
	WINDOW *opts;
	int w, h, wy, wx;

	w = 36;
	h = 12;
	wy = CENTER(getmaxy(stdscr), h);
	wx = CENTER(getmaxx(stdscr), w);
	opts = newwin(h, w, wy, wx);
	box(opts, 0, 0);

	/* fill menu */
	mvwprintw(opts, 1, 1, OPT_QUIT);
	mvwprintw(opts, 2, 1, OPT_MOVE_UP);
	mvwprintw(opts, 3, 1, OPT_MOVE_DOWN);
	mvwprintw(opts, 4, 1, OPT_MOVE_LEFT);
	mvwprintw(opts, 5, 1, OPT_MOVE_RIGHT);
	mvwprintw(opts, 6, 1, OPT_FLAG_CELL);
	mvwprintw(opts, 7, 1, OPT_DEFUSE);
	mvwprintw(opts, 8, 1, OPT_OPEN_CELL);
	mvwprintw(opts, 10, 1, MSG_QUIT_MENU);

	(void)wgetch(opts);
	delwin(opts);
}

static void
endscreen(struct minecurses *m, int state)
{
	curs_set(0);
	wclear(m->gw);
	wrefresh(m->gw);
	attron(A_BOLD);
	switch (state) {
	case GAME_WON:
		mvprintw(YMID(stdscr)-2, XMID(stdscr)-11, MSG_WIN_1);
		mvprintw(YMID(stdscr)-1, XMID(stdscr)-3, MSG_WIN_2);
		break;
	case GAME_LOST:
		mvprintw(YMID(stdscr)-2, XMID(stdscr)-24, MSG_LOSE_1);
		mvprintw(YMID(stdscr)-1, XMID(stdscr)-4, MSG_LOSE_2);
		// TODO: print mine board
		break;
	}
	mvprintw(YMID(stdscr), XMID(stdscr)-11, MSG_CONT);
	refresh();
	attroff(A_BOLD);
	getchar();
}

static void *
emalloc(size_t nb)
{
	void *p;

	if ((p = malloc(nb)) == NULL)
		err(1, "malloc");

	return (p);
}

int
main(int argc, char *argv[])
{
	struct minecurses *m;

	m = emalloc(sizeof(struct minecurses));
	cursesinit(m);

	gamereset(m);
	gamestart(m);

	boardsdealloc(m);
	delwin(m->gw);
	endwin();
	free(m);

	return (0);
}
