#include <chrono>
#include <cstdlib>
#include <ctime>
#include <list>
#include <thread>

#include <ncurses.h>

#define XMAX (getmaxx(stdscr))
#define YMAX (getmaxy(stdscr))

struct Snake {
	struct Seg {
		int x;
		int y;
	};
	std::list<Seg> body;
	int x;
	int y;
	int score;
	bool dead;

	Snake();
	void update(int key);
	void grow();
	void draw();
	bool collided();
};

struct Food {
	int x;
	int y;

	Food();
	void spawn();
	void draw();
};

Snake::Snake()
{
	y = YMAX >> 1;
	x = XMAX >> 1;
	body = {{x, y}, {x + 1, y + 1}};	
	score = 1;
	dead = false;
}

void
Snake::update(int key)
{
	switch (key) {
	case KEY_UP:
		y--;
		body.push_front({body.front().x, body.front().y - 1});
		break;
	case KEY_DOWN:
		y++;
		body.push_front({body.front().x, body.front().y + 1});
		break;
	case KEY_LEFT:
		x--;
		body.push_front({body.front().x - 1, body.front().y});
		break;
	case KEY_RIGHT:
		x++;
		body.push_front({body.front().x + 1, body.front().y});
		break;
	}
	body.pop_back();
}

void
Snake::grow()
{
	for (int i = 0; i < 3; i++)
		body.push_back({body.back().x, body.back().y});
}

void
Snake::draw()
{
	for (const auto& b : body)
		mvaddch(b.y, b.x, ACS_CKBOARD);
}

bool
Snake::collided()
{
	dead = y < 2 || y > YMAX - 1 || x < 1 || x > XMAX - 1;
	for (std::list<Seg>::iterator i = body.begin(); i != body.end(); i++)
		if (i != body.begin()
		&&  i->x == body.front().x
		&&  i->y == body.front().y)
			dead = true;
	return dead;
}

Food::Food()
{
	x = std::rand() % XMAX - 1;
	y = std::rand() % (YMAX - 2) + 2;
}

void
Food::spawn()
{
	x = std::rand() % XMAX - 1;
	y = std::rand() % (YMAX - 2) + 2;
}

void
Food::draw()
{
	mvaddch(y, x, 'O');
}

static void
initcurses()
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, true);
	nodelay(stdscr, true);
}

static bool
kbhit()
{
	int c;

	if ((c = getch()) != ERR) {
		ungetch(c);
		return true;
	}
	return false;
}

int
main(int argc, char *argv[])
{
	initcurses();
	std::srand(std::time(nullptr));

	Snake snake;
	Food food;
	int key, nextkey;

	key = KEY_RIGHT;

	for (;;) {
		erase();
		if (kbhit()) {
			if ((nextkey = getch()) != key)
				key = nextkey;
			else
				continue;
		}

		snake.update(key);
		if (snake.collided() || key == 'q')
			break;

		if (snake.body.front().y == food.y
		&&  snake.body.front().x == food.x) {
			food.spawn();
			snake.grow();
			snake.score++;
		}

		food.draw();
		snake.draw();

		mvprintw(0, 0, "Score: %d", snake.score);
		mvhline(1, 0, ACS_HLINE, XMAX);
		refresh();
		std::this_thread::sleep_for(std::chrono::milliseconds(60));
	}

	endwin();

	return 0;
}
