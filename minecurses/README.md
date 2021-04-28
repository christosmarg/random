# minecurses

A minesweeper game using `ncurses`. The game was originally made as a
university assignment of mine, but I decided to take it a bit further afterwards.

## How to play

The objective is to simply find and defuse all the mines, not open all the non-mine
cells, like in most minesweeper games; this make the game relatively harder.

* Enter number of columns
* Enter number of rows
* Enter number of mines
* Move with `w`/`s`/`a`/`d` or Vim keys (`k`/`j`/`h`/`l`)
* Open cell with `[ENTER]`, or `o`
* Flag cell with `f`
* Defuse mine (only if flagged) with `g`
* You win if you defuse all the mines
* You lose in case you open a mine or you try to defuse a flagged cell not containing a mine

Additional controls

* `m`: Open control menu
* `r`: Restart the game
* `q`: Quit

## Usage

```shell
$ cd path/to/minecurses
$ make && make run
$ make clean # optional
```
You can install `minecurses` by running `sudo make install clean`.
The binary will be installed in `/usr/local/bin`.

## Preview

### Start screen
![startmenu](https://user-images.githubusercontent.com/54286563/102023826-b110f780-3d96-11eb-89a3-4b0679a36a50.png)
### Gameplay
![gameplay](https://user-images.githubusercontent.com/54286563/102023832-ba9a5f80-3d96-11eb-9341-b2a07a7356de.png)

## To Do

* Add colors
* Fix occasional wrong mine spawning bug
