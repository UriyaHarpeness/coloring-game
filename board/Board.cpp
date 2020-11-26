#include "Board.h"

const vector<tile> Board::colors = {'r', 'g', 'b', 'y', 'c', 'm'};

const map<tile, unsigned char> Board::color_codes = {{'B', 0},  // Black.
                                                     {'r', 1},  // Red.
                                                     {'g', 2},  // Green.
                                                     {'y', 3},  // Yellow.
                                                     {'b', 4},  // Blue.
                                                     {'m', 5},  // Magenta.
                                                     {'c', 6},  // Cyan.
                                                     {'W', 7}}; // While.

Board::Board(dimension width, dimension height, unsigned short int colors_num) :
        m_width(width), m_height(height), m_position({0, 0}), m_board(width, vector<tile>(height)) {
    if ((colors_num > colors.size()) || (colors_num < 2)) {
        throw runtime_error("Invalid number of colors.");
    }

    srand((unsigned) time(nullptr));

    // Fill the board with random generated tiles.
    for (dimension x = 0; x < width; x++) {
        for (dimension y = 0; y < height; y++) {
            auto generated = rand();

            // The generated + x + (y * m_width) check, is that if the chance is a multiplication of the number of
            // possible colors, one color will have a higher change or being a joker, this avoids it.
            if (((generated + x + (y * m_width)) % joker_chance == 0) && (x + y > 0))
                m_board[x][y] = joker; // Joker.
            else m_board[x][y] = colors[generated % colors_num]; // Color.
        }
    }
}

tile Board::get_base() const {
    return m_board[m_position.first][m_position.second];
}

bool Board::set_base(const OptionalPoint &position) {
    if (!in_boundaries(position) || ((position.first == m_position.first) && (position.second == m_position.second)) ||
        (m_board[position.first][position.second] == joker))
        // Position is not valid for the base.
        return false;

    m_position = position;
    return true;
}

void Board::paint(tile color, set<Point> &jokers) {
    // Expand coloring.
    paint(color, get_base(), jokers, m_position);

    // Chess knight move coloring.
    for (unsigned int i = 0; i < 8; i++) {
        optional_dimension x = (i & 0b001) ? 1 : 2;
        optional_dimension y = 3 - x;
        x *= (i & 0b010) ? -1 : 1;
        y *= (i & 0b100) ? -1 : 1;

        paint(color, get_base(), jokers, {x + m_position.first, y + m_position.second}, true);
    }
}

void Board::paint(const tile color, const tile original, set<Point> &jokers, OptionalPoint position, bool node,
                  bool probe) {
    if (!in_boundaries(position)) return;

    // Joker, simply add.
    if (m_board[position.first][position.second] == joker) {
        jokers.insert(position);
        return;
    }

    // Expected color, break.
    if (m_board[position.first][position.second] == color) return;

    if (node && !probe) m_board[position.first][position.second] = color; // Colored by joker.
    else if ((m_board[position.first][position.second] != original) || probe) return; // Probe, or breaking coloring.

    // Change color.
    m_board[position.first][position.second] = color;

    // Color neighbor cells.
    paint(color, original, jokers, {position.first + 1, position.second}, node, node);
    paint(color, original, jokers, {position.first, position.second + 1}, node, node);
    paint(color, original, jokers, {position.first - 1, position.second}, node, node);
    paint(color, original, jokers, {position.first, position.second - 1}, node, node);
}

void Board::paint_jokers(const tile color, set<Point> jokers) {
    set<Point> new_jokers;

    // Paint given jokers and new collected jokers.
    while (!jokers.empty()) {
        for (const auto &j : jokers) {
            // Color.
            m_board[j.first][j.second] = color;

            // Iterate the 8 close neighbors.
            for (optional_dimension x = j.first - 1; x <= j.first + 1; x++) {
                for (optional_dimension y = j.second - 1; y <= j.second + 1; y++) {
                    paint(color, joker, new_jokers, {x, y}, true);
                }
            }
        }

        // Change to new collected jokers.
        jokers = move(new_jokers);
        new_jokers.clear();
    }
}

string Board::zfill(string str, unsigned int length, char filler) {
    if (str.length() < length) {
        str.insert(0, string(length - str.length(), filler));
    }
    return str;
}

void Board::print_index(const dimension i) {
    cout << "\033[" << to_string(color_codes.at((i % 2) ? 'W' : 'B') + foreground_color_code) << ";"
         << to_string(color_codes.at((i % 2) ? 'B' : 'W') + background_color_code) << "m"
         << zfill(to_string(min((int) i, 99)), 2, '0') << "\033[0m";
}

void Board::print(const unsigned int moves) const {
    // Display title.
    cout << endl << "--= Board =--" << endl;

    // Display upper indexes (X axis).
    cout << "  ";
    for (dimension y = 0; y < m_height; y++) print_index(y);
    cout << endl;

    // Display board.
    for (dimension x = 0; x < m_width; x++) {
        // Display left index (Y axis).
        print_index(x);
        for (dimension y = 0; y < m_height; y++) {
            if (m_board[x][y] == joker) {
                // Display joker.
                cout << "\033[1mJK\033[0m";
            } else if ((x == m_position.first) && (y == m_position.second)) {
                // Display base position.
                cout << "\033[" << to_string(color_codes.at(m_board[x][y]) + foreground_color_code) << "m"
                     << zfill(to_string(min((int) moves, 99)), 2, '0') << "\033[0m";
            } else {
                // Display color.
                cout << "\033[" << to_string(color_codes.at(m_board[x][y]) + background_color_code) << "m  \033[0m";
            }
        }
        // Display right index (Y axis).
        print_index(x);
        cout << endl;
    }

    // Display lower indexes (X axis).
    cout << "  ";
    for (dimension y = 0; y < m_height; y++) print_index(y);
    cout << endl << endl;
}

bool Board::solved() const {
    tile color = m_board[0][0];

    return all_of(m_board.cbegin(), m_board.cend(), [color](const vector<tile> &line) {
        return all_of(line.cbegin(), line.cend(), [color](tile current) { return current == color; });
    });
}

unsigned int Board::count_remaining_tiles() const {
    unsigned int count = 0;

    for (dimension x = 0; x < m_width; x++) {
        for (dimension y = 0; y < m_height; y++) {
            if (m_board[x][y] != get_base()) count++;
        }
    }

    return count;
}

void Board::save_board() {
    m_previous_boards.push_back(m_board);
}

bool Board::undo_board() {
    // No previous moves.
    if (m_previous_boards.empty()) return false;

    // Undo last move.
    m_board = m_previous_boards[m_previous_boards.size() - 1];
    m_previous_boards.pop_back();
    return true;
}
