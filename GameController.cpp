#include <iostream>
#include <fstream>
#include "GameController.h"
#include "Leaderboard.h"

bool GameController::play(BlockFall& game, const string& commands_file) {
    ifstream file(commands_file);
    if (!file.is_open()) {
        cerr << "Error: Unable to open commands file: " << commands_file << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line == "PRINT_GRID") {
            print_grid(game);
        } else if (line == "ROTATE_RIGHT") {
            rotate_right(game);
        } else if (line == "ROTATE_LEFT") {
            rotate_left(game);
        } else if (line == "MOVE_RIGHT") {
            move_right(game);
        } else if (line == "MOVE_LEFT") {
            move_left(game);
        } else if (line == "DROP") {
            drop_block(game);
        } else if (line == "GRAVITY_SWITCH") {
            if (game.gravity_mode_on){
                game.gravity_mode_on = false;
                toggle_gravity(game);
            } else {
                game.gravity_mode_on = true;
                toggle_gravity(game);
            }
        } else {
            cout << "Unknown command: " << line << endl;
        }

        // Check if the game is over
        if (game.game_over) {
            // Get current time
            time_t currentTime = time(nullptr);

            // Create a new leaderboard entry
            LeaderboardEntry* newEntry = new LeaderboardEntry(game.current_score, currentTime, game.player_name);

            // Insert the new entry to the leaderboard
            game.leaderboard.insert_new_entry(newEntry);
            cout << "GAME OVER!" << endl;
            cout << "Next block that couldn't fit:" << endl;
            print_2d_vectorBool(game.active_rotation->shape);
            cout << endl;
            cout << "Final grid and score:" << endl;
            cout << endl;
            cout << "Score: " << game.current_score << endl;
            cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
            print_2d_vector(game.grid);
            cout << endl;
            game.leaderboard.print_leaderboard();
            game.leaderboard.write_to_file(game.leaderboard_file_name);
            return false;
        }

        // Check if no more blocks are available
        if (!game.has_next_block(game)) {

            // Get current time
            time_t currentTime = time(nullptr);

            // Create a new leaderboard entry
            LeaderboardEntry* newEntry = new LeaderboardEntry(game.current_score, currentTime, game.player_name); // Replace "Player" with the actual player name

            // Insert the new entry to the leaderboard
            game.leaderboard.insert_new_entry(newEntry);
            cout << "YOU WIN!" << endl;
            cout << "No more blocks." << endl;
            cout << "Final grid and score:" << endl;
            cout << endl;
            cout << "Score: " << game.current_score << endl;
            cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
            print_2d_vector(game.grid);
            cout << endl;
            game.leaderboard.print_leaderboard();
            game.leaderboard.write_to_file(game.leaderboard_file_name);
            return true;
        }
    }

//  If the file is exhausted
    // Get current time
    time_t currentTime = time(nullptr);

    // Create a new leaderboard entry
    LeaderboardEntry* newEntry = new LeaderboardEntry(game.current_score, currentTime, game.player_name); // Replace "Player" with the actual player name

    // Insert the new entry to the leaderboard
    game.leaderboard.insert_new_entry(newEntry);
    cout << "GAME FINISHED!" << endl;
    cout << "No more commands." << endl;
    cout << "Final grid and score:" << endl;
    cout << endl;
    cout << "Score: " << game.current_score << endl;
    cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
    print_2d_vector(game.grid);
    cout << endl;
    game.leaderboard.write_to_file(game.leaderboard_file_name);
    game.leaderboard.print_leaderboard();
    return true;
}

bool GameController::is_collision(BlockFall& game, int x_offset, int y_offset) {
    Block* active_block = game.active_rotation;


    int new_x = game.x_offset + x_offset;
    int new_y = game.y_offset + y_offset;

    // Collision check
    for (int i = 0; i < active_block->shape.size(); i++) {
        for (int j = 0; j < active_block->shape[0].size(); j++) {
            // Check block boundaries
            if (new_x + j < 0 || new_x + j >= game.cols || new_y + i >= game.rows) {
                return true; // There is a collision, we crossed the borders
            }

            // Check the cell of the grid with the cell of the active block
            if (active_block->shape[i][j] == 1 && game.get_grid_cell(new_x + j, new_y + i) != 0) {
                return true; // There is a collision, the cell is already full
            }
        }
    }

    return false; // no collision
}

bool GameController::is_valid_position(BlockFall& game, int x_offset, int y_offset) {
    Block* active_block = game.active_rotation;

    int new_x = game.x_offset + x_offset;
    int new_y = game.y_offset + y_offset;

    // Check boundaries
    if (new_x < 0 || new_x + active_block->shape[0].size() > game.cols || new_y + active_block->shape.size() > game.rows) {
        return false; // New position is out of bounds+
    }

    // Check collision with other blocks
    for (int i = 0; i < active_block->shape.size(); ++i) {
        for (int j = 0; j < active_block->shape[0].size(); ++j) {
            if (active_block->shape[i][j] == 1) {
                // Check if the cell is filled in the game grid
                if (game.get_grid_cell(new_x + j, new_y + i) != 0) {
                    return false; // There is a collision with another block
                }
            }
        }
    }

    return true; // The position is valid
}


void GameController::rotate_right(BlockFall& game) {
    // Save the current state of the active block
    Block* original_block = game.active_rotation;
    int original_rotation = game.active_rotation_index;

    // Rotate the block to the right
    game.rotate_active_block(true);

    // Check for collision and valid position
    if (is_collision(game, 0, 0) || !is_valid_position(game, 0, 0)) {
        // If there is a collision or the position is not valid, revert the rotation
        game.active_rotation = original_block;
        game.active_rotation_index = original_rotation;
    }
}

void GameController::rotate_left(BlockFall& game) {
    // Save the current state of the active block
    Block* original_block = game.active_rotation;
    int original_rotation = game.active_rotation_index;

    // Rotate the block to the left
    game.rotate_active_block(false);

    // Check for collision and valid position
    if (is_collision(game, 0, 0) || !is_valid_position(game, 0, 0)) {
        // If there is a collision or the position is not valid, revert the rotation
        game.active_rotation = original_block;
        game.active_rotation_index = original_rotation;
    }
}

void GameController::move_right(BlockFall& game) {
    // Save the current state of the active block
    Block* original_block = game.active_rotation;
    int original_x_offset = game.x_offset;

    // Move the block one cell to the right
    game.x_offset++;

    // Check for collision and valid position
    if (is_collision(game, 0, 0) || !is_valid_position(game, 0, 0)) {
        // If there is a collision or the position is not valid, revert the movement
        game.active_rotation = original_block;
        game.x_offset = original_x_offset;
    }
}

void GameController::move_left(BlockFall& game) {
    // Save the current state of the active block
    Block* original_block = game.active_rotation;
    int original_x_offset = game.x_offset;

    // Move the block one cell to the left
    game.x_offset--;

    // Check for collision and valid position
    if (is_collision(game, 0, 0) || !is_valid_position(game, 0, 0)) {
        // If there is a collision or the position is not valid, revert the movement
        game.active_rotation = original_block;
        game.x_offset = original_x_offset;
    }
}

void GameController::drop_block(BlockFall& game) {
    // Save the current state of the active block
    Block* original_block = game.active_rotation;

    // Move the block down until a collision is detected
    while (!is_collision(game, 0, 1) && is_valid_position(game, 0, 1)) {
        game.y_offset++;
    }

    int numberof1 = 0;
    for (int i = 0; i < original_block->shape.size(); ++i) {
        for (int j = 0; j < original_block->shape[0].size(); ++j) {
            if (original_block->shape[i][j] == 1) {
                numberof1++;
            }
        }
    }

    game.current_score += game.y_offset * numberof1;

    // Update the grid with the settled block
    update_grid(game);

    // Check for power-ups
    check_power_ups(game);

    // Check for completed rows and update score
    if (!game.gravity_mode_on){
        int completed_rows = check_completed_rows(game);

        if (completed_rows > 0) {
            // Remove completed rows and update the grid
            std::cout << "Before clearing:" << std::endl;
            print_2d_vector(game.grid);
            std::cout << std::endl;
            std::cout << std::endl;
            remove_completed_rows(game);
        }
    }

    if (game.has_next_block_2(game)){
        // Update the active block to the next block
        game.active_rotation = game.active_rotation->next_block;
        game.active_rotation_index = 0;
        game.y_offset = 0;
        game.x_offset = 0;

        // Check if the game is over (if the new block cannot enter the grid)
        if (is_collision(game, 0, 0) || !is_valid_position(game, 0, 0)) {
            game.game_over = true;
        }
    } else {
        game.active_rotation = nullptr;
    }
}

void GameController::update_grid(BlockFall& game) {
    Block* active_block = game.active_rotation;

    // Iterate over the active block's shape and update the grid
    for (int i = 0; i < active_block->shape.size(); ++i) {
        for (int j = 0; j < active_block->shape[0].size(); ++j) {
            if (active_block->shape[i][j] == 1) {
                // Update the corresponding cell in the grid with the block's value
                game.grid[game.y_offset + i][game.x_offset + j] = 1;
            }
        }
    }

    toggle_gravity(game);
}

int GameController::check_completed_rows(BlockFall& game) {
    int completed_rows = 0;

    // Iterate over each row in the grid
    for (int i = 0; i < game.rows; ++i) {
        // Check if the current row is full
        bool is_row_full = true;
        for (int j = 0; j < game.cols; ++j) {
            if (game.grid[i][j] == 0) {
                is_row_full = false;
                break;
            }
        }

        // If the row is full, increment the completed_rows count
        if (is_row_full) {
            completed_rows++;
        }
    }

    game.current_score += completed_rows * game.cols;

    return completed_rows;
}

void GameController::remove_completed_rows(BlockFall& game) {
    // Iterate over each row in the grid
    for (int i = 0; i < game.rows; ++i) {
        // Check if the current row is full
        bool is_row_full = true;
        for (int j = 0; j < game.cols; ++j) {
            if (game.grid[i][j] == 0) {
                is_row_full = false;
                break;
            }
        }

        // If the row is full, remove it by shifting the rows above it down
        if (is_row_full) {
            // Clear the current row
            for (int j = 0; j < game.cols; ++j) {
                game.grid[i][j] = 0;
            }

            // Shift the rows above it down
            for (int k = i; k > 0; --k) {
                for (int j = 0; j < game.cols; ++j) {
                    game.grid[k][j] = game.grid[k - 1][j];
                }
            }
        }
    }
}


void GameController::check_power_ups(BlockFall& game) {

    bool foundPowerUp;

    foundPowerUp = findMatrix(game.grid, game.power_up);

    // If the power-up shape is found, clear the corresponding portion of the grid
    int numberOfOne = 0;
    if (foundPowerUp) {
        std::cout << "Before clearing:" << std::endl;
        print_2d_vector(game.grid);
        std::cout << std::endl;
        std::cout << std::endl;
        for (int i = 0; i < game.rows; ++i) {
            for (int j = 0; j < game.cols; ++j) {
                if (game.grid[i][j] == 1){
                    game.grid[i][j] = 0;
                    numberOfOne++;
                }
            }
        }
        game.current_score += 1000;
        game.current_score += numberOfOne;
    }

}

bool GameController::findMatrix(const std::vector<std::vector<int>>& source, const std::vector<std::vector<bool>>& target) {
    for (size_t i = 0; i <= source.size() - target.size(); ++i) {
        for (size_t j = 0; j <= source[i].size() - target[0].size(); ++j) {
            bool found = true;
            for (size_t k = 0; k < target.size(); ++k) {
                for (size_t l = 0; l < target[k].size(); ++l) {
                    if (source[i + k][j + l] != target[k][l]) {
                        found = false;
                        break;
                    }
                }
                if (!found) {
                    break;
                }
            }
            if (found) {
                return true;
            }
        }
    }
    return false;
}

void GameController::toggle_gravity(BlockFall& game) {

    // If the gravity mode is GRAVITY_ON, update the block's fall behavior
    if (game.gravity_mode_on) {
        // Implement gravity mode behavior

        int number = 0;
        while(number < game.rows){
            for (int i = 0; i < game.rows; ++i) {
                for (int j = 0; j < game.cols; ++j) {
                    if (i != game.rows -1 && game.grid[i][j] == 1 && game.grid[i + 1][j] == 0){
                        game.grid[i][j] = 0;
                        game.grid[i + 1][j] = 1;
                    }
                }
            }
            number++;
        }
    }

    // Check for completed rows, remove them, and update the score
    int completed_rows = check_completed_rows(game);

    if (completed_rows > 0) {
        // Remove completed rows and update the grid
        std::cout << "Before clearing:" << std::endl;
        print_2d_vector(game.grid);
        std::cout << std::endl;
        std::cout << std::endl;
        remove_completed_rows(game);
    }
}

void GameController::print_grid(BlockFall& game) {
    // Print player's current score
    std::cout << "Score: " << game.current_score << std::endl;

    // Print all-time high score
    std::cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << std::endl;

    // Print the grid
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            // Check if the current cell belongs to the active block
            if (i >= game.y_offset && i < game.y_offset + game.active_rotation->shape.size() &&
                j >= game.x_offset && j < game.x_offset + game.active_rotation->shape[0].size() &&
                game.active_rotation->shape[i - game.y_offset][j - game.x_offset] == 1) {
                std::cout << occupiedCellChar; // Print black cell for active block
            } else {
                // Print white cell for empty cell, black cell for filled cell
                std::cout << (game.grid[i][j] == 0 ? unoccupiedCellChar : occupiedCellChar);
            }
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::endl;
}

void GameController::print_2d_vector(const vector<vector<int>>& vec) const {
    for (const auto &row: vec) {
        for (const auto &element: row) {
            cout << (element == 0 ? unoccupiedCellChar : occupiedCellChar) ;
        }
        cout << endl;
    }
}

void GameController::print_2d_vectorBool(const vector<vector<bool>>& vec) const {
    for (const auto &row: vec) {
        for (const auto &element: row) {
            cout << (element == 0 ? unoccupiedCellChar : occupiedCellChar);
        }
        cout << endl;
    }
}













