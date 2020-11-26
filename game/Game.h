#pragma once

#include "../board/Board.h"

#include <iostream>
#include <termios.h>

using namespace std;

/**
 * A class to manage the course of the game.
 */
class Game {
public:

    /// Undo action identifier.
    static const tile undo_action = 'u';

    /// Change base action identifier.
    static const tile change_base_action = 's';

    /// Quit action identifiers.
    static const set<tile> quit_actions;

    /**
     * Constructor.
     *
     * Initializes the board.
     *
     * @param moves         Maximum number of moves.
     * @param width         Width of the board.
     * @param height        Height of the board.
     * @param colors_num    Number of colors to use.
     */
    Game(unsigned int moves, dimension width, dimension height, unsigned short int colors_num);

    /**
     * Get single keyboard input without enter.
     *
     * A linux implementation for the windows `getch()` function.
     * Based on https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux.
     *
     * @return  The input character.
     */
    static char getch();

    /**
     * Play a single turn.
     *
     * The following options are possible in each turn:
     * 1.   Undo last move ('u') - undo the last move, possible only where previous moves were made.
     * 2.   Change base ('s') - change the base, where the coloring is performed from.
     * 3.   Color ('r', 'g', 'y', 'b', 'm', 'c') - color the base and touching tiles with the same color, recursively.
     * 4.   Quit ('q', ESC, BACKSPACE, DELETE) - quit the game.
     *
     * @param quit  Did the player quit.
     */
    void turn(bool &quit);

    /**
     * Play the game.
     *
     * Play single turns until one of the following occurs:
     * 1.   Victory - the whole board is comprised of a single color.
     * 2.   Defeat - all moves have been made and the board is multicolor.
     * 3.   Disqualification - the player quit the game.
     *
     * @return  Did the player win the game.
     */
    bool play();


private:

    /// The game board.
    Board m_board;

    /// Number of remaining moves.
    unsigned int m_moves;

    /// Number of colors.
    unsigned short int m_colors_num;
};
