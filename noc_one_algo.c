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

int deliver (struct Flit, int [][SIZE]);
struct Point deliver_direction (struct Flit);
void deliver_new_path (struct Flit, int [][SIZE]);
void deliver_new_path_dfs (struct Flit, int [][SIZE], int [][SIZE], int *);
void deliver_new_path_fitness (struct Flit, int [][SIZE]);
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
        
        printf ("Delivering new random flit\n");
        
        int hasBeenDelivered = deliver (flit, grid);

        if (hasBeenDelivered) {

            printf ("Flit delivered!\n");
        } else {

            printf ("Flit delivery failed!\n");
        }
    }

    return 0;
}

void deliver_new_path_dfs (struct Flit flit, int visited[][SIZE], int grid[][SIZE], char * paths, int * number_of_paths, char direction, int curr_path_charIndex) {

    // add to path[number_of_paths][];
    * paths = (char *) realloc (paths, (* number_of_paths) * (curr_path_charIndex ++) * sizeof (char));

    // store all paths that end at flit.destination
    // todo: store all paths that end at flit.destination
    if (flit.source.x == flit.destination.x && flit.source.y == flit.destination.y) {

        // add this path to vector
        // append to path matrix from function call
        // i ++. then add to paths matrix
        * number_of_paths ++;
    }

    // mark current node as visited
    visited[flit.source.x][flit.source.y] = 1;

    for (int i = flit.source.x - 1; i <= flit.source.x + 1; i ++) {

        for (int j = flit.source.y - 1; j <= flit.source.y + 1; j ++) {

            // currently loop running for all 8 directions

            // check for only 4 directions, and discard diagonal advancements
            if ((i >= 0 && j >= 0 && i < SIZE && j < SIZE)) { // DFS movement to be strictly inside the grid

                // copy flit so as to not alter the original flit, for next DFS iteration
                struct Flit flit_copy = flit;
                // next DFS iteration would start from this grid position, for next node in grid
                flit_copy.source.x = i, flit_copy.source.y = j;

                if (i == flit.source.x - 1 && j == flit.source.y) {
                    
                    // if left
                    deliver_new_path_dfs (flit_copy, visited, grid, & paths, & number_of_paths, 'L', curr_path_charIndex);
                } else if (i == flit.source.x + 1 && j == flit.source.y) {
                    
                    // if right
                    deliver_new_path_dfs (flit_copy, visited, grid, & paths, & number_of_paths, 'R', curr_path_charIndex);
                } else if (i == flit.source.x && j == flit.source.y - 1) {
                    
                    // if up
                    deliver_new_path_dfs (flit_copy, visited, grid, & paths, & number_of_paths, 'U', curr_path_charIndex);
                } else if (i == flit.source.x && j == flit.source.y + 1) {
                    
                    // if down
                    deliver_new_path_dfs (flit_copy, visited, grid, & paths, & number_of_paths, 'D', curr_path_charIndex);
                }
            }
        }
    }
}

void deliver_new_path (struct Flit flit, int grid[][SIZE]) {

    // start a new DFS from current flit node

    int visited[SIZE][SIZE];

    for (int i = 0; i < SIZE; i ++) {

        for (int j = 0; j < SIZE; j ++) {

            visited[i][j] = 0;
        }
    }

    int number_of_paths = 0;
    char * paths = malloc (number_of_paths * sizeof (char));

    // todo: pass paths matrix as reference
    deliver_new_path_dfs (flit, visited, grid, paths, &number_of_paths); // todo: receive paths matrix from dfs
}

struct Point deliver_direction (struct Flit flit) {

    // calculates flit movement direction at every node in transmission
    // U: up, D: down, L: left, R: right

    // calls delivery_util_path

    // return point as in (i, j) to deliver ()

    // todo: best parents?
    // todo: cross over?
}

int deliver (struct Flit flit, int grid[][SIZE]) {

    if (flit.source.x == flit.destination.x && flit.source.y == flit.destination.y) {

        return 1; // 1: delivered
    } else if (0/*todo: no more paths*/) {

        return 0; // 0: delivery failed
    }

    // next hop is determined by deliver_direction ()
    struct Point direction = deliver_direction (flit); // 4 directions

    grid_node_failure (grid); // randomly switches off nodes in the grid, one at a time

    if (1) {

        // check if any node has failed in our original path
        // call deliver_new_path () ?

        // check if (i, j) == 0
    } else {

        // otherwise: continue original path
    }

    // update filt.source to curr grid(i, j)
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