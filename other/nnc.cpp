#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <ncurses.h>

static constexpr int ymax()
{
	return getmaxy(stdscr);
}

static constexpr int xmax()
{
	return getmaxx(stdscr);
}

struct Color {
	int x, y;
	int color;

	Color(int x, int y, int color)
		:x(x), y(y), color(color) {}

	constexpr int dist(const Color& c)
	{
		int dx = this->x - c.x;
		int dy = this->y - c.y;

		return std::sqrt(dx * dx + dy * dy);
	}
};

struct Blue: public Color {
	Blue(int x, int y, int color)
		:Color(x, y, color) {}
};

struct Green: public Color {
	Green(int x, int y, int color)
		:Color(x, y, color) {}
};

struct White: public Color {
	White(int x, int y, int color)
		:Color(x, y, color) {}
};

static void
init_curses()
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLUE);
	init_pair(2, COLOR_GREEN, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_WHITE);
}

static void
makepts(std::vector<Color *>& points)
{
	int x, y;

	for (std::size_t i = 0; i < 5; i++) {
		x = std::rand() % (xmax() - 1);
		y = std::rand() % (ymax() - 1);
		points.push_back(new Blue(x, y, 1));	
	}
	for (std::size_t i = 0; i < 5; i++) {
		x = std::rand() % (xmax() - 1);
		y = std::rand() % (ymax() - 1);
		points.push_back(new Green(x, y, 2));	
	}
}

static void
makewts(std::vector<White *>& whites)
{
	int x, y;

	for (std::size_t i = 0; i < 5; i++) {
		x = std::rand() % (xmax() - 1);
		y = std::rand() % (ymax() - 1);
		whites.push_back(new White(x, y, 3));	
	}
}

template<typename T> static void
print(const std::vector<T *>& vec)
{
	for (auto& v : vec) {
		attron(COLOR_PAIR(v->color));
		mvaddch(v->y, v->x, ACS_CKBOARD);
		attroff(COLOR_PAIR(v->color));
	}
}

static std::vector<int> 
calc_dists(const std::vector<Color *>& points, const White& w)
{
	std::vector<int> dists; 

	for (auto& point : points)
		dists.push_back(point->dist(w));
	return dists;
}

static void
find_nn(const std::vector<Color *>& points, std::vector<White *>& whites)
{
	std::vector<int> dists;
	int mindist;

	for (const auto& point : points) {
		for (auto&& white : whites) {
			dists = calc_dists(points, *white);
			mindist = *std::min_element(dists.begin(), dists.end());
			if (point->dist(*white) == mindist)
				white->color = point->color;		
		}
	}
}

template<typename T> static void
dealloc(std::vector<T *>& vec)
{
	for (auto&& v : vec)
		if (v != nullptr)
			delete v;
	if (!vec.empty())
		vec.clear();
}

int
main(int argc, char **argv)
{
	init_curses();
	std::srand(std::time(nullptr));

	std::vector<Color *> points;
	std::vector<White *> whites;
	makepts(points);
	makewts(whites);

	erase();
	print<Color>(points);
	print<White>(whites);
	refresh();
	getch();

	find_nn(points, whites);

	erase();
	print<Color>(points);
	print<White>(whites);
	refresh();
	getch();

	endwin();
	dealloc<Color>(points);
	dealloc<White>(whites);

	return 0;
}
