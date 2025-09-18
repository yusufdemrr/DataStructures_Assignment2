# BlockFall (C++) — Terminal Block‑Dropping Game

A small C++ project that simulates a Tetris‑like, command‑driven block game in the terminal.

**Assignment focus:** practice **Linked Lists**, **Dynamic Memory Allocation**, and **Matrices** (2D grids). Blocks are chained with pointers in a circular rotation list and a next‑block list.

---

## Features

* Rotate block **left/right**, move **left/right**, and **drop**.
* **Gravity mode** toggle: pieces in the grid fall down automatically when enabled.
* **Power‑up**: if its shape appears in the grid, clear all filled cells and gain bonus points.
* **Leaderboard** (top 10): load/save to a file.
* Clear ASCII grid printing (filled vs. empty cells).

---

## Architecture Overview

* **Block**: holds a binary matrix `shape` and pointers to `right_rotation`, `left_rotation`, and `next_block` (linked lists!).
* **BlockFall**: overall game state (grid/matrix, active block+rotation index, gravity, score, power‑up shape). Also reads grid and blocks from files.
* **GameController**: applies commands, checks collisions/bounds, drop and settle blocks, row clear, gravity flow, power‑up detection, scoring, printing.
* **Leaderboard / LeaderboardEntry**: singly linked list of scores, read/write/insert/print.

<img width="1001" height="699" alt="image" src="https://github.com/user-attachments/assets/e68cf2b4-6820-47ee-8891-50caa8929aec" />


---

## Directory / Files

```
Block.{h,cpp}           // Block shape + pointers (rotations, next block)
BlockFall.{h,cpp}       // Game state (grid, power-up, active block), file I/O, rotation mgmt
GameController.{h,cpp}  // Commands, movement, collision, clearing, gravity, scoring, printing
Leaderboard.{h,cpp}     // Score list (linked), read/write/print/insert top 10
LeaderboardEntry.{h,cpp}// Single entry node for leaderboard (linked list)
```

---

## Build & Run

Requires a **C++17** compiler (g++/clang).

```bash
# build
g++ -std=c++17 -O2 *.cpp -o blockfall

# run
# Usage: ./blockfall <grid.txt> <blocks.txt> <gravity_on:0|1> <leaderboard.txt> <player_name> <commands.txt>
./blockfall grid.txt blocks.txt 1 leaderboard.txt Yusuf commands.txt
```

> If your repository provides a `main.cpp` that parses the arguments above, compile with it. Otherwise, see the quick example below.

---

## Input Files

### 1) Grid file (e.g., `grid.txt`)

Plain‑text matrix of **0/1** values separated by spaces. Each line is a row.

Example (5×6):

```
0 0 0 0 0 0
0 0 0 0 0 0
0 0 0 1 0 0
0 0 0 1 0 0
0 0 0 1 0 0
```

### 2) Blocks file (e.g., `blocks.txt`)

Each block is a binary matrix delimited on the **first row** by a leading `[` and on the **last row** by a trailing `]`.

* Rows contain `0` and `1` separated by spaces.
* Example two blocks, the **last** block is treated as **power‑up**:

```
[1 1 1
0 1 0]
[1 1
1 1]
```

The code automatically creates the **four rotations** and links them in a circular list, and also chains blocks with `next_block`.

### 3) Commands file (e.g., `commands.txt`)

One command per line (case‑sensitive):

```
PRINT_GRID
ROTATE_RIGHT
ROTATE_LEFT
MOVE_RIGHT
MOVE_LEFT
DROP
GRAVITY_SWITCH
```

---

## Scoring & Gameplay (Short)

* **DROP**: piece falls until collision; score increases by `y_offset * (# of 1s in the block)`.
* **Completed rows**: each full row gives `cols` points and is cleared.
* **Power‑up**: if the grid contains the power‑up shape, **all filled cells are cleared**, and you gain `1000 + (count of cleared 1s)` points.
* **Gravity mode**: when on, filled cells are iteratively pulled downward; row checks and clears still apply.
* **End conditions**: if next block can’t be placed → **GAME OVER**; if no more blocks → **YOU WIN**. In all cases, the score is inserted into the leaderboard.

---

## Quick Start (minimal `main.cpp`)

If you need a quick driver:

```cpp
#include <iostream>
#include "BlockFall.h"
#include "GameController.h"

int main(int argc, char** argv) {
    if (argc < 7) {
        std::cerr << "Usage: "
                  << argv[0]
                  << " <grid.txt> <blocks.txt> <gravity_on:0|1> <leaderboard.txt> <player_name> <commands.txt>\n";
        return 1;
    }

    std::string grid      = argv[1];
    std::string blocks    = argv[2];
    bool gravity_on       = std::string(argv[3]) == "1";
    std::string lb_file   = argv[4];
    std::string player    = argv[5];
    std::string commands  = argv[6];

    BlockFall game(grid, blocks, gravity_on, lb_file, player);
    GameController controller;
    controller.play(game, commands);
    return 0;
}
```

Compile & run:

```bash
g++ -std=c++17 -O2 main.cpp *.cpp -o blockfall
./blockfall grid.txt blocks.txt 1 leaderboard.txt Yusuf commands.txt
```

---

## API at a Glance

* **BlockFall**

  * `initialize_grid(path)`, `read_blocks(path)`, `rotate_active_block(bool clockwise)`
  * `get_grid_cell(x,y)`, `has_next_block(game)`, `has_next_block_2(game)`
  * State fields: `grid`, `rows`, `cols`, `active_rotation`, `x_offset`, `y_offset`, `active_rotation_index`, `gravity_mode_on`, `current_score`, `power_up`
* **GameController**

  * `play(game, commands_file)`, `is_collision`, `is_valid_position`
  * Movement: `rotate_left/right`, `move_left/right`, `drop_block`
  * Grid ops: `update_grid`, `check_completed_rows`, `remove_completed_rows`, `check_power_ups`, `toggle_gravity`
  * Printing: `print_grid`, `print_2d_vector`, `print_2d_vectorBool`, `findMatrix`
* **Leaderboard**

  * `read_from_file`, `write_to_file`, `insert_new_entry`, `print_leaderboard` (keeps top `MAX_LEADERBOARD_SIZE`)

---

## Known Limitations

* Input file formats must match the expectations exactly (block brackets on first/last row).
* Manual memory management with raw pointers in block rotations and linked lists (be careful with ownership/deletes).
* Command input is file‑driven (no interactive controls).

---

## Example Files (Ready to Copy)

**grid.txt** (6×6 empty grid):

```
0 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
```

**blocks.txt** (first block = T‑like, second = square → becomes power‑up):

```
[1 1 1
0 1 0]
[1 1
1 1]
```

**commands.txt** (a tiny session):

```
PRINT_GRID
MOVE_RIGHT
ROTATE_RIGHT
DROP
GRAVITY_SWITCH
PRINT_GRID
DROP
PRINT_GRID
```

> Tip: The **last block** in `blocks.txt` is used as the **power‑up** shape.

---
