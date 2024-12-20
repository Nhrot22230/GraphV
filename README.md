# GraphV - Interactive Graph Visualization

GraphV is a lightweight and interactive graph visualization tool implemented in C using the [Raylib](https://www.raylib.com/) library. It allows users to create, manipulate, and visualize graphs with intuitive mouse and keyboard interactions. Just works for Linux :c.

## Table of Contents

- [Getting Started](#getting-started)
- [Features](#features)
- [Usage](#usage)
- [Key Bindings](#keyboard-shortcuts)
- [Examples](#examples)
- [License](#license)

## Getting Started

### Prerequisites

Make sure you have Raylib installed on your system. Follow the [Raylib installation instructions](https://github.com/raysan5/raylib/wiki/) for your platform.

For Ubuntu, you can install the required dependencies using the following commands:
```bash
sudo apt install build-essential git
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

### Compilation

Clone the repository and compile the source code using a C compiler:

```bash
git clone https://github.com/Nhrot22230/GraphV.git
cd GraphV
make
./build/graphv
```

# Features

    Interactive Graph Manipulation: Create, modify, and delete nodes in the graph using mouse clicks.
    Node States: Nodes can have different states, such as NODE_MOVING and NODE_SELECTED.
    Step-by-Step Search: Perform step-by-step graph search algorithms.

# Usage

The core functionality of GraphV is driven by the state_logic function. This function is responsible for handling user input and updating the state of the graph accordingly.
Mouse Interaction

- **Left Mouse Button**:
    - Click on an existing node: Selects the node if not already selected. If the node is in NODE_MOVING state, it returns it to the default state.
    - Click on empty space: Adds a new vertex to the graph at the mouse position.
- **Right Mouse Button**: Deletes the node at the mouse position and its corresponding edges. If the deleted node was selected, the selection is cleared.

# Keyboard Shortcuts

- **SPACE**: Prints the ID of the current node. If a node is selected, also prints its state.
- **P**: Prints the current state of the graph.
- **C**: Clears the graph and resets various parameters, such as time, selected nodes, and search state.
- **W**: Resets the search state and clears visited nodes in the graph.
- **S**: Switches between search mode and normal mode. In search mode, you can perform graph search algorithms.
- **A**: Prints information about the search state.
- **D**: Performs one step of the search algorithm if a starting node is defined.

# Examples
- ## First Example!!!
![First Example](./examples/ex_1.png)

- ## Searching
![Searching on the First Example](./examples/ex_1_search.png)

- ## Second Example
![Second Example](./examples/ex_2.png)


# License
This project is licensed under the MIT License - see the LICENSE file for details.

Feel free to customize it further based on your needs!
