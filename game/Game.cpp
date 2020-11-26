#include "Game.h"

const set<tile> Game::quit_actions = {'q', 127, 27};

Game::Game(unsigned int moves, dimension width, dimension height, unsigned short int colors_num) :
        m_board(width, height, colors_num), m_moves(moves), m_colors_num(colors_num) {}

char Game::getch() {
    struct termios old_settings{}, new_settings{};

    tcgetattr(0, &old_settings); //grab old terminal i/o settings
    new_settings = old_settings; //make new settings same as old settings
    new_settings.c_lflag &= ~ICANON; //disable buffered i/o
    new_settings.c_lflag &= ~ECHO; //set echo mode
    tcsetattr(0, TCSANOW, &new_settings); //apply terminal io settings

    char ch;
    ch = (char) getchar();

    tcsetattr(0, TCSANOW, &old_settings);

    return ch;
}

void Game::turn(bool &quit) {
    tile action;

    // Display current status.
    m_board.print(m_moves);
    cout << m_moves << " moves left to fill " << m_board.count_remaining_tiles() << " more tiles, Enter action [";
    for (unsigned short int option = 0; option < m_colors_num; option++) {
        if (Board::colors[option] != m_board.get_base()) cout << (char) Board::colors[option];
    }
    cout << "] (" << (m_board.has_history() ? "u, " : "") << "s): ";

    // Loop until a valid action has been made.
    while (true) {
        // Get action.
        do {
            action = (tile) tolower(getch());
        } while (isspace(action));
        cout << (char) action << endl;

        if (action == undo_action) {
            // Undo.
            if (!m_board.undo_board()) {
                cout << "Undo is not possible, retry: ";
            } else {
                m_moves++;

                break;
            }
        } else if (action == change_base_action) {
            // Change base.
            optional_dimension x, y;
            cout << "Enter new base (x y):" << endl;
            cin >> y >> x;
            if (!m_board.set_base({x, y})) {
                cout << "Position is not valid, retry: ";
            } else {
                break;
            }
        } else if (quit_actions.find(action) != quit_actions.end()) {
            // Quit.
            quit = true;
            break;
        } else if ((m_board.get_base() == action) ||
                   (find(Board::colors.begin(), Board::colors.begin() + m_colors_num, action) ==
                    Board::colors.begin() + m_colors_num)) {
            // Invalid color.
            cout << "Color " << (char) action << " is invalid, retry: ";
        } else {
            // Color.
            m_board.save_board();
            set<Point> jokers;
            m_board.paint(action, jokers);
            m_board.paint_jokers(action, move(jokers));
            m_moves--;

            break;
        }
    }
}

bool Game::play() {
    // Display title.
    cout << "--= Coloring Game by Uriya Harpeness =--" << endl << endl;
    cout << "Try to fill the whole board (" << (int) m_board.get_height() << "X" << (int) m_board.get_width() << ") in "
         << m_moves << " moves or less." << endl;
    cout << "Controls are: 'r' - red, 'g' - green, 'b' - blue, 'y' - yellow, 'c' - cyan, 'm' - magenta, 'u' - undo"
            ", 's' - change base, 'q' ESC DEL BACKSPACE - quit." << endl;

    bool quit = false;
    // Run the game.
    while (!m_board.solved() && (m_moves > 0) && !quit) {
        turn(quit);
    }

    // Disqualification.
    if (quit) {
        cout << endl << "--= Rookie move... =--" << endl;
        return false;
    }

    m_board.print(m_moves);

    if (m_board.solved()) {
        // Victory.
        cout << "--= Congrats you win =--" << endl;
        return true;
    } else {
        // Defeat.
        cout << "--= Boo you lose =--" << endl;
        return false;
    }
}
