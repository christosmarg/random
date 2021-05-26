#ifndef DEFS_H
#define DEFS_H

#define MOVE_UP			'w'
#define MOVE_DOWN		's'
#define MOVE_LEFT		'a'
#define MOVE_RIGHT		'd'
#define MOVE_ENTER		'\n'
#define MOVE_OPEN_CELL		'o'
#define MOVE_FLAG_CELL		'f'
#define MOVE_DEFUSE_CELL	'g'
#define MOVE_OPEN_MENU		'c'
#define MOVE_QUIT		'q'

#define MINE_DEFUSED		'D'
#define CELL_FLAGGED		'F'
#define CELL_BLANK		' '
#define CELL_MINE		'*'
#define GRID_BOX		"[ ]"

#define OPT_CTRLS		"c Controls"
#define OPT_QUIT		"q	   Quit"
#define OPT_MOVE_UP		"w/k	   Move up"
#define OPT_MOVE_DOWN		"s/j	   Move down"
#define OPT_MOVE_LEFT		"a/h	   Move left"
#define OPT_MOVE_RIGHT		"d/l	   Move right"
#define OPT_FLAG_CELL		"f	   Flag cell"
#define OPT_DEFUSE		"g	   Defuse (if flagged only)"
#define OPT_OPEN_CELL		"[ENTER]/o Open cell"

#define MSG_COLS		"Columns (Min = %d, Max = %d): "
#define MSG_ROWS		"Rows (Min = %d, Max = %d): "
#define MSG_MINES		"Mines (Min = %d, Max = %d): "
#define MSG_QUIT_MENU		"Press any key to quit the menu"
#define MSG_CONT		"Press any key to continue"
#define MSG_CURPOS		"Current position: (%d, %d) "
#define MSG_NDEFUSED		"Defused mines: %d/%d"
#define MSG_WIN_1		"You defused all the mines!"
#define MSG_WIN_2		"You won :)"
#define MSG_LOSE_1		"You hit a mine! (or tried to defuse the wrong cell)"
#define MSG_LOSE_2		"Game over :("

#define GAME_LOST		0
#define GAME_WON		1

#define YMID(x)			(getmaxy((x)) >> 1)
#define XMID(x)			(getmaxx((x)) >> 1)
#define SCRSPACE_X(x)		((x) * 3 + 2)
#define SCRSPACE_Y(y)		((y) + 2)
#define ARRSPACE_X(x)		(((x) - 2) / 3)
#define ARRSPACE_Y(y)		((y) - 1)
#define CENTER(x, y)		(((x) >> 1) - ((y) >> 1))
#define CURS_UPDATE(m, y, x)	(wmove((m)->gw, (y), (x)))
#define IS_MINE(m, r, c)	((m)->mineboard[(r)][(c)] == CELL_MINE)
#define IS_FLAGGED(m, r, c)	((m)->dispboard[(r)][(c)] == CELL_FLAGGED)
#define IS_BLANK(m, r, c)	((m)->dispboard[(r)][(c)] == CELL_BLANK)
#define OUT_OF_BOUNDS(m, r, c)	\
	((r) < 0 || (r) > ((m)->rows - 1) || (c) < 0 || (c) > ((m)->cols - 1))

#endif /* DEFS_H */
