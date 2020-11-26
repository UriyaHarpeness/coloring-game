#include "game/Game.h"

int main(int argc, char *argv[]) {
    // Set default game settings.
    unsigned int moves = 21;
    dimension width = 18, height = 18;
    unsigned short int colors_num = 4;

    vector<unsigned short int> valid_input_lengths = {1, 2, 4, 5};
    // Check input.
    if ((find(valid_input_lengths.begin(), valid_input_lengths.end(), argc) == valid_input_lengths.end()) ||
        ((argc >= 2) && (string(argv[1]) == "-h"))) {
        throw runtime_error("Usage: coloring [MOVES (21)] [WIDTH (18)] [HEIGHT (18)] [COLOR_NUM (4)]");
    }

    // Parse arguments.
    if (argc >= 2) moves = stoi(argv[1]);
    if (argc >= 4) {
        width = stoi(argv[3]);
        height = stoi(argv[2]);
    }
    if (argc == 5) colors_num = stoi(argv[4]);

    // Initialize the game.
    Game game(moves, width, height, colors_num);

    // Play.
    return game.play() ? 0 : 1;
}
