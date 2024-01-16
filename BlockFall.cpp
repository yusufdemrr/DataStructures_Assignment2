#include "BlockFall.h"
#include "GameController.h"
#include <fstream>
#include <sstream>
#include <iostream>

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}


void BlockFall::initialize_grid(const string &input_file) {
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Error: Unable to open grid file." << endl;
        exit(EXIT_FAILURE);
    }


    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream iss(line);
        int value;
        while (iss >> value) {
            row.push_back(value);
        }
        grid.push_back(row);
    }

    rows = grid.size();
    cols = grid[0].size();

    file.close();

}


// Helper function to rotate a block 90 degrees clockwise
vector<vector<bool>> BlockFall::rotate_block(const vector<vector<bool>> &block) {
    int rows = block.size();
    int cols = block[0].size();
    vector<vector<bool>> rotated(cols, vector<bool>(rows, false));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            rotated[j][rows - 1 - i] = block[i][j];
        }
    }

    return rotated;
}

// Helper function to create rotations of a block
Block* BlockFall::create_rotations(const vector<vector<bool>>& shape) {
    Block* head = new Block();
    head->shape = shape;

    Block* current = head;

    // Create three rotations (90, 180, and 270 degrees)
    for (int i = 0; i < 3; ++i) {
        Block* rotation = new Block();
        rotation->shape = BlockFall::rotate_block(current->shape);
        current->right_rotation = rotation;
        rotation->left_rotation = current;
        current = rotation;
    }

    // Link the last rotation to the initial block
    head->left_rotation = current;
    current->right_rotation = head;

    return head;
}

void BlockFall::read_blocks(const string &input_file) {
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Error: Unable to open blocks file: " << input_file << endl;
        exit(EXIT_FAILURE);
    }

    Block* tail = nullptr;  // To keep track of the last block in the list

    // Read each line (block) from the file
    string line;
    vector<vector<bool>> block_shape;
    while (getline(file, line)) {
        // Skip lines that contain '[' character
        if (line.find('[') != string::npos) {
            line = line.substr(1);
        }

        if (line.size() == 0){
            continue;
        }

        // If the line contains ']' character, it means the block is complete
        if (line.find(']') != string::npos) {
            line = line.substr(0,line.length()-1);
            // Parse the block shape from the line
            istringstream iss(line);
            char cell;
            vector<bool> row;
            while (iss >> cell) {
                row.push_back(cell == '1');
            }
            block_shape.push_back(row);

            // Create rotations and link them
            Block* block_rotations = create_rotations(block_shape);

            // If this is the first block, set it as the initial_block
            if (initial_block == nullptr) {
                initial_block = block_rotations;
            }

            // If there is a tail (previous block), link it to the current block
            if (tail != nullptr) {
                tail->next_block = block_rotations;
                tail->right_rotation->next_block = block_rotations;
                tail->left_rotation->next_block = block_rotations;
                tail->right_rotation->right_rotation->next_block = block_rotations;
            }

            // Update the tail to the last block in the current rotations
            tail = block_rotations;

            // Clear the block_shape vector for the next block
            block_shape.clear();
        } else {
            // Parse the block shape from the line
            istringstream iss(line);
            char cell;
            vector<bool> row;
            while (iss >> cell) {
                row.push_back(cell == '1');
            }
            block_shape.push_back(row);
        }
    }

    // Set the power-up as the last block (tail)
    power_up = tail->shape;

    Block* block2 = initial_block;
    while(block2->next_block->next_block != nullptr){
        block2 = block2->next_block;
    }

    tail = block2;

    delete tail->next_block->right_rotation->right_rotation;
    delete tail->next_block->right_rotation;
    delete tail->next_block->left_rotation;
    delete tail->right_rotation->next_block;

    tail->next_block = nullptr;
    tail->right_rotation->right_rotation->next_block = nullptr;
    tail->right_rotation->next_block = nullptr;
    tail->left_rotation->next_block = nullptr;

    active_rotation = initial_block;

    file.close();
}




BlockFall::~BlockFall() {
    Block* current = initial_block;
    while (current != nullptr) {
        Block* next = current->next_block;
        delete current->right_rotation->right_rotation;
        delete current->right_rotation;
        delete current->left_rotation;
        delete current;
        current = next;
    }

    power_up.clear();
}

void BlockFall::rotate_active_block(bool clockwise) {
    // Clockwise rotation if clockwise is true, counter-clockwise otherwise
    int rotation_direction = clockwise ? 1 : -1;

    // Save the current state of the active block
    Block* original_block = active_rotation;
    int original_rotation = active_rotation_index;

    // Move to the next rotation
    active_rotation = clockwise ? active_rotation->right_rotation : active_rotation->left_rotation;

    // Update the rotation index
    active_rotation_index = (original_rotation + rotation_direction) % 4;
    if (active_rotation_index < 0) {
        active_rotation_index += 4;
    }

    // If there is a collision or the new position is not valid, revert the rotation
    if (GameController::is_collision(*this, 0, 0) || !GameController::is_valid_position(*this, 0, 0)) {
        active_rotation = original_block;
        active_rotation_index = original_rotation;
    }
}


int BlockFall::get_grid_cell(int x, int y) const {
    // Belirtilen konumdaki grid hücresini döndür
    return grid[y][x];
}

bool BlockFall::has_next_block(BlockFall &game) const {
    return game.active_rotation != nullptr;
}

bool BlockFall::has_next_block_2(BlockFall &game) const {
    return game.active_rotation->next_block != nullptr;
}


