#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    bool play(BlockFall &game, const string &commands_file); // Function that implements the gameplay

    static bool is_collision(BlockFall &game, int x_offset, int y_offset);

    static bool is_valid_position(BlockFall &game, int x_offset, int y_offset);

    void rotate_right(BlockFall &game);

    void rotate_left(BlockFall &game);

    void move_right(BlockFall &game);

    void move_left(BlockFall &game);

    void drop_block(BlockFall &game);

    void update_grid(BlockFall &game);

    int check_completed_rows(BlockFall &game);

    void remove_completed_rows(BlockFall &game);

    void check_power_ups(BlockFall &game);

    void toggle_gravity(BlockFall &game);

    void print_grid(BlockFall &game);

    void print_2d_vector(const vector<vector<int>> &vec) const;

    void print_2d_vectorBool(const vector<vector<bool>> &vec) const;

    bool findMatrix(const vector<std::vector<int>> &source, const vector<std::vector<bool>> &target);
};


#endif //PA2_GAMECONTROLLER_H
