#pragma once

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

/// Define tile as unsigned char, minimum required size.
typedef unsigned char tile;

/// Define dimension as unsigned char, supports sizes up to 255.
typedef unsigned char dimension;

/// Define optional_dimension as short int, supports sizes from -256 to 255.
typedef short int optional_dimension;

/// Define BoardData as a two dimensional vectors of tiles.
typedef vector<vector<tile>> BoardData;

/// Define point as a pair of two dimensions: X axis, and Y axis.
typedef pair<dimension, dimension> Point;

/// Define optional point as a pair of two optional_dimensions: X axis, and Y axis.
typedef pair<optional_dimension, optional_dimension> OptionalPoint;

/**
 * A class that manages the board throughout the game.
 */
class Board {
public:

    /// The possible tile of colors generated for the game.
    static const vector<tile> colors;

    /// The joker tile identifier.
    static const tile joker = 'j';

    /// The chance of generating a joker tile, written as: 1 / joker_chance.
    static const unsigned int joker_chance = 12;

    /// Code for changing foreground color in the terminal.
    static const char foreground_color_code = 30;

    /// Code for changing background color in the terminal.
    static const char background_color_code = 40;

    /// Base codes for each of the colors, added to foreground_color_code and background_color_code to change the
    /// terminal output colors.
    static const map<tile, unsigned char> color_codes;

    /**
     * Constructor.
     *
     * Fills the board with colors and jokers.
     *
     * @param width     Width of the board.
     * @param height    Height of the board.
     * @param colors_num    Number of colors to use.
     */
    Board(dimension width, dimension height, unsigned short int colors_num);

    /**
     * Get the tile in the base position.
     *
     * @return  The tile in the base position.
     */
    [[nodiscard]] tile get_base() const;

    /**
     * Set the base position.
     *
     * The position will not be valid in any of the following cases:
     * 1.   The position is beyond the boundaries of the board.
     * 2.   The position is occupied by a joker tile.
     * 3.   The position is already the position of the base.
     *
     * @param position  The new position for the base.
     * @return  Is the position valid.
     */
    bool set_base(const OptionalPoint &position);

    /**
     * Paint from the base onwards.
     *
     * Change the color of the base position, colors the board in several ways:
     * 1.   Colors are changed recursively, all neighbor tiles in 4 directions (up, down, left, right) with the same
     *      color as the base are changed, repeating this step, until confronted with a different color.
     * 2.   Tiles in chess knight move positions in relation to the base are colored.
     * 3.   A joker tile that one of its close neighbors in 4 directions (up, down, left, right) changes its color -
     *      colors its close neighbors in 8 directions (up, up-left, left, left-down, down, down-right, right, right-up)
     *      regardless of their original color, this may chain more joker triggers.
     *      @note: this function does not perform the jokers' action, just collects them.
     *
     * @param color     New color to set from the base onwards.
     * @param jokers    Jokers triggered during the painting.
     */
    void paint(tile color, set<Point> &jokers);

    /**
     * Paint in a given position.
     *
     * @see Board::paint
     *
     * @param color     The color to set.
     * @param original  The original color of the triggering tile.
     * @param jokers    Jokers triggered during the painting.
     * @param position  The position to paint.
     * @param node      Is the position a node, meaning it can change its color, but does not chain color changes.
     * @param probe     Is the position being probed, meaning it cannot change its color or chain color changes.
     */
    void paint(tile color, tile original, set<Point> &jokers, OptionalPoint position, bool node = false,
               bool probe = false);

    /**
     * Paint jokers.
     *
     * Paint jokers collected in previous painting.
     *
     * @param color     The color to set.
     * @param jokers    Jokers triggered during the painting.
     */
    void paint_jokers(tile color, set<Point> jokers);

    /**
     * Right padding.
     *
     * @param str       The string to pad.
     * @param length    The desired final length.
     * @param filler    The character used to fill up to the desired length.
     * @return  The padded string.
     */
    static string zfill(string str, unsigned int length, char filler);

    /**
     * Print an index, alternate black and white.
     *
     * @param i The index to print.
     */
    static void print_index(dimension i);

    /**
     * Print the entire board.
     *
     * @param moves Number of moves left.
     */
    void print(unsigned int moves) const;

    /**
     * Check if the board is solved.
     *
     * The board is defined solved if it is comprised of only one color.
     *
     * @return  Is the board solved.
     */
    [[nodiscard]] bool solved() const;

    /**
     * Count the remaining tiles that are not the same color as the base.
     *
     * @return  The number of remaining tiles.
     */
    [[nodiscard]] unsigned int count_remaining_tiles() const;

    /**
     * Save the current board state.
     */
    void save_board();

    /**
     * Return the board to the previous state, if possible.
     *
     * @return  Is undo possible, meaning previous moves has been made.
     */
    bool undo_board();

    /**
     * Get height.
     *
     * @return  The height of the board.
     */
    [[nodiscard]] inline dimension get_height() const { return m_height; }

    /**
     * Get width.
     *
     * @return  The width of the board.
     */
    [[nodiscard]] inline dimension get_width() const { return m_width; }

    /**
     * Check for previous board states.
     *
     * @return  Are there previous board states.
     */
    [[nodiscard]] inline bool has_history() const { return !m_previous_boards.empty(); }

    /**
     * Check if the position is inside the board's boundaries.
     *
     * @return  Is the position inside the board's boundaries.
     */
    [[nodiscard]] inline bool in_boundaries(const OptionalPoint &position) const {
        return ((position.first >= 0) && (position.first < m_width) && (position.second >= 0) &&
                (position.second < m_height));
    }


private:

    /// Dimensions of the board, height and width.
    const dimension m_width, m_height;

    /// Position of the base.
    Point m_position;

    /// The current board state, contains all the tiles.
    BoardData m_board;

    /// Previous board states.
    vector<BoardData> m_previous_boards;
};
