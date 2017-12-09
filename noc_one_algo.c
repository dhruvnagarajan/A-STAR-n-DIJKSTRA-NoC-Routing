#include <stdio.h>
#include <stdlib.h>

int SIZE = 16;
int point[1][1];

struct Point {

    int x;
    int y;
};

struct Flit {

    int payload;
    struct Point source, destination;
};

void deliver (struct Flit, int [][SIZE]);
char deliver_util_direction (struct Flit);
void deliver_util_path (struct Flit, int [][SIZE]);
void deliver_util_path_util (struct Flit, int [][SIZE]);
void grid_init ();
void grid_node_failure (int [][SIZE]);

int main () {

    // grid setup
    int grid[SIZE][SIZE];

    grid_init (grid);

    int t = 10; // number of test cases for flit delivery

    while (t --) {

        // simulating 'flit' generation by the network
        // for every 'flit' that has to be delivered from source 's' to destination 'd'...
        struct Flit flit;

        // todo: set flit source and destination, and payload
        
        deliver (flit, grid);
    }

    return 0;
}

void deliver_util_path_util (struct Flit flit, int grid[][SIZE]) {

    // store all paths that end at flit.destination
}

void deliver_util_path (struct Flit flit, int grid[][SIZE]) {

    // start a DFS from flit.source

    int visited[SIZE][SIZE];

    for (int i = 0; i < SIZE; i ++) {

        for (int j = 0; j < SIZE; j ++) {

            visited[i][j] = 0;
        }
    }

    struct Point * path;
}

char deliver_util_direction (struct Flit flit) {

    // calculates flit movement direction at every node in transmission
    // U: up, D: down, L: left, R: right
}

void deliver (struct Flit flit, int grid[][SIZE]) {

    int source_x = flit.source.x,
        source_y = flit.source.y,
        destination_x = flit.destination.x,
        destination_y = flit.destination.y;

    char direction = deliver_util_direction (flit); // U: (i, j - 1), D: (i, j + 1), L: (i - 1, j), R: (i + 1, j)

    grid_node_failure (grid); // randomly switches off nodes in the grid.

    if (1) {

        // check if any node has failed in our original path
    } else {

        // otherwise: continue original path
    }
}

void grid_node_failure (int grid[][SIZE]) {

    // failed nodes do not become functional later

    int point_x = rand (), point_y = rand ();

    // todo: comporess bw 0 - SIZE. Convert into 'int'

    grid[point_x][point_y] = -1; // 0: node functional, -1: node failed
}

void grid_init (int grid[][SIZE]) {

    for (int i = 0; i < SIZE; i ++) {

        for (int j = 0; j < SIZE; j ++) {

            grid[i][j] = 0; // 0: node functional, -1: node failed (controlled by grid_node_failure ())
        }
    }
}